// File: gcpointerbase.h
// Description:
// shared pointer class

#ifndef _GC_SHAREDPOINTER_H
#define _GC_SHAREDPOINTER_H

#include "gc.h"

namespace gcNamespace {

class gcObject_B_;


class gcSharedPointer_B_ : public gcPointer_B_{
protected:

    mutable gcObject_B_*                    object;

public:

    using gcPointer_B_::operator==;
    using gcPointer_B_::operator!=;
    using gcPointer_B_::operator<;
    using gcPointer_B_::operator>;
    using gcPointer_B_::operator<=;
    using gcPointer_B_::operator>=;

    ~gcSharedPointer_B_() override;
    gcSharedPointer_B_();

    bool                                    gc_is_empty() const override;

    void                                    gc_copy(const gcPointer_B_&) override;
    void                                    gc_set_object(gcObject_B_*const) override;

    gcObject_B_*                            gc_get_object() const override;
    const gcObject_B_*                      gc_get_const_object() const override;
    void                                    gc_deallocate() override;
    bool                                    gc_check_n_clear() const override;

    void                                    gc_mark() const override;
    bool                                    gc_is_marked() const override;

    const gcContainer_B_*                   gc_get_const_childreen() const override;

    void                                    gc_make_nonfinalizable() const override;
    void                                    gc_make_finalizable() const override;
    bool                                    gc_is_finalizable() const override;
    bool                                    gc_is_weak_pointer() const override;

};


}
#endif // _GC_SHAREDPOINTER_H
