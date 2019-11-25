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
    #define DEBMDList_t( list ) MDList_t list( #list )

    #define DEB( code ) code
#else
    #define DEB( code )

    #define DEBMDList_t( list )
#endif

const int NormCanary = -230 - 230 * 256 - 230 * 256 * 256 - 230 * 256 * 256 * 256;

typedef int ListElem_t;

const int BeginNumOfMember = 15;

const std::string DotPath = "\"C:\\Program Files (x86)\\Graphviz2.38\\bin\\dot.exe\"";

struct MDList_t
    {
    DEB(int LCanary);
    ListElem_t* data;

    int* next;
    int* prev;
    DEB(int NumOfDumps);
    int* head;
    int* LFree;
    int* LFreeTail;   // has -2
    int* tail;        // to do fast "push back"     has -3
    bool sorted;      // 1 when data is equal to array
    int LSize;
    DEB(std::string LName);

    bool LResize();

    int* SearchingEmpty();

    bool PushBack(ListElem_t PushingElem);

    bool InsertAfter(ListElem_t PushingElem, int RawPos);

    bool InsertAfterRaw(ListElem_t PushingElem, int Pos);

    ListElem_t at(int RawPos); // raw position to element

    ListElem_t elem(int Pos);

    bool SortList();

    bool ArrOfElems(ListElem_t* RetArr, int* PosOfElems, int SizeOfArrs);

    bool DeleteElemRaw(int Pos);

    bool DeleteElem(int Pos);

    bool InsertElemRaw(ListElem_t PushingElem, int Pos);

    bool InsertBeforeRaw(ListElem_t PushingElem, int Pos);

    bool InsertElem(ListElem_t PushingElem, int Pos);

    bool InsertBefore(ListElem_t PushingElem, int Pos);

    DEB(bool Verify());

    DEB(void LDUMP(int err));

    #ifdef _DEBUG
    MDList_t( std::string name );
    #else
    MDList_t();
    #endif
    ~MDList_t();

    DEB(int RCanary);
    };
/**
    \brief Constructor of MDList_t
    \author andreevmaxi
	\version 1.0
	\date november  2019 year
	\copyright korobcom
    \details This is function that constracte our list
    \param[in] name string variable of name of our list
*/

#ifdef _DEBUG
MDList_t::MDList_t( std::string name )
    {
    NumOfDumps = 0;

    LSize  = BeginNumOfMember;

    data   = (ListElem_t*) calloc (BeginNumOfMember, sizeof(ListElem_t));
    next   = (int*) calloc (BeginNumOfMember, sizeof(int));
    prev   = (int*) calloc (BeginNumOfMember, sizeof(int));
    head   = next + 4; // now we reserved 5 elems left from head
    tail   = next + 4; // has -3
    LFree   = tail + 1;

    assert(data != NULL);
    assert(next != NULL);
    assert(head != NULL);
    assert(tail != NULL);
    assert(LFree != NULL);


    for(int i = 0; i < LSize; ++i)
        {
        *(next + i) = -1;
        *(prev + i) = -1;
        }
    for(int* i = (LFree + 1); i < (next + LSize); ++i)
        {
        *(i - 1) = i - next;
        }

    *(next + LSize - 1) = 0;
    for(int* i = (next + 1); i < (next + 4); ++i)
        {
        *(i - 1) = i - next;
        }
    LFreeTail = next + 3;
    *(LFreeTail) = -2;

    LCanary = NormCanary;
    RCanary = NormCanary;
    *tail = -3;
    sorted = 1;
    LName = name;
    return;
    }
#else
MDList_t::MDList_t()
    {
    LSize  = BeginNumOfMember;

    data   = (ListElem_t*) calloc (BeginNumOfMember, sizeof(ListElem_t));
    next   = (int*) calloc (BeginNumOfMember, sizeof(int));
    prev   = (int*) calloc (BeginNumOfMember, sizeof(int));
    head   = next + 4; // now we reserved 5 elems left from head
    tail   = next + 4;

    assert(data != NULL);
    assert(next != NULL);
    assert(head != NULL);
    assert(tail != NULL);// than it needs to be if (...) {errnum = 124124125125; LDUMP}
    assert(LFree != NULL);


    for(int i = 0; i < LSize; ++i)
        {
        *(next + i) = -1;
        *(prev + i) = -1;
        }

    for(int* i = (LFree + 1); i < (next + LSize); ++i)
        {
        *(i - 1) = i - next;
        }

    *(next + LSize - 1) = 0;
    for(int* i = (next + 1); i < (next + 4); ++i)
        {
        *(i - 1) = i - next;
        }
    LFreeTail = next + 3;
    *(LFreeTail) = -2;
    *tail = -3;
    sorted = 1;

    return;
    }
