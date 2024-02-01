
#include "pch.h"

extern void ReadWriteSchemaTest();
extern void ModelCheckerTests();

extern int main()
{
    try {
        printf("--------- Starting ToolBoxEx tests\n");

#ifdef VLD_ON
        VLDEnable();
#endif
        auto startingHeap = HeapUsed();

        ReadWriteSchemaTest();
        ModelCheckerTests();

        auto finalHeap = HeapUsed();

        auto lostMem = finalHeap - startingHeap;
        printf("Lost memory %d bytes.\n", lostMem);
        ASSERT(lostMem <= 1500);

#ifdef VLD_ON
        VLDReportLeaks();
#endif
        printf("---------- Finished ToolBoxEx enginde C++ tests.\n");

        return 0;
    }
    catch (int& code) {
        printf("!!!! ToolBoxEx tests are failed\n");
        return code;
    }
}

