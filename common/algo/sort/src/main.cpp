#include "lcs.h"
#include "sort.h"

#include <iostream>

void main()
{
    int p[] = {3,2,1,12,33,35,63,39,87,61,0};

    PartialMinHeapSort(p, sizeof(p) / sizeof(*p), 5);
    for ( int i = 0; i < sizeof(p) / sizeof(*p); ++i)
    {
        std::cout << p[i] << ",";
    }

    int i;
    std::cin >> i;

}