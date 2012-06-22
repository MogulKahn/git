#include "cassert"
#include "fuzzy_radix_tree.h"
#include <stack>
#include <fstream>


#define MAX_RANGE_POI_COUNT (80)
#define MAX_ATTRIBUTE_KEY_CNT (5)

#define IS_UTF8(c) (0 != (c & 0x80))

uint32_t GetMatchedLen(const char* pStrX, uint32_t nLenX, const char* pStrY, uint32_t nLenY)
{
    uint32_t nLen = 0;
    for (int i = 0; (i < nLenX) && (i < nLenY); ++i)
    {
        if (pStrX[i] == pStrY[i])
            ++nLen;
        else
            break;
    }

    return nLen;
}

bool IsPrefix(const std::string& prefix, const std::string& basestr)
{
    if (prefix.size() > basestr.size())
        return false;

    uint32_t nLen = GetMatchedLen(prefix.c_str(), prefix.size(), basestr.c_str(), basestr.size());
    return (nLen == prefix.size());
}

void FuzzyEnglishDict::SplitEnglishWord(std::string& strKey, std::vector<std::string>& vEngWords)
{
    std::string strNewString;
    for (size_t i = 0; i < strKey.size(); ++i)
    {
        if (IS_UTF8(strKey[i]))
        {
            strNewString.push_back(strKey[i]);
        }
        else if (isalnum(strKey[i]))
        {
            strKey[i] = toupper(strKey[i]);
            size_t nDigitBeg = i;
            while ((i < strKey.size()) && !IS_UTF8(strKey[i]) && isalnum(strKey[i]))
            {
                strKey[i] = toupper(strKey[i]);
                ++i;
            }

            vEngWords.push_back(std::string(strKey.c_str() + nDigitBeg, strKey.c_str() + i));
            --i;
        }
    }
    strNewString.swap(strKey);
}

void FuzzyEnglishDict::AddItem(const char* pChar, uint32_t nLen, uint32_t nCnt)
{
    m_vItem.push_back(FuzzyEnglishDictItem());
    m_vItem.back().m_word.assign(pChar, pChar + nLen);
    if (nCnt > 65535)
    {
        m_vItem.back().m_nCnt = 65535;
    }
    else
    {
        m_vItem.back().m_nCnt = nCnt;
    }
    m_vItem.back().m_nFlag = SINGLE;
}

void FuzzyEnglishDict::BuildSecLevelIndex()
{
    if (m_vItem.empty())
    {
        return;
    }
    BuildTree();
    //
    // calculate combination
    std::set<FuzzyEnglishDictItem> setCombinationItem;
    for (size_t nIndex = 0; nIndex < m_vItem.size(); ++nIndex)
    {
        FuzzyEnglishDictItem* pCur = &m_vItem[nIndex];
        std::pair<uint32_t, uint32_t> range = GetPrefixRange(nIndex, m_vItem.size());
        if ((range.second - range.first) > MAX_ATTRIBUTE_KEY_CNT)
        {
            pCur->m_nFlag = pCur->m_nFlag | COMBINATION;
            setCombinationItem.insert(*pCur);
        }
    }
    Output();

    // add fake item, this kind of English word is not a single, but are prefixes of more than MAX_ATTRIBUTE_KEY_CNT of item
    while (true)
    {
        if (AddPrefixItem(setCombinationItem) <= 0)
            break;
    }
    Output();
}


