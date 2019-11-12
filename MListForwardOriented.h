#include <algorithm>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <cassert>
#include <string>
#include <cstdlib>
#include <windows.h>
#include <stdio.h>
#ifdef _DEBUG
    #define DEBMList_t( list ) MList_t list( #list )

    #define DEB( code ) code
#else
    #define DEB( code )

    #define InitDEB( list )
#endif

const int NormCanary = -230 - 230 * 256 - 230 * 256 * 256 - 230 * 256 * 256 * 256;

typedef int ListElem_t;

const int BeginNumOfMember = 15;

const std::string DotPath = "\"C:\\Program Files (x86)\\Graphviz2.38\\bin\\dot.exe\"";

struct MList_t
    {
    DEB(int LCanary);
    ListElem_t* data;
    int* next;
    int* head;
    int* tail;        // to do fast "push back"
    bool sorted;      // 1 when data is equal to array
    int LSize;
    DEB(std::string LName);

    bool PushBack(ListElem_t PushingElem);

    bool InsertAfter(ListElem_t PushingElem, int RawPos);

    bool DeleteAfter(int Pos);

    ListElem_t elem(int Pos);

    bool SortList();

    bool ArrOfElems(ListElem_t* RetArr, int* PosOfElems, int SizeOfArrs);

    DEB(bool Verify());

    DEB(void LDUMP(int err));

    #ifdef _DEBUG
    MList_t( std::string name );
    #else
    MList_t();
    #endif
    ~MList_t();

    DEB(int RCanary);

    DEB(unsigned long long MAXHash());

    DEB(unsigned long long LHash);
    };

bool LResize(ListElem_t data, int* next, int* LSize);

int* SearchingEmpty(ListElem_t data, int* next, int* LSize);


#ifdef _DEBUG
MList_t::MList_t( std::string name )
    {
    data   = (ListElem_t*) calloc (BeginNumOfMember, sizeof(ListElem_t));
    next   = (int*) calloc (BeginNumOfMember, sizeof(int));
    head   = next + 4; // now we reserved 5 elems left from head
    tail   = next + 4;

    assert(data);
    assert(next);
    assert(head);
    assert(tail);// than it needs to be if (...) {errnum = 124124125125; LDUMP}

    LCanary = NormCanary;
    RCanary = NormCanary;
    sorted = 1;
    LSize  = BeginNumOfMember;
    LName = name;

    for(int i = 0; i < LSize; ++i)
        {
        *(next + i) = -1;
        }
    LHash = MList_t::MAXHash();
    return;
    }
#else
MList_t::MList_t()
    {
    data   = (ListElem_t*) calloc (BeginNumOfMember, sizeof(ListElem_t));
    next   = (int*) calloc (BeginNumOfMember, sizeof(int));
    head   = next + 4; // now we reserved 5 elems left from head
    tail   = next + 4;

    assert(data != NULL);
    assert(next != NULL);
    assert(head != NULL);
    assert(tail != NULL);// than it needs to be if (...) {errnum = 124124125125; LDUMP}

    sorted = 1;
    LSize  = BeginNumOfMember;

    for(int i = 0; i < LSize; ++i)
        {
        *(next + i) = -1;
        }

    return;
    }
#endif

bool LResize(ListElem_t* data, int* next, int* LSize)
    {
    assert(LSize != NULL);

    *LSize *= 2;
    next = (int*)        realloc(next, (*LSize));
    data = (ListElem_t*) realloc(data, (*LSize));

    assert(next != NULL);
    assert(data != NULL);

    for(int i = (*LSize)/2; i < (*LSize); ++i)
        {
        *(next + i) = -1;
        }
    return 1;
    }

int* SearchingEmpty(ListElem_t* data, int* next, int* LSize)
    {
    for(int i = 0; i < *LSize; ++i)
        {
        if(*(next + i) == -1)
            {
            return (next + i);
            }
        }
    LResize(data, next, LSize);

    return (next + (*LSize)/2);
    }

