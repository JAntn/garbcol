#define _GC_HIDE_METHODS
#include <stack>
#include "gc.h"

namespace gcNamespace {

void _gc_wait_marking()
{
    if(_gc_collector->is_marking)
    {
        std::unique_lock<std::mutex> a_lock(_gc_collector->mutex_instance);

        while(_gc_collector->is_marking)
        {
            // wait until signal
            _gc_collector->is_marking_cv.wait(
                        a_lock,
                        []{ return !(_gc_collector->is_marking); }
            );
        }
    }
}

void _gc_wait_sweeping()
{
    if(_gc_collector->is_sweeping)
    {
        std::unique_lock<std::mutex> a_lock(_gc_collector->mutex_instance);

        while(_gc_collector->is_sweeping)
        {
            // wait until signal
            _gc_collector->is_sweeping_cv.wait(
                        a_lock,
                        []{ return !(_gc_collector->is_sweeping); }
            );
        }
    }
}

gcConnectThread::gcConnectThread() {

    scope_info = new gcScopeInfo;

    _GC_THREAD_LOCK;

    _gc_scope_info = scope_info;

    _gc_collector->scope_info_list.push_back(_gc_scope_info);

    _gc_scope_info->position = --(_gc_collector->scope_info_list.end());
    _gc_scope_info->root_scope = new gcScopeContainer;
    _gc_scope_info->current_scope = _gc_scope_info->root_scope;
    _gc_scope_info->from_allocator = false;
}

gcConnectThread::~gcConnectThread(){

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

    // NOTE             ///////////////////////////////////////

    // User might change a pointer contents while it is in mark loop
    // If above sentence is removed, it is need additional protection:

    // From 0. to 0.04.4 it is safe that user create new objects while in mark loop
    // Since all objects are connected to root nodes, while mark loop is running,
    // all objects will be marked UNLESS:
    //   1. Rock leaf is moved from a unmarked branch to a marked branch.
    //   2. Rock pointer changes its contents just in the midle of its processing inside <<THIS*>> loop.

    // 0.04.5 - SO, threads will stop until marking ends in these conditions

    // ENDNOTE          ///////////////////////////////////////

    _gc_collector->is_marking = true;
    _gc_collector->is_marking_cv.notify_all();

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

    // Change mark state.
    mark_bit ^= _gc_mark_bit;

    // Mark is done.
    _gc_collector->is_marking = false;
    _gc_collector->is_marking_cv.notify_all();
}

void gcCollector::gc_sweep() {

    _gc_collector->is_sweeping = true;
    _gc_collector->is_sweeping_cv.notify_all();

    // Not thread lock required since 'deleted' objects are not in use

    // Clean thread-finalized stack
    for (auto scope_info : remove_scope_info_stack)
    {
        scope_info_list.erase(scope_info->position);
        delete scope_info->root_scope;
        delete scope_info;
    }
    remove_scope_info_stack.clear();

    // Remove marked objects
    auto position_prev = heap.before_begin();
    auto position= position_prev;
    ++position;

    while (position != heap.end())
    {
        gcObject_B_* object = *position;

        if (object->gc_is_finalized())
        {
            if (object->gc_is_safe_finalizable())
            {
                heap.erase_after(position_prev);
                position = position_prev;
                ++position;
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
                if (object->gc_is_finalizable() && object->gc_is_safe_finalizable())
                {
                    heap.erase_after(position_prev);
                    position = position_prev;
                    ++position;
                    delete object;
                }else{
                    ++position_prev;
                    position = position_prev;
                    ++position;
                }
                continue;
            }

            object->gc_make_safe_finalizable();
            object->gc_make_unreachable();

            ++position_prev;
            position = position_prev;
            ++position;
            continue;
        }

        ++position_prev;
        position = position_prev;
        ++position;
    }

    _gc_collector->is_sweeping = false;
    _gc_collector->is_sweeping_cv.notify_all();
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
