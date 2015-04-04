// File: gcpointer.h
// Description:
// define smart pointer classes

#ifndef _GC_POINTER_H
#define _GC_POINTER_H

#include <type_traits>

#include "gc.h"

namespace gcNamespace {

class gcSharedPointer_B_;
class gcWeakPointer_B_;

// 'Generic template' pointer class
template<class _Type, class _PointerBase = gcSharedPointer_B_, bool _TypeIsObject = std::is_convertible<_Type*, gcObject_B_*>::value>
class gcPointer;

#define _GC_SELF gcPointer<_Type, _PointerBase, true>
#define _GC_TEMPLATE template<class _Type, class _PointerBase>

// Class specialization for gcObject class derived types
_GC_TEMPLATE class gcPointer<_Type, _PointerBase, true> : public _PointerBase{
public:

    gcPointer();
    gcPointer(_Type*const other);
    gcPointer(const _GC_SELF& other);

    _GC_SELF&                           operator= (_Type*const other);
    _GC_SELF&                           operator= (const _GC_SELF& other);

    _Type&                              operator*();
    const _Type&                        operator*() const;

    _Type*                              operator->();
    const _Type*                        operator->() const;

    _Type&                              operator[](int);
    const _Type&                        operator[](int) const;

    template<class _OtherType, class _OtherPointerBase>
    operator gcPointer<_OtherType, _OtherPointerBase, true>();

    template<class _OtherType, class _OtherPointerBase>
    operator const gcPointer<_OtherType, _OtherPointerBase, true>() const;

    bool                                operator== (const _GC_SELF& other) const;
    bool                                operator!= (const _GC_SELF& other) const;
    bool                                operator< (const _GC_SELF& other) const;
    bool                                operator> (const _GC_SELF& other) const;
    bool                                operator<= (const _GC_SELF& other) const;
    bool                                operator>= (const _GC_SELF& other) const;

};

#ifndef _GC_HIDE_METHODS

// methods

_GC_TEMPLATE _GC_SELF::gcPointer() : _PointerBase() {}

_GC_TEMPLATE _GC_SELF::gcPointer(_Type*const other) : _PointerBase() {
    this->gc_set_object(other);
}

_GC_TEMPLATE _GC_SELF::gcPointer(const _GC_SELF& other) : _PointerBase() {
    this->gc_copy(other);
}

_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (_Type*const other) {
    this->gc_set_object(other);
    return *this;
}

_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (const _GC_SELF& other) {
    this->gc_copy(other);
    return *this;
}

_GC_TEMPLATE _Type& _GC_SELF::operator*() {
    return *static_cast<_Type*>(this->gc_get_object());
}

_GC_TEMPLATE const _Type& _GC_SELF::operator*() const {
    return *static_cast<_Type*>(this->gc_get_const_object());
}

_GC_TEMPLATE _Type* _GC_SELF::operator->() {
    return static_cast<_Type*>(this->gc_get_object());
}

_GC_TEMPLATE const _Type* _GC_SELF::operator->() const {
    return static_cast<_Type*>(this->gc_get_const_object());
}

_GC_TEMPLATE
_Type& _GC_SELF::operator[](int i) {
    return static_cast<_Type*>(this->gc_get_object())[i];
}

_GC_TEMPLATE const _Type& _GC_SELF::operator[](int i) const {
    return static_cast<_Type*>(this->gc_get_const_object())[i];
}

_GC_TEMPLATE template<class _OtherType, class _OtherPointerBase>
        _GC_SELF::operator gcPointer<_OtherType, _OtherPointerBase, true>(){
    return gcPointer<_OtherType, _OtherPointerBase, true>(static_cast<_OtherType*>(this->gc_get_object()));
}

_GC_TEMPLATE template<class _OtherType, class _OtherPointerBase>
        _GC_SELF::operator const gcPointer<_OtherType, _OtherPointerBase, true>() const {
    return gcPointer<_OtherType, _OtherPointerBase, true>(static_cast<_OtherType*>(this->gc_get_object()));
}

_GC_TEMPLATE bool _GC_SELF::operator== (const _GC_SELF& other) const{
    return this->gc_get_const_object() == other.gc_get_const_object();
}

_GC_TEMPLATE bool _GC_SELF::operator!= (const _GC_SELF& other) const{
    return this->gc_get_const_object() != other.gc_get_const_object();
}

_GC_TEMPLATE bool _GC_SELF::operator> (const _GC_SELF& other) const{
    return this->gc_get_const_object() > other.gc_get_const_object();
}

_GC_TEMPLATE bool _GC_SELF::operator< (const _GC_SELF& other) const{
    return this->gc_get_const_object() < other.gc_get_const_object();
}

_GC_TEMPLATE bool _GC_SELF::operator<= (const _GC_SELF& other) const{
    return this->gc_get_const_object() <= other.gc_get_const_object();
}

_GC_TEMPLATE bool _GC_SELF::operator>= (const _GC_SELF& other) const{
    return this->gc_get_const_object() >= other.gc_get_const_object();
}

#endif
#undef _GC_SELF
#undef _GC_TEMPLATE

#define _GC_SELF gcPointer<_Type, _PointerBase, false>
#define _GC_TEMPLATE template<class _Type, class _PointerBase>

// Class specialization for non-gcObject derived types
_GC_TEMPLATE class gcPointer<_Type, _PointerBase, false> : public _PointerBase{
public:

    gcPointer();
    gcPointer(_Type*const other);
    gcPointer(const _GC_SELF& other);

    _GC_SELF&                           operator= (_Type*const other);
    _GC_SELF&                           operator= (const _GC_SELF& other);

    _Type&                              operator*();
    _Type*                              operator->();
    _Type&                              operator[](int);

    const _Type&                        operator*() const;
    const _Type*                        operator->() const;
    const _Type&                        operator[](int) const;

    template<class _OtherType, class _OtherPointerBase>
    operator gcPointer<_OtherType, _OtherPointerBase, false>();

    template<class _OtherType, class _OtherPointerBase>
    operator const gcPointer<_OtherType, _OtherPointerBase, false>() const;

    bool                                operator== (const _GC_SELF& other) const;
    bool                                operator!= (const _GC_SELF& other) const;
    bool                                operator< (const _GC_SELF& other) const;
    bool                                operator> (const _GC_SELF& other) const;
    bool                                operator<= (const _GC_SELF& other) const;
    bool                                operator>= (const _GC_SELF& other) const;

};

#ifndef _GC_HIDE_METHODS

_GC_TEMPLATE _GC_SELF::gcPointer() : _PointerBase() {}


_GC_TEMPLATE _GC_SELF::gcPointer(_Type*const other) : _PointerBase() {
    this->gc_set_object(new gcObjectAdapter<_Type>(other));
}

_GC_TEMPLATE _GC_SELF::gcPointer(const _GC_SELF& other) : _PointerBase() {
    this->gc_copy(other);
}

_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (_Type*const other) {
    this->gc_set_object(new gcObjectAdapter<_Type>(other));
    return *this;
}

_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (const _GC_SELF& other) {
    this->gc_copy(other);
    return *this;
}

_GC_TEMPLATE _Type& _GC_SELF::operator*() {
    return *(static_cast<const gcObjectAdapter<_Type>*>(this->gc_get_object())->adaptee);
}

_GC_TEMPLATE _Type* _GC_SELF::operator->() {
    return static_cast<const gcObjectAdapter<_Type>*>(this->gc_get_object())->adaptee;
}

_GC_TEMPLATE _Type& _GC_SELF::operator[](int i) {
    return (static_cast<const gcObjectAdapter<_Type>*>(this->gc_get_object())->adaptee)[i];
}

_GC_TEMPLATE const _Type& _GC_SELF::operator*() const {
    return *(static_cast<const gcObjectAdapter<_Type>*>(this->gc_get_const_object())->adaptee);
}

_GC_TEMPLATE const _Type* _GC_SELF::operator->() const{
    return static_cast<const gcObjectAdapter<_Type>*>(this->gc_get_const_object())->adaptee;
}

_GC_TEMPLATE const _Type& _GC_SELF::operator[](int i) const {
    return (static_cast<const gcObjectAdapter<_Type>*>(this->gc_get_const_object())->adaptee)[i];
}

_GC_TEMPLATE template<class _OtherType, class _OtherPointerBase>
        _GC_SELF::operator gcPointer<_OtherType, _OtherPointerBase, false>(){
    gcPointer<_OtherType, _OtherPointerBase, false> tmp;
    tmp.gc_copy(*this);// NO TYPE CHECK YET!!!!
    return tmp;
}

_GC_TEMPLATE template<class _OtherType, class _OtherPointerBase>
        _GC_SELF::operator const gcPointer<_OtherType, _OtherPointerBase, false>() const{
    gcPointer<_OtherType, _OtherPointerBase, false> tmp;
    tmp.gc_copy(*this);// NO TYPE CHECK YET!!!!
    return tmp;
}

_GC_TEMPLATE
bool _GC_SELF::operator == (const _GC_SELF& other) const{
    return static_cast<const gcObjectAdapter<_Type>*>(this->gc_get_const_object())->adaptee ==
            static_cast<const gcObjectAdapter<_Type>*>(other.gc_get_const_object())->adaptee;
}

_GC_TEMPLATE
bool _GC_SELF::operator != (const _GC_SELF& other) const{
    return static_cast<const gcObjectAdapter<_Type>*>(this->gc_get_const_object())->adaptee !=
            static_cast<const gcObjectAdapter<_Type>*>(other.gc_get_const_object())->adaptee;
}

_GC_TEMPLATE
bool _GC_SELF::operator < (const _GC_SELF& other) const{
    return static_cast<const gcObjectAdapter<_Type>*>(this->gc_get_const_object())->adaptee <
            static_cast<const gcObjectAdapter<_Type>*>(other.gc_get_const_object())->adaptee;
}

_GC_TEMPLATE
bool _GC_SELF::operator > (const _GC_SELF& other) const{
    return static_cast<const gcObjectAdapter<_Type>*>(this->gc_get_const_object())->adaptee >
            static_cast<const gcObjectAdapter<_Type>*>(other.gc_get_const_object())->adaptee;
}

_GC_TEMPLATE
bool _GC_SELF::operator <= (const _GC_SELF& other) const{
    return static_cast<const gcObjectAdapter<_Type>*>(this->gc_get_const_object())->adaptee <=
            static_cast<const gcObjectAdapter<_Type>*>(other.gc_get_const_object())->adaptee;
}

_GC_TEMPLATE
bool _GC_SELF::operator >= (const _GC_SELF& other) const{
    return static_cast<const gcObjectAdapter<_Type>*>(this->gc_get_const_object())->adaptee >=
            static_cast<const gcObjectAdapter<_Type>*>(other.gc_get_const_object())->adaptee;
}

#endif
#undef _GC_SELF
#undef _GC_TEMPLATE

template<class _Type> using gcSharedPointer = gcPointer <_Type, gcSharedPointer_B_>;
template<class _Type> using gcWeakPointer = gcPointer <_Type, gcWeakPointer_B_>;
template<class _Type> using gcScopedPointer = gcPointer <_Type, gcScopedPointer_B_<gcSharedPointer_B_>>;

}

#endif // _GC_POINTER_H
