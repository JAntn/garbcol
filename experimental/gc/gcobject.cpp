#define _GC_HIDE_METHODS
#include "gc.h"

namespace gcNamespace {

// [1]
// Called before gcObject constructor and before new pointer members are declared
gcObjectScope::gcObjectScope() {

    // Create childreen list (without any scope management)
    childreen = new gcScopeContainer;

    // Push current context and set this one
    _gc_scope_info->current_scope_stack.push_back(_gc_scope_info->current_scope);
    _gc_scope_info->current_scope = childreen;
}

// [2]
// Called inside object's constructor
gcConnectObject::gcConnectObject(gcObject*const object){

    parent = object;

    // Continue at root scope at time of object construction (come out from object scope)
    _gc_scope_info->current_scope = _gc_scope_info->root_scope;
}

// [3]
gcConnectObject::~gcConnectObject(){

    // Return to previous context of object's context
    _gc_scope_info->current_scope = _gc_scope_info->current_scope_stack.back();
    _gc_scope_info->current_scope_stack.pop_back();
}

// [4]
// Called inside object's destructor
// Actually, only prevents that destructor were at correct context
gcDisconnectObject::gcDisconnectObject(gcObject*const object){

    parent = object;

    // Continue at root scope at time of object destruction (come out from object scope)
    _gc_scope_info->current_scope_stack.push_back(_gc_scope_info->current_scope);
    _gc_scope_info->current_scope = _gc_scope_info->root_scope;
}

gcDisconnectObject::~gcDisconnectObject(){

    // Change to object's context
    _gc_scope_info->current_scope = parent->object_scope.childreen;
}

// [5]
// Called after gcObject destructor and before pointer members are deleted
gcObjectScope::~gcObjectScope() {

    // Free child connections
    delete childreen;

    // Return to previous context
    _gc_scope_info->current_scope = _gc_scope_info->current_scope_stack.back();
    _gc_scope_info->current_scope_stack.pop_back();
}


// gcObject methods
// ----------------

gcObject::~gcObject() {
    // nothing
}

gcObject::gcObject() {
    mark = 0;
}

void gcObject::gc_mark() {
    mark &= ~_gc_mark_bit;
    mark |= _gc_collector->mark_bit;
}

bool gcObject::gc_is_marked() const {
    return (mark & _gc_mark_bit) == (_gc_collector->mark_bit & _gc_mark_bit);
}

void gcObject::gc_make_reachable() {
    mark &= ~_gc_unreachable_bit;
}

void gcObject::gc_make_unreachable() {
    mark |= _gc_unreachable_bit;
}

bool gcObject::gc_is_reachable() const {
    return !(mark & _gc_unreachable_bit);
}

gcContainer_B_* gcObject::gc_get_childreen() const {
    return object_scope.childreen;
}

const gcContainer_B_* gcObject::gc_get_const_childreen() const {
    return object_scope.childreen;
}

void gcObject::gc_make_finalizable() {
    mark &= ~_gc_nonfinalizable_bit;
}

void gcObject::gc_make_nonfinalizable() {
    mark |= _gc_nonfinalizable_bit;
}

bool gcObject::gc_is_finalizable() const {
    return !(mark & _gc_nonfinalizable_bit);
}

void gcObject::gc_make_lvalue() {
    mark |= _gc_lvalue_bit;
}

bool gcObject::gc_is_lvalue() const {
    return mark & _gc_lvalue_bit;
}

void gcObject::gc_deallocate(){    
    if (gc_is_finalizable()) {
        mark |= _gc_deallocate_bit;
    }
}

bool gcObject::gc_is_finalized() const {
    return mark & _gc_deallocate_bit;
}

bool gcObject::gc_is_safe_finalizable() const {
    return mark & _gc_deallocate_is_safe_bit;
}

void gcObject::gc_make_safe_finalizable() {
    mark |= _gc_deallocate_is_safe_bit;
}

}

//end
