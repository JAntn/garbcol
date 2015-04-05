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

    // Prevent that other threads to change collector attributes
    _GC_THREAD_LOCK;

    _gc_collector->remove_scope_info_stack.push_back(scope_info);
}

void gcCollector::gc_free_heap() {

    // Free program memory
    for (gcObject_B_* object : heap)
    {
        if(object->gc_is_finalizable())
            delete object;
    }
}

void gcCollector::gc_mark() {

    for (auto
         scope_info_position = scope_info_list.begin();
         scope_info_position != scope_info_list.end();
         ++scope_info_position
         )
    {
        auto scope_info = *scope_info_position;

        std::stack<gcIterator_B_*> current_position_stack;
        std::stack<const gcContainer_B_*> current_parent_stack;

        const gcContainer_B_* parent = scope_info->root_scope;  // Since parent is a const container:
        gcIterator_B_* position = parent->gc_begin();           // This returns a const_iterator (you see a gcIterator_B_)
        gcIterator_B_* end_position = parent->gc_end();         // This returns a const_iterator (you see a gcIterator_B_)

        while (true)
        {
            // Check position is not last item in parent
            while (!(position->gc_is_equal(end_position)))
            {
                const gcPointer_B_* pointer = position->gc_get_const_pointer();

                // Advance if it is null
                if (pointer->gc_is_empty() || pointer->gc_is_weak_pointer()) {
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

                    // Experimental feature: set to zero if the object is removed by user
                    pointer->gc_clear_if_finalized();

                    if (parent == 0) {
                        position = 0;
                        end_position = 0;
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

    // Mark is done.
    // Change mark state. Prevent from wrong object initialization
    _GC_THREAD_LOCK;

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
            if (object->gc_is_finalize_safe())
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

                if (object->gc_is_finalizable())
                {
                    heap.erase(tmp);
                    delete object;
                }
                continue;
            }

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
    _gc_collector->sleep_time = sleep_time;    // default time step for each cleaning operation

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
gcCollector* _gc_collector;

// Specific thread info
thread_local gcScopeInfo* _gc_scope_info;
}

//end
