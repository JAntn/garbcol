#define _GC_HIDE_METHODS
#include "gcsharedpointer.h"
#include "gccontainer.h"
#include <iostream>
namespace gcNamespace {

gcSharedPointer_B_::gcSharedPointer_B_() {

    // Connect pointer to current scope
    // RAII method

    object = nullptr;

    if(!_gc_scope_info->from_allocator) {
        static_cast<gcScopeContainer*>(_gc_scope_info->current_scope)->gc_push_back(this);
    }
}

gcSharedPointer_B_::~gcSharedPointer_B_() {

    // Disconnect pointer from scope
    // RAII method

    if(!_gc_scope_info->from_allocator) {
        static_cast<gcScopeContainer*>(_gc_scope_info->current_scope)->gc_pop_back();
    }
}

void gcSharedPointer_B_::gc_copy(const gcPointer_B_& other) {

    object = other.gc_get_object();
}

void gcSharedPointer_B_::gc_copy(gcPointer_B_&& other) {

    object = other.gc_get_object();
}

void gcSharedPointer_B_::gc_set_object(gcObject_B_*&& obj) {

    // Note:
    // `object` and `obj` are not checked to be the same object
    // If you use std::move(obj), be sure that obj is not pushed to heap yet

    object = obj;

    if (obj != nullptr) {

        // Since it is a new object, push it to heap
        _gc_collector->gc_heap_push(obj);
    }
}

void gcSharedPointer_B_::gc_set_object(gcObject_B_ *const& obj) {

    // Assigning a object created outside a pointer context?
    // Generating lvale simulation
    // Otherwise, assignation should be using gc_copy

    object = obj;

    if (obj != nullptr)
    {
        _GC_THREAD_LOCK;

        if (!obj->gc_is_lvalue())
        {
            obj->gc_make_lvalue();
            _gc_collector->gc_heap_push(obj);
        }
    }

}

gcObject_B_* gcSharedPointer_B_::gc_get_object() const {
    return object;
}

const gcObject_B_* gcSharedPointer_B_::gc_get_const_object() const {
    return object;
}

void gcSharedPointer_B_::gc_deallocate(){
    object->gc_deallocate();
}

void gcSharedPointer_B_::gc_make_nonfinalizable() const{
    object->gc_make_nonfinalizable();
}

void gcSharedPointer_B_::gc_make_finalizable() const{
    object->gc_make_finalizable();
}

bool gcSharedPointer_B_::gc_is_finalizable() const{
    return object->gc_is_finalizable();
}

bool gcSharedPointer_B_::gc_is_weak_pointer() const{
    return false;
}

void gcSharedPointer_B_::gc_mark() const{
    object->gc_mark();
}

bool gcSharedPointer_B_::gc_is_empty() const{
    return (object == nullptr);
}

bool gcSharedPointer_B_::gc_is_marked() const{
    return object->gc_is_marked();
}

const gcContainer_B_* gcSharedPointer_B_::gc_get_const_childreen() const {
    return object->gc_get_const_childreen();
}

bool gcSharedPointer_B_::gc_check_n_clear() const {

    if (object == nullptr)
        return true;

    if (object->gc_is_finalized()) {
        object = nullptr;
        return true;
    }

    return false;
}

}

//end
