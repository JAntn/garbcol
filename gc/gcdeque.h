// File: gcdeque.h
// Description:
// stl deque wrapper

#ifndef _GC_DEQUE_H
#define _GC_DEQUE_H

#include "gcsequence.h"

namespace gcNamespace {

template<class T> using gcDeque = gcSequenceAdapter < std::deque, T > ;
template<class T> using gcDequePointer = gcPointer < gcDeque< T >, true > ;

}

#endif // _GC_DEQUE_H

