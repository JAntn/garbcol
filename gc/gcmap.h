#ifndef _GC_MAP_H
#define _GC_MAP_H

// File: gcmap.h
// Description:
// define smart pointer classes

#include <utility>
#include <functional>
#include <map>
#include "gccontainer.h"

namespace collector
{

    // GC internal map pointer iterator adapter

    template<class _Iterator>
    class gcMapIteratorAdapter : public gcIteratorInterface {
    public:

        _Iterator adaptee;

        virtual gcIteratorInterface*		gc_next();
        virtual const gcPointerBase*        gc_get_pointer() const;
        virtual bool						gc_is_equal(gcIteratorInterface*const other) const;
        virtual								~gcMapIteratorAdapter();
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
    gcIteratorInterface* gcMapIteratorAdapter<_Iterator>::gc_next() {
        ++adaptee;
        return this;
    }

    template<class _Iterator>
    const gcPointerBase* gcMapIteratorAdapter<_Iterator>::gc_get_pointer() const {
        return &((*adaptee).second);
    }

    template<class _Iterator>
    bool gcMapIteratorAdapter<_Iterator>::gc_is_equal(gcIteratorInterface*const other) const {
        return ((static_cast<gcMapIteratorAdapter<_Iterator>*>(other)->adaptee) == adaptee);
    }

#endif

// GC pointer map adapter

#define _GC_TEMPLATE	template <class _Key, class _Type, class _Compare>
#define _GC_CONTAINER	std::map<_Key, gcPointer<_Type>, _Compare, gcContainerAllocator< std::pair<const _Key, gcPointer<_Type> > > >
#define _GC_ITERATOR	typename _GC_CONTAINER::iterator
#define _GC_SELF		gcMap< _Key, _Type, _Compare>

    template <class _Key, class _Type, class _Compare=std::less<_Key>>
    class gcMap : public gcObject, public gcContainerInterface {
    public:

        // !!Just most basic implementation of derived types yet

        typedef _GC_CONTAINER Container;
        typedef _GC_ITERATOR Iterator;

        _GC_CONTAINER adaptee;

        gcMap();
        virtual ~gcMap();

        virtual gcIteratorInterface*		gc_begin();
        virtual gcIteratorInterface*		gc_end();

        gcPointer<_Type>&              operator[](const _Key &);

        bool				operator== (const _GC_SELF& other) const;
        bool				operator!= (const _GC_SELF& other) const;
        bool				operator> (const _GC_SELF& other) const;
        bool				operator< (const _GC_SELF& other) const;
        bool				operator>= (const _GC_SELF& other) const;
        bool				operator<= (const _GC_SELF& other) const;
    };

#ifndef _GC_HIDE_METHODS


    _GC_TEMPLATE gcIteratorInterface* _GC_SELF::gc_begin(){
        return new gcMapIteratorAdapter<_GC_ITERATOR>(adaptee.begin());
    }

    _GC_TEMPLATE gcIteratorInterface* _GC_SELF::gc_end(){
        return new gcMapIteratorAdapter<_GC_ITERATOR>(adaptee.end());
    }

    _GC_TEMPLATE _GC_SELF::~gcMap() {

        gcDisconnectObject _gc_val(this);

        object_scope.childreen = 0;
    }

    _GC_TEMPLATE _GC_SELF::gcMap() : gcObject() {

        gcConnectObject _gc_val(this);

        delete object_scope.childreen;
        object_scope.childreen = this;
    }

    _GC_TEMPLATE gcPointer<_Type>& _GC_SELF::operator[](const _Key& k) {
        return adaptee[k];
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

#define _GC_TEMPLATE template< class _Key, class _Type, class _Compare>
#define	_GC_CONTAINER_ADAPTER gcMap<_Key, _Type, _Compare>
#define _GC_SELF gcPointer<_GC_CONTAINER_ADAPTER, true>
#define _GC_CONTAINER std::map<_Key, gcPointer<_Type>, _Compare, gcContainerAllocator< std::pair<const _Key, gcPointer<_Type> > > >

    _GC_TEMPLATE
    class gcPointer< _GC_CONTAINER_ADAPTER, true>
            : public gcPointerBase{
    public:

        typedef _GC_CONTAINER Container;
        typedef typename _GC_CONTAINER::iterator Iterator;

        gcPointer();
        gcPointer(_GC_CONTAINER_ADAPTER*const other);
        gcPointer(const _GC_SELF& other);
        ~gcPointer();

        _GC_SELF&			operator= (_GC_CONTAINER_ADAPTER*const other);
        _GC_SELF&			operator= (const _GC_SELF& other);
        _GC_CONTAINER&		operator*();
        _GC_CONTAINER*		operator->();
        template<class _Other> explicit operator gcPointer<_Other>();

        gcPointer<_Type>&              operator[] (const _Key&);

        bool				operator== (const _GC_SELF& other) const;
        bool				operator!= (const _GC_SELF& other) const;
        bool				operator> (const _GC_SELF& other) const;
        bool				operator< (const _GC_SELF& other) const;
        bool				operator<= (const _GC_SELF& other) const;
        bool				operator>= (const _GC_SELF& other) const;
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

    _GC_TEMPLATE
    template<class _Other> _GC_SELF::operator gcPointer<_Other>(){
        return gcPointer<_Other>(static_cast<_Other*>(object));
    }

    _GC_TEMPLATE gcPointer<_Type>& _GC_SELF::operator[](const _Key& k) {
        return static_cast<_GC_CONTAINER_ADAPTER*>(object)->adaptee[k];
    }

    _GC_TEMPLATE bool _GC_SELF::operator == (const _GC_SELF& other) const {
        return object == other.object;
    }

    _GC_TEMPLATE bool _GC_SELF::operator != (const _GC_SELF& other) const {
        return object != other.object;
    }

    _GC_TEMPLATE bool _GC_SELF::operator > (const _GC_SELF& other) const {
        return object > other.object;
    }

    _GC_TEMPLATE bool _GC_SELF::operator < (const _GC_SELF& other) const {
        return object < other.object;
    }

    _GC_TEMPLATE bool _GC_SELF::operator >= (const _GC_SELF& other) const {
        return object >= other.object;
    }

    _GC_TEMPLATE bool _GC_SELF::operator <= (const _GC_SELF& other) const {
        return object <= other.object;
    }

#endif

#undef _GC_TEMPLATE
#undef _GC_CONTAINER_ADAPTER
#undef _GC_SELF
#undef _GC_CONTAINER

template<class _Key, class _Type, class _Compare=std::less<_Key> > using gcMapPointer = gcPointer< gcMap<_Key,_Type,_Compare>, true >;

}

#endif