bool MList_t::PushBack(ListElem_t PushingElem)
    {
    DEB(MList_t::Verify());
    if(*tail == -1)
        {
        *(data + (tail - next)) = PushingElem;
        if(sorted == 0)
            {
            *tail = SearchingEmpty(data, next, &LSize) - next;
            tail = *tail + next;
            } else
            {
            if(tail - next + 1 == LSize)
                {
                LResize(data, next, &LSize);
                }
            *tail = tail - next + 1;
            ++tail;
            }

        DEB(LHash = MList_t::MAXHash());
        return 1;
        }
    return 0;
    }

bool MList_t::InsertAfter(ListElem_t PushingElem, int RawPos)
    {
    DEB(MList_t::Verify());
    int Pos;
    if(RawPos == -1)
        {
        Pos = -1;
        } else
        {
        if(sorted == 1)
            {
            Pos = head - next + RawPos;
            } else
            {
            int TmpP = 0;
            int* NowElem = head;
            while (TmpP != RawPos && *NowElem != -1)
                {
                NowElem = next + (*NowElem);
                ++TmpP;
                }
            Pos = NowElem - next;
            }
        }

    if(Pos + next >= tail)
        {
        MList_t::PushBack(PushingElem);
        return 1;
        }

    if(Pos == -1 && sorted == 1)
        {
        if(head == next)
            {
            LSize *= 2;
            ListElem_t* TmpArr1 = (ListElem_t*) calloc(LSize, sizeof(ListElem_t));
            for(int i = 0; i < (LSize/2); ++i)
                {
                *(TmpArr1 + i) = -1;
                }
            for(int i = (LSize/2); i < LSize; ++i)
                {
                *(TmpArr1 + i) = *(data + i - LSize/2);
                }
            free(data);
            data = TmpArr1;

            int* TmpArr2 = (int*) calloc(LSize, sizeof(int));
            for(int i = (LSize/2); i < LSize; ++i)
                {
                *(TmpArr2 + i) = *(next + i - LSize/2);
                }
            free(next);
            next = TmpArr2;
            }
        --head;
        *head = head + 1 - next;
        *(data + (head - next) )   = PushingElem;


        DEB(LHash = MList_t::MAXHash());
        return 1;
        }
    sorted = 0;
    int* NewElem = SearchingEmpty(data, next, &LSize);
    *NewElem = *(next + Pos);
    *(next + Pos) = NewElem - next;
    *(data + (NewElem - next)) = PushingElem;

    DEB(LHash = MList_t::MAXHash());
    return 1;
    }

bool MList_t::DeleteAfter(int Pos)
    {
    DEB(MList_t::Verify());
    int NowPos = 0;
    int* NowElem = head;
    while (NowPos != Pos && *NowElem != -1)
        {
        NowElem = next + (*NowElem);
        ++NowPos;
        }

    int* DeletingElem = 0;
    if(Pos == -1)
        {
        DeletingElem = head;
        head = *head + next;
        *DeletingElem = -1;
        } else
        {
        DeletingElem = next + *(next + Pos);
        *(next + Pos) = *DeletingElem;
        *DeletingElem = -1;
        }

    DEB(LHash = MList_t::MAXHash());
    return 1;
    }

MList_t::~MList_t()
    {
    DEB(MList_t::Verify());
    free(data);
    free(next);


    DEB(LHash = MList_t::MAXHash());
    LSize = -1; // it's like it's dead, like destructed you knawww
    return;
    }

ListElem_t MList_t::elem(int Pos)
    {
    DEB(MList_t::Verify());
    int NowPos = 0;
    int* NowElem = head;
    while (NowPos != Pos && *NowElem != -1)
        {
        NowElem = next + (*NowElem);
        ++NowPos;
        }

    return *(data + (NowElem - next));
    }

bool MList_t::ArrOfElems(ListElem_t* RetArr, int* PosOfElems, int SizeOfArrs)
    {
    DEB(MList_t::Verify());
    std::sort(PosOfElems, (PosOfElems + SizeOfArrs) );

    if(sorted == 0)
        {
        int  NowRet       = 0;
        int* NowElem      = head;
        int  NowNumOfElem = 0;
        while(NowRet < SizeOfArrs && NowElem != tail)
            {
            while(*(PosOfElems + NowRet) != NowNumOfElem)
                {
                ++NowNumOfElem;
                NowElem = *NowElem + next;
                }
            *(RetArr + NowRet) = *(data + (NowElem - next));
            ++NowRet;
            }
        if(NowRet == SizeOfArrs - 1)
            {
            return 1; // printed all elems
            } else
            {
            return 0; // didn't find something
            }
        } else
        {
        int NowPos = 0;
        for(int i = 0; i < SizeOfArrs; ++i)
            {
            *(RetArr + i) = *(data + (head - next) + i);
            if((*(head + i) ) == -1)
                {
                return 0;
                }
            }
        return 1;
        }
    }