uint32_t FuzzyEnglishDict::AddPrefixItem(std::set<FuzzyEnglishDictItem> &setCombinationItem)
{
    std::set<FuzzyEnglishDictItem> setFakeItem;
    uint32_t nCommonPrefixItemCnt = 0;
    FuzzyEnglishDictItem prefixItem;
    prefixItem.m_nFlag = COMBINATION;
    for (size_t nIndex = 1; nIndex < m_vItem.size(); ++nIndex)
    {
        FuzzyEnglishDictItem* pCur = &m_vItem[nIndex];
        if (0 != (pCur->m_nFlag & COMBINATION))
        {
            nCommonPrefixItemCnt = 0;
            prefixItem.m_word.clear();
            continue;
        }

        // calculate common prefix
        uint32_t nCommonLen;
        if (prefixItem.m_word.size() == 0)
        {
            FuzzyEnglishDictItem* pLastItem = &m_vItem[nIndex - 1];
            nCommonLen = GetMatchedLen(pCur->m_word.c_str(), pCur->m_word.size(), pLastItem->m_word.c_str(), pLastItem->m_word.size());
            nCommonPrefixItemCnt = 2;
        }
        else
        {
            nCommonLen = GetMatchedLen(pCur->m_word.c_str(), pCur->m_word.size(), prefixItem.m_word.c_str(), prefixItem.m_word.size());
            ++nCommonPrefixItemCnt;
        }

        prefixItem.m_word = pCur->m_word;
        prefixItem.m_word.resize(nCommonLen);
        if (nCommonLen == 0)
        {
            nCommonPrefixItemCnt = 0;
        }
        else
        {
            std::vector<FuzzyEnglishDictItem>::iterator it = std::lower_bound(m_vItem.begin(), m_vItem.end(), prefixItem);
            if (it != m_vItem.end() && (it->m_word == prefixItem.m_word))
            {
                nCommonPrefixItemCnt = 0;
                prefixItem.m_word.clear();
            }
            else
            {
                if (nCommonPrefixItemCnt >= MAX_ATTRIBUTE_KEY_CNT)
                {
                    setFakeItem.insert(prefixItem);
                    nCommonPrefixItemCnt = 1;
                }
            }
        }
    }

    m_vItem.insert(m_vItem.end(), setFakeItem.begin(), setFakeItem.end());
    std::sort(m_vItem.begin(), m_vItem.end());
    return setFakeItem.size();
}

void FuzzyEnglishDict::Prune()
{
    std::vector<FuzzyEnglishDictItem> vNewItem;
    for (int nIndex = m_vItem.size() - 1; nIndex > 0; --nIndex)
    {
        FuzzyEnglishDictItem* pCurItem = &m_vItem[nIndex];
        FuzzyEnglishDictItem* pLastItem = &m_vItem[nIndex-1];
        if ((pCurItem->m_nCnt > MAX_RANGE_POI_COUNT) || ((pLastItem->m_nCnt + pCurItem->m_nCnt) > MAX_RANGE_POI_COUNT))
        {
            vNewItem.push_back(*pCurItem);
        }
        else
        {
            uint32_t nLen = GetMatchedLen(pCurItem->m_word.c_str(), pCurItem->m_word.size(),
                                            pLastItem->m_word.c_str(), pLastItem->m_word.size());
            if (nLen == 0)
            {
                vNewItem.push_back(*pCurItem);
            }
            else
            {
                bool bInsert = true;
                for (int i = (int)(vNewItem.size() - 1); i >= 0; --i)
                {
                    FuzzyEnglishDictItem* pCurRange = &vNewItem[i];
                    bool isPrefix = IsPrefix(pLastItem->m_word, pCurRange->m_word);
                   
                    if (!isPrefix)
                    {
                        break;
                    }
                    if (pCurItem->m_nFlag & RANGE)
                    {
                        vNewItem.push_back(*pCurItem);
                        bInsert = false;
                        break;
                    }
                }
                if (bInsert)
                {
                    pLastItem->m_nFlag = RANGE;
                    pLastItem->m_nCnt += pCurItem->m_nCnt;
                }
            }
        }
    }
    vNewItem.push_back(m_vItem.front());
    std::reverse(vNewItem.begin(), vNewItem.end());

    vNewItem.swap(m_vItem);
}

