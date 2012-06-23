/*pop sort*/
typedef unsigned int uint32_t;
void PopSort(char* pBuffer, unsigned int nLen);
void InsertSort(char* pBuffer, unsigned int nLen);

// heap sort, inplace, o(nlgn), not-stable
void MaxHeapify(int* pBuffer, uint32_t nIndex, uint32_t nBufferSize);
void BuildMaxHeap(int* pBuffer, uint32_t nSize);
void PartialHeapSort(int* pBuffer, uint32_t nLen, uint32_t nPartialSize);
void HeapSort(int* pBuffer, unsigned int nLen);


void MinHeapify(int* pBuffer, uint32_t nIndex, uint32_t nBufferSize);
void BuildMinHeap(int* pBuffer, uint32_t nSize);
void ReverseHeapSort(int* pBuffer, unsigned int nLen);
void PartialMinHeapSort(int* pBuffer, uint32_t nLen, uint32_t nPartialSize);

