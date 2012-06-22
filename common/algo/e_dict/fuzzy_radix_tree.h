#ifndef  FUZZY_RADIX_TREE_H___


#include <string>
#include <vector>
#include <algorithm>
#include <set>


typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef short int uint16_t;



enum EnglishDictItemType
{
    RANGE = 0x01,
    COMBINATION = 0x02,
    SINGLE = 0x04,
    PATHNODE = 0x08,
    DEL = 0x10,
    LAST_SIBLING = 0x20,
    NodeFlag_FirstSibling = 0x40,
    NodeFlag_Leaf = 0x80
};


// for service
class FuzzyEnglishDictItem
{
public:
    std::string m_word;
    char m_nFlag;
    uint16_t m_nCnt;    // if the count exceeds 65535. set it as 65535

    FuzzyEnglishDictItem* m_pParent;
    std::vector<FuzzyEnglishDictItem*> m_pChild;

    uint32_t m_nChildKeyCnt;
    uint32_t m_nChildPoiCnt;
    bool m_bHasRangeChild;

    FuzzyEnglishDictItem()
    {
        m_nChildKeyCnt = 0;
        m_nChildPoiCnt = 0;
        m_nCnt = 0;
        m_pParent = NULL;
        m_bHasRangeChild = false;
    }

    bool operator == (const FuzzyEnglishDictItem& r) const;
    bool operator < (const FuzzyEnglishDictItem& r) const;
};

class FuzzyEnglishDict
{
public:
    static void SplitEnglishWord(std::string& strKey, std::vector<std::string>& vEngWords);
public:
    void AddItem(const char* pChar, uint32_t nLen, uint32_t nCnt);

    void LoadDict();

    void Output();   

    void CollectStats(FuzzyEnglishDictItem* pItem);

    void OutputItem(FuzzyEnglishDictItem* pRoot, std::vector<FuzzyEnglishDictItem>& vNewItem);

    /**
     * @param OutputStream * pOutputStream [in]:
     * @return int -1 if failed
     */
    //int WriteDict(OutputStream* pOutputStream);
    /**
     * @param InputStream * pInputStream [in]:
     * @param FilePosition pos [in]:
     * @return int -1 if failed
     */
    //int LoadDict(InputStream* pInputStream, FilePosition pos);
   
    void BuildSecLevelIndex();

    uint32_t AddPrefixItem(std::set<FuzzyEnglishDictItem> &setCombinationItem);

    void BuildTree();

private:
    void Prune();
    std::pair<size_t, size_t> GetPrefixRange(size_t nStart, size_t nEnd);
private:
    std::vector<FuzzyEnglishDictItem> m_vItem;
};




#endif