#endif
/**
    \brief LResize of MDList_t
    \author andreevmaxi
	\version 1.0
	\date november  2019 year
	\copyright korobcom
    \details This is function that resizes our list
    \return 1 if ok, 0 if bad
*/

bool MDList_t::LResize()
    {
    int TmpHead = head - next;
    int TmpTail = tail - next;
    int TmpLFree = LFree - next;
    int TmpLFreeTail = LFreeTail - next;

    LSize *= 2;
    next = (int*)        realloc(next, (LSize) * sizeof(int));
    data = (ListElem_t*) realloc(data, (LSize) * sizeof(ListElem_t));
    prev = (int*)        realloc(prev, (LSize) * sizeof(int));
    assert(next != NULL);
    assert(data != NULL);

    for(int i = (LSize)/2; i < (LSize); ++i)
        {
        *(next + i) = -1;
        *(prev + i) = -1;
        }
    int* NowFree;
    int SecondFree;
    LFreeTail = next + TmpLFreeTail;

    if(sorted == 1)
        {
        NowFree    = next + (LSize/2) - 1;
        SecondFree = *LFreeTail;
        }
    else
        {
        NowFree = LFreeTail;
        }
    *NowFree = (LSize)/2;
    for(int* i = next + (LSize)/2 + 1; i < (next + LSize); ++i)
        {
        *(i - 1) = i - next;
        }
    if(sorted == 1)
        {
        *(next + LSize - 1) = SecondFree;
        }
    else
        {
        *(next + LSize - 1) = -2;
        }
    LFreeTail = next + LSize - 1;
    head = next + TmpHead;
    tail = next + TmpTail;
    LFree = next + TmpLFree;
    return 1;
    }

/**
    \brief SearchingEmpty in MDList_t
    \author andreevmaxi
	\version 1.0
	\date november  2019 year
	\copyright korobcom
    \details This is function that searches new free elem in our list
    \return 1 if ok, 0 if bad
*/
int* MDList_t::SearchingEmpty()
    {
    if(LFree == (next - 3)) //because the last free element had next -3 in list of free elems
        {
        MDList_t::LResize();
        }
    int* Ans = LFree;
    LFree = *LFree + next;
    return (Ans);
    }

/**
    \brief PushBack of MDList_t
    \author andreevmaxi
	\version 1.0
	\date november  2019 year
	\copyright korobcom
    \details This is function that pushbackes in our list
    \return 1 if ok, 0 if bad
    \param[in] PushingElem element that pushes back in list
*/
bool MDList_t::PushBack(ListElem_t PushingElem)
    {
    DEB(MDList_t::Verify());
    if(*tail == -3)
        {
        *(data + (tail - next)) = PushingElem;
        if(sorted == 0)
            {
            *tail = MDList_t::SearchingEmpty() - next;
            int TmpPrev = tail - next;
            tail = *tail + next;
            *tail = -3;
            *(prev + (tail - next)) = TmpPrev;
            }
        else
            {
            if(tail - next + 2 == LSize)
                {
                MDList_t::LResize();
                }

            *tail = tail - next + 1;
            ++tail;
            *tail = -3;
            *(prev + (tail - next)) = (tail - next) - 1;
            ++LFree;
            }

        return 1;
        }
    return 0;
    }

/**
    \brief InsertAfter in MDList_t
    \author andreevmaxi
	\version 1.0
	\date november  2019 year
	\copyright korobcom
    \details This is function that inserts new elem after logic position of given elem in our list
    \return 1 if ok, 0 if bad
    \param[in] PushingElem Data of pushing element
    \param[in] Logic position of element after that is inserting new elem
*/
bool MDList_t::InsertAfter(ListElem_t PushingElem, int RawPos)
    {
    DEB(MDList_t::Verify());
    int Pos;
    if(RawPos == -1)
        {
        Pos = -1;
        }
    else
        {
        if(sorted == 1)
            {
            Pos = head - next + RawPos;
            if(Pos > tail - next)
                {
                Pos = tail - next;
                }
            }
        else
            {
            int TmpP = 0;
            int* NowElem = head;
            while (TmpP != RawPos && *NowElem != -3)
                {
                NowElem = next + (*NowElem);
                ++TmpP;
                }
            Pos = NowElem - next;
            }
        }
    MDList_t::InsertAfterRaw(PushingElem, Pos);
    return 1;
    }
