// File: gccontainer.h
// Description:
// scontainer interfaces

#ifndef _GC_CONTAINER_H
#define _GC_CONTAINER_H

#include "gcpointer.h"

namespace collector{
	
	// iterator interface class
	class gcIteratorInterface{
	public:

		virtual								~gcIteratorInterface() = 0;
		virtual gcIteratorInterface*		gc_next() = 0;
        virtual const gcPointerBase*        gc_get_pointer() const = 0;
        virtual bool						gc_is_equal(gcIteratorInterface*const other) const = 0;
	};

	// container interface class
	class gcContainerInterface{
	public:

		virtual								~gcContainerInterface() = 0;
        virtual gcIteratorInterface*		gc_begin() = 0;
        virtual gcIteratorInterface*		gc_end() = 0;

	};

    class gcDequeContainerInterface: public gcContainerInterface{
    public:
        virtual								~gcDequeContainerInterface() = 0;
        virtual void						gc_push_back(gcPointerBase*const val) = 0;
        virtual void						gc_pop_back() = 0;
    };

	// iterator base class
	class gcScopeIterator : public gcIteratorInterface{
	public:

        typedef typename std::list<gcPointerBase*>::iterator Iterator;

        Iterator adaptee;

		virtual								~gcScopeIterator();
        virtual gcIteratorInterface*		gc_next();
        virtual const gcPointerBase*        gc_get_pointer() const;
        virtual bool						gc_is_equal(gcIteratorInterface*const other) const;
                                            gcScopeIterator(const Iterator&);

	};

	// container base class
	// It should have a complete set of methods of list 
    class gcScopeContainer : public gcDequeContainerInterface{
	public:

		std::list<gcPointerBase*> adaptee;

		virtual								~gcScopeContainer();
        virtual gcIteratorInterface*		gc_begin();
        virtual gcIteratorInterface*		gc_end();
		virtual void						gc_push_back(gcPointerBase*const val);
		virtual void						gc_pop_back();
	};

    // Allocator for pointer stl containers
    template<typename T>
    class gcContainerAllocator {
    public:

        typedef T value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

    public:

        // convert an allocator<T> to allocator<U>

        template<typename U>
        struct rebind {
            typedef gcContainerAllocator<U> other;
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

        template<typename U>
        inline explicit gcContainerAllocator(gcContainerAllocator<U> const&) {
            //nothing
        }

        // address

        inline pointer address(reference r) {
            return &r;
        }

        inline const_pointer address(const_reference r) {
            return &r;
        }

        // Memory allocation

        inline pointer allocate(size_type cnt,
            typename std::allocator<void>::const_pointer hint = 0) {
            return reinterpret_cast<pointer>(::operator new(cnt * sizeof(T)));
        }

        inline void deallocate(pointer p, size_type) {
            ::operator delete((void*)p);
        }

        // size
        inline size_type max_size() const {
            return std::numeric_limits<size_type>::max() / sizeof(T);
        }

        // construction/destruction
        template< class Node_, class ...Args_ >
        void construct( Node_* p, Args_&&...args){

            // push state
            bool tmp = _gc_scope_info->from_allocator;

            _gc_scope_info->from_allocator = true;
            ::new((void *)p) Node_(std::forward<Args_>(args)...);

            // restore state
            _gc_scope_info->from_allocator = tmp;
        }

        template<class Node_>
        inline void destroy(Node_* p) {

            // push state
            bool tmp = _gc_scope_info->from_allocator;

            _gc_scope_info->from_allocator = true;
            p->~Node_();

            // restore state
            _gc_scope_info->from_allocator = tmp;
        }

        inline bool operator==(gcContainerAllocator const&) {
            return true;
        }

        inline bool operator!=(gcContainerAllocator const& a) {
            return !operator==(a);
        }
    };
}

#endif
