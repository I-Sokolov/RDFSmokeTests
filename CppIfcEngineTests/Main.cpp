
#include "pch.h"

extern void EngineTests();
extern void EarlyBound_IFC4_test();
extern void EarlyBound_IFC4x4_test();
extern void EarlyBound_AP242_test();
extern void EarlyBound_GuideExamples();
extern void ADBcreate();

static long HeapUsed()
{
    _HEAPINFO info = { 0, 0, 0 };
    long used = 0;
    int rc;

    while ((rc = _heapwalk(&info)) == _HEAPOK) {
        if (info._useflag == _USEDENTRY)
            used += (long) info._size;
    }
    if (rc != _HEAPEND && rc != _HEAPEMPTY)
        used = (used ? -used : -1);

    return used;
}

extern int main()
{
    try {
        printf("--------- Starting IFC engine C++ tests\n");
        

        auto startingHeap = HeapUsed();
        
        bool stop = true;
        int i = 0;
        do
            {
            ADBcreate();
            EngineTests();
            EarlyBound_IFC4_test();
            EarlyBound_IFC4x4_test();
            EarlyBound_AP242_test();
            EarlyBound_GuideExamples();

            i++;
            if (i % 100 == 0)
                {
                auto heapSize = HeapUsed();
                printf("Cycle %d, lost memory %d\n", i, heapSize-startingHeap);
                }
        } while (!stop);

        auto finalHeap = HeapUsed();
        
        auto lostMem = finalHeap - startingHeap;
        ASSERT(lostMem <= 232);

        printf("---------- Finished IFC enginde C++ tests. Lost memory %dKB\n", lostMem / 1024);
        return 0;
    }
    catch (int& code) {
        printf("!!!! IFC engine C++ tests are failed\n");
        return code;
    }
}

