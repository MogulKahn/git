#include "datastructures\heap.h"


template <class Iterator>
void BuildMinHeap(Iterator itBegin, Iterator itEnd)
{
    for (int i = nSize/2; i >= 0; --i)
    {
        MinHeapify(itBegin, i, itEnd - itBegin);
    }
}

template <class Iterator>
void MinHeapify(Iterator pBuffer, uint32_t nIndex, uint32_t nBufferSize)
{
    while (true)
    {
        uint32_t nLeft = HEAP_LEFT_CHILD(nIndex);
        uint32_t nRight = HEAP_RIGHT_CHILD(nIndex);
        uint32_t nMinimum = nIndex;
        if ((nLeft < nBufferSize) && (pBuffer[nLeft] < pBuffer[nIndex]))
        {
            nMinimum = nLeft;
        }

        if ((nRight < nBufferSize) && (pBuffer[nLeft] > pBuffer[nRight]))
        {
            nMinimum = nRight;
        }
    
        if (nMinimum != nIndex)
        {
            std::swap(pBuffer[nMinimum], pBuffer[nIndex]);
            nIndex = nMinimum;    
        }
        else
        {
            break;
        }
    }


    return;
}