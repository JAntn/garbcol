#define _GC_HIDE_METHODS
#include "gc.h"
#include "gcsnapshot.h"
namespace gcNamespace {

gcSnapshot::~gcSnapshot() {
    // nothing
}

void gcSnapshot::gc_copy(const gcPointer_B_& other) {
    object = other.gc_get_object();
}

void gcSnapshot::gc_set_object(gcObject_B_*const obj) {
    object = obj;
}

gcObject_B_* gcSnapshot::gc_get_object() const {
    return object;
}

const gcObject_B_* gcSnapshot::gc_get_const_object() const {
    return object;
}

void gcSnapshot::gc_deallocate(){
    object->gc_deallocate();
}

void gcSnapshot::gc_make_nonfinalizable() const{
    object->gc_make_nonfinalizable();
}

void gcSnapshot::gc_make_finalizable() const{
    object->gc_make_finalizable();
}

bool gcSnapshot::gc_is_finalizable() const{
    return object->gc_is_finalizable();
}

bool gcSnapshot::gc_is_weak_pointer() const{
    return false;
}

void gcSnapshot::gc_mark() const{
    object->gc_mark();
}

bool gcSnapshot::gc_is_empty() const{
    return (object == nullptr);
}

bool gcSnapshot::gc_is_marked() const{
    return object->gc_is_marked();
}

gcContainer_B_* gcSnapshot::gc_get_childreen() const {
    return object->gc_get_childreen();
}

const gcContainer_B_* gcSnapshot::gc_get_const_childreen() const {
    return object->gc_get_const_childreen();
}

bool gcSnapshot::gc_check_n_clear() const {

    if (object == nullptr) {
        return true;
    }

    return false;
}

gcPointer_B_*  gcSnapshot::gc_pop_snapshot() const {
    return nullptr;
}

void gcSnapshot::gc_push_snapshot() const{
    // nothing
}

}
