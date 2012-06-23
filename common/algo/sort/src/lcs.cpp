#include "lcs.h"


std::string CalLCS(std::string x, std::string y)
{
    // initialize a table to record length
    char** cl = new char*[x.size() + 1];
    for (size_t nIndex = 0; nIndex < x.size() + 1; ++nIndex)
    {
        cl[nIndex] = new char[y.size()];
    }

    for (int i = 0; i < x.size(); ++i)
        cl[i][0] = 0;
    for (int i = 0; i < y.size(); ++i)
        cl[0][i] = 0;

    for (int i = 0; i < x.size(); ++i)
        for (int j = 0; j < y.size(); ++j)
        {
            if (x[i] == y[j])
                cl[i+1][j+1] = cl[i][j] + 1;
            else if (cl[i][j+1] >= cl[i+1][j])
                cl[i+1][j+1] = cl[i][j+1];
            else 
                cl[i+1][j+1] = cl[i+1][j];
        }

    int nLen = cl[x.size()][y.size()];

    std::string ret(nLen, '0');
    for (int i = x.size(), j=y.size(), z=nLen-1; i>0,j>0,z>=0;)
    {
        if (cl[i][j] == cl[i-1][j-1] +1)
        {
            ret[z--] = x[--i];
            --j;
        }else if(cl[i-1][j] >= cl[i][j-1])
        {
            --i;
        }else
        {
            --j;
        }
    }

    return ret;
}



bool Palindrome(const char* pszString, unsigned int nLen)
{
    if (0 == nLen)
        return false;
    if (1 == nLen)
        return true;

    for (int i=0, j=nLen-1; i<=j; i++,j--)
    {
        if (pszString[i] != pszString[j])
            return false;
    }
    return true;
}

