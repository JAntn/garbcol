#define _GC_HIDE_METHODS
#include "gcuniquepointer.h"
#include "gccontainer.h"

namespace gcNamespace {

gcUniquePointer_B_::gcUniquePointer_B_() {

    // Connect pointer to current scope
    // RAII method

    object = 0;

    if(!_gc_scope_info->from_allocator) {
        static_cast<gcScopeContainer*>(_gc_scope_info->current_scope)->gc_push_back(this);
    }
}

gcUniquePointer_B_::~gcUniquePointer_B_() {

    // Disconnect pointer from scope
    // RAII method

    if(!_gc_scope_info->from_allocator) {
        static_cast<gcScopeContainer*>(_gc_scope_info->current_scope)->gc_pop_back();
    }
}

// for moving operations
void gcUniquePointer_B_::gc_copy(const gcPointer_B_& other) {
    object = other.gc_get_object();
}

void gcUniquePointer_B_::gc_set_object(gcObject_B_*const obj) {

    if(obj == 0) {
        object = obj;
        return;
    }

    // rvalue simulation
    if(!(obj->gc_is_lvalue())) {
        obj->gc_make_lvalue();
        obj->gc_mark();
        _gc_collector->heap.push_back(obj);
        object = obj;
        return;
    }

    // forbidden but doesn't raise an error
    //object = obj;
}

gcObject_B_* gcUniquePointer_B_::gc_get_object() const {
    return object;
}

const gcObject_B_* gcUniquePointer_B_::gc_get_const_object() const {
    return object;
}

void gcUniquePointer_B_::gc_deallocate(){
    object->gc_deallocate();
}

void gcUniquePointer_B_::gc_make_nonfinalizable() const{
    object->gc_make_nonfinalizable();
}

void gcUniquePointer_B_::gc_make_finalizable() const{
    object->gc_make_finalizable();
}

bool gcUniquePointer_B_::gc_is_finalizable() const{
    return object->gc_is_finalizable();
}

bool gcUniquePointer_B_::gc_is_weak_pointer() const{
    return false;
}

void gcUniquePointer_B_::gc_mark() const{
    object->gc_mark();
}

bool gcUniquePointer_B_::gc_is_empty() const{
    return (object == 0);
}

bool gcUniquePointer_B_::gc_is_marked() const{
    return object->gc_is_marked();
}

const gcContainer_B_* gcUniquePointer_B_::gc_get_const_childreen() const {
    return object->gc_get_const_childreen();
}

bool gcUniquePointer_B_::gc_check_n_clear() const {

    if (object==0)
        return true;

    if (object->gc_is_finalized()) {
        object = 0;
        return true;
    }

    return false;
}

}