/**
    \brief Destrucktor in MDList_t
    \author andreevmaxi
	\version 1.0
	\date november  2019 year
	\copyright korobcom
    \details This is function that destruckts our list
*/
MDList_t::~MDList_t()
    {
    DEB(MDList_t::Verify());
    free(data);
    free(next);
    free(prev);

    LSize = -1; // it's like it's dead, like destructed you knawww
    return;
    }

/**
    \brief elem in MDList_t
    \author andreevmaxi
	\version 1.0
	\date november  2019 year
	\copyright korobcom
    \details This is function that returns elem of our list
    \return element of logic position
    \param[in] Pos Logic position of element
*/
ListElem_t MDList_t::elem(int Pos)
    {
    DEB(MDList_t::Verify());
    int NowPos = 0;
    int* NowElem = head;

    if(sorted == 0)
        {
        while (NowPos != Pos && *NowElem != -1)
            {
            NowElem = next + (*NowElem);
            ++NowPos;
            }
        }
    else
        {
        if(head + Pos < tail)
            {
            NowElem = head + Pos;
            }
        }
    return *(data + (NowElem - next));
    }

/**
    \brief ArrOfElems in MDList_t
    \author andreevmaxi
	\version 1.0
	\date november 2019 year
	\copyright korobcom
    \details This is function that returns array of elems of our list
    \return 1 if ok, 0 if bad
    \param[out] RetArr pointer on array of returning elements
    \param[in] PosOfElems pointer on array of returning elements
    \param[in] SizeOfArrs size of our arrays
*/
bool MDList_t::ArrOfElems(ListElem_t* RetArr, int* PosOfElems, int SizeOfArrs)
    {
    DEB(MDList_t::Verify());
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
            }
        else
            {
            return 0; // didn't find something
            }
        }
    else
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

/**
    \brief SortList in MDList_t
    \author andreevmaxi
	\version 1.0
	\date november 2019 year
	\copyright korobcom
    \details This is function that sorts our list
    \return 1 if ok, 0 if bad
*/
bool MDList_t::SortList()
    {
    DEB(MDList_t::Verify());
    ListElem_t* TmpData = (ListElem_t*) calloc(LSize, sizeof(ListElem_t));
    int*        TmpNext =        (int*) calloc(LSize, sizeof(int));
    int*        TmpPrev =        (int*) calloc(LSize, sizeof(int));

    for(int i = 0; i < LSize; ++i)
        {
        *(TmpNext + i)= -1;
        *(TmpPrev + i)= -1;
        }
    int* NowElem = head;
    int  i = 0;
    head = TmpNext + i;
    *(TmpPrev + i) = -1;
    while(*(NowElem) != -3)
        {
        *(TmpData + i) = *(data + (NowElem - next));
        *(TmpNext + i) = i + 1;
        *(TmpPrev + i) = i - 1;
        NowElem = next + *NowElem;
        ++i;
        }
    tail = TmpNext + i;
    *(TmpPrev + i) = i - 1;
    *tail = -3;
    ++i;
    ListElem_t* TmpD = data;
    int*        TmpN = next;
    int*        TmpP = prev;
    next = TmpNext;
    data = TmpData;
    prev = TmpPrev;
    LFree = TmpNext + i;
    ++i;
    while(i < LSize)
        {
        *(TmpNext + i - 1) = i;
        *(TmpPrev + i - 1) = -1;
        ++i;
        }
    *(TmpPrev + i - 1) = -1;
    *(TmpNext + i - 1) = -2;
    LFreeTail = TmpNext + i - 1;
    free(TmpD);
    free(TmpN);
    free(TmpP);

    sorted = 1;


    return 1;
    }


