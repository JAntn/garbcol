// File: gclist.h
// Description:
// stl list wrapper

#ifndef _GC_LIST_H
#define _GC_LIST_H
#include "gcsequence.h"

namespace collector {

template<class T> using gcList = gcSequenceAdapter < std::list, T > ;
template<class T> using gcListPointer = gcPointer < gcList< T >, true >;

}

#endif // _GC_LIST_H

