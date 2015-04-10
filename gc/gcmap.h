// File: gcdeque.h
// Description:
// stl map wrapper

#ifndef _GC_MAP_H
#define _GC_MAP_H

#include <utility>
#include <functional>
#include <map>

#include "gc.h"

namespace gcNamespace
{

// GC internal map pointer iterator adapter

template<class _Iterator>
class gcMapIteratorAdapter : public gcIterator_B_ {

    _Iterator                           adaptee;

public:

    gcIterator_B_*                      gc_next() override;
    const gcPointer_B_*                 gc_get_const_pointer() const override;
    bool                                gc_is_equal(const gcIterator_B_* other) const override;

    ~gcMapIteratorAdapter() override;
    gcMapIteratorAdapter(const _Iterator&);
};

#ifndef _GC_HIDE_METHODS

template<class _Iterator>
gcMapIteratorAdapter<_Iterator>::gcMapIteratorAdapter(const _Iterator& other){
    adaptee = other;
}

template<class _Iterator>
gcMapIteratorAdapter<_Iterator>::~gcMapIteratorAdapter(){
    //nothing
}

template<class _Iterator>
gcIterator_B_* gcMapIteratorAdapter<_Iterator>::gc_next() {
    ++adaptee;
    return this;
}

template<class _Iterator>
const gcPointer_B_* gcMapIteratorAdapter<_Iterator>::gc_get_const_pointer() const {
    return &((*adaptee).second);
}

template<class _Iterator>
bool gcMapIteratorAdapter<_Iterator>::gc_is_equal(const gcIterator_B_* other) const {
    return ((static_cast<const gcMapIteratorAdapter<_Iterator>*>(other)->adaptee) == adaptee);
}

#endif


// GC pointer map adapter

#define _GC_TEMPLATE                    template <class _Key, class _Type, class _Compare, class _ItemPointerBase>
#define _GC_ADAPTEE                     std::map<_Key, gcPointer<_Type, _ItemPointerBase>, _Compare, gcContainerAllocator< std::pair<const _Key, gcPointer<_Type, _ItemPointerBase> > > >
#define _GC_ITERATOR                    typename _GC_ADAPTEE::iterator
#define _GC_CONST_ITERATOR              typename _GC_ADAPTEE::const_iterator
#define _GC_SELF                        gcMap< _Key, _Type, _Compare, _ItemPointerBase>

template <class _Key, class _Type, class _Compare=std::less<_Key>, class _ItemPointerBase = gcSharedPointer_B_>
class gcMap : public gcObjectAdapter<_GC_ADAPTEE>, public gcContainer_B_ {

    using gcObjectAdapter<_GC_ADAPTEE>::adaptee;

    // Any pointer
    template<class _TypePP, class _PointerBasePP, bool _TypeIsObjectPP>
    friend class gcPointer;

public:

    // !!Just most basic implementation of derived types yet
    typedef _GC_ADAPTEE container;
    typedef _GC_ITERATOR iterator;
    typedef _GC_CONST_ITERATOR const_iterator;

    template<class ..._Args>            gcMap(_Args...);
    ~gcMap() override;

    gcIterator_B_*                      gc_begin() override;
    gcIterator_B_*                      gc_begin() const override;

    gcIterator_B_*                      gc_end() override;
    gcIterator_B_*                      gc_end() const override;

    gcPointer<_Type,_ItemPointerBase>&
    operator[](const _Key &);

    const gcPointer<_Type,_ItemPointerBase>&
    operator[](const _Key &) const;

    bool                                operator== (const _GC_SELF& other) const;
    bool                            	operator!= (const _GC_SELF& other) const;
    bool                        		operator> (const _GC_SELF& other) const;
    bool                        		operator< (const _GC_SELF& other) const;
    bool                        		operator>= (const _GC_SELF& other) const;
    bool                        		operator<= (const _GC_SELF& other) const;