bool MList_t::SortList()
    {
    DEB(MList_t::Verify());
    ListElem_t* TmpData = (ListElem_t*) calloc(LSize, sizeof(ListElem_t));
    int*        TmpNext =        (int*) calloc(LSize, sizeof(int));

    for(int i = 0; i < LSize; ++i)
        {
        *(TmpNext + i)= -1;
        }
    int* NowElem = head;
    int  i = head - next;
    head = TmpNext + i;
    while(*(NowElem) != -1)
        {
        *(TmpData + i) = *(data + (NowElem - next));
        *(TmpNext + i) = i + 1;
        NowElem = next + *NowElem;
        ++i;
        }
    tail = TmpNext + i;
    ListElem_t* TmpD = data;
    int*        TmpN = next;
    next = TmpNext;
    data = TmpData;
    free(TmpD);
    free(TmpN);

    sorted = 1;


    DEB(LHash = MList_t::MAXHash());

    return 1;
    }

#ifdef _DEBUG
    bool MList_t::Verify()
        {
        int err = 0;

        if(LCanary != RCanary || LCanary != NormCanary)
            {
            err = 5;
            MList_t::LDUMP(err); // err 5 :: something went on list's memory!
            }

        if(LHash != MList_t::MAXHash())
            {
            err = 4;
            MList_t::LDUMP(err); // err 4 :: list has been attacked!
            }

        int* NowElem = head;
        int NowPos   = 0;
        while (*NowElem != -1)
            {
            if(NowPos > LSize)
                {
                err = 1;
                MList_t::LDUMP(err); // err 1 :: list is circled!!!
                }
            NowElem = *NowElem + next;
            ++NowPos;
            }
        NowElem = next;

        for(int i = 0; i < LSize; ++i)
            {
            if(*NowElem == -1)
                {
                ++NowPos;
                }
            ++NowElem;
            }
        if(NowPos < LSize)
            {
            printf("Np and LS: %d %d\n", NowPos, LSize);
            err = 2;
            MList_t::LDUMP(err); // err 2 :: list has gap!
            }

        if(NowPos > LSize)
            {
            err = 3;
            MList_t::LDUMP(err); // err 3 :: list lost connection, how hz, but hz.
            }

        return 1; // if err 6 :: it's just printing the list
        }
#endif

#ifdef _DEBUG
    unsigned long long MList_t::MAXHash()
        {
        const unsigned long long MaxHashKey = 0x5BD1E995;

        const int ByteShift = 10;

        unsigned long long HashSum = 0;

        // initializing HashSum

        DEB(HashSum = MaxHashKey ^ LSize);

        ListElem_t tmp = 0;

        // adding all Data to hash

        int* NowFByte = (int*)&LCanary;

        while(NowFByte + 4 < (int*)&LHash)
            {
            DEB(tmp = *NowFByte);
            DEB(tmp *= MaxHashKey);
            DEB(tmp ^= tmp >> ByteShift);
            DEB(tmp *= MaxHashKey);

            DEB(HashSum *= MaxHashKey);
            DEB(HashSum ^= tmp);
            ++NowFByte;
            }

        return HashSum;
        }

#endif

