// File: gcpointer.h
// Description:
// stl set wrapper

#ifndef _GC_SET_H
#define _GC_SET_H

#include <utility>
#include <functional>
#include <set>

#include "gccontainer.h"

namespace collector
{

    // GC internal set pointer iterator adapter

    template<class _Iterator>
    class gcSetIteratorAdapter : public gcIteratorInterface {
    public:

        _Iterator                           adaptee;

        gcIteratorInterface*                gc_next() override;
        const gcPointerBase*                gc_get_pointer() const override;
        bool                                gc_is_equal(gcIteratorInterface*const other) const override;

                                            ~gcSetIteratorAdapter() override;
                                            gcSetIteratorAdapter(const _Iterator& );
    };

#ifndef _GC_HIDE_METHODS

    template<class _Iterator>
    gcSetIteratorAdapter<_Iterator>::gcSetIteratorAdapter(const _Iterator& other){
        adaptee = other;
    }

    template<class _Iterator>
    gcSetIteratorAdapter<_Iterator>::~gcSetIteratorAdapter(){
        //nothing
    }

    template<class _Iterator>
    gcIteratorInterface* gcSetIteratorAdapter<_Iterator>::gc_next() {
        ++adaptee;
        return this;
    }

    template<class _Iterator>
    const gcPointerBase* gcSetIteratorAdapter<_Iterator>::gc_get_pointer() const {
        return &(*adaptee);
    }

    template<class _Iterator>
    bool gcSetIteratorAdapter<_Iterator>::gc_is_equal(gcIteratorInterface*const other) const {
        return ((static_cast<gcSetIteratorAdapter<_Iterator>*>(other)->adaptee) == adaptee);
    }

#endif

// GC pointer set adapter

#define _GC_TEMPLATE	template< class _Type, class _Compare>
#define _GC_CONTAINER	std::set<gcPointer<_Type>, _Compare, gcContainerAllocator< gcPointer<_Type> > >
#define _GC_ITERATOR	typename _GC_CONTAINER::iterator
#define _GC_SELF		gcSet< _Type, _Compare>

    template <class _Type, class _Compare=std::less< gcPointer<_Type> > >
    class gcSet : public gcObject, public gcContainerInterface {
    public:

        // !!Just most basic implementation of derived types yet
        typedef _GC_CONTAINER Container;
        typedef _GC_ITERATOR Iterator;

        _GC_CONTAINER                       adaptee;

                                            gcSet();
                                            ~gcSet() override;

        gcIteratorInterface*                gc_begin() override;
        gcIteratorInterface*                gc_end() override;

        bool                                operator== (const _GC_SELF& other) const;
        bool                                operator!= (const _GC_SELF& other) const;
        bool                                operator> (const _GC_SELF& other) const;
        bool                                operator< (const _GC_SELF& other) const;
        bool                                operator>= (const _GC_SELF& other) const;
        bool                                operator<= (const _GC_SELF& other) const;
    };

#ifndef _GC_HIDE_METHODS

    _GC_TEMPLATE gcIteratorInterface* _GC_SELF::gc_begin(){
        return new gcSetIteratorAdapter<_GC_ITERATOR>(adaptee.begin());
    }

    _GC_TEMPLATE gcIteratorInterface* _GC_SELF::gc_end(){
        return new gcSetIteratorAdapter<_GC_ITERATOR>(adaptee.end());

    }

    _GC_TEMPLATE _GC_SELF::~gcSet() {

        gcDisconnectObject _gc_val(this);

        object_scope.childreen = 0;
    }

    _GC_TEMPLATE _GC_SELF::gcSet() : gcObject() {

        gcConnectObject _gc_val(this);

        delete object_scope.childreen;
        object_scope.childreen = this;
    }

    _GC_TEMPLATE bool _GC_SELF::operator == (const _GC_SELF& other) const {
        return adaptee == other.adaptee;
    }

    _GC_TEMPLATE bool _GC_SELF::operator != (const _GC_SELF& other) const {
        return adaptee != other.adaptee;
    }

    _GC_TEMPLATE bool _GC_SELF::operator > (const _GC_SELF& other) const {
        return adaptee > other.adaptee;
    }

    _GC_TEMPLATE bool _GC_SELF::operator < (const _GC_SELF& other) const {
        return adaptee < other.adaptee;
    }

    _GC_TEMPLATE bool _GC_SELF::operator >= (const _GC_SELF& other) const {
        return adaptee >= other.adaptee;
    }

