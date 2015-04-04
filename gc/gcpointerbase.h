// File: gcpointerbase.h
// Description:
// base pointer class

#ifndef _GC_POINTERBASE_H
#define _GC_POINTERBASE_H

#include "gc.h"

namespace gcNamespace {

class gcObject_B_;

// Interface for pointer objects

class gcPointer_B_{
public:

    // Pointer contents

    virtual bool                        gc_is_empty() const = 0;

    virtual void                        gc_copy(const gcPointer_B_&) = 0;
    virtual void                        gc_set_object(gcObject_B_*const) = 0;

    virtual gcObject_B_*                gc_get_object() const = 0;
    virtual const gcObject_B_*          gc_get_const_object() const = 0;

    virtual void                        gc_mark() const = 0;
    virtual bool                        gc_is_marked() const = 0;

    virtual const gcContainer_B_*       gc_get_const_childreen() const = 0;

    virtual void                        gc_make_nonfinalizable() const = 0;
    virtual void                        gc_make_finalizable() const = 0;
    virtual bool                        gc_is_finalizable() const = 0;

    virtual bool                        gc_is_weak_pointer() const = 0;

    virtual void                        gc_deallocate() = 0;

    virtual                             ~gcPointer_B_() {}

};


}
#endif // _GC_POINTERBASE_H