std::pair<size_t, size_t> FuzzyEnglishDict::GetPrefixRange(size_t nPrefixIndex, size_t nEnd)
{
    for (size_t nIndex = nPrefixIndex + 1; nIndex < nEnd; ++nIndex)
    {
        if (GetMatchedLen(m_vItem[nPrefixIndex].m_word.c_str(), m_vItem[nPrefixIndex].m_word.size(),
                            m_vItem[nIndex].m_word.c_str(), m_vItem[nIndex].m_word.size()) 
                            != m_vItem[nPrefixIndex].m_word.size())
        {
            return std::make_pair(nPrefixIndex + 1, nIndex);
        }
    }
    return std::make_pair(nPrefixIndex + 1, nEnd);
}

void FuzzyEnglishDict::LoadDict()
{
    char pBuffer[256];
    std::ifstream ifs("D:/e_dict/dict_all.csv");
    FuzzyEnglishDictItem item;
    item.m_nFlag = SINGLE;
    while (!ifs.eof())
    {
        ifs.getline(pBuffer, 256);
        if (strlen(pBuffer) != 0)
        {
            const char* pPos = strchr(pBuffer, ',');
            item.m_word.assign(pBuffer, pPos);
            item.m_nCnt = atoi(pPos + 1);
            m_vItem.push_back(item);
            memset(pBuffer, 0, 256);
        }
    }
    ifs.close();
}

void FuzzyEnglishDict::BuildTree()
{
    std::string prefix;
    uint32_t nIndex = 0;
    FuzzyEnglishDictItem* pRoot = new FuzzyEnglishDictItem;
    pRoot->m_pParent = NULL;
    
    FuzzyEnglishDictItem* pLastItem = &m_vItem[nIndex];
    pRoot->m_pChild.push_back(pLastItem);
    pLastItem->m_pParent = pRoot;

    for (nIndex = 1; nIndex < m_vItem.size(); ++nIndex)
    {
        if (IsPrefix(pLastItem->m_word, m_vItem[nIndex].m_word))
        {
            pLastItem->m_pChild.push_back(&m_vItem[nIndex]);
            m_vItem[nIndex].m_pParent = pLastItem;
            pLastItem = &m_vItem[nIndex];
        }
        else
        {
            pLastItem = pLastItem->m_pParent;
            if (pLastItem == NULL)
            {
                assert(false);
                break;
            }
            if (pLastItem == pRoot)
            {
                pRoot->m_pChild.push_back(&m_vItem[nIndex]);
                m_vItem[nIndex].m_pParent = pLastItem;
                pLastItem = &m_vItem[nIndex];
            }
            else
            {
                --nIndex;
            }
                
        }
    }

    CollectStats(pRoot);

    // output items
    std::vector<FuzzyEnglishDictItem> vNewItem;
    
    for (size_t i = 0; i < pRoot->m_pChild.size(); ++i)
    {
        OutputItem(pRoot->m_pChild[i], vNewItem);
    }
    vNewItem.swap(m_vItem);
    Output();


    delete pRoot;
}