/**
    \brief Verify in MDList_t
    \author andreevmaxi
	\version 1.0
	\date november 2019 year
	\copyright korobcom
    \details This is function that verifies our list
    \return 1 if ok, 0 if bad
*/
#ifdef _DEBUG
    bool MDList_t::Verify()
        {
        int err = 0;

        if(LCanary != RCanary || LCanary != NormCanary)
            {
            err = 4;
            MDList_t::LDUMP(err); // err 4 :: something went on list's memory!
            }

        int* NowElem = head;
        int NowPos   = 0;

        while (*NowElem != -3)
            {
            if(NowPos > LSize)
                {
                err = 1;
                MDList_t::LDUMP(err); // err 1 :: list is circled!!!
                }
            NowElem = *NowElem + next;
            ++NowPos;
            }

        NowElem = LFree;
        while (*NowElem != -2)
            {
            if(NowPos > LSize)
                {
                err = 5;
                MDList_t::LDUMP(err); // err 5 :: free is circled!!!
                }
            NowElem = *NowElem + next;
            ++NowPos;
            }

        if(NowPos < LSize - 2)
            {
            //printf("Np and LS: %d %d\n", NowPos, LSize);
            err = 2;
            MDList_t::LDUMP(err); // err 2 :: list has gap!
            }

        if(NowPos > LSize - 2)
            {
            err = 3;
            MDList_t::LDUMP(err); // err 3 :: list lost connection, how hz, but hz.
            }

        return 1; // if err 9 :: it's just printing the list
        }
#endif

