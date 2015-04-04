// File: gcobject.h
// Description:
// pointer contents class

#ifndef _GC_OBJECTADAPTER_H
#define _GC_OBJECTADAPTER_H

#include "gc.h"
#include "gcobjectbase.h"

namespace gcNamespace {

// forward declare
template<class, class, bool> class gcPointer;

// Adapter to use non-gcObject derived classes with pointers
template<class _Type>
class gcObjectAdapter : public gcObject_B_{

    template<class _TypePP, class _PointerBasePP, bool _TypeIsObjectPP>
    friend class gcPointer;

    // Used in mark sweep algorithm
    unsigned char                       mark;

protected:

    // Adaptee object
    _Type*                              adaptee;

public:

    gcObjectAdapter();
    gcObjectAdapter(_Type*const);
    ~gcObjectAdapter() override;

    void                                gc_mark() override;
    bool                                gc_is_marked() const override;

    void                                gc_make_unreachable() override;
    void                                gc_make_reachable() override;
    bool                                gc_is_reachable() const override;

    const gcContainer_B_*               gc_get_const_childreen() const override;

    void                                gc_make_nonfinalizable() override;
    void                                gc_make_finalizable() override;
    bool                                gc_is_finalizable() const override;

    void                                gc_make_lvalue() override;
    bool                                gc_is_lvalue() const override;

    void                                gc_deallocate() override;
    bool                                gc_is_finalized() const override;
};


#ifndef _GC_HIDE_METHODS

// methods

template<class _Type>
gcObjectAdapter<_Type>::gcObjectAdapter() {
    mark = 0;
    adaptee = 0;
}

template<class _Type>
gcObjectAdapter<_Type>::gcObjectAdapter(_Type*const other) {
    mark = 0;
    adaptee = other;
}

template<class _Type>
gcObjectAdapter<_Type>::~gcObjectAdapter() {
    delete adaptee;
}

template<class _Type>
void gcObjectAdapter<_Type>::gc_mark() {
    mark &= ~_gc_mark_bit;                              // clear bit
    mark |= _gc_collector->mark_bit;     // set bit
}

template<class _Type>
bool gcObjectAdapter<_Type>::gc_is_marked() const {
    return (mark & _gc_mark_bit) == (_gc_collector->mark_bit & _gc_mark_bit);
}

template<class _Type>
void gcObjectAdapter<_Type>::gc_make_reachable() {
    mark &= ~_gc_unreachable_bit;
}

template<class _Type>
void gcObjectAdapter<_Type>::gc_make_unreachable() {
    mark |= _gc_unreachable_bit;
}

template<class _Type>
bool gcObjectAdapter<_Type>::gc_is_reachable() const {
    return !(mark & _gc_unreachable_bit);
}


template<class _Type>
const gcContainer_B_* gcObjectAdapter<_Type>::gc_get_const_childreen() const {
    return 0;
}

template<class _Type>
void gcObjectAdapter<_Type>::gc_make_finalizable() {
    mark &= ~_gc_nonfinalizable_bit;
}

template<class _Type>
void gcObjectAdapter<_Type>::gc_make_nonfinalizable() {
    mark |= _gc_nonfinalizable_bit;
}

template<class _Type>
bool gcObjectAdapter<_Type>::gc_is_finalizable() const {
    return !(mark & _gc_nonfinalizable_bit);
}

template<class _Type>
void gcObjectAdapter<_Type>::gc_make_lvalue() {
    mark |= _gc_lvalue_bit;
}

template<class _Type>
bool gcObjectAdapter<_Type>::gc_is_lvalue() const {
    return mark & _gc_lvalue_bit;
}

template<class _Type>
void gcObjectAdapter<_Type>::gc_deallocate(){
    mark |= _gc_force_deallocate_bit;
}

template<class _Type>
bool gcObjectAdapter<_Type>::gc_is_finalized() const {
    return mark & _gc_force_deallocate_bit;
}

#endif

}

#endif // _GC_OBJECTADAPTER_H

