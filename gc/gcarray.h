///////////////////////////////////////////////////////////////////////////////
// File: gcarray.h
// Description:
// STL array wrapper

#ifndef _GC_ARRAY_H
#define _GC_ARRAY_H

#include <array>
#include "gc.h"

namespace gcNamespace {

///////////////////////////////////////////////////////////////////////////////
// STL iterator wrapper
template<class _Iterator>
class gcArrayIteratorAdapter : public gcIterator_B_ {

    _Iterator                           adaptee;

public:

    ~gcArrayIteratorAdapter() override;
    gcArrayIteratorAdapter(const _Iterator&);

    gcIterator_B_*                      gc_next() override;
    const gcPointer_B_*                 gc_get_const_pointer() const override;
    bool                                gc_is_equal(const gcIterator_B_* other) const override;
};

#ifndef _GC_HIDE_METHODS

template<class _Iterator>
gcArrayIteratorAdapter<_Iterator>::gcArrayIteratorAdapter(const _Iterator& other){
    adaptee = other;
}

template<class _Iterator>
gcArrayIteratorAdapter<_Iterator>::~gcArrayIteratorAdapter(){
    //nothing
}

template<class _Iterator>
gcIterator_B_* gcArrayIteratorAdapter<_Iterator>::gc_next() {
    ++adaptee;
    return this;
}

template<class _Iterator>
const gcPointer_B_*  gcArrayIteratorAdapter<_Iterator>::gc_get_const_pointer() const {
    return &(*adaptee);
}

template<class _Iterator>
bool gcArrayIteratorAdapter<_Iterator>::gc_is_equal(const gcIterator_B_* other)  const{
    return ((static_cast<const gcArrayIteratorAdapter<_Iterator>*>(other)->adaptee) == adaptee);
}

#endif

///////////////////////////////////////////////////////////////////////////////
// STL array wrapper

#define _GC_TEMPLATE                    template < class _Type, std::size_t _N, class _ItemPointerBase>
#define _GC_ADAPTEE                     std::array<gcPointer<_Type, _ItemPointerBase>, _N>
#define _GC_ITERATOR                    typename _GC_ADAPTEE::iterator
#define _GC_CONST_ITERATOR              typename _GC_ADAPTEE::const_iterator
#define _GC_SELF                        gcArray<_Type, _N, _ItemPointerBase>

template < class _Type, std::size_t _N, class _ItemPointerBase = gcSharedPointer_B_>
class gcArray :  public gcObjectAdapter<_GC_ADAPTEE>, public gcContainer_B_ {

    using  gcObjectAdapter<_GC_ADAPTEE>::adaptee;

    template<class _TypePP, class _PointerBasePP, bool _TypeIsObjectPP>
    friend class gcPointer;

public:

    typedef _GC_ADAPTEE container;
    typedef _GC_ITERATOR iterator;
    typedef _GC_CONST_ITERATOR const_iterator;

    template <class ..._Args>  gcArray(_Args...);

    ~gcArray() override;

    gcIterator_B_*                      gc_begin() override;
    gcIterator_B_*                      gc_begin() const override;

    gcIterator_B_*                      gc_end() override;
    gcIterator_B_*                      gc_end() const override;

    const gcContainer_B_*               gc_get_const_childreen() const override;

    bool                                operator== (const _GC_SELF& other) const;
    bool                                operator!= (const _GC_SELF& other) const;
    bool                                operator> (const _GC_SELF& other) const;
    bool                                operator< (const _GC_SELF& other) const;
    bool                                operator>= (const _GC_SELF& other) const;
    bool                                operator<= (const _GC_SELF& other) const;

    gcPointer<_Type,_ItemPointerBase>&
    operator[](int);

