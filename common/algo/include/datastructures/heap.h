#ifndef GALAXY_HEAP_H
#define GALAXY_HEAP_H

#include "common_def.h"

template <class Iterator>
void BuildMinHeap(Iterator itBegin, Iterator end);
template <class Iterator>
void MinHeapify(Iterator pBuffer, uint32_t nIndex, uint32_t nBufferSize);

#endif