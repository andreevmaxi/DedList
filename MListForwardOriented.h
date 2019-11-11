#include <algorithm>
#include <cstdlib>
#include <cassert>
#include <string>

#ifdef _DEBUG
    #define InitDEB( list ) MList_t list( #list )

    #define DEB( code ) code
#else
    #define DEB( code )

    #define InitDEB( list )
#endif


typedef int ListElem_t;

const int BeginNumOfMember = 15;

struct MList_t
    {
    ListElem_t* data;
    int* next;
    int* head;
    int* tail;        // to do fast "push back"
    bool sorted;      // 1 when data is equal to array
    int LSize;
    DEB(string LName);

    bool PushBack(ListElem_t PushingElem);

    bool InsertAfter(ListElem_t PushingElem, int RawPos);

    bool DeleteAfter(int Pos);

    ListElem_t elem(int Pos);

    bool SortList();

    bool ArrOfElems(ListElem_t* RetArr, int* PosOfElems, int SizeOfArrs);

    #ifdef _DEBUG
    MList_t( string name );
    #else
    MList_t();
    #endif
    ~MList_t();
    };

bool LResize(ListElem_t data, int* next, int* LSize);

int* SearchingEmpty(ListElem_t data, int* next, int* LSize);


#ifdef _DEBUG
MList_t::MList_t( string name )
    {
    data   = (ListElem_t*) calloc (BeginNumOfMember, sizeof(ListElem_t));
    next   = (int*) calloc (BeginNumOfMember, sizeof(int));
    head   = next + 4; // now we reserved 5 elems left from head
    tail   = next + 4;

    assert(data);
    assert(next);
    assert(head);
    assert(tail);// than it needs to be if (...) {errnum = 124124125125; DUMP}

    sorted = 1;
    LSize  = BeginNumOfMember;
    LName = name;

    for(int i = 0; i < LSize; ++i)
        {
        *(next + i) = -1;
        }

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
    assert(tail != NULL);// than it needs to be if (...) {errnum = 124124125125; DUMP}

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
        return 1;
        }
    return 0;
    }

bool MList_t::InsertAfter(ListElem_t PushingElem, int RawPos)
    {
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

        return 1;
        }
    sorted = 0;
    int* NewElem = SearchingEmpty(data, next, &LSize);
    *NewElem = *(next + Pos);
    *(next + Pos) = NewElem - next;
    *(data + (NewElem - next)) = PushingElem;

    return 1;
    }

bool MList_t::DeleteAfter(int Pos)
    {
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
    return 1;
    }

MList_t::~MList_t()
    {
    free(data);
    free(next);

    LSize = -1; // it's like it's dead, like destructed you knawww
    return;
    }

ListElem_t MList_t::elem(int Pos)
    {
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
    ListElem_t* TmpData = (ListElem_t*) calloc(LSize, sizeof(ListElem_t));
    int*        TmpNext =        (int*) calloc(LSize, sizeof(int));

    for(int i = 0; i < LSize; ++i)
        {
        *(TmpNext + i)= -1;
        }
    int* NowElem = head;
    int  i = head - next;
    while(*(NowElem) != -1)
        {
        *(TmpData + i) = *(data + (NowElem - next));
        *(TmpNext + i) = i + 1;
        NowElem = next + *NowElem;
        ++i;
        }
    ListElem_t* TmpD = data;
    int*        TmpN = next;
    next = TmpNext;
    data = TmpData;
    free(TmpD);
    free(TmpN);

    sorted = 1;
    return 1;
    }
