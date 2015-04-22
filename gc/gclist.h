///////////////////////////////////////////////////////////////////////////////
// File: gclist.h
// Description:
// STL list wrapper

#ifndef _GC_LIST_H
#define _GC_LIST_H

#include <list>
#include "gcsequence.h"

namespace gcNamespace {

template<class _Type, class _ItemPointerBase = gcSharedPointer_B_>
using gcList = gcSequenceAdapter < std::list, _Type ,_ItemPointerBase> ;

template<class _Type, class _PointerBase = gcSharedPointer_B_, class _ItemPointerBase = gcSharedPointer_B_>
using gcListPointer = gcPointer < gcList< _Type, _ItemPointerBase >, _PointerBase, true > ;

}

#endif // _GC_LIST_H