/**
    \brief LDump in MDList_t
    \author andreevmaxi
	\version 1.0
	\date november 2019 year
	\copyright korobcom
    \details This is function that verifies our list
    err 1 :: list is circled!!!
    err 2 :: list has gap!
    err 3 :: list lost connection, how hz, but hz.
    err 4 :: something went on list's memory!
    err 5 :: free is circled!!!

    err 9 :: it's just printing the list
    \param[in] err it's number of your error
*/
#ifdef _DEBUG
    void MDList_t::LDUMP(int err)
        {
        FILE* LDot;
        std::string DTime;
        std::string NewPath;

        time_t now = std::chrono::system_clock::to_time_t ( std::chrono::system_clock::now() );
        if(err == 9)
            {
            DTime = "debug_duo";
            }
        else
            {
            std::string CritStr = "crit_err_duo_";
            char ErrStr[17];
            itoa(err, ErrStr, 10);
            CritStr += ErrStr;
            DTime += CritStr;
            }
        DTime += "_";
        DTime += LName;
        DTime += "_";
        DTime += ctime(&now);
        DTime += "#";
        char HashStr[17];
        ++NumOfDumps;
        itoa(NumOfDumps, HashStr, 10);
        DTime += HashStr;
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
            //DTime.erase(DTime.find(   ':'), 1);
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

        if(err == 9) // debug start of dump
            {
            NewPath.replace(NewPath.rfind("debug"), 5, "LMems");
            }
        else
            {
            std::string CritStr = "crit_err_";
            char ErrStr[17];
            itoa(err, ErrStr, 10);
            CritStr += ErrStr;
            NewPath.replace(NewPath.rfind(CritStr), sizeof(CritStr), "LMems");
            }
        LDot = fopen(NewPath.c_str(), "w");
        assert(LDot != NULL);


        fprintf(LDot, "digraph G{\n");
        fprintf(LDot, "    data [shape=record,label=\"");
        fprintf(LDot, "    {Memory of list: %s} | {{DataPointer:\\n%d | Data:\\n%d | NextPointer:\\n%d | Next:\\n%d | PhysPos:\\n0 | Prev:\\n%d}\n", LName.c_str(), data, *(data), next, *(next), *(prev));
        for(int i = 1; i < LSize; ++i)
            {
            fprintf(LDot, "    | {DataPointer:\\n%d | Data:\\n%d | NextPointer:\\n%d | Next:\\n%d | PhysPos:\\n%d | Prev:\\n%d}\n", data + i, *(data + i), next + i, *(next + i), i, *(prev + i));
            }
        fprintf(LDot, "    }\"];\n");
        fprintf(LDot, "    Shild [shape=record,label=\"{ RCanary:\\n%d | LCanary:\\n%d  | NormalCanary:\\n%d | Sorted:\\n%d } | { head:\\n%d | tail:\\n%d | LFree:\\n%d | LFreeTail:\\n%d} | { *head:\\n%d | *tail:\\n%d | *LFree:\\n%d | *LFreeTail:\\n%d}\"];\n", RCanary, LCanary, NormCanary, sorted, head, tail, LFree, LFreeTail, *head, *tail, *LFree, *LFreeTail);
        fprintf(LDot, "    Shild->data\n}\n");
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

        int NowPos = 0;
        int* NowElem = (int*)3;
        NewPath.replace(NewPath.rfind("LMems"), 5, "PhysModel");
        NewPath += ".dot";

        LDot = fopen(NewPath.c_str(), "w");
        fprintf(LDot, "digraph G{\n");

        fprintf(LDot, "    subgraph clusterlist {\n    style=filled;\n    color=lightgrey;\n");
        fprintf(LDot, "        rankdir=LR;\n");
        fprintf(LDot, "        Model [shape=record,style=\"filled\",fillcolor=\"mediumpurple\",label=\"{{<%d>} | {ElemPointer:\\n%d | {PhysPosition\\n:%d | Data:\\n%d | Next:\\n%d | Prev:\\n%d}}}", NowPos, NowPos + next, NowPos, *(data + NowPos), *(next + NowPos),*(prev + NowPos));

        ++NowPos;
        while(NowPos < LSize)
            {
            fprintf(LDot, " | {{<%d>} | {ElemPointer:\\n%d | {PhysPosition\\n:%d | Data:\\n%d | Next:\\n%d | Prev:\\n%d}}}", NowPos, NowPos + next, NowPos, *(data + NowPos), *(next + NowPos), *(prev + NowPos));
            ++NowPos;
            }
        fprintf(LDot, "\"];\n");
        NowElem = head;
        while(*NowElem != -3)
            {
            if( (NowElem - next) == *(prev + *NowElem) )
                {
                fprintf(LDot, "        Model:<%d>->Model:<%d>[color=\"green\";style=\"bold\";dir=\"both\"];\n", NowElem - next, *NowElem);
                }
            else
                {
                fprintf(LDot, "        Model:<%d>->Model:<%d>[color=\"green\";style=\"bold\"];\n", NowElem - next, *NowElem);
                }
            NowElem = *NowElem + next;
            }
        NowElem = LFree;
        while(*NowElem != -2)
            {
            fprintf(LDot, "        Model:<%d>->Model:<%d>[color=\"blue\";style=\"bold\"];\n", NowElem - next, *NowElem);
            NowElem = *NowElem + next;
            }
        fprintf(LDot, "    label = \"List with name: %s\"}\n", LName.c_str());

        fprintf(LDot, "}\n");
        fclose(LDot);


        std::string DotDoPhys;
        DotDoPhys += DotPath;
        DotDoPhys += " -Tpng ";
        DotDoPhys += NewPath;
        DotDoPhys += " -o ";
        NewPath.erase(NewPath.rfind(".dot"));
        DotDoPhys += NewPath;
        DotDoPhys += ".png";
        std::system(DotDoPhys.c_str());

        if(err == 9) // debug start of dump
            {
            NewPath.replace(NewPath.rfind("PhysModel"), 9, "debug");
            }
        else
            {
            std::string CritStr = "crit_err_";
            char ErrStr[17];
            itoa(err, ErrStr, 10);
            CritStr += ErrStr;
            NewPath.replace(NewPath.rfind("PhysModel"), 9, CritStr);
            }
        NewPath += ".dot";

        LDot = fopen(NewPath.c_str(), "w");
        fprintf(LDot, "digraph G{\n");

        fprintf(LDot, "    subgraph clustermem {\n    style=filled;\n    color=powderblue;\n");
        NowPos = 0;
        NowElem = LFree;
        fprintf(LDot, "        f%d [shape=record, label=\"{FreePointer:\\n%d | PhysPos:\\n%d } | {Position\\n:%d | Data:\\n%d | Next:\\n%d | Prev:\\n%d}\",style=\"filled\",fillcolor=\"gold4\"];\n", NowPos, NowElem, (NowElem - next), NowPos, *(data + (NowElem - next)), *NowElem, *(prev + (NowElem - next)));
        if(*NowElem != -2)
            {
            fprintf(LDot, "        f%d->f%d\n", NowPos, (NowPos + 1));
            NowElem = *NowElem + next;
            ++NowPos;
            }
        while(*NowElem != -2 && NowPos < LSize)
            {
            fprintf(LDot, "        f%d [shape=record, label=\"{FreePointer:\\n%d | PhysPos:\\n%d } | {Position\\n:%d | Data:\\n%d | Next:\\n%d}\",style=\"filled\",fillcolor=\"green4\"];\n", NowPos, NowElem, (NowElem - next), NowPos, *(data + (NowElem - next)), *NowElem);
            fprintf(LDot, "        f%d->f%d\n", NowPos, (NowPos + 1));
            NowElem = *NowElem + next;
            ++NowPos;
            }
        fprintf(LDot, "        f%d [shape=record, label=\"{FreePointer:\\n%d | PhysPos:\\n%d } | {Position\\n:%d | Data:\\n%d | Next:\\n%d}\",style=\"filled\",fillcolor=\"slateblue4\"];\n", NowPos, NowElem, (NowElem - next), NowPos, *(data + (NowElem - next)), *NowElem);


        fprintf(LDot, "    label = \"Free memory of list: %s\"}\n\n", LName.c_str());


        NowElem = head;
        NowPos = 0;
        fprintf(LDot, "    rankdir=LR;\n");
        fprintf(LDot, "    subgraph clusterlist {\n    style=filled;\n    color=lightgrey;\n");
        fprintf(LDot, "        %d [shape=record, label=\"{ElemPointer:\\n%d | PhysPos:\\n%d } | {Position\\n:%d | Data:\\n%d | Next:\\n%d | Prev:\\n%d}\",style=\"filled\",fillcolor=\"gold2\"];\n", NowPos, NowElem, (NowElem - next), NowPos, *(data + (NowElem - next)), *NowElem, *(prev + (NowElem - next)));
        if(*NowElem != -3)
            {
            if( (NowElem - next) == *(prev + *NowElem) )
                {
                fprintf(LDot, "        %d->%d[dir=\"both\";style=\"bold\"];\n", NowPos, (NowPos + 1));
                }
            else
                {
                fprintf(LDot, "        %d->%d[color=\"red\"];\n", NowPos, (NowPos + 1));
                }
            NowElem = *NowElem + next;
            ++NowPos;
            }
        while(*NowElem != -3 && NowPos < LSize)
            {
            fprintf(LDot, "        %d [shape=record, label=\"{ElemPointer:\\n%d | PhysPos:\\n%d }  | {Position\\n:%d | Data:\\n%d | Next:\\n%d | Prev:\\n%d}\",style=\"filled\",fillcolor=\"lawngreen\"];\n", NowPos, NowElem, (NowElem - next), NowPos, *(data + (NowElem - next)), *NowElem, *(prev + (NowElem - next)));
            if( (NowElem - next) == *(prev + *NowElem) )
                {
                fprintf(LDot, "        %d->%d[dir=\"both\";style=\"bold\"];\n", NowPos, (NowPos + 1));
                }
            else
                {
                fprintf(LDot, "        %d->%d[color=\"red\"];\n", NowPos, (NowPos + 1));
                }
            NowElem = *NowElem + next;
            ++NowPos;
            }
        fprintf(LDot, "        %d [shape=record, label=\"{ElemPointer:\\n%d | PhysPos:\\n%d }  | {Position\\n:%d | Data:\\n%d | Next:\\n%d | Prev:\\n%d}\",style=\"filled\",fillcolor=\"royalblue1\"];\n", NowPos, NowElem, (NowElem - next), NowPos, *(data + (NowElem - next)), *NowElem, *(prev + (NowElem - next)));
        fprintf(LDot, "    label = \"List with name: %s\"}\n", LName.c_str());

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

        return;
        }
