// File: gcarray.h
// Description:
// stl array wrapper

#ifndef _GC_ARRAY_H
#define _GC_ARRAY_H

#include <utility>
#include <functional>
#include <array>

#include "gccontainer.h"

namespace gcNamespace {


    // GC internal pointer iterator adapter

    template<class _Iterator>
    class gcArrayIteratorAdapter : public gcIteratorInterface {
    public:

        _Iterator                           adaptee;


        gcIteratorInterface*                gc_next() override;
        const gcPointerBase*                gc_get_const_pointer() const override;
        bool                                gc_is_equal(gcIteratorInterface*const other) const override;

                                            ~gcArrayIteratorAdapter() override;
                                            gcArrayIteratorAdapter(const _Iterator&) override;
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
    gcIteratorInterface* gcArrayIteratorAdapter<_Iterator>::gc_next() {
        ++adaptee;
        return this;
    }

    template<class _Iterator>
    const gcPointerBase*  gcArrayIteratorAdapter<_Iterator>::gc_get_const_pointer() const {
        return &(*adaptee);
    }

    template<class _Iterator>
    bool gcArrayIteratorAdapter<_Iterator>::gc_is_equal(gcIteratorInterface*const other)  const{
        return ((static_cast<gcArrayIteratorAdapter<_Iterator>*>(other)->adaptee) == adaptee);
    }

#endif

    // GC internal pointer container adapter (used for list, deque, vector)
#define _GC_TEMPLATE	template < class _Type, std::size_t _N>
#define _GC_CONTAINER	std::array<gcPointer<_Type>, _N>
#define _GC_ITERATOR	typename _GC_CONTAINER::iterator
#define _GC_CONST_ITERATOR	typename _GC_CONTAINER::const_iterator
#define _GC_SELF		gcArray<_Type, _N>

    _GC_TEMPLATE
    class gcArray :  public gcObject, public gcContainerInterface {
    public:

        // !!Just most basic implementation of derived types yet
        typedef _GC_CONTAINER Container;
        typedef _GC_ITERATOR Iterator;

        _GC_CONTAINER* adaptee;

                                            gcArray();
                                            ~gcArray() override;

        gcIteratorInterface*                gc_begin() override;
        gcIteratorInterface*                gc_end() override;

        bool                                operator== (const _GC_SELF& other) const;
        bool                                operator!= (const _GC_SELF& other) const;
        bool                                operator> (const _GC_SELF& other) const;
        bool                                operator< (const _GC_SELF& other) const;
        bool                                operator>= (const _GC_SELF& other) const;
        bool                                operator<= (const _GC_SELF& other) const;

        gcPointer<_Type>&                   operator[](int);
        const gcPointer<_Type>&             operator[](int) const;

    };

#ifndef _GC_HIDE_METHODS

    _GC_TEMPLATE gcIteratorInterface* _GC_SELF::gc_begin() {
        return new gcArrayIteratorAdapter<_GC_ITERATOR>(adaptee.begin());
    }

    _GC_TEMPLATE gcIteratorInterface* _GC_SELF::gc_end() {
        return new gcArrayIteratorAdapter<_GC_ITERATOR>(adaptee.end());
    }

    _GC_TEMPLATE _GC_SELF::~gcArray() {

        gcDisconnectObject _gc_val(this);

        // push state
        bool tmp = _gc_scope_info->from_allocator;

        _gc_scope_info->from_allocator = true;
        delete adaptee;

        // restore state
        _gc_scope_info->from_allocator = tmp;

        object_scope.childreen = 0;
    }

    _GC_TEMPLATE _GC_SELF::gcArray() : gcObject() {

        gcConnectObject _gc_val(this);

        bool tmp = _gc_scope_info->from_allocator;
        _gc_scope_info->from_allocator = true;

        adaptee = new _GC_CONTAINER();

        _gc_scope_info->from_allocator = tmp;

        delete object_scope.childreen;
        object_scope.childreen = this;
    }

    _GC_TEMPLATE gcPointer<_Type>& _GC_SELF::operator[](int i) {
        return (*adaptee)[i];
    }

    _GC_TEMPLATE const gcPointer<_Type>& _GC_SELF::operator[](int i) const {
        return (*adaptee)[i];
    }

    _GC_TEMPLATE bool _GC_SELF::operator == (const _GC_SELF& other) const {
        return *adaptee == *(other.adaptee);
    }

    _GC_TEMPLATE bool _GC_SELF::operator != (const _GC_SELF& other) const {
        return *adaptee != other.adaptee;
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

#endif
#undef _GC_TEMPLATE
#undef _GC_SELF
#undef _GC_CONTAINER
#undef _GC_ITERATOR

    // Class gcPointer specialization for gcArray class parameter

#define _GC_TEMPLATE template < class _Type, std::size_t _N>
#define	_GC_CONTAINER_ADAPTER gcArray<_Type, _N>
#define _GC_SELF gcPointer<_GC_CONTAINER_ADAPTER, true>
#define _GC_CONTAINER	std::array<_Type, _N>

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
        const _GC_CONTAINER&                operator*() const;

        _GC_CONTAINER*                      operator->();
        const _GC_CONTAINER*                operator->() const;

        gcPointer<_Type>&                   operator[](int);
        const gcPointer<_Type>&             operator[](int) const;

        template<class _Other> explicit operator gcPointer<_Other>();

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

    _GC_TEMPLATE _GC_SELF::gcPointer(_GC_CONTAINER_ADAPTER*const val) : gcPointerBase() {
        gc_set_object(val);
    }

    _GC_TEMPLATE _GC_SELF::gcPointer(const _GC_SELF& other) : gcPointerBase() {
        gc_copy(other);
    }

    _GC_TEMPLATE _GC_SELF::~gcPointer() {
        // nothing
    }

    _GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (_GC_CONTAINER_ADAPTER*const val) {
        gc_set_object(val);
        return *this;
    }

    _GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (const _GC_SELF& other) {
        gc_copy(other);
        return *this;
    }

    _GC_TEMPLATE _GC_CONTAINER& _GC_SELF::operator*() {
        return *(static_cast<_GC_CONTAINER_ADAPTER*>(object)->adaptee);
    }

    _GC_TEMPLATE const _GC_CONTAINER& _GC_SELF::operator*() const{
        return *(static_cast<_GC_CONTAINER_ADAPTER*>(object)->adaptee);
    }

    _GC_TEMPLATE _GC_CONTAINER* _GC_SELF::operator->() {
        return static_cast<_GC_CONTAINER_ADAPTER*>(object)->adaptee;
    }

    _GC_TEMPLATE const _GC_CONTAINER* _GC_SELF::operator->() const {
        return static_cast<_GC_CONTAINER_ADAPTER*>(object)->adaptee;
    }

    _GC_TEMPLATE gcPointer<_Type>& _GC_SELF::operator[](int i) {
        return (*(static_cast<_GC_CONTAINER_ADAPTER*>(object)->adaptee))[i];
    }

    _GC_TEMPLATE const gcPointer<_Type>& _GC_SELF::operator[](int i) const{
        return (*(static_cast<_GC_CONTAINER_ADAPTER*>(object)->adaptee))[i];
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

    template<class _Type, std::size_t _N> using gcArrayPointer = gcPointer < gcArray<_Type, _N>, true >;

}

#endif // _GC_ARRAY_H

