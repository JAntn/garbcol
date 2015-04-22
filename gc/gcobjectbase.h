///////////////////////////////////////////////////////////////////////////////
// File: gcobjectbase.h
// Description:
// object base

#ifndef _GC_OBJECTBASE_H
#define _GC_OBJECTBASE_H
namespace gcNamespace{

class gcContainer_B_;

///////////////////////////////////////////////////////////////////////////////
// most object top base class

class gcObject_B_ {
public:

    virtual void                        gc_mark() = 0;
    virtual bool                        gc_is_marked() const = 0;

    virtual void                        gc_make_unreachable() = 0;
    virtual void                        gc_make_reachable() = 0;
    virtual bool                        gc_is_reachable() const = 0;

    virtual const gcContainer_B_*       gc_get_const_childreen() const = 0;

    virtual void                        gc_make_nonfinalizable() = 0;
    virtual void                        gc_make_finalizable() = 0;
    virtual bool                        gc_is_finalizable() const = 0;

    virtual void                        gc_make_lvalue() = 0;
    virtual bool                        gc_is_lvalue() const = 0;

    virtual void                        gc_deallocate() = 0;
    virtual bool                        gc_is_finalized() const = 0;
    virtual bool                        gc_is_safe_finalizable() const = 0;
    virtual void                        gc_make_safe_finalizable() = 0;

    virtual                             ~gcObject_B_() {}
};

}

#endif // _GC_OBJECTBASE_H

