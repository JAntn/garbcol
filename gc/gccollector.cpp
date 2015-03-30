#define _GC_HIDE_METHODS

#include "gc.h"
#include "gccontainer.h"

namespace collector {

	gcConnectThread::gcConnectThread() {

		scope_info = new gcScopeInfo;

        // Prevent that other threads to change collector attributes
		_GC_THREAD_LOCK

        _gc_scope_info = scope_info;

		_gc_collector->scope_info_list.push_back(_gc_scope_info);
		_gc_scope_info->position = --(_gc_collector->scope_info_list.end());
		_gc_scope_info->root_scope = new gcScopeContainer;
		_gc_scope_info->current_scope = _gc_scope_info->root_scope;
        _gc_scope_info->from_allocator = false;
	}

	gcConnectThread::~gcConnectThread(){

        // Prevent that other threads to change collector attributes
		_GC_THREAD_LOCK

		_gc_collector->scope_info_remove_stack.push_back(scope_info);
	}

	void gcCollector::free_heap() {

        // Free program memory
		for (auto position = heap.begin(); position != heap.end(); ++position)
			delete *position;
	}

	void gcCollector::mark() {

		for (auto 
			scope_info_position = scope_info_list.begin();
			scope_info_position != scope_info_list.end();
			++scope_info_position
			)
		{
			auto scope_info = *scope_info_position;

			std::stack<gcIteratorInterface*> current_position_stack;
			std::stack<gcContainerInterface*> current_parent_stack;

			auto parent = scope_info->root_scope;
			auto position = parent->gc_begin();
			auto end_position = parent->gc_end();

			while (true)
			{
				// Check position is not last item in parent				
				while (!(position->gc_is_equal(end_position)))
				{
                    auto object = position->gc_get_pointer()->object;

					// Advance if it is null
					if (object == 0) {
						position->gc_next();
						continue;
					}

					// Check object is not marked
					if (object->marked[0] == marked_condition)
					{
						// Mark
						object->marked[0] = !marked_condition;
						object->marked[1] = false;

						// Move to a deeper node
						current_position_stack.push(position);
						current_parent_stack.push(parent);

						parent = object->object_scope.childreen;
						position = parent->gc_begin();

						// Reset end position
						delete end_position;
						end_position = parent->gc_end();
					}
					else
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
        // Clean thread-to-remove stack. Prevent adding new threads.
		_GC_THREAD_LOCK

        // Change mark state.
		marked_condition = !marked_condition;

        // Clean thread-to-remove stack
		for (auto scope_info : scope_info_remove_stack)
		{
			scope_info_list.erase(scope_info->position);
			delete scope_info->root_scope;
			delete scope_info;
		}
		scope_info_remove_stack.clear();
	}

	void gcCollector::sweep() {

        // All marked to remove are free of being used. Not thread lock required

		for (auto position = heap.begin(); position != heap.end();)
		{
			auto object_pointer = *position;

			if (object_pointer->marked[0] != marked_condition)
			{
				if (object_pointer->marked[1] == true)
				{
					auto tmp = position;
					++position;
					heap.erase(tmp);
					delete object_pointer;
				}
				else {
					object_pointer->marked[1] = true;
					++position;
				}
			}
			else {
				++position;
			}			
		}
	}

    void gcCollector::collect() {

        // Simple and effective

		gcConnectThread _gc_val;

		auto start_time = std::chrono::steady_clock::now();
		auto step_time = std::chrono::milliseconds(_gc_collector->sleep_time);
		auto end_time = start_time + step_time;

		while (!_gc_collector->exit_flag) {

			std::this_thread::sleep_until(end_time);

			_gc_collector->mark();
			_gc_collector->sweep();

			start_time = std::chrono::steady_clock::now();
			end_time = start_time + step_time;
		}
	}
    gcCollector::gcCollector() : gcCollector(300) {
        // 300 ms is default collector cleaning step
    }

    gcCollector::gcCollector(int sleep_time) {

		_gc_collector = this;
		_gc_collector->marked_condition = true;
		_gc_collector->exit_flag = false;
        _gc_collector->sleep_time = sleep_time;    // default time step for each cleaning operation

        // default thread connection object
		connect_thread = new gcConnectThread;

        _gc_collector->thread_instance = std::thread(&gcCollector::collect);
	}

	gcCollector::~gcCollector() {		

		delete connect_thread;

		_gc_collector->exit_flag = true;
		_gc_collector->thread_instance.join();		
		free_heap();
	}

    // One instance only allowed
	gcCollector* _gc_collector;

    // Specific thread info
    thread_local gcScopeInfo* _gc_scope_info;
}

//end
