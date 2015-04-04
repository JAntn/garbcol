// File: gccontainer.h
// Description:
// scontainer interfaces

#ifndef _GC_CONTAINER_H
#define _GC_CONTAINER_H

#include <deque>

#include "gc.h"

namespace gcNamespace{

// iterator interface class
class gcIterator_B_{
public:

    virtual								~gcIterator_B_() = 0;
    virtual const gcPointer_B_*         gc_get_const_pointer() const = 0;       // PROTECTION: Garbage collector can't modify pointer objects
    virtual gcIterator_B_*              gc_next() = 0;
    virtual bool						gc_is_equal(const gcIterator_B_* other) const = 0;
};

// container interface class
class gcContainer_B_{
public:

    virtual								~gcContainer_B_() = 0;

    virtual gcIterator_B_*              gc_begin() = 0;
    virtual gcIterator_B_*              gc_begin() const = 0;

    virtual gcIterator_B_*              gc_end() = 0;
    virtual gcIterator_B_*              gc_end() const = 0;

};

// scope iterator
template<class _Iterator>
class gcScopeIterator : public gcIterator_B_{

    _Iterator                           adaptee;

public:

    ~gcScopeIterator() override;

    gcIterator_B_*                      gc_next() override;
    const gcPointer_B_*                 gc_get_const_pointer() const override;

    bool                                gc_is_equal(const gcIterator_B_* other) const override;

    gcScopeIterator(const _Iterator&);

};


// scope container
class gcScopeContainer : public gcContainer_B_ {

    std::deque<const gcPointer_B_*>           adaptee;

public:

    typedef typename std::deque<const gcPointer_B_*>::iterator iterator;
    typedef typename std::deque<const gcPointer_B_*>::const_iterator const_iterator;
    typedef typename std::deque<const gcPointer_B_*> container;


    ~gcScopeContainer() override;

    gcIterator_B_*                      gc_begin() override;
    gcIterator_B_*                      gc_begin() const override;

    gcIterator_B_*                      gc_end() override;
    gcIterator_B_*                      gc_end() const override;

    virtual void                        gc_push_back(const gcPointer_B_* val);
    virtual void                        gc_pop_back();
};

// Allocator for pointer stl containers
template<typename _Type>
class gcContainerAllocator {
public:

    typedef _Type value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

public:

    // convert an allocator<_Type> to allocator<_Other>

    template<typename _Other>
    struct rebind {
        typedef gcContainerAllocator<_Other> other;
    };

public:

    // still investigating

    inline explicit gcContainerAllocator() {
        //nothing
    }

    inline ~gcContainerAllocator() {
        //nothing
    }

    inline explicit gcContainerAllocator(gcContainerAllocator const&) {
        //nothing
    }

    template<typename _Other>
    inline explicit gcContainerAllocator(gcContainerAllocator<_Other> const&) {
        //nothing
    }

    // address

    inline pointer address(reference r) const{
        return &r;
    }

    inline const_pointer address(const_reference r) const{
        return &r;
    }

    // Memory allocation

    inline pointer allocate(
            size_type cnt,
            typename std::allocator<void>::const_pointer hint = 0)
    {
        return reinterpret_cast<pointer>(::operator new(cnt * sizeof(value_type)));
    }

    inline void deallocate(pointer p, size_type) {
        ::operator delete((void*)p);
    }

    // size
    inline size_type max_size() const {
        return std::numeric_limits<size_type>::max() / sizeof(value_type);
    }

    // construction/destruction
    template< class _Node, class ..._Args >
    void construct( _Node* p, _Args&&...args){

        // push state
        bool tmp = _gc_scope_info->from_allocator;

        _gc_scope_info->from_allocator = true;
        ::new((void *)p) _Node(std::forward<_Args>(args)...);

        // restore state
        _gc_scope_info->from_allocator = tmp;
    }

    template<class _Node>
    inline void destroy(_Node* p) {

        // push state
        bool tmp = _gc_scope_info->from_allocator;

        _gc_scope_info->from_allocator = true;
        p->~_Node();

        // restore state
        _gc_scope_info->from_allocator = tmp;
    }

    inline bool operator==(gcContainerAllocator const&) {
        return true;
    }

    inline bool operator!=(gcContainerAllocator const& a) {
        return false;
    }
};
}

#endif
