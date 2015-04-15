// File: gcpointerbase.h
// Description:
// base pointer class

#ifndef _GC_POINTERBASE_H
#define _GC_POINTERBASE_H

#include "gc.h"

namespace gcNamespace {

class gcObject_B_;
class gcNullPointer;

// Interface for pointer objects

class gcPointer_B_{
public:

    // Pointer contents

    virtual                             ~gcPointer_B_() {}

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
    virtual bool                        gc_check_n_clear() const = 0;

    bool                                operator== (const gcNullPointer&) const
                                        { return gc_get_const_object()==nullptr;}

    bool                                operator!= (const gcNullPointer&) const
                                        { return gc_get_const_object()!=nullptr;}

    bool                                operator== (const gcPointer_B_& other) const
                                        { return gc_get_const_object()==other.gc_get_const_object();}

    bool                                operator!= (const gcPointer_B_& other) const
                                        { return gc_get_const_object()!=other.gc_get_const_object();}

    bool                                operator< (const gcPointer_B_& other) const
                                        { return gc_get_const_object()<other.gc_get_const_object();}

    bool                                operator> (const gcPointer_B_& other) const
                                        { return gc_get_const_object()>other.gc_get_const_object();}

    bool                                operator<= (const gcPointer_B_& other) const
                                        { return gc_get_const_object()<=other.gc_get_const_object();}

    bool                                operator>= (const gcPointer_B_& other) const
                                        { return gc_get_const_object()>=other.gc_get_const_object();}

};

class gcNullPointer : public gcPointer_B_{
protected:

    gcObject_B_*                        gc_get_object() const override {return nullptr;}

public:

    ~gcNullPointer() override {}
    gcNullPointer(std::nullptr_t){}
    gcNullPointer(){}

    bool                                gc_is_empty() const override { return true;}
    void                                gc_copy(const gcPointer_B_&) override {}
    void                                gc_set_object(gcObject_B_*const) override {}
    const gcObject_B_*                  gc_get_const_object() const override {return nullptr;}
    void                                gc_mark() const override {}
    bool                                gc_is_marked() const override {return true;}
    const gcContainer_B_*               gc_get_const_childreen() const override { return nullptr;}
    void                                gc_make_nonfinalizable() const override {}
    void                                gc_make_finalizable() const override {}
    bool                                gc_is_finalizable() const override {return false;}
    bool                                gc_is_weak_pointer() const override {return true;}
    void                                gc_deallocate() override {}
    bool                                gc_check_n_clear() const override {return true;}

};



}
#endif // _GC_POINTERBASE_H

