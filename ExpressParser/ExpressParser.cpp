
#include "pch.h"


extern int main()
{
    try {
        printf("--------- Starting EXPRESS Parser tests\n");

        printf("---------- Finished EXPRESS Parser tests\n");
        return 0;
    }
    catch (int& code) {
        printf("!!!! EXPRESS parser tests are failed\n");
        return code;
    }
}

