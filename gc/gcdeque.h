///////////////////////////////////////////////////////////////////////////////
// File: gcdeque.h
// Description:
// STL deque wrapper

#ifndef _GC_DEQUE_H
#define _GC_DEQUE_H

#include <deque>
#include "gcsequence.h"

namespace gcNamespace {

template<class _Type, class _ItemPointerBase = gcSharedPointer_B_>
using gcDeque = gcSequenceAdapter < std::deque, _Type ,_ItemPointerBase> ;

template<class _Type, class _PointerBase = gcSharedPointer_B_, class _ItemPointerBase = gcSharedPointer_B_>
using gcDequePointer = gcPointer < gcDeque< _Type, _ItemPointerBase >, _PointerBase, true > ;

}

#endif // _GC_DEQUE_H

