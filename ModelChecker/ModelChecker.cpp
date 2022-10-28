
#include "pch.h"


extern int main()
{
    try {
        printf("--------- Starting Model Checker tests\n");

        printf("---------- Finished Model Checker tests\n");
        return 0;
    }
    catch (int& code) {
        printf("!!!! Model Checker tests are failed\n");
        return code;
    }
}

