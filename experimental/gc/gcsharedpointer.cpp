#define _GC_HIDE_METHODS
#include "gcsharedpointer.h"
#include "gccontainer.h"

namespace gcNamespace {

gcSharedPointer_B_::gcSharedPointer_B_() {

    // Connect pointer to current scope
    // RAII method

    object = nullptr;
    snapshot = gc_sentinel;

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

    // EXPERIMENTAL     /////////////////////////////////////////////////////
    _GC_THREAD_LOCK;
    if (_gc_collector->is_marking) {
        gc_push_snapshot();
    }
    // EXPERIMENTAL     /////////////////////////////////////////////////////

    object = other.gc_get_object();
}

void gcSharedPointer_B_::gc_set_object(gcObject_B_*const obj) {

    // EXPERIMENTAL     /////////////////////////////////////////////////////
    _GC_THREAD_LOCK;
    if (_gc_collector->is_marking) {
        gc_push_snapshot();
    }
    // EXPERIMENTAL     /////////////////////////////////////////////////////

    if (obj == nullptr) {
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

    object = obj;
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

gcContainer_B_* gcSharedPointer_B_::gc_get_childreen() const {
    return object->gc_get_childreen();
}


const gcContainer_B_* gcSharedPointer_B_::gc_get_const_childreen() const {
    return object->gc_get_const_childreen();
}

bool gcSharedPointer_B_::gc_check_n_clear() const {

    _GC_THREAD_LOCK;

    if (object == nullptr)
        return true;

    if (object->gc_is_finalized()) {
        object = nullptr;
        return true;
    }
    return false;

}

// EXPERIMENTAL     /////////////////////////////////////////////////////

gcPointer_B_*  gcSharedPointer_B_::gc_pop_snapshot() const {

    if (snapshot == gc_sentinel) {
        return nullptr;
    }

    gcSharedPointer_B_* pointer_ = new gcSharedPointer_B_;

    pointer_->object = snapshot;
    pointer_->snapshot = snapshot;

    snapshot = gc_sentinel;
    return pointer_;

}

void gcSharedPointer_B_::gc_push_snapshot() const{
    snapshot = object;
}
// EXPERIMENTAL     /////////////////////////////////////////////////////

}

//end