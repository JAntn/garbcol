#define _GC_HIDE_METHODS
#include "gccontainer.h"

namespace gcNamespace{

gcIteratorInterface::~gcIteratorInterface(){
    //nothing
}

gcContainerInterface::~gcContainerInterface(){
    //nothing
}

gcDequeContainerInterface::~gcDequeContainerInterface(){
    //nothing
}

gcScopeIterator::gcScopeIterator(const typename gcScopeIterator::Iterator& other){
    adaptee = other;
}

gcScopeIterator::~gcScopeIterator(){
    //nothing
}

gcIteratorInterface* gcScopeIterator::gc_next(){
    ++adaptee;
    return this;
}

const gcPointerBase* gcScopeIterator::gc_get_const_pointer() const {
    return *adaptee;
}

bool gcScopeIterator::gc_is_equal(gcIteratorInterface*const other) const{
    return (static_cast<gcScopeIterator*>(other)->adaptee == adaptee);
}

gcScopeContainer::~gcScopeContainer(){
    //nothing
}

gcIteratorInterface* gcScopeContainer::gc_begin() {
    return new gcScopeIterator(adaptee.begin());
}

gcIteratorInterface* gcScopeContainer::gc_end() {
    return new gcScopeIterator(adaptee.end());
}

void gcScopeContainer::gc_push_back(gcPointerBase*const val) {

    // Prevent collector while adding elements
    _GC_THREAD_LOCK;

    adaptee.push_back(val);
}

void gcScopeContainer::gc_pop_back() {

    // Prevent collector while removing elements
    _GC_THREAD_LOCK;

    adaptee.pop_back();
}

}
