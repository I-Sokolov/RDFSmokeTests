
#include "pch.h"
#include "ToolBoxEx.h"

extern int main()
{
    try {
        printf("--------- Starting ToolBoxEx tests\n");

#ifdef VLD_ON
        VLDEnable();
#endif
        auto startingHeap = HeapUsed();

        ReadWriteDataFileTest();
        ModelCheckerTests();
        ReadWriteSchemaTest();

        auto finalHeap = HeapUsed();

        auto lostMem = finalHeap - startingHeap;
        printf("Lost memory %d bytes.\n", lostMem);
        ASSERT(lostMem <= 5000);

#ifdef VLD_ON
        VLDReportLeaks();
#endif
        printf("---------- Finished ToolBoxEx engine C++ tests.\n");

        return 0;
    }
    catch (int& code) {
        printf("!!!! ToolBoxEx tests are failed\n");
        return code;
    }
}

