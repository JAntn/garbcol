#ifndef _GC_SCOPEDPOINTER_H
#define _GC_SCOPEDPOINTER_H

#include "gc.h"

namespace gcNamespace {

class gcObject_B_;

template<class _PointerBase>
class gcScopedPointer_B_ : public _PointerBase{
public:

    using _PointerBase::_PointerBase;

    ~gcScopedPointer_B_() override;
};

#ifndef _GC_HIDE_METHODS

// methods


template<class _PointerBase>
gcScopedPointer_B_<_PointerBase>::~gcScopedPointer_B_() {

    this->gc_get_object()->gc_deallocate();
    this->gc_set_object(0);
}


#endif

}
#endif // GCSCOPEDPOINTER

