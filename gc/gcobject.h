#ifndef _GC_OBJECT_H
#define _GC_OBJECT_H

#include "gc.h"


// File: gcobject.h
// Description:
// Data managed by garbage collector
//
namespace collector {

	// Has a list of connections to gcPointerBase elements 
	class gcObjectScope {
	public:

		gcObjectScope();
		~gcObjectScope();

        // List of pointers connected to this context.
        gcContainerInterface* childreen;
	};

	// Base class for classes which have gcPointerBase class members
	class gcObject {
	public:

		// Used in mark sweep algorithm
		bool marked[2];

		// Inner scope of this object
		gcObjectScope object_scope;

		// Polymorphic members
		virtual						~gcObject();
	};

	// Manages gcObject constructor
	class gcConnectObject{
	public:

		gcObject* parent;

		gcConnectObject(gcObject*const);
		~gcConnectObject();
	};

	// Manages gcObject destroy
	class gcDisconnectObject{
	public:

		gcObject* parent;

		gcDisconnectObject(gcObject*const);
		~gcDisconnectObject();

	};

	// Adapter to use non-gcObject derived classes with pointers
	template<class T>
	class gcObjectAdapter : public gcObject{
	public:

		T* adaptee;

		gcObjectAdapter(T*const);
        virtual	~gcObjectAdapter();
	};


#ifndef _GC_HIDE_METHODS

	// methods

	template<class T>
	gcObjectAdapter<T>::gcObjectAdapter(T*const other) {
		gcConnectObject new_object(this);
		adaptee = other;
	}

	template<class T>
	gcObjectAdapter<T>::~gcObjectAdapter() {
		gcDisconnectObject delete_object(this);
		delete adaptee;
	}

#endif

}

#endif // OBJECT
