#ifndef _GC_SEQUENCE_H
#define _GC_SEQUENCE_H

#include <utility>
#include <functional>

#include "gccontainer.h"

namespace collector {


	// GC internal pointer iterator adapter

	template<class _Iterator>
    class gcSequenceIteratorAdapter : public gcIteratorInterface {
	public:

        _Iterator adaptee;

        virtual gcIteratorInterface*		gc_next();
        virtual const gcPointerBase*        gc_get_pointer() const;
        virtual bool						gc_is_equal(gcIteratorInterface*const other) const;
        virtual								~gcSequenceIteratorAdapter();
                                            gcSequenceIteratorAdapter(const _Iterator&);
	};

#ifndef _GC_HIDE_METHODS
	
	template<class _Iterator>
    gcSequenceIteratorAdapter<_Iterator>::gcSequenceIteratorAdapter(const _Iterator& other){
        adaptee = other;
	}

	template<class _Iterator>
    gcSequenceIteratorAdapter<_Iterator>::~gcSequenceIteratorAdapter(){
		//nothing
	}

	template<class _Iterator>
    gcIteratorInterface* gcSequenceIteratorAdapter<_Iterator>::gc_next() {
		++adaptee;
		return this;
	}

    template<class _Iterator>
    const gcPointerBase* gcSequenceIteratorAdapter<_Iterator>::gc_get_pointer() const {
        return &(*adaptee);
    }

	template<class _Iterator>
    bool gcSequenceIteratorAdapter<_Iterator>::gc_is_equal(gcIteratorInterface*const other)  const{
        return ((static_cast<gcSequenceIteratorAdapter<_Iterator>*>(other)->adaptee) == adaptee);
	}

#endif

    // GC internal pointer container adapter (used for list, deque, vector)
#define _GC_TEMPLATE	template < template<typename,typename> class  _Container, class _Type>
#define _GC_CONTAINER	_Container<gcPointer<_Type>, gcContainerAllocator<gcPointer<_Type>>>
#define _GC_ITERATOR	typename _GC_CONTAINER::iterator
#define _GC_SELF		gcSequenceAdapter<_Container, _Type>

	_GC_TEMPLATE
    class gcSequenceAdapter :  public gcObject, public gcContainerInterface {
	public:

        // !!Just most basic implementation of derived types yet

        typedef _GC_CONTAINER Container;
        typedef _GC_ITERATOR Iterator;

		_GC_CONTAINER adaptee;

        gcSequenceAdapter();
        virtual ~gcSequenceAdapter();

        virtual gcIteratorInterface*		gc_begin();
        virtual gcIteratorInterface*		gc_end();

        gcPointer<_Type>&              operator[](int);

        bool				operator== (const _GC_SELF& other) const;
        bool				operator!= (const _GC_SELF& other) const;
        bool				operator> (const _GC_SELF& other) const;
        bool				operator< (const _GC_SELF& other) const;
        bool				operator>= (const _GC_SELF& other) const;
        bool				operator<= (const _GC_SELF& other) const;
	};

#ifndef _GC_HIDE_METHODS

    _GC_TEMPLATE gcIteratorInterface* _GC_SELF::gc_begin(){
        return new gcSequenceIteratorAdapter<_GC_ITERATOR>(adaptee.begin());
    }

    _GC_TEMPLATE gcIteratorInterface* _GC_SELF::gc_end(){
        return new gcSequenceIteratorAdapter<_GC_ITERATOR>(adaptee.end());
	}

    _GC_TEMPLATE _GC_SELF::~gcSequenceAdapter() {

		gcDisconnectObject _gc_val(this);

		object_scope.childreen = 0;
	}

    _GC_TEMPLATE _GC_SELF::gcSequenceAdapter() : gcObject() {

		gcConnectObject _gc_val(this);

		delete object_scope.childreen;
		object_scope.childreen = this;
	}

    _GC_TEMPLATE gcPointer<_Type>& _GC_SELF::operator[](int i) {
        return adaptee[i];
    }

    _GC_TEMPLATE bool _GC_SELF::operator == (const _GC_SELF& other) const {
        return adaptee == other.adaptee;
    }

    _GC_TEMPLATE bool _GC_SELF::operator != (const _GC_SELF& other) const {
        return adaptee != other.adaptee;
    }

    _GC_TEMPLATE bool _GC_SELF::operator > (const _GC_SELF& other) const {
        return adaptee > other.adaptee;
    }

    _GC_TEMPLATE bool _GC_SELF::operator < (const _GC_SELF& other) const {
        return adaptee < other.adaptee;
    }

