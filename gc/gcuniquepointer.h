///////////////////////////////////////////////////////////////////////////////
// File: gcuniquepointer.h
// Description:
// unique pointer

#ifndef _GC_UNIQUEPOINTER_H
#define _GC_UNIQUEPOINTER_H

#include "gc.h"

namespace gcNamespace {

///////////////////////////////////////////////////////////////////////////////
// unique pointer base

class gcUniquePointer_B_ : public gcPointer_B_ {

    mutable gcObject_B_*                    object;

public:

    using gcPointer_B_::operator==;
    using gcPointer_B_::operator!=;
    using gcPointer_B_::operator<;
    using gcPointer_B_::operator>;
    using gcPointer_B_::operator<=;
    using gcPointer_B_::operator>=;

    ~gcUniquePointer_B_() override;
    gcUniquePointer_B_();

    bool                                    gc_is_empty() const override;

    void                                    gc_copy(const gcPointer_B_&) override;
    void                                    gc_set_object(gcObject_B_*const) override;

    gcObject_B_*                            gc_get_object() const override;
    const gcObject_B_*                      gc_get_const_object() const override;
    void                                    gc_deallocate() override;
    bool                                    gc_check_n_clear() const override;

    void                                    gc_mark() const override;
    bool                                    gc_is_marked() const override;

    const gcContainer_B_*                   gc_get_const_childreen() const override;

