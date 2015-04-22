///////////////////////////////////////////////////////////////////////////////
// File: gcobject.h
// Description:
// pointer contents

#ifndef _GC_OBJECT_H
#define _GC_OBJECT_H

#include "gc.h"
#include "gcobjectbase.h"

namespace gcNamespace {

///////////////////////////////////////////////////////////////////////////////
// scope operations

class gcObjectScope {

    friend class gcConnectObject;
    friend class gcDisconnectObject;
    friend class gcObject;

    // List of pointers connected to this scope
    gcScopeContainer                   childreen;

public:

    gcObjectScope();
    ~gcObjectScope();
};

///////////////////////////////////////////////////////////////////////////////
// a class that have gcPointer type members must be derived from this class

class gcObject : public gcObject_B_{

    unsigned char                       mark;

protected:

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

///////////////////////////////////////////////////////////////////////////////
// connect disconnet object to GC

class gcConnectObject{
public:

    gcConnectObject(gcObject*const);
    ~gcConnectObject();
};

class gcDisconnectObject{

    gcObject*                           parent;

public:

    gcDisconnectObject(gcObject*const);
    ~gcDisconnectObject();

};


}

#endif // _GC_OBJECT_H