void FuzzyEnglishDict::CollectStats(FuzzyEnglishDictItem* pCurNode)
{
    pCurNode->m_nChildKeyCnt = 1;
    pCurNode->m_nChildPoiCnt = pCurNode->m_nCnt;
    bool bRange = false;
    bool bDirectRange = false;
    for (size_t i = 0; i < pCurNode->m_pChild.size(); ++i)
    {
        CollectStats(pCurNode->m_pChild[i]);
        pCurNode->m_nChildKeyCnt += pCurNode->m_pChild[i]->m_nChildKeyCnt;
        pCurNode->m_nChildPoiCnt += pCurNode->m_pChild[i]->m_nChildPoiCnt;
        if (pCurNode->m_pChild[i]->m_nChildPoiCnt > MAX_RANGE_POI_COUNT)
            pCurNode->m_bHasRangeChild = true;
        if (pCurNode->m_pChild[i]->m_nFlag & RANGE)
        {
            bDirectRange = true;
        }
        if (pCurNode->m_pChild[i]->m_bHasRangeChild)
            bRange = true;
    }
    if (bRange)
    {
        pCurNode->m_bHasRangeChild = true;
    }

    if (!pCurNode->m_pChild.empty())
    {
        if (!bRange && (pCurNode->m_nChildPoiCnt < MAX_RANGE_POI_COUNT))
        {
            pCurNode->m_nFlag |= RANGE;
            pCurNode->m_nChildKeyCnt = 1;
            pCurNode->m_nCnt = pCurNode->m_nChildPoiCnt;
        }
        else
        {
            for (int nIndex = pCurNode->m_pChild.size() - 1; nIndex > 0; --nIndex)
            {
                FuzzyEnglishDictItem* pCurItem = pCurNode->m_pChild[nIndex];
                //if (pCurItem->m_bHasRangeChild )
                FuzzyEnglishDictItem* pLastItem = pCurNode->m_pChild[nIndex-1];
                if (pCurItem->m_bHasRangeChild || pLastItem->m_bHasRangeChild)
                    continue;
                if ((pCurItem->m_nCnt > MAX_RANGE_POI_COUNT) || ((pLastItem->m_nCnt + pCurItem->m_nCnt) > MAX_RANGE_POI_COUNT))
                {
                    ;
                }
                else
                {
                    uint32_t nLen = GetMatchedLen(pCurItem->m_word.c_str(), pCurItem->m_word.size(),
                                                    pLastItem->m_word.c_str(), pLastItem->m_word.size());
                    if (nLen == 0)
                    {
                        ;
                    }
                    else
                    {
                        pCurNode->m_bHasRangeChild = true;
                        pCurItem->m_nFlag |= DEL;
                        pLastItem->m_nFlag |= RANGE;
                        pLastItem->m_nCnt += pCurItem->m_nCnt;
                    }
                }
            }
        }
    }

}

void FuzzyEnglishDict::OutputItem(FuzzyEnglishDictItem* pRoot, std::vector<FuzzyEnglishDictItem>& vNewItem)
{
    FuzzyEnglishDictItem* pCurNode = pRoot;

    if (pCurNode->m_nFlag & DEL)
        return;

    
    if (pCurNode->m_nFlag & RANGE)
    {
        pCurNode->m_nFlag = RANGE;
    }
    vNewItem.push_back(*pCurNode);
    
    if (pCurNode->m_nFlag & RANGE)
    {
        pCurNode->m_nFlag = RANGE;
        return;
    }

    for (size_t i = 0; i < pCurNode->m_pChild.size(); ++i)
    {
        OutputItem(pCurNode->m_pChild[i], vNewItem);
    }
}

void FuzzyEnglishDict::Output()
{
    std::ofstream ofs("d:/dict.csv");
    for (size_t i = 0; i < m_vItem.size(); ++i)
    {
        ofs << m_vItem[i].m_word << "," << m_vItem[i].m_nCnt << ",";
        if (m_vItem[i].m_nFlag & SINGLE)
        {
            ofs << "SINGLE";
        }
        ofs << ",";
        if (m_vItem[i].m_nFlag & COMBINATION)
        {
            ofs << "COMBINATION";
        }
        ofs << ",";
        if (m_vItem[i].m_nFlag & RANGE)
        {
            ofs << "RANGE";
        }
        ofs << std::endl;
    }
    ofs.close();
}


bool FuzzyEnglishDictItem::operator==( const FuzzyEnglishDictItem& r ) const
{
    return (m_word != r.m_word);
}

bool FuzzyEnglishDictItem::operator<( const FuzzyEnglishDictItem& r ) const
{
    return m_word < r.m_word;
}


void main()
{
    FuzzyEnglishDict dict;
    dict.LoadDict();

    dict.BuildSecLevelIndex();
}