#ifdef _DEBUG
    void MList_t::LDUMP(int err)
        {
        FILE* LDot;
        std::string DTime;
        std::string NewPath;

        time_t now = std::chrono::system_clock::to_time_t ( std::chrono::system_clock::now() );
        if(err == 6)
            {
            DTime = "debug";
            } else
            {
            DTime = "crit_err_";
            DTime += (err + '0');
            }
        DTime += "_";
        DTime += LName;
        DTime += "_";
        DTime += ctime(&now);
        DTime += ".dot";
        DTime.erase(DTime.find('\n'), 1);
        int tmp = DTime.find(' ');
        while(tmp != std::string::npos)
            {
            //DTime.erase(DTime.find(' '), 1);
            DTime[DTime.find(' ')] = '_';
            tmp = DTime.find(' ');
            }
        tmp = DTime.find(':');
        while(tmp != std::string::npos)
            {
            //DTime.erase(DTime.find(':'), 1);
            DTime[DTime.find(':')] = '_';
            tmp = DTime.find(':');
            }
        assert(LDot != NULL);

        DTime.erase(DTime.find(".dot"));
        std::string MKDIR = "mkdir " + DTime;
        std::system(MKDIR.c_str());
        NewPath = DTime;
        NewPath += "\\";
        DTime += ".dot";
        NewPath += DTime;

        LDot = fopen(NewPath.c_str(), "w");
        fprintf(LDot, "digraph G{\n");
        int* NowElem = head;
        int NowPos = 0;
        fprintf(LDot, "rankdir=LR;\n");

        fprintf(LDot, "subgraph clusterlist {\n");
        while(*NowElem != -1 && NowPos < LSize)
            {
            fprintf(LDot, "%d [shape=record, label=\"ElemPointer:\\n%d | {Position\\n:%d | Data:\\n%d | Next:\\n%d}\"];\n", NowPos, NowElem, NowPos, *(data + (NowElem - next)), *NowElem);
            fprintf(LDot, "%d [shape=record, label=\"ElemPointer:\\n%d | {Position\\n:%d | Data:\\n%d | Next:\\n%d}\"];\n", (NowPos + 1), (*NowElem + next), *(data + *NowElem), *(next + *NowElem));
            fprintf(LDot, "%d->%d\n", NowPos, (NowPos + 1));
            NowElem = *NowElem + next;
            ++NowPos;
            }
        fprintf(LDot, "label = \"List with name: %s\"}\n", LName.c_str());


        fprintf(LDot, "}\n");
        fclose(LDot);


        std::string DotDoDot;
        DotDoDot += DotPath;
        DotDoDot += " -Tpng ";
        DotDoDot += NewPath;
        DotDoDot += " -o ";
        NewPath.erase(NewPath.rfind(".dot"));
        DotDoDot += NewPath;
        DotDoDot += ".png";
        std::system(DotDoDot.c_str());

        if(err == 6)
            {
            NewPath.replace(NewPath.rfind("debug"), 5, "LMems");
            } else
            {
            std::string CritStr = "crit_err_";
            CritStr += (err + '0');
            NewPath.replace(NewPath.rfind(CritStr), sizeof(CritStr), "LMems");
            }
        NewPath += ".dot";
        LDot = fopen(NewPath.c_str(), "w");
        assert(LDot != NULL);


        fprintf(LDot, "digraph G{\n");
        fprintf(LDot, "data [shape=record,label=\"");
        fprintf(LDot, "{Memory of list: %s} | {{DataPointer:\\n%d | Data:\\n%d | NextPointer:\\n%d | Next:\n%d}\n", LName.c_str(), data, *(data), next, *(next));
        for(int i = 1; i < LSize; ++i)
            {
            fprintf(LDot, "| {DataPointer:\\n%d | Data:\\n%d | NextPointer:\\n%d | Next:\\n%d}\n", data + i, *(data + i), next + i, *(next + i));
            }
        fprintf(LDot, "}\"];\n");
        fprintf(LDot, "Shild [shape=record,label=\" RCanary:\\n%d | LCanary:\\n%d | Hash:\\n%lld | NormalCanary:\\n%d\"];\n", RCanary, LCanary, LHash, NormCanary);
        fprintf(LDot, "Shild->data\n}\n");
        fclose(LDot);
        std::string DotDoData;
        DotDoData += DotPath;
        DotDoData += " -Tpng ";
        DotDoData += NewPath;
        DotDoData += " -o ";
        NewPath.erase(NewPath.find(".dot"));
        DotDoData += NewPath;
        DotDoData += ".png";
        std::system(DotDoData.c_str());

        //Sleep(1);
        return;
        }
#endif