    void                                    gc_make_nonfinalizable() const override;
    void                                    gc_make_finalizable() const override;
    bool                                    gc_is_finalizable() const override;
    bool                                    gc_is_weak_pointer() const override;

};

// 'Generic template' unique pointer class
template<class _Type, bool _TypeIsObject = std::is_convertible<_Type*, gcObject_B_*>::value>
class gcUniquePointer;

#define _GC_SELF gcUniquePointer<_Type, true>
#define _GC_TEMPLATE template<class _Type>

///////////////////////////////////////////////////////////////////////////////
// specialization for gcObject types

_GC_TEMPLATE class gcUniquePointer<_Type, true> : public gcUniquePointer_B_ {
protected:
    using gcUniquePointer_B_::gc_get_object;

public:

    using gcUniquePointer_B_::gc_is_empty;
    using gcUniquePointer_B_::gc_copy;
    using gcUniquePointer_B_::gc_set_object;
    using gcUniquePointer_B_::gc_get_const_object;
    using gcUniquePointer_B_::gc_mark;
    using gcUniquePointer_B_::gc_is_marked;
    using gcUniquePointer_B_::gc_get_const_childreen;
    using gcUniquePointer_B_::gc_make_nonfinalizable;
    using gcUniquePointer_B_::gc_make_finalizable;
    using gcUniquePointer_B_::gc_is_finalizable;
    using gcUniquePointer_B_::gc_is_weak_pointer;
    using gcUniquePointer_B_::gc_deallocate;
    using gcUniquePointer_B_::gc_check_n_clear;
    using gcUniquePointer_B_::operator==;
    using gcUniquePointer_B_::operator!=;
    using gcUniquePointer_B_::operator<;
    using gcUniquePointer_B_::operator>;
    using gcUniquePointer_B_::operator<=;
    using gcUniquePointer_B_::operator>=;

    gcUniquePointer();
    gcUniquePointer(_Type*const other);
    gcUniquePointer(gcUniquePointer_B_&&);

    _GC_SELF&                           operator= (_Type*const other);

    _Type&                              operator*();
    const _Type&                        operator*() const;

    _Type*                              operator->();
    const _Type*                        operator->() const;

    _Type&                              operator[](int);
    const _Type&                        operator[](int) const;

};

#ifndef _GC_HIDE_METHODS


_GC_TEMPLATE _GC_SELF::gcUniquePointer() : gcUniquePointer_B_() {}

_GC_TEMPLATE _GC_SELF::gcUniquePointer(_Type*const other) : gcUniquePointer_B_() {
    gc_set_object(other);
}

_GC_TEMPLATE _GC_SELF::gcUniquePointer(gcUniquePointer_B_&& other) : gcUniquePointer_B_() {
    gc_set_object(other.gc_get_object());
}

_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (_Type*const other) {
    gc_set_object(other);
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

_GC_TEMPLATE _Type& _GC_SELF::operator[](int i) {
    return static_cast<_Type*>(gc_get_object())[i];
}

_GC_TEMPLATE const _Type& _GC_SELF::operator[](int i) const {
    return static_cast<_Type*>(gc_get_const_object())[i];
}

#endif
#undef _GC_SELF
#undef _GC_TEMPLATE

#define _GC_SELF gcUniquePointer<_Type, false>
#define _GC_TEMPLATE template<class _Type>

///////////////////////////////////////////////////////////////////////////////
// Class specialization for non-gcObject derived types

_GC_TEMPLATE class gcUniquePointer<_Type, false> : public gcUniquePointer_B_ {

public:

    using gcUniquePointer_B_::gc_is_empty;
    using gcUniquePointer_B_::gc_copy;
    using gcUniquePointer_B_::gc_set_object;
    using gcUniquePointer_B_::gc_get_object;
    using gcUniquePointer_B_::gc_get_const_object;
    using gcUniquePointer_B_::gc_mark;
    using gcUniquePointer_B_::gc_is_marked;
    using gcUniquePointer_B_::gc_get_const_childreen;
    using gcUniquePointer_B_::gc_make_nonfinalizable;
    using gcUniquePointer_B_::gc_make_finalizable;
    using gcUniquePointer_B_::gc_is_finalizable;
    using gcUniquePointer_B_::gc_is_weak_pointer;
    using gcUniquePointer_B_::gc_deallocate;
    using gcUniquePointer_B_::gc_check_n_clear;
    using gcUniquePointer_B_::operator==;
    using gcUniquePointer_B_::operator!=;
    using gcUniquePointer_B_::operator<;
    using gcUniquePointer_B_::operator>;
    using gcUniquePointer_B_::operator<=;
    using gcUniquePointer_B_::operator>=;

    gcUniquePointer();
    gcUniquePointer(_Type*const other);
    gcUniquePointer(gcUniquePointer_B_&&);

    _GC_SELF&                           operator= (_Type*const other);

    _Type&                              operator*();
    const _Type&                        operator*() const;

    _Type*                              operator->();
    const _Type*                        operator->() const;

    _Type&                              operator[](int);
    const _Type&                        operator[](int) const;

    bool                                operator== (const _GC_SELF& other) const;
    bool                                operator!= (const _GC_SELF& other) const;
    bool                                operator< (const _GC_SELF& other) const;
    bool                                operator> (const _GC_SELF& other) const;
    bool                                operator<= (const _GC_SELF& other) const;
    bool                                operator>= (const _GC_SELF& other) const;

};

#ifndef _GC_HIDE_METHODS


_GC_TEMPLATE _GC_SELF::gcUniquePointer() : gcUniquePointer_B_() {}

_GC_TEMPLATE _GC_SELF::gcUniquePointer(_Type*const other) : gcUniquePointer_B_() {
    gc_set_object(new gcObjectAdapter<_Type>(other));
}

_GC_TEMPLATE _GC_SELF::gcUniquePointer(gcUniquePointer_B_&& other) : gcUniquePointer_B_() {
    gc_copy(other);
}

_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (_Type*const other) {
    gc_set_object(new gcObjectAdapter<_Type>(other));
    return *this;
}

_GC_TEMPLATE _Type& _GC_SELF::operator*() {
    return *(static_cast<const gcObjectAdapter<_Type>*>(gc_get_object())->adaptee);
}

_GC_TEMPLATE const _Type& _GC_SELF::operator*() const {
    return *(static_cast<const gcObjectAdapter<_Type>*>(gc_get_const_object())->adaptee);
}

_GC_TEMPLATE _Type* _GC_SELF::operator->() {
    return static_cast<const gcObjectAdapter<_Type>*>(gc_get_object())->adaptee;
}

_GC_TEMPLATE const _Type* _GC_SELF::operator->() const {
    return static_cast<const gcObjectAdapter<_Type>*>(gc_get_const_object())->adaptee;
}

_GC_TEMPLATE _Type& _GC_SELF::operator[](int i) {
    return (static_cast<const gcObjectAdapter<_Type>*>(gc_get_object())->adaptee)[i];
}

_GC_TEMPLATE const _Type& _GC_SELF::operator[](int i) const {
    return (static_cast<const gcObjectAdapter<_Type>*>(gc_get_const_object())->adaptee)[i];
}

_GC_TEMPLATE
bool _GC_SELF::operator== (const _GC_SELF& other) const{
    return static_cast<const gcObjectAdapter<_Type>*>(gc_get_const_object())->adaptee ==
            static_cast<const gcObjectAdapter<_Type>*>(other.gc_get_const_object())->adaptee;}

_GC_TEMPLATE
bool _GC_SELF::operator!= (const _GC_SELF& other) const{
    return static_cast<const gcObjectAdapter<_Type>*>(gc_get_const_object())->adaptee !=
            static_cast<const gcObjectAdapter<_Type>*>(other.gc_get_const_object())->adaptee;}

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

}

#endif // _GC_UNIQUEPOINTER_H

