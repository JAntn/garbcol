///////////////////////////////////////////////////////////////////////////////
// File: gcpointer.h
// Description:
// smart pointer

#ifndef _GC_POINTER_H
#define _GC_POINTER_H

#include <type_traits>
#include "gc.h"

namespace gcNamespace {

class gcSharedPointer_B_;
class gcWeakPointer_B_;
class gcUniquePointer_B_;

// 'Generic template' pointer class
template<class _Type, class _PointerBase = gcSharedPointer_B_, bool _TypeIsObject = std::is_convertible<_Type*, gcObject_B_*>::value>
class gcPointer;

///////////////////////////////////////////////////////////////////////////////
// specialization for gcObject class derived types

#define _GC_SELF gcPointer<_Type, _PointerBase, true>
#define _GC_TEMPLATE template<class _Type, class _PointerBase>

_GC_TEMPLATE class gcPointer<_Type, _PointerBase, true> : public _PointerBase{
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

    gcPointer();
    gcPointer(_Type*const& other);
    gcPointer(_Type*&& other);

    gcPointer(const _GC_SELF& other);
    gcPointer(_GC_SELF&& other);

    _GC_SELF&                           operator= (_Type*const& other);
    _GC_SELF&                           operator= (_Type*&& other);

    _GC_SELF&                           operator= (const _GC_SELF& other);
    _GC_SELF&                           operator= (_GC_SELF&& other);

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

};

#ifndef _GC_HIDE_METHODS

_GC_TEMPLATE _GC_SELF::gcPointer() : _PointerBase() {}

_GC_TEMPLATE _GC_SELF::gcPointer(_Type*const& other) : _PointerBase() {

    static_assert(std::is_base_of<gcSharedPointer_B_,_PointerBase>::value,
                  "unique pointer initialization from lvalue");

    static_assert(!std::is_base_of<gcWeakPointer_B_,_PointerBase>::value,
                   "referencing object to weak pointer directly" );

    gc_set_object(other);
}

_GC_TEMPLATE _GC_SELF::gcPointer(_Type*&& other) : _PointerBase() {

    static_assert(!std::is_base_of<gcWeakPointer_B_,_PointerBase>::value,
                   "referencing object to weak pointer directly" );

    gc_set_object(std::move(other));
}

_GC_TEMPLATE _GC_SELF::gcPointer(const _GC_SELF& other) : _PointerBase() {

    static_assert(std::is_base_of<gcSharedPointer_B_,_PointerBase>::value,
                  "unique pointer initialization from lvalue");

    gc_copy(other);
}

_GC_TEMPLATE _GC_SELF::gcPointer(_GC_SELF&& other) : _PointerBase() {
    gc_copy(std::move(other));
}

_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (_Type*const& other) {

    static_assert(std::is_base_of<gcSharedPointer_B_,_PointerBase>::value,
                  "unique pointer initialization from lvalue");

    static_assert(!std::is_base_of<gcWeakPointer_B_,_PointerBase>::value,
                   "referencing object to weak pointer directly" );

    gc_set_object(other);
    return *this;
}


_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (_Type*&& other) {

    static_assert(!std::is_base_of<gcWeakPointer_B_,_PointerBase>::value,
                   "referencing object to weak pointer directly" );

    gc_set_object(std::move(other));
    return *this;
}

_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (const _GC_SELF& other) {

    static_assert(std::is_base_of<gcSharedPointer_B_,_PointerBase>::value,
                  "unique pointer initialization from lvalue");

    gc_copy(other);
    return *this;
}

_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (_GC_SELF&& other) {
    gc_copy(std::move(other));
    return *this;
}

_GC_TEMPLATE _Type& _GC_SELF::operator*() {
    return *static_cast<_Type*>(gc_get_object());
}

_GC_TEMPLATE const _Type& _GC_SELF::operator*() const {
    return *static_cast<_Type*>(gc_get_const_object());
}

_GC_TEMPLATE _Type* _GC_SELF::operator->() {
    return static_cast<_Type*>(gc_get_object());
}

_GC_TEMPLATE const _Type* _GC_SELF::operator->() const {
    return static_cast<_Type*>(gc_get_const_object());
}

_GC_TEMPLATE
_Type& _GC_SELF::operator[](int i) {    
    return static_cast<_Type*>(gc_get_object())[i];
}

_GC_TEMPLATE const _Type& _GC_SELF::operator[](int i) const {
    return static_cast<_Type*>(gc_get_const_object())[i];
}

_GC_TEMPLATE template<class _OtherType, class _OtherPointerBase>
        _GC_SELF::operator gcPointer<_OtherType, _OtherPointerBase, true>()
{
    static_assert(std::is_convertible<_Type*,_OtherType*>::value,
                  "object type is not convertible to other object type");

    static_assert(!std::is_base_of<gcUniquePointer_B_,_OtherPointerBase>::value,
                  "unique pointer casting forbidden");

    static_assert(!std::is_base_of<gcUniquePointer_B_,_PointerBase>::value,
                  "unique pointer casting forbidden");

    gcPointer<_OtherType, _OtherPointerBase, true> tmp;
    tmp.gc_copy(*this);
    return tmp;
}

_GC_TEMPLATE template<class _OtherType, class _OtherPointerBase>
        _GC_SELF::operator const gcPointer<_OtherType, _OtherPointerBase, true>() const
{
    static_assert(std::is_convertible<_Type*,_OtherType*>::value,
                  "object type is not convertible to other object type");

    static_assert(!std::is_base_of<gcUniquePointer_B_,_OtherPointerBase>::value,
                  "unique pointer casting forbidden");

    static_assert(!std::is_base_of<gcUniquePointer_B_,_PointerBase>::value,
                  "unique pointer casting forbidden");

    gcPointer<_OtherType, _OtherPointerBase, true> tmp;
    tmp.gc_copy(*this);
    return tmp;
}

#endif
#undef _GC_SELF
#undef _GC_TEMPLATE

#define _GC_SELF gcPointer<_Type, _PointerBase, false>
#define _GC_TEMPLATE template<class _Type, class _PointerBase>

///////////////////////////////////////////////////////////////////////////////
// specialization for non-gcObject derived types

_GC_TEMPLATE class gcPointer<_Type, _PointerBase, false> : public _PointerBase{
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

    gcPointer();
    gcPointer(_Type*const& other);
    gcPointer(_Type*&& other);

    gcPointer(const _GC_SELF& other);
    gcPointer(_GC_SELF&& other);

    _GC_SELF&                           operator= (_Type*const& other);
    _GC_SELF&                           operator= (_Type*&& other);
    _GC_SELF&                           operator= (const _GC_SELF& other);
    _GC_SELF&                           operator= (_GC_SELF&& other);

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


_GC_TEMPLATE _GC_SELF::gcPointer(_Type*const& other) : _PointerBase(){

    static_assert(std::is_base_of<gcSharedPointer_B_,_PointerBase>::value,
                  "unique pointer initialization from lvalue");

    static_assert(!std::is_base_of<gcWeakPointer_B_,_PointerBase>::value,
                   "referencing object to weak pointer directly" );

    gc_set_object(new gcObjectAdapter<_Type>(other));
}

_GC_TEMPLATE _GC_SELF::gcPointer(_Type*&& other) : _PointerBase(){

    static_assert(!std::is_base_of<gcWeakPointer_B_,_PointerBase>::value,
                   "referencing object to weak pointer directly" );

    gc_set_object(new gcObjectAdapter<_Type>(std::move(other)));
}

_GC_TEMPLATE _GC_SELF::gcPointer(const _GC_SELF& other) : _PointerBase() {

    static_assert(std::is_base_of<gcSharedPointer_B_,_PointerBase>::value,
                  "unique pointer initialization from lvalue");

    gc_copy(other);
}

_GC_TEMPLATE _GC_SELF::gcPointer(_GC_SELF&& other) : _PointerBase() {
    gc_copy(std::move(other));
}

_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (_Type*const& other) {

    static_assert(std::is_base_of<gcSharedPointer_B_,_PointerBase>::value,
                  "unique pointer initialization from lvalue");

    static_assert(!std::is_base_of<gcWeakPointer_B_,_PointerBase>::value,
                   "referencing object to weak pointer directly" );

    gc_set_object(new gcObjectAdapter<_Type>(other));
    return *this;
}

_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (_Type*&& other) {

    static_assert(!std::is_base_of<gcWeakPointer_B_,_PointerBase>::value,
                   "referencing object to weak pointer directly" );

    gc_set_object(new gcObjectAdapter<_Type>(std::move(other)));
    return *this;
}

_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (const _GC_SELF& other) {    

    static_assert(std::is_base_of<gcSharedPointer_B_,_PointerBase>::value,
                  "unique pointer initialization from lvalue");

    gc_copy(other);
    return *this;
}

_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (_GC_SELF&& other) {
    gc_copy(std::move(other));
    return *this;
}

_GC_TEMPLATE _Type& _GC_SELF::operator*() {
    return *(static_cast<const gcObjectAdapter<_Type>*>(gc_get_object())->adaptee);
}

_GC_TEMPLATE _Type* _GC_SELF::operator->() {
    return static_cast<const gcObjectAdapter<_Type>*>(gc_get_object())->adaptee;
}

_GC_TEMPLATE _Type& _GC_SELF::operator[](int i) {
    return (static_cast<const gcObjectAdapter<_Type>*>(gc_get_object())->adaptee)[i];
}

_GC_TEMPLATE const _Type& _GC_SELF::operator*() const {
    return *(static_cast<const gcObjectAdapter<_Type>*>(gc_get_const_object())->adaptee);
}

_GC_TEMPLATE const _Type* _GC_SELF::operator->() const{
    return static_cast<const gcObjectAdapter<_Type>*>(gc_get_const_object())->adaptee;
}

_GC_TEMPLATE const _Type& _GC_SELF::operator[](int i) const {
    return (static_cast<const gcObjectAdapter<_Type>*>(gc_get_const_object())->adaptee)[i];
}


_GC_TEMPLATE template<class _OtherType, class _OtherPointerBase>
        _GC_SELF::operator gcPointer<_OtherType, _OtherPointerBase, false>()
{
    static_assert(std::is_convertible<_Type*,_OtherType*>::value,
                  "object type is not convertible to other object type");

    static_assert(!std::is_base_of<gcUniquePointer_B_,_OtherPointerBase>::value,
                  "unique pointer casting forbidden");

    static_assert(!std::is_base_of<gcUniquePointer_B_,_PointerBase>::value,
                  "unique pointer casting forbidden");

    gcPointer<_OtherType, _OtherPointerBase, false> tmp;
    tmp.gc_copy(*this);
    return tmp;
}

_GC_TEMPLATE template<class _OtherType, class _OtherPointerBase>
        _GC_SELF::operator const gcPointer<_OtherType, _OtherPointerBase, false>() const
{
    static_assert(std::is_convertible<_Type*,_OtherType*>::value,
                  "object type is not convertible to other object type");

    static_assert(!std::is_base_of<gcUniquePointer_B_,_OtherPointerBase>::value,
                  "unique pointer casting forbidden");

    static_assert(!std::is_base_of<gcUniquePointer_B_,_PointerBase>::value,
                  "unique pointer casting forbidden");

    gcPointer<_OtherType, _OtherPointerBase, false> tmp;
    tmp.gc_copy(*this);
    return tmp;
}

_GC_TEMPLATE
bool _GC_SELF::operator == (const _GC_SELF& other) const{
    return static_cast<const gcObjectAdapter<_Type>*>(gc_get_const_object())->adaptee ==
            static_cast<const gcObjectAdapter<_Type>*>(other.gc_get_const_object())->adaptee;
}

_GC_TEMPLATE
bool _GC_SELF::operator != (const _GC_SELF& other) const{
    return static_cast<const gcObjectAdapter<_Type>*>(gc_get_const_object())->adaptee !=
            static_cast<const gcObjectAdapter<_Type>*>(other.gc_get_const_object())->adaptee;
}

_GC_TEMPLATE
bool _GC_SELF::operator < (const _GC_SELF& other) const{
    return static_cast<const gcObjectAdapter<_Type>*>(gc_get_const_object())->adaptee <
            static_cast<const gcObjectAdapter<_Type>*>(other.gc_get_const_object())->adaptee;
}

_GC_TEMPLATE
bool _GC_SELF::operator > (const _GC_SELF& other) const{
    return static_cast<const gcObjectAdapter<_Type>*>(gc_get_const_object())->adaptee >
            static_cast<const gcObjectAdapter<_Type>*>(other.gc_get_const_object())->adaptee;
}

_GC_TEMPLATE
bool _GC_SELF::operator <= (const _GC_SELF& other) const{
    return static_cast<const gcObjectAdapter<_Type>*>(gc_get_const_object())->adaptee <=
            static_cast<const gcObjectAdapter<_Type>*>(other.gc_get_const_object())->adaptee;
}

_GC_TEMPLATE
bool _GC_SELF::operator >= (const _GC_SELF& other) const{
    return static_cast<const gcObjectAdapter<_Type>*>(gc_get_const_object())->adaptee >=
            static_cast<const gcObjectAdapter<_Type>*>(other.gc_get_const_object())->adaptee;
}

#endif
#undef _GC_SELF
#undef _GC_TEMPLATE

///////////////////////////////////////////////////////////////////////////////
// alias

template<class _Type> using gcSharedPointer = gcPointer <_Type, gcSharedPointer_B_>;
template<class _Type> using gcWeakPointer = gcPointer <_Type, gcWeakPointer_B_>;
template<class _Type> using gcScopedPointer = gcPointer <_Type, gcScopedPointer_B_<gcSharedPointer_B_>>;
template<class _Type> using gcUniquePointer = gcPointer <_Type, gcUniquePointer_B_>;
template<class _Type> using gcScopedUniquePointer = gcPointer <_Type, gcScopedPointer_B_<gcUniquePointer_B_>>;

}

#endif // _GC_POINTER_H
