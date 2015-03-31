// File: gcpointer.h
// Description:
// define smart pointer classes

#ifndef _GC_POINTER_H
#define _GC_POINTER_H

#include "gc.h"

namespace gcNamespace {

	// Base class for pointer objects
	class gcPointerBase{
    public:

        gcObject*                           object;

        // Pointer contents

        void                                gc_copy(const gcPointerBase&);
        void                                gc_set_object(gcObject*const);
        void                                gc_delete_object();

        void                                gc_make_persistent();
        void                                gc_make_collectable();

                                            gcPointerBase();
                                            gcPointerBase(gcObject*const);
                                            gcPointerBase(const gcPointerBase&);
        virtual                             ~gcPointerBase();
	};

	// 'Generic template' pointer class
    template<class _Type, bool B = std::is_convertible<_Type*, gcObject*>::value>
	class gcPointer;

	// Class specialization for gcObject class derived types
    template<class _Type>
    class gcPointer<_Type, true> : public gcPointerBase{
	public:

                                            gcPointer();
                                            gcPointer(_Type*const other);
                                            gcPointer(const gcPointer<_Type, true>& other);
                                            ~gcPointer() override;

        gcPointer<_Type, true>&             operator= (_Type*const other);
        gcPointer<_Type, true>&             operator= (const gcPointer<_Type, true>& other);

        _Type&                              operator*();
        const _Type&                        operator*() const;

        _Type*                              operator->();
        const _Type*                        operator->() const;

        _Type&                              operator[](int);
        const _Type&                        operator[](int) const;

        template<class _Other> explicit     operator gcPointer<_Other>();

        bool                                operator== (const gcPointer<_Type, true>& other) const;
        bool                                operator!= (const gcPointer<_Type, true>& other) const;
        bool                                operator< (const gcPointer<_Type, true>& other) const;
        bool                                operator> (const gcPointer<_Type, true>& other) const;
        bool                                operator<= (const gcPointer<_Type, true>& other) const;
        bool                                operator>= (const gcPointer<_Type, true>& other) const;

    };

#ifndef _GC_HIDE_METHODS

	// methods

    template<class _Type>
    gcPointer<_Type, true>::gcPointer() : gcPointerBase() {}

    template<class _Type>
    gcPointer<_Type, true>::gcPointer(_Type*const other) : gcPointerBase() {
        gc_set_object(other);
    }

    template<class _Type>
    gcPointer<_Type, true>::gcPointer(const gcPointer<_Type, true>& other) : gcPointerBase() {
        gc_copy(other);
	}

    template<class _Type>
    gcPointer<_Type, true>::~gcPointer() {
        //nothing
	}

    template<class _Type>
    gcPointer<_Type, true>& gcPointer<_Type, true>::operator = (_Type*const other) {
        gc_set_object(other);
        return *this;
	}

    template<class _Type>
    gcPointer<_Type, true>& gcPointer<_Type, true>::operator = (const gcPointer<_Type, true>& other) {
        gc_copy(other);
        return *this;
    }

    template<class _Type>
    _Type& gcPointer<_Type, true>::operator*() {
        return *static_cast<_Type*>(object);
    }

    template<class _Type>
    const _Type& gcPointer<_Type, true>::operator*() const {
        return *static_cast<_Type*>(object);
    }

    template<class _Type>
    _Type* gcPointer<_Type, true>::operator->() {
        return static_cast<_Type*>(object);
    }

    template<class _Type>
    const _Type* gcPointer<_Type, true>::operator->() const {
        return static_cast<_Type*>(object);
    }

    template<class _Type>
    _Type& gcPointer<_Type, true>::operator[](int i) {
        return static_cast<_Type*>(object)[i];
    }

    template<class _Type>
    const _Type& gcPointer<_Type, true>::operator[](int i) const {
        return static_cast<_Type*>(object)[i];
    }

    template<class _Type>
    template<class _Other> gcPointer<_Type, true>::operator gcPointer<_Other>(){
        return gcPointer<_Other>(static_cast<_Other*>(object));
    }

    template<class _Type>
    bool gcPointer<_Type, true>::operator== (const gcPointer<_Type, true>& other) const{
        return object == other.object;
	}

    template<class _Type>
    bool gcPointer<_Type, true>::operator!= (const gcPointer<_Type, true>& other) const{
        return object != other.object;
	}

    template<class _Type>
    bool gcPointer<_Type, true>::operator> (const gcPointer<_Type, true>& other) const{
        return object > other.object;
    }

    template<class _Type>
    bool gcPointer<_Type, true>::operator< (const gcPointer<_Type, true>& other) const{
        return object < other.object;
    }

    template<class _Type>
    bool gcPointer<_Type, true>::operator<= (const gcPointer<_Type, true>& other) const{
        return object <= other.object;
    }

    template<class _Type>
    bool gcPointer<_Type, true>::operator>= (const gcPointer<_Type, true>& other) const{
        return object >= other.object;
    }

#endif

    // Class specialization for non-gcObject derived types
    template<class _Type>
    class gcPointer<_Type, false> : public gcPointerBase{
    public:

                                            gcPointer();
                                            gcPointer(_Type*const other);
                                            gcPointer(const gcPointer<_Type, false>& other);
                                            ~gcPointer() override;

