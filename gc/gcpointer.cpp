#define _GC_HIDE_METHODS
#include "gcpointer.h"
#include "gccontainer.h"

namespace gcNamespace {

gcPointerBase::gcPointerBase() {

    // Connect pointer to current scope
    // RAII method

    object = 0;

    if(!_gc_scope_info->from_allocator) {
        static_cast<gcDequeContainerInterface*>(_gc_scope_info->current_scope)->gc_push_back(this);
    }
}

gcPointerBase::gcPointerBase(gcObject*const obj) {

    gc_set_object(obj);

    if(!_gc_scope_info->from_allocator) {
        static_cast<gcDequeContainerInterface*>(_gc_scope_info->current_scope)->gc_push_back(this);
    }
}

gcPointerBase::gcPointerBase(const gcPointerBase& other) {

    gc_copy(other);

    if(!_gc_scope_info->from_allocator) {
        static_cast<gcDequeContainerInterface*>(_gc_scope_info->current_scope)->gc_push_back(this);
    }
}

gcPointerBase::~gcPointerBase() {

    // Disconnect pointer from scope
    // RAII method

    if(!_gc_scope_info->from_allocator) {
        static_cast<gcDequeContainerInterface*>(_gc_scope_info->current_scope)->gc_pop_back();
    }
}

void gcPointerBase::gc_copy(const gcPointerBase& other) {
    object = other.object;
}

void gcPointerBase::gc_set_object(gcObject*const obj) {

    if(obj == 0) {
        object = obj;
        return;
    }

    // rvalue simulation
    if(obj->mark & _gc_rvalue_bit) {

        _GC_THREAD_LOCK;
        obj->mark &= ~_gc_rvalue_bit;               // set rvalue bit to zero
        obj->mark |= _gc_collector->mark_bit;       // set bit to marked state
        _gc_collector->heap.push_back(obj);
        object = obj;
        return;
    }

    object = obj;
}

void gcPointerBase::gc_delete_object(){
    object->mark |= _gc_force_remove_bit;
}

void gcPointerBase::gc_make_persistent(){
    object->mark |= _gc_persistent_bit;
}

void gcPointerBase::gc_make_collectable(){
    object->mark &= ~_gc_persistent_bit;
}
}

//end