#endif

/**
    \brief at in MDList_t
    \author andreevmaxi
	\version 1.0
	\date november 2019 year
	\copyright korobcom
    \details This is function that gives element from our list by physical position
    \param[in] RawPos physical position of our elem
*/
ListElem_t MDList_t::at(int RawPos)
    {
    DEB(MDList_t::Verify());
    return *(data + RawPos);
    }

/**
    \brief InsertAfterRaw in MDList_t
    \author andreevmaxi
	\version 1.0
	\date november 2019 year
	\copyright korobcom
    \details This is function that inserts an element in our list after physical position
    \param[in] RawPos physical position after what we insert elem
    \param[in] PushingElem our element
*/
bool MDList_t::InsertAfterRaw(ListElem_t PushingElem, int Pos)
    {
    DEB(MDList_t::Verify());

    if(Pos >= LSize)
        {
        MDList_t::PushBack(PushingElem);
        return 1;
        }

    if(Pos + next == tail)
        {
        MDList_t::PushBack(PushingElem);
        return 1;
        }
    if(Pos == -1 && sorted == 1)
        {
        if(head == next)
            {
            int TmpHead = head - next;
            int TmpTail = tail - next;
            int TmpFree = LFree - next;
            int TmpLFreeTail = LFreeTail - next;

            LSize *= 2;
            ListElem_t* TmpArr1 = (ListElem_t*) calloc(LSize, sizeof(ListElem_t));

            for(int i = (LSize/2); i < LSize; ++i)
                {
                *(TmpArr1 + i) = *(data + i - LSize/2);
                }
            free(data);
            data = TmpArr1;

            int* TmpArr2 = (int*) calloc(LSize, sizeof(int));
            for(int i = (LSize/2); i < LSize; ++i)
                {
                if(*(next + i - LSize/2) != -1 && *(next + i - LSize/2) != -2 && *(next + i - LSize/2) != -3)
                    {
                    *(TmpArr2 + i) = *(next + i - LSize/2) + (LSize/2);
                    }
                else
                    {
                    *(TmpArr2 + i) = *(next + i - LSize/2);
                    }
                }
            *(TmpArr2 + TmpLFreeTail + (LSize/2)) = 0;
            for(int i = 1; i < (LSize/2); ++i)
                {
                *(TmpArr2 + i - 1) = i;
                }
            *(TmpArr2 + (LSize/2) - 1) = - 2;
            free(next);
            next = TmpArr2;

            int* TmpArr3 = (int*) calloc(LSize, sizeof(int));
            for(int i = (LSize/2); i < LSize; ++i)
                {
                if(*(prev + i - LSize/2) != -1)
                    {
                    *(TmpArr3 + i) = *(prev + i - LSize/2) + (LSize/2);
                    }
                else
                    {
                    *(TmpArr3 + i) = *(prev + i - LSize/2);
                    }
                }
            for(int i = 1; i < (LSize/2); ++i)
                {
                *(TmpArr3 + i) = -1;
                }
            free(prev);
            prev = TmpArr3;

            head = next + TmpHead + (LSize/2);
            tail = next + TmpTail + (LSize/2);
            LFree = next + TmpFree + (LSize/2);
            LFreeTail = next + (LSize/2) - 1;
            }
        --head;
        *head = head + 1 - next;
        *(prev + (head - next) + 1) = (head - next);
        --LFreeTail;
        *LFreeTail = -2;
        *(data + (head - next) )   = PushingElem;

        return 1;
        }
    sorted = 0;
    int* NewElem = MDList_t::SearchingEmpty();
    *NewElem = *(next + Pos);
    *(prev + (NewElem - next)) = Pos;
    *(next + Pos) = NewElem - next;
    *(prev + *NewElem) = NewElem - next;
    *(data + (NewElem - next)) = PushingElem;

    return 1;
    }