    _GC_TEMPLATE bool _GC_SELF::operator >= (const _GC_SELF& other) const {
        return adaptee >= other.adaptee;
    }

    _GC_TEMPLATE bool _GC_SELF::operator <= (const _GC_SELF& other) const {
        return adaptee <= other.adaptee;
    }

#endif
#undef _GC_TEMPLATE
#undef _GC_SELF
#undef _GC_CONTAINER
#undef _GC_ITERATOR

    // Class gcPointer specialization for gcContainerAdapter class parameter

#define _GC_TEMPLATE template< template<typename,typename> class _Container, class _Type>
#define	_GC_CONTAINER_ADAPTER gcSequenceAdapter<_Container, _Type>
#define _GC_SELF gcPointer<_GC_CONTAINER_ADAPTER, true>
#define _GC_CONTAINER _Container<gcPointer<_Type>, gcContainerAllocator<gcPointer<_Type>>>

	_GC_TEMPLATE
    class gcPointer< _GC_CONTAINER_ADAPTER, true>
			: public gcPointerBase{
	public:
		
        typedef _GC_CONTAINER Container;
		typedef typename _GC_CONTAINER::iterator Iterator;

		gcPointer();
        gcPointer(_GC_CONTAINER_ADAPTER*const other);
		gcPointer(const _GC_SELF& other);
		~gcPointer();

        _GC_SELF&			operator= (_GC_CONTAINER_ADAPTER*const other);
		_GC_SELF&			operator= (const _GC_SELF& other);
		_GC_CONTAINER&		operator*();
		_GC_CONTAINER*		operator->();

        template<class _Other> explicit operator gcPointer<_Other>();

        gcPointer<_Type>&              operator[] (int);

        bool				operator== (const _GC_SELF& other) const;
        bool				operator!= (const _GC_SELF& other) const;
        bool				operator< (const _GC_SELF& other) const;
        bool				operator> (const _GC_SELF& other) const;
        bool				operator<= (const _GC_SELF& other) const;
        bool				operator>= (const _GC_SELF& other) const;
	};

#ifndef _GC_HIDE_METHODS

// methods

	_GC_TEMPLATE _GC_SELF::gcPointer() : gcPointerBase() {}

    _GC_TEMPLATE _GC_SELF::gcPointer(_GC_CONTAINER_ADAPTER*const other) : gcPointerBase() {
		object = other;
	}

	_GC_TEMPLATE _GC_SELF::gcPointer(const _GC_SELF& other) : gcPointerBase() {
		object = other.object;
	}

	_GC_TEMPLATE _GC_SELF::~gcPointer() { 
        // nothing
    }

    _GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (_GC_CONTAINER_ADAPTER*const other) {
		object = other;
		return *this;
	}

	_GC_TEMPLATE _GC_SELF& _GC_SELF::operator = (const _GC_SELF& other) {
		object = other.object;
		return *this;
	}

	_GC_TEMPLATE _GC_CONTAINER& _GC_SELF::operator*() {
        return static_cast<_GC_CONTAINER_ADAPTER*>(object)->adaptee;
	}

	_GC_TEMPLATE _GC_CONTAINER* _GC_SELF::operator->() {
        return &static_cast<_GC_CONTAINER_ADAPTER*>(object)->adaptee;
	}

    _GC_TEMPLATE gcPointer<_Type>& _GC_SELF::operator[](int i) {
        return static_cast<_GC_CONTAINER_ADAPTER*>(object)->adaptee[i];
    }

	_GC_TEMPLATE
    template<class _Other> _GC_SELF::operator gcPointer<_Other>(){
        return gcPointer<_Other>(static_cast<_Other*>(object));
	}

    _GC_TEMPLATE bool _GC_SELF::operator == (const _GC_SELF& other) const {
        return object == other.object;
    }

    _GC_TEMPLATE bool _GC_SELF::operator != (const _GC_SELF& other) const {
        return object != other.object;
    }

    _GC_TEMPLATE bool _GC_SELF::operator < (const _GC_SELF& other) const {
        return object < other.object;
    }

    _GC_TEMPLATE bool _GC_SELF::operator > (const _GC_SELF& other) const {
        return object > other.object;
    }

    _GC_TEMPLATE bool _GC_SELF::operator <= (const _GC_SELF& other) const {
        return object <= other.object;
    }

    _GC_TEMPLATE bool _GC_SELF::operator >= (const _GC_SELF& other) const {
        return object >= other.object;
    }

#endif

#undef _GC_TEMPLATE
#undef _GC_CONTAINER_ADAPTER
#undef _GC_SELF
#undef _GC_CONTAINER


}
#endif
