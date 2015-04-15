// File: gc.h
// Description:
// Smart pointers with garbage collector of mark and sweep type, thread support and stl containers wrapper

#ifndef _GC_COLLECTOR_H
#define _GC_COLLECTOR_H

#include <chrono>
#include <list>
#include <forward_list>
#include <deque>
#include <thread>
#include <mutex>
#include <type_traits>
#include <condition_variable>

#define _GC_THREAD_LOCK std::lock_guard<std::mutex> _gc_lock_guard(_gc_collector->mutex_instance);
#define _GC_THREAD_WAIT_MARKING _gc_wait_marking();
#define _GC_THREAD_WAIT_SWEEPING _gc_wait_sweeping();

namespace gcNamespace {

enum gc_delegate_t {gc_delegate=1};

const unsigned char _gc_lvalue_bit                  = 0x01; // object not stored in a pointer yet
const unsigned char _gc_mark_bit                    = 0x02; // gc marks as connected here
const unsigned char _gc_unreachable_bit             = 0x04; // it was detected as not connected in sweep but will wait to next sweep to remove (just in case)
const unsigned char _gc_nonfinalizable_bit          = 0x08; // not to delete by garbage mark&sweep algorithm
const unsigned char _gc_deallocate_bit              = 0x10; // force to deallocate
const unsigned char _gc_deallocate_is_safe_bit      = 0x20; // deallocate step is safe

class gcContainer_B_;
class gcScopeContainer;
class gcObject_B_;
class gcScopeInfo;

// Specific scope information at the current thread
class gcScopeInfo{
public:

    // Position in global instance table
    std::list<gcScopeInfo*>::iterator   position;

    // Rock stack of scopes
    std::deque<gcScopeContainer*>       current_scope_stack;

    gcScopeContainer*                   root_scope;
    gcScopeContainer*                   current_scope;
    bool                                from_allocator;
};

// Configure a thread
class gcConnectThread {
public:

    gcScopeInfo*                        scope_info;

    gcConnectThread();
    ~gcConnectThread();
};

// Garbage Collector main object
class gcCollector {
public:

    // All objects are referenced in a heap
    std::forward_list<gcObject_B_*>     heap;

    // Scope information table
    std::list<gcScopeInfo*>             scope_info_list;

    // This condition is checked in mark algorithm. It changes at each mrk-swp pass
    unsigned char                       mark_bit;
    bool                                exit_flag;

    // GC has its own instance
    std::thread                         thread_instance;

    // Process mutex tool
    std::mutex                          mutex_instance;

    // At GC cleaning of a thread, it stores info for correct removing
    std::list<gcScopeInfo*>             remove_scope_info_stack;

    // collector autoconnects to main thread
    gcConnectThread*                    connect_thread;

    // Minimum time between each mark-sweep event
    int                                 sleep_time;

    // Wait flag and thread support
    bool                                is_marking;
    std::condition_variable             is_marking_cv;

    bool                                is_sweeping;
    std::condition_variable             is_sweeping_cv;

    // Cts. Dts.

    gcCollector();
    gcCollector(int);
    ~gcCollector();

    // Methods
    void                                gc_free_heap();
    void                                gc_mark();
    void                                gc_sweep();
    static void                         gc_collect();
};

// Global objects

extern gcCollector*                     _gc_collector;
extern thread_local gcScopeInfo*        _gc_scope_info;

// Waits until marking finishes
void _gc_wait_marking();
void _gc_wait_sweeping();

}

#include "gcobjectbase.h"
#include "gcsentinel.h"
#include "gccontainer.h"
#include "gcobject.h"
#include "gcobjectadapter.h"
#include "gcpointerbase.h"
#include "gcweakpointer.h"
#include "gcsharedpointer.h"
#include "gcscopedpointer.h"
#include "gcpointer.h"
#include "gcuniquepointer.h"
#include "gcmacro.h"

#endif // _GC_COLLECTOR_H
