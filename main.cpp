#include <cstdio>
#include "MListForwardOriented.h"

int main()
{
    DEBMList_t(test);

    test.PushBack(10);
    test.PushBack(20);
    test.PushBack(30);
    test.PushBack(40);
    test.InsertAfter(50, 0);
    test.InsertAfter(50, 5);

    printf("sort: %d\n", test.sorted);
    printf("%d\n\n", test.elem(3));
    int n = 3;
    int* mass = (int*) calloc(n, sizeof(int));
    int* args = (int*) calloc(n, sizeof(int));
    *args       = 4;
    *(args + 1) = 3;
    *(args + 2) = 0;
    test.ArrOfElems(mass, args, n);
    for(int i = 0; i < n; ++i)
        {
        printf("%d %d\n", *(args + i), *(mass + i));
        }

    printf("\n\n");


    for(int i = 0; i < 13; ++i)
    {
    printf("%d %d\n", *(test.data + i), *(test.next + i));
    }

    test.LDUMP(9);
    printf("\n\n");
    for(int i = 0; i < 15; ++i)
        {
        printf("%d %d\n", *(test.data + i), *(test.next + i));
        }


    printf("\n%d\n", test.head - test.next);


    test.SortList();

    printf("\n\n");
    for(int i = 0; i < 15; ++i)
        {
        printf("%d %d\n", *(test.data + i), *(test.next + i));
        }
    printf("\n%d\n", test.head - test.next);
    printf("\n%d\n", test.tail - test.next);

    test.PushBack(10);
    test.PushBack(20);
    test.PushBack(30);
    test.PushBack(40);
    test.PushBack(10);
    test.PushBack(20);
    test.PushBack(30);
    test.PushBack(40);

    test.LDUMP(9);
    for(int i = 0; i < 8; ++i)
        {
        test.InsertAfter(i, -1);
        }


    test.LDUMP(9);
    test.InsertAfterRaw(100, 38);
    for(int i = 0; i < 60; ++i)
        {
        printf("%d %d\n", *(test.data + i), *(test.next + i));
        }
    test.LDUMP(9);
    return 0;
}
