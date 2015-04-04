#define _GC_HIDE_METHODS
#include "gcweakpointer.h"

namespace gcNamespace{

gcWeakPointer_B_::gcWeakPointer_B_() {
    object = 0;
}

gcWeakPointer_B_::gcWeakPointer_B_(gcObject_B_*const obj) {
    gc_set_object(obj);
}

gcWeakPointer_B_::gcWeakPointer_B_(const gcPointer_B_& other) {
    gc_copy(other);
}

gcWeakPointer_B_::~gcWeakPointer_B_() {
    // Nothing
}

void gcWeakPointer_B_::gc_copy(const gcPointer_B_& other) {
    object = other.gc_get_object();
}

void gcWeakPointer_B_::gc_set_object(gcObject_B_*const obj) {
    object = obj;
}

gcObject_B_* gcWeakPointer_B_::gc_get_object() const {
    return object;
}

const gcObject_B_* gcWeakPointer_B_::gc_get_const_object() const {
    return object;
}

void gcWeakPointer_B_::gc_deallocate(){
    object->gc_deallocate();
}

void gcWeakPointer_B_::gc_make_nonfinalizable() const{
    object->gc_make_nonfinalizable();
}

void gcWeakPointer_B_::gc_make_finalizable() const{
    object->gc_make_finalizable();
}

bool gcWeakPointer_B_::gc_is_finalizable() const{
    return object->gc_is_finalizable();
}

bool gcWeakPointer_B_::gc_is_weak_pointer() const{
    return true;
}

void gcWeakPointer_B_::gc_mark() const{
    object->gc_mark();
}

bool gcWeakPointer_B_::gc_is_empty() const{
    return (object == 0);
}

bool gcWeakPointer_B_::gc_is_marked() const{
    return object->gc_is_marked();
}

const gcContainer_B_* gcWeakPointer_B_::gc_get_const_childreen() const{
    return object->gc_get_const_childreen();
}

}

