#define _GC_HIDE_METHODS
#include <stack>
#include "gc.h"

namespace gcNamespace {

gcConnectThread::gcConnectThread() {

    scope_info = new gcScopeInfo;

    // Prevent that other threads to change collector attributes
    _GC_THREAD_LOCK;

    _gc_scope_info = scope_info;

    _gc_collector->scope_info_list.push_back(_gc_scope_info);
    _gc_scope_info->position = --(_gc_collector->scope_info_list.end());
    _gc_scope_info->root_scope = new gcScopeContainer;
    _gc_scope_info->current_scope = _gc_scope_info->root_scope;
    _gc_scope_info->from_allocator = false;
}

gcConnectThread::~gcConnectThread(){

    // Prevent other threads to change collector attributes
    _GC_THREAD_LOCK;

    _gc_collector->remove_scope_info_stack.push_back(scope_info);
}

void gcCollector::gc_free_heap() {

    // Free program memory
    for (gcObject_B_* object : heap)
    {
        if(object->gc_is_finalizable()){
            object->gc_make_safe_finalizable();
            delete object;
        }
    }
}

void gcCollector::gc_mark() {

    // NOTE         ///////////////////////////////////////

    // User might change a pointer contents while it is in mark loop
    // If above sentence is removed, it is need additional protection:

    // From 0. to 0.04.3 it is safe that user create new objects while in mark loop
    // Since all objects are connected to root nodes, while mark loop is running,
    // all objects will be marked UNLESS:
    //   1. A leaf is moved from a unmarked branch to a marked branch.
    //   2. A pointer changes its contents just in the midle of its processing inside <<THIS*>> loop.


    //
    // Possible implementation of (2):
    // Pointers have one extra reference. When a pointer is requested to change
    // while mark loop is running, this extra reference is filled with the current object reference;
    // Once the pointer is not used anymore in mark loop, this reference is reset.

    // This method fails yet because of (1). IF containers worked entirely with smart pointers (the links were sm.pt.)
    // this issue would be replaced by an issue of type (2), and therefore, the mark loop would run totally concurrent.

    // ENDNOTE      ///////////////////////////////////////

    _gc_collector->is_marking = true;
    gcPointer_B_* pointer_snapshot = nullptr;

    for (auto
         scope_info_position = scope_info_list.begin();
         scope_info_position != scope_info_list.end();
         ++scope_info_position
         )
    {
        auto scope_info = *scope_info_position;

        std::stack<gcIterator_B_*> current_position_stack;
        std::stack<const gcContainer_B_*> current_parent_stack;

        const gcContainer_B_* parent = scope_info->root_scope;
        gcIterator_B_* position = parent->gc_begin();
        gcIterator_B_* end_position = parent->gc_end();

        while (true)
        {
            // Check position is not last item in parent
            while (!(position->gc_is_equal(end_position)))
            {
                const gcPointer_B_* pointer = position->gc_get_const_pointer();

                // EXPERIMENTAL     /////////////////////////////////////////////////////

                // Get the snapshot of the pointer just before collector mark function begins

                delete pointer_snapshot;

                pointer_snapshot = pointer->gc_pop_snapshot();

                if (pointer_snapshot != nullptr) {
                    pointer = pointer_snapshot;
                }

                // EXPERIMENTAL     /////////////////////////////////////////////////////

                // Advance if it is null
                if (pointer->gc_is_empty() || pointer->gc_is_weak_pointer() || pointer->gc_check_n_clear()) {
                    position->gc_next();
                    continue;
                }

                // Check object is not marked
                if (!pointer->gc_is_marked())
                {
                    // Mark
                    pointer->gc_mark();

                    // Move to a deeper node
                    current_position_stack.push(position);
                    current_parent_stack.push(parent);

                    parent = pointer->gc_get_const_childreen();

                    if (parent == nullptr) {
                        position = nullptr;
                        end_position = nullptr;
                        break;
                    }

                    position = parent->gc_begin();

                    // Reset end position
                    delete end_position;
                    end_position = parent->gc_end();
                    continue;
                }

                // If it is already marked, move next
                position->gc_next();
            }

            // Return to a shallow node
            if (!current_position_stack.empty())
            {

                // Recover last position and parent
                delete position;
                position = current_position_stack.top();
                parent = current_parent_stack.top();

                // Reset end position
                delete end_position;
                end_position = parent->gc_end();

                current_position_stack.pop();
                current_parent_stack.pop();
            }
            else
                // There are not more shallow childreen
                break;
        }
        delete position;
        delete end_position;
    }


    delete pointer_snapshot;

    _GC_THREAD_LOCK;

    // Mark is done.
    _gc_collector->is_marking = false;

    // Change mark state.
    mark_bit ^= _gc_mark_bit;
}

void gcCollector::gc_sweep() {

    // Not thread lock required since objects are not in use

    // Clean thread-finalized stack
    for (auto scope_info : remove_scope_info_stack)
    {
        scope_info_list.erase(scope_info->position);
        delete scope_info->root_scope;
        delete scope_info;
    }
    remove_scope_info_stack.clear();

    // Remove marked objects
    for (auto position = heap.begin(); position != heap.end();)
    {
        gcObject_B_* object = *position;

        if (object->gc_is_finalized())
        {
            if (object->gc_is_safe_finalizable())
            {
                auto tmp = position;
                ++position;
                heap.erase(tmp);
                delete object;
                continue;
            }else{
                object->gc_make_safe_finalizable();
            }
        }

        if (object->gc_is_marked())
        {
            if (!object->gc_is_reachable())
            {
                auto tmp = position;
                ++position;

                if (object->gc_is_finalizable() && object->gc_is_safe_finalizable())
                {
                    heap.erase(tmp);
                    delete object;
                }
                continue;
            }

            object->gc_make_safe_finalizable();
            object->gc_make_unreachable();

            ++position;
            continue;
        }

        ++position;
    }
}

void gcCollector::gc_collect() {

    gcConnectThread do_it;

    auto start_time = std::chrono::steady_clock::now();
    auto step_time = std::chrono::milliseconds(_gc_collector->sleep_time);
    auto end_time = start_time + step_time;

    while (!_gc_collector->exit_flag) {

        std::this_thread::sleep_until(end_time);

        _gc_collector->gc_mark();
        _gc_collector->gc_sweep();

        start_time = std::chrono::steady_clock::now();
        end_time = start_time + step_time;
    }
}

gcCollector::gcCollector() : gcCollector(100) {
    // 100 ms is default collector cleaning step
}

gcCollector::gcCollector(int sleep_time) {   

    _gc_collector = this;
    _gc_collector->mark_bit = _gc_mark_bit;
    _gc_collector->exit_flag = false;

    // default time step for each cleaning operation
    _gc_collector->sleep_time = sleep_time;

    _gc_collector->is_marking = false;

    // default thread connection object
    connect_thread = new gcConnectThread;

    _gc_collector->thread_instance = std::thread(&gcCollector::gc_collect);
}

gcCollector::~gcCollector() {

    delete connect_thread;

    _gc_collector->exit_flag = true;
    _gc_collector->thread_instance.join();
    gc_free_heap();

}

// One instance only allowed
gcCollector*                _gc_collector;
// Specific thread info
thread_local gcScopeInfo*   _gc_scope_info;

// Sentinel
gcSentinelObject            _gc_sentinel;

}

//end