    _GC_TEMPLATE bool _GC_SELF::operator <= (const _GC_SELF& other) const {
        return adaptee <= other.adaptee;
    }

#endif

#undef _GC_TEMPLATE
#undef _GC_SELF
#undef _GC_CONTAINER
#undef _GC_ITERATOR

#define _GC_TEMPLATE template< class _Type, class _Compare>
#define	_GC_CONTAINER_ADAPTER gcSet< _Type, _Compare>
#define _GC_SELF gcPointer<_GC_CONTAINER_ADAPTER, true>
#define _GC_CONTAINER std::set<gcPointer<_Type>, _Compare, gcContainerAllocator< gcPointer<_Type> > >

    _GC_TEMPLATE
    class gcPointer< _GC_CONTAINER_ADAPTER, true>
            : public gcPointerBase{
    public:

        // !!Just most basic implementation of derived types yet
        typedef _GC_CONTAINER Container;
        typedef typename _GC_CONTAINER::iterator Iterator;

                                            gcPointer();
                                            gcPointer(_GC_CONTAINER_ADAPTER*const other);
                                            gcPointer(const _GC_SELF& other);
                                            ~gcPointer() override;

        _GC_SELF&                           operator= (_GC_CONTAINER_ADAPTER*const other);
        _GC_SELF&                           operator= (const _GC_SELF& other);
        _GC_CONTAINER&                      operator*();
        _GC_CONTAINER*                      operator->();
        const _GC_CONTAINER&                operator*() const;
        const _GC_CONTAINER*                operator->() const;

        template<class _Other> explicit     operator gcPointer<_Other>();

        bool                                operator== (const _GC_SELF& other) const;
        bool                                operator!= (const _GC_SELF& other) const;
        bool                                operator< (const _GC_SELF& other) const;
        bool                                operator> (const _GC_SELF& other) const;
        bool                                operator<= (const _GC_SELF& other) const;
        bool                                operator>= (const _GC_SELF& other) const;
    };

#ifndef _GC_HIDE_METHODS

// methods

    _GC_TEMPLATE _GC_SELF::gcPointer() : gcPointerBase() {}

    _GC_TEMPLATE _GC_SELF::gcPointer(_GC_CONTAINER_ADAPTER*const other) : gcPointerBase() {
        object = other;
    }

    _GC_TEMPLATE _GC_SELF::gcPointer(const _GC_SELF& other) : gcPointerBase() {
        object = other.object;
    }

    _GC_TEMPLATE _GC_SELF::~gcPointer() {
        // nothing
    }

    _GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (_GC_CONTAINER_ADAPTER*const other) {
        object = other;
        return *this;
    }

    _GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (const _GC_SELF& other) {
        object = other.object;
        return *this;
    }

    _GC_TEMPLATE _GC_CONTAINER& _GC_SELF::operator*() {
        return static_cast<_GC_CONTAINER_ADAPTER*>(object)->adaptee;
    }

    _GC_TEMPLATE _GC_CONTAINER* _GC_SELF::operator->() {
        return &(static_cast<_GC_CONTAINER_ADAPTER*>(object)->adaptee);
    }

    _GC_TEMPLATE const _GC_CONTAINER& _GC_SELF::operator*() const{
        return static_cast<_GC_CONTAINER_ADAPTER*>(object)->adaptee;
    }

    _GC_TEMPLATE const _GC_CONTAINER* _GC_SELF::operator->() const{
        return &(static_cast<_GC_CONTAINER_ADAPTER*>(object)->adaptee);
    }

    _GC_TEMPLATE
    template<class _Other> _GC_SELF::operator gcPointer<_Other>(){
        return gcPointer<_Other>(static_cast<_Other*>(object));
    }

    _GC_TEMPLATE bool _GC_SELF::operator == (const _GC_SELF& other) const {
        return object == other.object;
    }

    _GC_TEMPLATE bool _GC_SELF::operator != (const _GC_SELF& other) const {
        return object != other.object;
    }

    _GC_TEMPLATE bool _GC_SELF::operator < (const _GC_SELF& other) const {
        return object < other.object;
    }

    _GC_TEMPLATE bool _GC_SELF::operator > (const _GC_SELF& other) const {
        return object > other.object;
    }

    _GC_TEMPLATE bool _GC_SELF::operator <= (const _GC_SELF& other) const {
        return object <= other.object;
    }

    _GC_TEMPLATE bool _GC_SELF::operator >= (const _GC_SELF& other) const {
        return object >= other.object;
    }

#endif

#undef _GC_TEMPLATE
#undef _GC_CONTAINER_ADAPTER
#undef _GC_SELF
#undef _GC_CONTAINER

template<class _Type, class _Compare=std::less<gcPointer<_Type> > > using gcSetPointer = gcPointer< gcSet<_Type,_Compare>, true >;

}
#endif
