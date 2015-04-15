// File: gcobject.h
// Description:
// pointer contents class

#ifndef _GC_OBJECT_H
#define _GC_OBJECT_H

#include "gc.h"
#include "gcobjectbase.h"

namespace gcNamespace {

// Has a list of connections to gcPointerBase elements
class gcObjectScope {
public:

    // List of pointers connected to this context.
    gcScopeContainer                   childreen;

    gcObjectScope();
    ~gcObjectScope();
};

// Base class for classes which have gcPointerBase class members
class gcObject : public gcObject_B_{

    // Used in mark sweep algorithm
    unsigned char                       mark;

protected:

    // Inner scope of this object
    gcObjectScope                       object_scope;

    friend class gcConnectObject;
    friend class gcDisconnectObject;

public:

    gcObject();
    gcObject(gc_delegate_t);
    ~gcObject() override;

    void                                gc_mark() override;
    bool                                gc_is_marked() const override;

    void                                gc_make_unreachable() override;
    void                                gc_make_reachable() override;
    bool                                gc_is_reachable() const override;

    const gcContainer_B_*               gc_get_const_childreen() const override;

    void                                gc_make_nonfinalizable() override;
    void                                gc_make_finalizable() override;
    bool                                gc_is_finalizable() const override;

    void                                gc_make_lvalue() override;
    bool                                gc_is_lvalue() const override;

    void                                gc_deallocate() override;
    bool                                gc_is_finalized() const override;
    bool                                gc_is_safe_finalizable() const override;
    void                                gc_make_safe_finalizable() override;
};

// Manages gcObject constructor
class gcConnectObject{
public:

    gcConnectObject(gcObject*const);
    ~gcConnectObject();
};

// Manages gcObject destroy
class gcDisconnectObject{

    // Temporal pointer to parent
    gcObject*                           parent;

public:

    gcDisconnectObject(gcObject*const);
    ~gcDisconnectObject();

};


}

#endif // OBJECT