    const gcPointer<_Type,_ItemPointerBase>&
    operator[](int) const;
};

#ifndef _GC_HIDE_METHODS

_GC_TEMPLATE gcIterator_B_* _GC_SELF::gc_begin() {
    return new gcArrayIteratorAdapter<_GC_ITERATOR>(adaptee->begin());
}

_GC_TEMPLATE gcIterator_B_* _GC_SELF::gc_begin() const {
    return new gcArrayIteratorAdapter<_GC_CONST_ITERATOR>(adaptee->begin());
}

_GC_TEMPLATE gcIterator_B_* _GC_SELF::gc_end() {
    return new gcArrayIteratorAdapter<_GC_ITERATOR>(adaptee->end());
}

_GC_TEMPLATE gcIterator_B_* _GC_SELF::gc_end() const {
    return new gcArrayIteratorAdapter<_GC_CONST_ITERATOR>(adaptee->end());
}

_GC_TEMPLATE _GC_SELF::~gcArray() {

    bool tmp = _gc_scope_info->from_allocator;

    _gc_scope_info->from_allocator = true;
    delete adaptee;
    adaptee = 0;

    _gc_scope_info->from_allocator = tmp;
}


_GC_TEMPLATE
template <class ..._Args>
        _GC_SELF::gcArray( _Args ...args) : gcObjectAdapter<_GC_ADAPTEE>() {

    bool tmp = _gc_scope_info->from_allocator;
    _gc_scope_info->from_allocator = true;

    adaptee = new _GC_ADAPTEE(args...); //still not brace init {}. It is less flexible

    _gc_scope_info->from_allocator = tmp;
}

_GC_TEMPLATE gcPointer<_Type,_ItemPointerBase>& _GC_SELF::operator[](int i) {
    return (*adaptee)[i];
}

_GC_TEMPLATE const gcPointer<_Type,_ItemPointerBase>& _GC_SELF::operator[](int i) const {
    return (*adaptee)[i];
}

_GC_TEMPLATE bool _GC_SELF::operator == (const _GC_SELF& other) const {
    return *adaptee == *(other.adaptee);
}

_GC_TEMPLATE bool _GC_SELF::operator != (const _GC_SELF& other) const {
    return *adaptee != *(other.adaptee);
}

_GC_TEMPLATE bool _GC_SELF::operator > (const _GC_SELF& other) const {
    return *adaptee > *(other.adaptee);
}

_GC_TEMPLATE bool _GC_SELF::operator < (const _GC_SELF& other) const {
    return *adaptee < *(other.adaptee);
}

_GC_TEMPLATE bool _GC_SELF::operator >= (const _GC_SELF& other) const {
    return *adaptee >= *(other.adaptee);
}

_GC_TEMPLATE bool _GC_SELF::operator <= (const _GC_SELF& other) const {
    return *adaptee <= *(other.adaptee);
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

///////////////////////////////////////////////////////////////////////////////
// gcPointer specialization

#define _GC_TEMPLATE                    template < class _Type, std::size_t _N, class _PointerBase, class _ItemPointerBase>
#define _GC_ADAPTEE                     std::array<gcPointer<_Type, _ItemPointerBase>, _N>
#define	_GC_CONTAINER                   gcArray<_Type, _N, _ItemPointerBase>
#define _GC_SELF                        gcPointer<_GC_CONTAINER, _PointerBase, true>
#define _GC_CONTAINER_M_                static_cast<_GC_CONTAINER*>(gc_get_object())
#define _GC_CONST_CONTAINER_M_          static_cast<_GC_CONTAINER*>(gc_get_const_object())

_GC_TEMPLATE
class gcPointer< _GC_CONTAINER, _PointerBase, true>
        : public _PointerBase {
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

    typedef _GC_ADAPTEE container;
    typedef typename _GC_ADAPTEE::iterator iterator;
    typedef typename _GC_ADAPTEE::const_iterator const_iterator;

    gcPointer();
    gcPointer(_GC_CONTAINER*const other);
    gcPointer(const _GC_SELF& other);

    _GC_SELF&                           operator= (_GC_CONTAINER*const other);
    _GC_SELF&                           operator= (const _GC_SELF& other);

    _GC_ADAPTEE&                        operator*();
    const _GC_ADAPTEE&                  operator*() const;

    _GC_ADAPTEE*                        operator->();
    const _GC_ADAPTEE*                  operator->() const;

    gcPointer<_Type, _ItemPointerBase>&
    operator[](int);

    const gcPointer<_Type, _ItemPointerBase>&
    operator[](int) const;

    template<class _OtherType, class _OtherPointerBase>
    gcPointer<_OtherType, _OtherPointerBase, true>
    gc_to(typename std::enable_if<std::is_convertible<_Type*,_OtherType*>::value,gcObject_B_>::type*) const;

    template<class _Other, class _OtherPointerBase>
    operator gcPointer<_Other, _OtherPointerBase>();

    template<class _Other, class _OtherPointerBase>
    operator const gcPointer<_Other, _OtherPointerBase>() const;

};

#ifndef _GC_HIDE_METHODS

_GC_TEMPLATE _GC_SELF::gcPointer() : _PointerBase() {}

_GC_TEMPLATE _GC_SELF::gcPointer(_GC_CONTAINER*const val) : _PointerBase() {
    gc_set_object(val);
}

_GC_TEMPLATE _GC_SELF::gcPointer(const _GC_SELF& other) : _PointerBase() {
    gc_copy(other);
}

_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (_GC_CONTAINER*const val) {
    gc_set_object(val);
    return *this;
}

_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (const _GC_SELF& other) {
    gc_copy(other);
    return *this;
}

_GC_TEMPLATE _GC_ADAPTEE& _GC_SELF::operator*() {
    return *(_GC_CONTAINER_M_->adaptee);
}

_GC_TEMPLATE const _GC_ADAPTEE& _GC_SELF::operator*() const{
    return *(_GC_CONST_CONTAINER_M_->adaptee);
}

_GC_TEMPLATE _GC_ADAPTEE* _GC_SELF::operator->() {
    return _GC_CONTAINER_M_->adaptee;
}

_GC_TEMPLATE const _GC_ADAPTEE* _GC_SELF::operator->() const {
    return _GC_CONST_CONTAINER_M_->adaptee;
}

_GC_TEMPLATE gcPointer<_Type, _ItemPointerBase>& _GC_SELF::operator[](int i) {
    return (*(_GC_CONTAINER_M_->adaptee))[i];
}

_GC_TEMPLATE const gcPointer<_Type, _ItemPointerBase>& _GC_SELF::operator[](int i) const{
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

///////////////////////////////////////////////////////////////////////////////
// alias

template<class _Type, std::size_t _N, class _PointerBase = gcSharedPointer_B_, class _ItemPointerBase = gcSharedPointer_B_>
using gcArrayPointer = gcPointer < gcArray<_Type, _N, _ItemPointerBase>, _PointerBase, true >;

}

#endif // _GC_ARRAY_H