        gcPointer<_Type, false>&            operator= (_Type*const other);
        gcPointer<_Type, false>&            operator= (const gcPointer<_Type, false>& other);

        _Type&                              operator*();
        _Type*                              operator->();
        _Type&                              operator[](int);

        const _Type&                        operator*() const;
        const _Type*                        operator->() const;
        const _Type&                        operator[](int) const;

        template<class _Other> explicit     operator gcPointer<_Other>();

        bool                                operator== (const gcPointer<_Type, false>& other) const;
        bool                                operator!= (const gcPointer<_Type, false>& other) const;
        bool                                operator< (const gcPointer<_Type, false>& other) const;
        bool                                operator> (const gcPointer<_Type, false>& other) const;
        bool                                operator<= (const gcPointer<_Type, false>& other) const;
        bool                                operator>= (const gcPointer<_Type, false>& other) const;

    };

#ifndef _GC_HIDE_METHODS

    template<class _Type>
    gcPointer<_Type, false>::gcPointer() : gcPointerBase() {}


    template<class _Type>
    gcPointer<_Type, false>::gcPointer(_Type*const other) : gcPointerBase() {
        gc_set_object(new gcObjectAdapter<_Type>(other));
	}

    template<class _Type>
    gcPointer<_Type, false>::gcPointer(const gcPointer<_Type, false>& other) : gcPointerBase() {
        gc_copy(other);
	}

    template<class _Type>
    gcPointer<_Type, false>::~gcPointer() {
        //nothing
    }

    template<class _Type>
    gcPointer<_Type, false>& gcPointer<_Type, false>::operator = (_Type*const other) {
        gc_set_object(new gcObjectAdapter<_Type>(other));
		return *this;
	}

    template<class _Type>
    gcPointer<_Type, false>& gcPointer<_Type, false>::operator = (const gcPointer<_Type, false>& other) {
        gc_copy(other);
		return *this;
	}

    template<class _Type>
    _Type& gcPointer<_Type, false>::operator*() {
        return *(static_cast<const gcObjectAdapter<_Type>*>(object)->adaptee);
    }

    template<class _Type>
    _Type* gcPointer<_Type, false>::operator->() {
        return static_cast<const gcObjectAdapter<_Type>*>(object)->adaptee;
    }

    template<class _Type>
    _Type& gcPointer<_Type, false>::operator[](int i) {
        return (static_cast<const gcObjectAdapter<_Type>*>(object)->adaptee)[i];
    }

    template<class _Type>
    const _Type& gcPointer<_Type, false>::operator*() const {
        return *(static_cast<const gcObjectAdapter<_Type>*>(object)->adaptee);
    }

    template<class _Type>
    const _Type* gcPointer<_Type, false>::operator->() const{
        return static_cast<const gcObjectAdapter<_Type>*>(object)->adaptee;
    }

    template<class _Type>
    const _Type& gcPointer<_Type, false>::operator[](int i) const {
        return (static_cast<const gcObjectAdapter<_Type>*>(object)->adaptee)[i];
    }

    template<class _Type>
    template<class _Other> gcPointer<_Type, false>::operator gcPointer<_Other>(){
        return gcPointer<_Other>(
                    static_cast<_Other*>(
                    static_cast<gcObjectAdapter<_Type>*>(object)->adaptee));
    }

    template<class _Type>
    bool gcPointer<_Type, false>::operator == (const gcPointer<_Type, false>& other) const{
        return static_cast<const gcObjectAdapter<_Type>*>(object)->adaptee ==
               static_cast<const gcObjectAdapter<_Type>*>(other.object)->adaptee;
	}

    template<class _Type>
    bool gcPointer<_Type, false>::operator != (const gcPointer<_Type, false>& other) const{
        return static_cast<const gcObjectAdapter<_Type>*>(object)->adaptee !=
               static_cast<const gcObjectAdapter<_Type>*>(other.object)->adaptee;
	}

    template<class _Type>
    bool gcPointer<_Type, false>::operator < (const gcPointer<_Type, false>& other) const{
        return static_cast<const gcObjectAdapter<_Type>*>(object)->adaptee <
               static_cast<const gcObjectAdapter<_Type>*>(other.object)->adaptee;
    }

    template<class _Type>
    bool gcPointer<_Type, false>::operator > (const gcPointer<_Type, false>& other) const{
        return static_cast<const gcObjectAdapter<_Type>*>(object)->adaptee >
               static_cast<const gcObjectAdapter<_Type>*>(other.object)->adaptee;
    }

    template<class _Type>
    bool gcPointer<_Type, false>::operator <= (const gcPointer<_Type, false>& other) const{
        return static_cast<const gcObjectAdapter<_Type>*>(object)->adaptee <=
               static_cast<const gcObjectAdapter<_Type>*>(other.object)->adaptee;
    }

    template<class _Type>
    bool gcPointer<_Type, false>::operator >= (const gcPointer<_Type, false>& other) const{
        return static_cast<const gcObjectAdapter<_Type>*>(object)->adaptee >=
               static_cast<const gcObjectAdapter<_Type>*>(other.object)->adaptee;
    }

#endif

}

#endif // _GC_POINTER_H