    const gcContainer_B_*               gc_get_const_childreen() const override;
};

#ifndef _GC_HIDE_METHODS

_GC_TEMPLATE gcIterator_B_* _GC_SELF::gc_begin() {
    return new gcMapIteratorAdapter<_GC_ITERATOR>(adaptee->begin());
}

_GC_TEMPLATE gcIterator_B_* _GC_SELF::gc_begin() const {
    return new gcMapIteratorAdapter<_GC_CONST_ITERATOR>(adaptee->begin());
}

_GC_TEMPLATE gcIterator_B_* _GC_SELF::gc_end() {
    return new gcMapIteratorAdapter<_GC_ITERATOR>(adaptee->end());
}

_GC_TEMPLATE gcIterator_B_* _GC_SELF::gc_end() const {
    return new gcMapIteratorAdapter<_GC_CONST_ITERATOR>(adaptee->end());
}

_GC_TEMPLATE _GC_SELF::~gcMap() {

    // push state
    bool tmp = _gc_scope_info->from_allocator;

    _gc_scope_info->from_allocator = true;
    delete adaptee;
    adaptee = 0;

    // restore state
    _gc_scope_info->from_allocator = tmp;
}

_GC_TEMPLATE template<class ..._Args> _GC_SELF::gcMap(_Args... args) : gcObjectAdapter<_GC_ADAPTEE>() {

    bool tmp = _gc_scope_info->from_allocator;
    _gc_scope_info->from_allocator = true;

    adaptee = new _GC_ADAPTEE(args...);

    _gc_scope_info->from_allocator = tmp;
}

_GC_TEMPLATE gcPointer<_Type,_ItemPointerBase>& _GC_SELF::operator[](const _Key& k) {
    return (*adaptee)[k];
}

_GC_TEMPLATE const gcPointer<_Type,_ItemPointerBase>& _GC_SELF::operator[](const _Key& k) const {
    return (*adaptee)[k];
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

#define _GC_TEMPLATE                    template< class _Key, class _Type, class _Compare, class _PointerBase, class _ItemPointerBase>
#define	_GC_CONTAINER                   gcMap<_Key, _Type, _Compare, _ItemPointerBase>
#define _GC_SELF                        gcPointer<_GC_CONTAINER, _PointerBase, true>
#define _GC_ADAPTEE                     std::map<_Key, gcPointer<_Type, _ItemPointerBase>, _Compare, gcContainerAllocator< std::pair<const _Key, gcPointer<_Type, _ItemPointerBase> > > >
#define _GC_CONTAINER_M_                static_cast<_GC_CONTAINER*>(this->gc_get_object())
#define _GC_CONST_CONTAINER_M_          static_cast<_GC_CONTAINER*>(this->gc_get_const_object())

_GC_TEMPLATE
class gcPointer< _GC_CONTAINER, _PointerBase, true>
        : public _PointerBase{
public:

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
    const _GC_ADAPTEE&                  operator*() const;

    _GC_ADAPTEE*                        operator->();
    const _GC_ADAPTEE*                  operator->() const;

    template<class _Other, class _OtherPointerBase>
    operator gcPointer<_Other,_OtherPointerBase>();

    template<class _Other, class _OtherPointerBase>
    operator const gcPointer<_Other,_OtherPointerBase>() const;

    gcPointer<_Type, _ItemPointerBase>&
    operator[] (const _Key&);

    const gcPointer<_Type, _ItemPointerBase>&
    operator[] (const _Key&) const;

    bool                                operator== (const _GC_SELF& other) const;
    bool                        		operator!= (const _GC_SELF& other) const;
    bool                        		operator> (const _GC_SELF& other) const;
    bool                        		operator< (const _GC_SELF& other) const;
    bool                           		operator<= (const _GC_SELF& other) const;
    bool                        		operator>= (const _GC_SELF& other) const;
};

#ifndef _GC_HIDE_METHODS

// methods

_GC_TEMPLATE _GC_SELF::gcPointer() : _PointerBase() {}

_GC_TEMPLATE _GC_SELF::gcPointer(_GC_CONTAINER*const other) : _PointerBase() {
    this->gc_set_object(other);
}

_GC_TEMPLATE _GC_SELF::gcPointer(const _GC_SELF& other) : _PointerBase() {
    this->gc_copy(other);
}

_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (_GC_CONTAINER*const other) {
    this->gc_set_object(other);
    return *this;
}

_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (const _GC_SELF& other) {
    this->gc_copy(other);
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

_GC_TEMPLATE const _GC_ADAPTEE* _GC_SELF::operator->() const{
    return _GC_CONST_CONTAINER_M_->adaptee;
}

_GC_TEMPLATE
template<class _Other, class _OtherPointerBase> _GC_SELF::operator gcPointer<_Other,_OtherPointerBase>(){
    return gcPointer<_Other,_OtherPointerBase>(static_cast<_Other*>(this->gc_get_object()));
}

_GC_TEMPLATE
template<class _Other, class _OtherPointerBase> _GC_SELF::operator const gcPointer<_Other,_OtherPointerBase>() const {
    return gcPointer<_Other,_OtherPointerBase>(static_cast<_Other*>(this->gc_get_object()));
}

_GC_TEMPLATE gcPointer<_Type, _ItemPointerBase>& _GC_SELF::operator[](const _Key& k) {
    return (*(_GC_CONTAINER_M_->adaptee))[k];
}

_GC_TEMPLATE const gcPointer<_Type, _ItemPointerBase>& _GC_SELF::operator[](const _Key& k) const {
    return (*(_GC_CONST_CONTAINER_M_->adaptee))[k];
}

_GC_TEMPLATE bool _GC_SELF::operator == (const _GC_SELF& other) const {
    return this->gc_get_const_object() == other.gc_get_const_object();
}

_GC_TEMPLATE bool _GC_SELF::operator != (const _GC_SELF& other) const {
    return this->gc_get_const_object() != other.gc_get_const_object();
}

_GC_TEMPLATE bool _GC_SELF::operator > (const _GC_SELF& other) const {
    return this->gc_get_const_object() > other.gc_get_const_object();
}

_GC_TEMPLATE bool _GC_SELF::operator < (const _GC_SELF& other) const {
    return this->gc_get_const_object() < other.gc_get_const_object();
}

_GC_TEMPLATE bool _GC_SELF::operator >= (const _GC_SELF& other) const {
    return this->gc_get_const_object() >= other.gc_get_const_object();
}

_GC_TEMPLATE bool _GC_SELF::operator <= (const _GC_SELF& other) const {
    return this->gc_get_const_object() <= other.gc_get_const_object();
}

#endif

#undef _GC_TEMPLATE
#undef _GC_CONTAINER
#undef _GC_SELF
#undef _GC_ADAPTEE
#undef _GC_CONTAINER_M_
#undef _GC_CONST_CONTAINER_M_

template<class _Key, class _Type, class _Compare=std::less<_Key> ,
         class _PointerBase = gcSharedPointer_B_, class _ItemPointerBase = gcSharedPointer_B_>
using gcMapPointer = gcPointer< gcMap<_Key,_Type,_Compare,_ItemPointerBase>, _PointerBase, true >;

}

#endif
