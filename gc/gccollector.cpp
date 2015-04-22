#define _GC_HIDE_METHODS
#include <stack>
#include "gc.h"

namespace gcNamespace {

gcConnectThread::gcConnectThread()
{
    _GC_THREAD_LOCK;

    scope_info = new gcScopeInfo;
    scope_info->root_scope = new gcScopeContainer;
    scope_info->current_scope = scope_info->root_scope;
    scope_info->from_allocator = false;

    _gc_collector->scope_info_list.push_back(scope_info);
    scope_info->position = --(_gc_collector->scope_info_list.end());

    _gc_scope_info = scope_info;
}

gcConnectThread::~gcConnectThread()
{
    _GC_THREAD_LOCK;

    _gc_collector->remove_scope_info_stack.push_back(scope_info);
}

void gcCollector::gc_heap_push(gcObject_B_*const object)
{
    heap.push_front(object);
}

void gcCollector::gc_free_heap()
{
    for (gcObject_B_* object : heap)
    {
        if (object->gc_is_finalizable()){
            object->gc_make_safe_finalizable();
            delete object;
        }
    }
}

void gcCollector::gc_mark() {

    ///////////////////////////////////////////////////////////////////////////////
    //  User might change a pointer content while gc_mark() is running
    //
    //  From 0. to 0.04.4 it is safe create new objects while gc_mark() runs
    //  Since all objects are connected to root nodes, while mark loop is running
    //  all objects will be marked UNLESS:
    //      1.  A leaf is moved from a unmarked branch to a marked branch.
    //      2.  A pointer changes its contents just in the midle of its processing
    //          inside gc_mark()

    _GC_THREAD_LOCK;

    for (auto
         scope_info_position = scope_info_list.begin();
         scope_info_position != scope_info_list.end();
         ++scope_info_position)
    {
        std::stack<gcIterator_B_*> current_position_stack;
        std::stack<const gcContainer_B_*> current_parent_stack;

        const gcContainer_B_* parent = (*scope_info_position)->root_scope;
        gcIterator_B_* position = parent->gc_begin();
        gcIterator_B_* end_position = parent->gc_end();

        while (true)
        {
            while (!position->gc_is_equal(end_position))
            {
                auto pointer = position->gc_get_const_pointer();

                if (pointer->gc_is_weak_pointer() || pointer->gc_check_n_clear()){
                    position->gc_next();
                    continue;
                }
                else if (!pointer->gc_is_marked())
                {
                    pointer->gc_mark();

                    current_position_stack.push(position);
                    current_parent_stack.push(parent);
                    parent = pointer->gc_get_const_childreen();

                    if (parent != nullptr)
                    {
                        position = parent->gc_begin();
                        delete end_position;
                        end_position = parent->gc_end();
                        continue;
                    }
                    else{
                        position = nullptr;
                        end_position = nullptr;
                        break;
                    }
                }
                else{
                    position->gc_next();
                    continue;
                }
            }

            if (!current_position_stack.empty())
            {
                // Return to a shallow node
                delete position;
                position = current_position_stack.top();
                parent = current_parent_stack.top();

                delete end_position;
                end_position = parent->gc_end();

                current_position_stack.pop();
                current_parent_stack.pop();
            }
            else{
                // We are at the root node
                break;
            }
        }
        delete position;
        delete end_position;
    }

    gc_mark_bit_value ^= _gc_mark_bit;
}

void gcCollector::gc_sweep()
{
    ///////////////////////////////////////////////////////////////////////////////
    // Not thread lock required since 'deleted' objects are not in use

    // Clean thread-finalized stack

    {
        _GC_THREAD_LOCK;
        for (auto scope_info_position = remove_scope_info_stack.begin();
             scope_info_position != remove_scope_info_stack.end();)
        {
            auto scope_info = *scope_info_position;
            scope_info_list.erase(scope_info->position);
            delete scope_info->root_scope;
            delete scope_info;

            ++scope_info_position;
            remove_scope_info_stack.pop_back();
        }
    }

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
            }
            else{
                object->gc_make_safe_finalizable();

                ++position_prev;
                position = position_prev;
                ++position;
                continue;
            }
        }
        else if (object->gc_is_marked())
        {
            if (!object->gc_is_reachable())
            {
                if (object->gc_is_finalizable())
                {
                    if (object->gc_is_safe_finalizable())
                    {
                        heap.erase_after(position_prev);
                        position = position_prev;
                        ++position;
                        delete object;
                        continue;
                    }
                    else
                    {
                        object->gc_make_safe_finalizable();

                        ++position_prev;
                        position = position_prev;
                        ++position;
                        continue;
                    }
                }
                else{
                    ++position_prev;
                    position = position_prev;
                    ++position;
                    continue;
                }

            }
            else
            {
                object->gc_make_unreachable();

                ++position_prev;
                position = position_prev;
                ++position;
                continue;
            }
        }

        ++position_prev;
        position = position_prev;
        ++position;

    }
}

void gcCollector::gc_collect()
{
    gcConnectThread connect_thread;

    auto start_time = std::chrono::steady_clock::now();
    auto step_time = std::chrono::milliseconds(_gc_collector->gc_sleep_time);
    auto end_time = start_time + step_time;

    while (!_gc_collector->gc_exit_flag) {

        std::this_thread::sleep_until(end_time);

        _gc_collector->gc_mark();
        _gc_collector->gc_sweep();

        start_time = std::chrono::steady_clock::now();
        end_time = start_time + step_time;
    }
}

gcCollector::gcCollector() : gcCollector(100) {}

gcCollector::gcCollector(int sleep_time)
{
    _gc_collector = this;

    gc_mark_bit_value = _gc_mark_bit;
    gc_exit_flag = false;
    gc_sleep_time = sleep_time;

    connect_thread = new gcConnectThread;
    gc_thread = std::thread(&gcCollector::gc_collect);
}

gcCollector::~gcCollector()
{
    delete connect_thread;

    gc_exit_flag = true;
    gc_thread.join();
    gc_free_heap();
}

// Singleton
gcCollector*                _gc_collector;

thread_local gcScopeInfo*   _gc_scope_info;
gcSentinelObject            _gc_sentinel;

}

//end
