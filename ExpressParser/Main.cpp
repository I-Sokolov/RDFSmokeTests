
#include "pch.h"

extern void ReadWriteSchemaTest();

extern int main()
{
    try {
        printf("--------- Starting EXPRESS Parser tests\n");

        ReadWriteSchemaTest();

        printf("---------- Finished EXPRESS Parser tests\n");
        return 0;
    }
    catch (int& code) {
        printf("!!!! EXPRESS parser tests are failed\n");
        return code;
    }
}

