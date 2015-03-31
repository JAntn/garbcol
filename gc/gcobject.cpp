#define _GC_HIDE_METHODS
#include "gcobject.h"
#include "gccontainer.h"

namespace gcNamespace {

	// [1]
	// Called before gcObject constructor and before new pointer members are declared
	gcObjectScope::gcObjectScope() {

		// Create childreen list (without any scope management)
		childreen = new gcScopeContainer;

		// Push current context and set this one
		_gc_scope_info->current_scope_stack.push_back(_gc_scope_info->current_scope);
		_gc_scope_info->current_scope = childreen;
	}

	// [2]
	// Called inside object's constructor
	gcConnectObject::gcConnectObject(gcObject*const object){

		parent = object;

		// Continue at root scope at time of object construction (come out from object scope)
		_gc_scope_info->current_scope = _gc_scope_info->root_scope;
	}

	// [3]
	gcConnectObject::~gcConnectObject(){

		// Return to previous context of object's context
		_gc_scope_info->current_scope = _gc_scope_info->current_scope_stack.back();
		_gc_scope_info->current_scope_stack.pop_back();

        // Simulate object as rawmem
        parent->mark = 0;                     // set bits to zero
        parent->mark |= _gc_rvalue_bit;       // set bit to rvalue (means that it's not stored in a smart pointer yet)
	}

	// [4]
	// Called inside object's destructor
    // Actually, only prevents that destructor were at correct context
	gcDisconnectObject::gcDisconnectObject(gcObject*const object){

		parent = object;

		// Continue at root scope at time of object destruction (come out from object scope)
		_gc_scope_info->current_scope_stack.push_back(_gc_scope_info->current_scope);
		_gc_scope_info->current_scope = _gc_scope_info->root_scope;
	}

	gcDisconnectObject::~gcDisconnectObject(){

		// Change to object's context
		_gc_scope_info->current_scope = parent->object_scope.childreen;
	}

	// [5]
	// Called after gcObject destructor and before pointer members are deleted
	gcObjectScope::~gcObjectScope() {

		// Free child connections
		delete childreen;

		// Return to previous context
		_gc_scope_info->current_scope = _gc_scope_info->current_scope_stack.back();
		_gc_scope_info->current_scope_stack.pop_back();
	}

	gcObject::~gcObject() {
		// nothing
	}


}

//end
