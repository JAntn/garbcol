// File: gcsequence.h
// Description:
// base class for sequence kind containers

#ifndef _GC_SEQUENCE_H
#define _GC_SEQUENCE_H

#include <utility>
#include <functional>

#include "gc.h"

namespace gcNamespace {


// GC internal pointer iterator adapter

template<class _Iterator>
class gcSequenceIteratorAdapter : public gcIterator_B_ {

    _Iterator                           adaptee;

public:


    gcIterator_B_*                      gc_next() override;
    const gcPointer_B_*                 gc_get_const_pointer() const override;
    bool                                gc_is_equal(const gcIterator_B_* other) const override;

    ~gcSequenceIteratorAdapter() override;
    gcSequenceIteratorAdapter(const _Iterator&);
};


#ifndef _GC_HIDE_METHODS

template<class _Iterator>
gcSequenceIteratorAdapter<_Iterator>::gcSequenceIteratorAdapter(const _Iterator& other){
    adaptee = other;
}

template<class _Iterator>
gcSequenceIteratorAdapter<_Iterator>::~gcSequenceIteratorAdapter(){
    //nothing
}

template<class _Iterator>
gcIterator_B_* gcSequenceIteratorAdapter<_Iterator>::gc_next() {
    ++adaptee;
    return this;
}

template<class _Iterator>
const gcPointer_B_* gcSequenceIteratorAdapter<_Iterator>::gc_get_const_pointer() const {
    return &(*adaptee);
}

template<class _Iterator>
bool gcSequenceIteratorAdapter<_Iterator>::gc_is_equal(const gcIterator_B_* other) const {
    return ((static_cast<const gcSequenceIteratorAdapter<_Iterator>*>(other)->adaptee) == adaptee);
}

#endif

// GC internal pointer container adapter (used for list, deque, vector)
#define _GC_TEMPLATE                    template < template<typename,typename> class  _Container, class _Type, class _ItemPointerBase>
#define _GC_ADAPTEE                     _Container<gcPointer<_Type,_ItemPointerBase>, gcContainerAllocator<gcPointer<_Type,_ItemPointerBase>>>
#define _GC_ITERATOR                    typename _GC_ADAPTEE::iterator
#define _GC_CONST_ITERATOR              typename _GC_ADAPTEE::const_iterator
#define _GC_SELF                        gcSequenceAdapter<_Container, _Type, _ItemPointerBase>

_GC_TEMPLATE
class gcSequenceAdapter :  public gcObjectAdapter<_GC_ADAPTEE>, public gcContainer_B_ {

    using gcObjectAdapter<_GC_ADAPTEE>::adaptee;

    // Any pointer
    template<class _TypePP, class _PointerBasePP, bool _TypeIsObjectPP>
    friend class gcPointer;

public:

    // !!Just most basic implementation of derived types yet
    typedef _GC_ADAPTEE container;
    typedef _GC_ITERATOR iterator;
    typedef _GC_CONST_ITERATOR const_iterator;

    template<class ..._Args>            gcSequenceAdapter(_Args...);
    ~gcSequenceAdapter() override;

    gcIterator_B_*                      gc_begin() override;
    gcIterator_B_*                      gc_begin() const override;

    gcIterator_B_*                      gc_end() override;
    gcIterator_B_*                      gc_end() const override;

    gcPointer<_Type,_ItemPointerBase>&
    operator[](int);

    const gcPointer<_Type,_ItemPointerBase>&
    operator[](int) const;

    bool                                operator== (const _GC_SELF& other) const;
    bool                                operator!= (const _GC_SELF& other) const;
    bool                                operator> (const _GC_SELF& other) const;
    bool                                operator< (const _GC_SELF& other) const;
    bool                                operator>= (const _GC_SELF& other) const;
    bool                                operator<= (const _GC_SELF& other) const;

