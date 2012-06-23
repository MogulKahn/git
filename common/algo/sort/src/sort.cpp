#include "sort.h"

#include <algorithm>

void PopSort(char* pBuffer, unsigned int nLen)
{
    bool bExit = false;
    for (int i = 0; i < nLen - 1; ++i)
    {
        bExit = true;
        for (int j = nLen - 1; j > i; --j)
        {
            if (pBuffer[j-1] > pBuffer[j])
            {
                std::swap(pBuffer[j - 1], pBuffer[j]);
                bExit = false;
            }
        }
        if (bExit)
            return;
        
    }
}


void InsertSort(char* pBuffer, unsigned int nLen)
{
    for (int i = 1; i < nLen; ++i)
    {
        char pin = pBuffer[i];
        int j;
        for (j = i - 1; j >=0; --j)
        {
           if (pBuffer[j] > pin)
           {
               pBuffer[j+1] = pBuffer[j];
           }
           else
           {
                pBuffer[j+1] = pin;
                break;
           }
        }
        if (j <0)
            pBuffer[0] = pin;
    }
}    

/*
0
1,2
3,4 5,6
7,8 9,10
*/

#define HEAP_LEFT_CHILD(index) ((index << 1) + 1)
#define HEAP_RIGHT_CHILD(index) ((index << 1) + 2)

void MaxHeapify(int* pBuffer, uint32_t nIndex, uint32_t nBufferSize)
{
    while (true)
    {
        uint32_t nLeft = HEAP_LEFT_CHILD(nIndex);
        uint32_t nRight = HEAP_RIGHT_CHILD(nIndex);
        uint32_t nLargest = nIndex;
        if ((nLeft < nBufferSize) && (pBuffer[nLeft] > pBuffer[nIndex]))
        {
            nLargest = nLeft;
        }

        if ((nRight < nBufferSize) && (pBuffer[nLeft] < pBuffer[nRight]))
        {
            nLargest = nRight;
        }
    
        if (nLargest != nIndex)
        {
            std::swap(pBuffer[nLargest], pBuffer[nIndex]);
            nIndex = nLargest;    
        }
        else
        {
            break;
        }
    }


    return;
}

void BuildMaxHeap(int* pBuffer, uint32_t nSize)
{
    for (int i = nSize/2; i >= 0; --i)
    {
        MaxHeapify(pBuffer, i, nSize);
    }
}


void HeapSort(int* pBuffer, unsigned int nLen)
{
    BuildMaxHeap(pBuffer, nLen);

    for (int i = nLen - 1; i > 0; --i)
    {
        std::swap(pBuffer[i], pBuffer[0]);
        MaxHeapify(pBuffer, 0, i);
    }
}

void PartialHeapSort(int* pBuffer, uint32_t nLen, uint32_t nPartialSize)
{
    BuildMaxHeap(pBuffer, nLen);

    for (int i = nLen - 1; i > nLen - nPartialSize - 1; --i)
    {
        std::swap(pBuffer[i], pBuffer[0]);
        MaxHeapify(pBuffer, 0, i);
    }

    for (int i = 0; i < nPartialSize + 1; ++i)
    {
        std::swap(pBuffer[i], pBuffer[nLen - 1 - i]);
    }
}


void MinHeapify(int* pBuffer, uint32_t nIndex, uint32_t nBufferSize)
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

void BuildMinHeap(int* pBuffer, uint32_t nSize)
{
    for (int i = nSize/2; i >= 0; --i)
    {
        MinHeapify(pBuffer, i, nSize);
    }
}


void ReverseHeapSort(int* pBuffer, unsigned int nLen)
{
    BuildMinHeap(pBuffer, nLen);

    for (int i = nLen - 1; i > 0; --i)
    {
        std::swap(pBuffer[i], pBuffer[0]);
        MinHeapify(pBuffer, 0, i);
    }
}


void PartialMinHeapSort(int* pBuffer, uint32_t nLen, uint32_t nPartialSize)
{
    BuildMinHeap(pBuffer, nLen);

    for (int i = nLen - 1; i > nLen - nPartialSize - 1; --i)
    {
        std::swap(pBuffer[i], pBuffer[0]);
        MinHeapify(pBuffer, 0, i);
    }

    for (int i = 0; i < nPartialSize + 1; ++i)
    {
        std::swap(pBuffer[i], pBuffer[nLen - 1 - i]);
    }
}


