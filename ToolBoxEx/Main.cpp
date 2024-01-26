
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
       // ASSERT(lostMem <= ); 

#ifdef VLD_ON
        VLDReportLeaks();
#endif
        printf("---------- Finished ToolBoxEx enginde C++ tests. Lost memory %dKB\n", lostMem / 1024);

        return 0;
    }
    catch (int& code) {
        printf("!!!! ToolBoxEx tests are failed\n");
        return code;
    }
}