    const gcContainer_B_*               gc_get_const_childreen() const override;

};

#ifndef _GC_HIDE_METHODS

_GC_TEMPLATE gcIterator_B_* _GC_SELF::gc_begin() {
    return new gcSequenceIteratorAdapter<_GC_ITERATOR>(adaptee->begin());
}

_GC_TEMPLATE gcIterator_B_* _GC_SELF::gc_begin() const {
    return new gcSequenceIteratorAdapter<_GC_CONST_ITERATOR>(adaptee->begin());
}

_GC_TEMPLATE gcIterator_B_* _GC_SELF::gc_end() {
    return new gcSequenceIteratorAdapter<_GC_ITERATOR>(adaptee->end());
}

_GC_TEMPLATE gcIterator_B_* _GC_SELF::gc_end() const {
    return new gcSequenceIteratorAdapter<_GC_CONST_ITERATOR>(adaptee->end());
}

_GC_TEMPLATE _GC_SELF::~gcSequenceAdapter() {

    // push state
    bool tmp = _gc_scope_info->from_allocator;

    _gc_scope_info->from_allocator = true;
    delete adaptee;
    adaptee = 0;

    // restore state
    _gc_scope_info->from_allocator = tmp;
}

_GC_TEMPLATE template<class ..._Args> _GC_SELF::gcSequenceAdapter(_Args... args) : gcObjectAdapter<_GC_ADAPTEE>() {

    bool tmp = _gc_scope_info->from_allocator;
    _gc_scope_info->from_allocator = true;

    adaptee = new _GC_ADAPTEE(args...);

    _gc_scope_info->from_allocator = tmp;
}

_GC_TEMPLATE gcPointer<_Type,_ItemPointerBase>& _GC_SELF::operator[](int i) {
    return (*adaptee)[i];
}

_GC_TEMPLATE const gcPointer<_Type,_ItemPointerBase>& _GC_SELF::operator[](int i) const {
    return (*adaptee)[i];
}

_GC_TEMPLATE bool _GC_SELF::operator == (const _GC_SELF& other) const {
    return (*adaptee) == *(other.adaptee);
}

_GC_TEMPLATE bool _GC_SELF::operator != (const _GC_SELF& other) const {
    return (*adaptee) != *(other.adaptee);
}

_GC_TEMPLATE bool _GC_SELF::operator > (const _GC_SELF& other) const {
    return (*adaptee) > *(other.adaptee);
}

_GC_TEMPLATE bool _GC_SELF::operator < (const _GC_SELF& other) const {
    return (*adaptee) < *(other.adaptee);
}

_GC_TEMPLATE bool _GC_SELF::operator >= (const _GC_SELF& other) const {
    return (*adaptee) >= *(other.adaptee);
}

_GC_TEMPLATE bool _GC_SELF::operator <= (const _GC_SELF& other) const {
    return (*adaptee) <= *(other.adaptee);
}

_GC_TEMPLATE const gcContainer_B_* _GC_SELF::gc_get_const_childreen() const {
    return this;
}

#endif
#undef _GC_TEMPLATE
#undef _GC_SELF
#undef _GC_ADAPTEE
#undef _GC_ITERATOR
#undef _GC_CONST_ITERATOR

// Class gcPointer specialization for gcContainerAdapter class parameter

#define _GC_TEMPLATE                    template< template<typename,typename> class _Container, class _Type, class _PointerBase, class _ItemPointerBase>
#define	_GC_CONTAINER                   gcSequenceAdapter<_Container, _Type, _ItemPointerBase>
#define _GC_SELF                        gcPointer<_GC_CONTAINER, _PointerBase, true>
#define _GC_ADAPTEE                     _Container<gcPointer<_Type,_ItemPointerBase>, gcContainerAllocator<gcPointer<_Type,_ItemPointerBase>>>
#define _GC_CONTAINER_M_                static_cast<_GC_CONTAINER*>(gc_get_object())
#define _GC_CONST_CONTAINER_M_          static_cast<_GC_CONTAINER*>(gc_get_const_object())

_GC_TEMPLATE
class gcPointer< _GC_CONTAINER, _PointerBase, true>
        : public _PointerBase{
public:

    using _PointerBase::gc_is_empty;
    using _PointerBase::gc_copy;
    using _PointerBase::gc_set_object;
    using _PointerBase::gc_get_object;
    using _PointerBase::gc_get_const_object;
    using _PointerBase::gc_mark;
    using _PointerBase::gc_is_marked;
    using _PointerBase::gc_get_const_childreen;
    using _PointerBase::gc_make_nonfinalizable;
    using _PointerBase::gc_make_finalizable;
    using _PointerBase::gc_is_finalizable;
    using _PointerBase::gc_is_weak_pointer;
    using _PointerBase::gc_deallocate;
    using _PointerBase::gc_check_n_clear;
    using _PointerBase::operator==;
    using _PointerBase::operator!=;
    using _PointerBase::operator<;
    using _PointerBase::operator>;
    using _PointerBase::operator<=;
    using _PointerBase::operator>=;

    // !!Just most basic implementation of derived types yet
    typedef _GC_ADAPTEE container;
    typedef typename _GC_ADAPTEE::iterator iterator;
    typedef typename _GC_ADAPTEE::const_iterator const_iterator;

    gcPointer();
    gcPointer(_GC_CONTAINER*const other);
    gcPointer(const _GC_SELF& other);

    _GC_SELF&                           operator= (_GC_CONTAINER*const other);
    _GC_SELF&                           operator= (const _GC_SELF& other);

    _GC_ADAPTEE&                        operator*();
    _GC_ADAPTEE*                        operator->();

    gcPointer<_Type,_ItemPointerBase>&
    operator[] (int);

    const _GC_ADAPTEE&                  operator*() const;
    const _GC_ADAPTEE*                  operator->() const;

    const gcPointer<_Type,_ItemPointerBase>&
    operator[] (int) const;

    template<class _OtherType, class _OtherPointerBase>
    gcPointer<_OtherType, _OtherPointerBase, true>
    gc_to(typename std::enable_if<std::is_convertible<_Type*,_OtherType*>::value,gcObject_B_>::type*) const;

    template<class _Other, class _OtherPointerBase>
    operator gcPointer<_Other,_OtherPointerBase>();

    template<class _Other, class _OtherPointerBase>
    operator const gcPointer<_Other,_OtherPointerBase>() const;

};

#ifndef _GC_HIDE_METHODS

// methods

_GC_TEMPLATE _GC_SELF::gcPointer() : _PointerBase() {}

_GC_TEMPLATE _GC_SELF::gcPointer(_GC_CONTAINER*const other) : _PointerBase() {
    gc_set_object(other);
}

_GC_TEMPLATE _GC_SELF::gcPointer(const _GC_SELF& other) : _PointerBase() {
    gc_copy(other);
}

_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (_GC_CONTAINER*const other) {
    gc_set_object(other);
    return *this;
}

_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (const _GC_SELF& other) {
    gc_copy(other);
    return *this;
}

_GC_TEMPLATE _GC_ADAPTEE& _GC_SELF::operator*() {
    return *(_GC_CONTAINER_M_->adaptee);
}

_GC_TEMPLATE _GC_ADAPTEE* _GC_SELF::operator->() {
    return _GC_CONTAINER_M_->adaptee;
}

_GC_TEMPLATE gcPointer<_Type,_ItemPointerBase>& _GC_SELF::operator[](int i) {
    return (*_GC_CONTAINER_M_)[i];
}

_GC_TEMPLATE const _GC_ADAPTEE& _GC_SELF::operator*() const {
    return *(_GC_CONST_CONTAINER_M_->adaptee);
}

_GC_TEMPLATE const _GC_ADAPTEE* _GC_SELF::operator->() const {
    return _GC_CONST_CONTAINER_M_->adaptee;
}

_GC_TEMPLATE const gcPointer<_Type,_ItemPointerBase>& _GC_SELF::operator[](int i) const {
    return (*(_GC_CONST_CONTAINER_M_->adaptee))[i];
}

_GC_TEMPLATE
template<class _OtherType, class _OtherPointerBase>
gcPointer<_OtherType, _OtherPointerBase, true>
_GC_SELF::gc_to(typename std::enable_if<std::is_convertible<_Type*,_OtherType*>::value,gcObject_B_>::type* obj) const {
    return gcPointer<_OtherType, _OtherPointerBase, true>(static_cast<_OtherType*>(obj));
}

_GC_TEMPLATE
template<class _Other, class _OtherPointerBase> _GC_SELF::operator gcPointer<_Other, _OtherPointerBase>(){
    return gc_to<_Other, _OtherPointerBase>(gc_get_object());
}

_GC_TEMPLATE
template<class _Other, class _OtherPointerBase> _GC_SELF::operator const gcPointer<_Other, _OtherPointerBase>() const{
    return gc_to<_Other, _OtherPointerBase>(gc_get_object());
}

#endif

#undef _GC_TEMPLATE
#undef _GC_CONTAINER
#undef _GC_SELF
#undef _GC_ADAPTEE
#undef _GC_CONTAINER_M_
#undef _GC_CONST_CONTAINER_M_

}
#endif