/**
    \brief DeleteElemRaw in MDList_t
    \author andreevmaxi
	\version 1.0
	\date november 2019 year
	\copyright korobcom
    \details This is function that deletes an element in our list on physical position
    \param[in] RawPos physical position what we delete elem
*/
bool MDList_t::DeleteElemRaw(int Pos)
    {
    DEB(MDList_t::Verify());

    int* DeletingElem = 0;
    DeletingElem = next + Pos;

    if(*(prev + Pos) == -1)
        {
        head = next + *DeletingElem;
        *(prev + *DeletingElem) = *(prev + Pos);
        *LFreeTail = Pos;
        *DeletingElem = -2;
        LFreeTail = DeletingElem;
        return 1;
        }
    if(*DeletingElem == -3)
        {
        tail = next + *(prev + Pos);
        *(next + *(prev + Pos)) = *DeletingElem;
        *LFreeTail = Pos;
        *DeletingElem = -2;
        LFreeTail = DeletingElem;
        return 1;
        }
    *(prev + *DeletingElem) = *(prev + Pos);
    *(next + *(prev + Pos)) = *DeletingElem;
    *(prev + Pos) = -1;
    *LFreeTail = Pos;
    *DeletingElem = -2;
    LFreeTail = DeletingElem;
    return 1;
    }

