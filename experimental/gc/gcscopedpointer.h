#ifndef _GC_SCOPEDPOINTER_H
#define _GC_SCOPEDPOINTER_H

#include "gc.h"

namespace gcNamespace {

class gcObject_B_;

template<class _PointerBase>
class gcScopedPointer_B_ : public _PointerBase{
public:

    using _PointerBase::_PointerBase;
    using _PointerBase::gc_is_empty;
    using _PointerBase::gc_copy;
    using _PointerBase::gc_set_object;
    using _PointerBase::gc_get_object;
    using _PointerBase::gc_get_const_object;
    using _PointerBase::gc_mark;
    using _PointerBase::gc_is_marked;
    using _PointerBase::gc_get_childreen;
    using _PointerBase::gc_get_const_childreen;
    using _PointerBase::gc_make_nonfinalizable;
    using _PointerBase::gc_make_finalizable;
    using _PointerBase::gc_is_finalizable;
    using _PointerBase::gc_is_weak_pointer;
    using _PointerBase::gc_deallocate;
    using _PointerBase::gc_check_n_clear;
    using _PointerBase::operator==;
    using _PointerBase::operator!=;
    using _PointerBase::operator<;
    using _PointerBase::operator>;
    using _PointerBase::operator<=;
    using _PointerBase::operator>=;
    using _PointerBase::gc_push_snapshot;
    using _PointerBase::gc_pop_snapshot;

    ~gcScopedPointer_B_() override;
};

#ifndef _GC_HIDE_METHODS

template<class _PointerBase>
gcScopedPointer_B_<_PointerBase>::~gcScopedPointer_B_() {
    gc_get_object()->gc_deallocate();
}

#endif

}
#endif // GCSCOPEDPOINTER

