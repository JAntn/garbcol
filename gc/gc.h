///////////////////////////////////////////////////////////////////////////////
// File: gc.h
// Description:
// Smart pointers , Garbage collector Mark & Sweep , Threads support, and STL Container Wrappers

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

#define _GC_THREAD_LOCK std::lock_guard<std::mutex> _gc_lock_guard(_gc_collector->gc_mutex);

namespace gcNamespace {

// Argument to delegate to an empty constructor
enum gc_delegate_t {gc_delegate=1};

// Flags for marking algorithms
const unsigned char _gc_lvalue_bit                  = 0x01; // object not stored in a pointer yet
const unsigned char _gc_mark_bit                    = 0x02; // gc marks as connected here
const unsigned char _gc_unreachable_bit             = 0x04; // it was detected as not connected in sweep but will wait to next sweep to remove (just in case)
const unsigned char _gc_nonfinalizable_bit          = 0x08; // not to delete by garbage mark&sweep algorithm
const unsigned char _gc_deallocate_bit              = 0x10; // force to deallocate
const unsigned char _gc_deallocate_is_safe_bit      = 0x20; // deallocate step is safe

// Forward declaration
class gcContainer_B_;
class gcScopeContainer;
class gcObject_B_;
class gcScopeInfo;

///////////////////////////////////////////////////////////////////////////////
// scope information of current thread

struct gcScopeInfo{

    // Position of this object in overall list
    std::list<gcScopeInfo*>::iterator   position;

    // Stack to backup previous scopes
    std::deque<gcScopeContainer*>       current_scope_stack;

    gcScopeContainer*                   current_scope;
    gcScopeContainer*                   root_scope;

    // Flag to indicate to a smart pointer constructor that
    // object is allocated from a STL container
    bool                                from_allocator;
};

///////////////////////////////////////////////////////////////////////////////
// configure a thread

class gcConnectThread {

    gcScopeInfo*                        scope_info;

public:

    gcConnectThread();
    ~gcConnectThread();
};

///////////////////////////////////////////////////////////////////////////////
// garbage collector main object

class gcCollector {

    friend class gcConnectThread;

    // Connect main() thread
    gcConnectThread*                    connect_thread;

    // At GC cleaning of a thread, store info for correct removing
    std::list<gcScopeInfo*>             remove_scope_info_stack;

    // All objects are referenced by heap
    std::forward_list<gcObject_B_*>     heap;

    // Scope info list
    std::list<gcScopeInfo*>             scope_info_list;

public:

    // Marking checks this. Changes at each pass
    unsigned char                       gc_mark_bit_value;

    // Stops GC if true
    bool                                gc_exit_flag;

    // GC thread
    std::thread                         gc_thread;

    // GC mutex
    std::mutex                          gc_mutex;

    // Minimum time between each cleanning pass
    int                                 gc_sleep_time;

    // Cts. Dts.
    gcCollector();
    gcCollector(int);
    ~gcCollector();

    // Methods
    void                                gc_heap_push(gcObject_B_*const);
    void                                gc_free_heap();
    void                                gc_mark();
    void                                gc_sweep();
    static void                         gc_collect();
};

// Global objects
extern gcCollector*                     _gc_collector;
extern thread_local gcScopeInfo*        _gc_scope_info;

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