/**
    \brief DeleteElem in MDList_t
    \author andreevmaxi
	\version 1.0
	\date november 2019 year
	\copyright korobcom
    \details This is function that deletes an element in our list on logic position
    \param[in] Pos logic position what we delete elem
*/
bool MDList_t::DeleteElem(int Pos)
    {
    DEB(MDList_t::Verify());
    int RawPos = 0;
    int* NowElem = head;
    if(sorted == 1)
        {
        RawPos = head - next + Pos;
        if(Pos > tail - next)
            {
            RawPos = tail - next;
            }
        }
    else
        {
        while (RawPos != Pos && *NowElem != -3)
            {
            NowElem = next + (*NowElem);
            ++RawPos;
            }
        RawPos = NowElem - next;
        }
    MDList_t::DeleteElemRaw(RawPos);
    return 1;
    }

/**
    \brief InsertElemRaw in MDList_t
    \author andreevmaxi
	\version 1.0
	\date november 2019 year
	\copyright korobcom
    \details This is function that inserts an element in our list on physical position
    \param[in] RawPos physical position where we insert elem
    \param[in] PushingElem our element
*/
bool MDList_t::InsertElemRaw(ListElem_t PushingElem, int Pos)
    {
    DEB(MDList_t::Verify());
    MDList_t::InsertAfterRaw(PushingElem, *(prev + Pos));
    return 1;
    }

/**
    \brief InsertBeforeRaw in MDList_t
    \author andreevmaxi
	\version 1.0
	\date november 2019 year
	\copyright korobcom
    \details This is function that inserts an element in our list before physical position
    \param[in] RawPos physical position before what we insert elem
    \param[in] PushingElem our element
*/
bool MDList_t::InsertBeforeRaw(ListElem_t PushingElem, int Pos)
    {
    DEB(MDList_t::Verify());
    MDList_t::InsertElemRaw(PushingElem, *(prev + Pos));
    return 1;
    }

/**
    \brief InsertElem in MDList_t
    \author andreevmaxi
	\version 1.0
	\date november 2019 year
	\copyright korobcom
    \details This is function that inserts an element in our list on logic position
    \param[in] RawPos logic position after where we insert elem
    \param[in] PushingElem our element
*/
bool MDList_t::InsertElem(ListElem_t PushingElem, int RawPos)
    {
    DEB(MDList_t::Verify());
    int Pos;
    if(RawPos == -1)
        {
        Pos = -1;
        }
    else
        {
        if(sorted == 1)
            {
            Pos = head - next + RawPos;
            if(Pos > tail - next)
                {
                Pos = tail - next;
                }
            }
        else
            {
            int TmpP = 0;
            int* NowElem = head;
            while (TmpP != RawPos && *NowElem != -3)
                {
                NowElem = next + (*NowElem);
                ++TmpP;
                }
            Pos = NowElem - next;
            }
        }
    MDList_t::InsertElemRaw(PushingElem, Pos);
    return 1;
    }

/**
    \brief InsertBefore in MDList_t
    \author andreevmaxi
	\version 1.0
	\date november 2019 year
	\copyright korobcom
    \details This is function that inserts an element in our list before logic position
    \param[in] RawPos logic position before where we insert elem
    \param[in] PushingElem our element
*/
bool MDList_t::InsertBefore(ListElem_t PushingElem, int RawPos)
    {
    DEB(MDList_t::Verify());
    int Pos;
    if(RawPos == -1)
        {
        Pos = -1;
        }
    else
        {
        if(sorted == 1)
            {
            Pos = head - next + RawPos;
            if(Pos > tail - next)
                {
                Pos = tail - next;
                }
            }
        else
            {
            int TmpP = 0;
            int* NowElem = head;
            while (TmpP != RawPos && *NowElem != -3)
                {
                NowElem = next + (*NowElem);
                ++TmpP;
                }
            Pos = NowElem - next;
            }
        }
    MDList_t::InsertBeforeRaw(PushingElem, Pos);
    return 1;
    }
