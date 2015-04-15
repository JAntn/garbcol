#define _GC_HIDE_METHODS
#include "gc.h"

namespace gcNamespace{

gcIterator_B_::~gcIterator_B_(){
    //nothing
}

gcContainer_B_::~gcContainer_B_(){
    //nothing
}

template<class _Iterator>
gcScopeIterator<_Iterator>::gcScopeIterator(const _Iterator& other){
    adaptee = other;
}

template<class _Iterator>
gcScopeIterator<_Iterator>::~gcScopeIterator(){
    //nothing
}
template<class _Iterator>
gcIterator_B_* gcScopeIterator<_Iterator>::gc_next(){
    ++adaptee;
    return this;
}

template<class _Iterator>
const gcPointer_B_* gcScopeIterator<_Iterator>::gc_get_const_pointer() const {
    return *adaptee;
}

template<class _Iterator>
bool gcScopeIterator<_Iterator>::gc_is_equal(const gcIterator_B_* other) const{
    return (static_cast<const gcScopeIterator<_Iterator>*>(other)->adaptee == adaptee);
}

gcScopeContainer::~gcScopeContainer(){
    //nothing
}

gcIterator_B_* gcScopeContainer::gc_begin() {
    return new gcScopeIterator<typename gcScopeContainer::iterator>(adaptee.begin());
}

gcIterator_B_* gcScopeContainer::gc_begin() const {
    return new gcScopeIterator<typename gcScopeContainer::const_iterator>(adaptee.begin());
}

gcIterator_B_* gcScopeContainer::gc_end() {
    return new gcScopeIterator<typename gcScopeContainer::iterator>(adaptee.end());
}

gcIterator_B_* gcScopeContainer::gc_end() const {
    return new gcScopeIterator<typename gcScopeContainer::const_iterator>(adaptee.end());
}

void gcScopeContainer::gc_push_back(const gcPointer_B_ *val) {

    _GC_THREAD_LOCK;
    adaptee.push_back(val);
}

void gcScopeContainer::gc_pop_back() {

    _GC_THREAD_LOCK;
    adaptee.pop_back();
}

}
