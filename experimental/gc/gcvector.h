// File: gcvector.h
// Description:
// stl vector wrapper

#ifndef _GC_VECTOR_H
#define _GC_VECTOR_H

#include "gcsequence.h"

namespace gcNamespace {

template<class _Type, class _ItemPointerBase = gcSharedPointer_B_>
using gcVector = gcSequenceAdapter < std::vector, _Type ,_ItemPointerBase> ;

template<class _Type, class _PointerBase = gcSharedPointer_B_, class _ItemPointerBase = gcSharedPointer_B_>
using gcVectorPointer = gcPointer < gcVector< _Type, _ItemPointerBase >, _PointerBase, true > ;

}
#endif // _GC_VECTOR_H

