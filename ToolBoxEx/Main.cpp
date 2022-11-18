
#include "pch.h"

extern void ReadWriteSchemaTest();
extern void ModelCheckerTests();

extern int main()
{
    try {
        printf("--------- Starting ToolBoxEx tests\n");

        ReadWriteSchemaTest();
        ModelCheckerTests();

        printf("---------- Finished ToolBoxEx tests\n");
        return 0;
    }
    catch (int& code) {
        printf("!!!! ToolBoxEx tests are failed\n");
        return code;
    }
}

