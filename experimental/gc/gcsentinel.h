#ifndef _GC_SENTINEL_H_
#define _GC_SENTINEL_H_


#include "gc.h"

namespace gcNamespace{

class gcSentinelObject : public gcObject_B_ {
public:

    void                        gc_mark() {}
    bool                        gc_is_marked() const { return true; }

    void                        gc_make_unreachable() {}
    void                        gc_make_reachable() {}
    bool                        gc_is_reachable() const {return true;}

    gcContainer_B_*             gc_get_childreen() const {return nullptr;}
    const gcContainer_B_*       gc_get_const_childreen() const {return nullptr;}

    void                        gc_make_nonfinalizable() {}
    void                        gc_make_finalizable() {}
    bool                        gc_is_finalizable() const {return false;}

    void                        gc_make_lvalue() {}
    bool                        gc_is_lvalue() const {return false;}

    void                        gc_deallocate() {}
    bool                        gc_is_finalized() const {return false;}
    bool                        gc_is_safe_finalizable() const {return true;}
    void                        gc_make_safe_finalizable() {}

                                ~gcSentinelObject() {}
};

extern gcSentinelObject         _gc_sentinel;

#define gc_sentinel (&_gc_sentinel)

}

#endif // GCSENTINEL

