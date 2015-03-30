// File: gcobject.h
// Description:
// pointer contents class

#ifndef _GC_OBJECT_H
#define _GC_OBJECT_H

#include "gc.h"

namespace collector {

	// Has a list of connections to gcPointerBase elements 
	class gcObjectScope {
	public:

        // List of pointers connected to this context.
        gcContainerInterface*               childreen;

                                            gcObjectScope();
                                            ~gcObjectScope();
	};

	// Base class for classes which have gcPointerBase class members
	class gcObject {
	public:

		// Used in mark sweep algorithm
        bool                                marked[2];

		// Inner scope of this object
        gcObjectScope                       object_scope;

		// Polymorphic members
        virtual                             ~gcObject();
	};

	// Manages gcObject constructor
	class gcConnectObject{
	public:

        // Temporal pointer to parent
        gcObject*                           parent;

                                            gcConnectObject(gcObject*const);
                                            ~gcConnectObject();
	};

	// Manages gcObject destroy
	class gcDisconnectObject{
	public:

        // Temporal pointer to parent
        gcObject*                           parent;

                                            gcDisconnectObject(gcObject*const);
                                            ~gcDisconnectObject();

	};

	// Adapter to use non-gcObject derived classes with pointers
    template<class _Type>
	class gcObjectAdapter : public gcObject{
	public:

        // Adaptee object
        _Type*                              adaptee;

                                            gcObjectAdapter(_Type*const);
                                            ~gcObjectAdapter() override;
	};


#ifndef _GC_HIDE_METHODS

	// methods

    template<class _Type>
    gcObjectAdapter<_Type>::gcObjectAdapter(_Type*const other) {
		gcConnectObject new_object(this);
		adaptee = other;
	}

    template<class _Type>
    gcObjectAdapter<_Type>::~gcObjectAdapter() {
		gcDisconnectObject delete_object(this);
		delete adaptee;
	}

#endif

}

#endif // OBJECT
