#define _GC_HIDE_METHODS
#include "gcpointer.h"
#include "gccontainer.h"

namespace collector {

	gcPointerBase::gcPointerBase() {
		
		// Connect pointer to current scope
		// RAII method

		object = 0;
        if(!_gc_scope_info->from_allocator) {
            static_cast<gcDequeContainerInterface*>(_gc_scope_info->current_scope)->gc_push_back(this);
        }
	}

	gcPointerBase::gcPointerBase(const gcPointerBase& other) {
		object = other.object;
        if(!_gc_scope_info->from_allocator) {
            static_cast<gcDequeContainerInterface*>(_gc_scope_info->current_scope)->gc_push_back(this);
        }
	}

    gcPointerBase::~gcPointerBase() {

		// Disconnect pointer from scope
		// RAII method

        if(!_gc_scope_info->from_allocator) {
            static_cast<gcDequeContainerInterface*>(_gc_scope_info->current_scope)->gc_pop_back();
        }
	}

}

//end
