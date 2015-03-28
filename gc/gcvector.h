#ifndef _GC_VECTOR_H
#define _GC_VECTOR_H

#include "gcsequence.h"

namespace collector {

template<class T> using gcVector = gcSequenceAdapter < std::vector, T >;
template<class T> using gcVectorPointer = gcPointer < gcVector< T >, true >;

}
#endif // _GC_VECTOR_H

