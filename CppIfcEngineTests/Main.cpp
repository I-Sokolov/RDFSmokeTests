
#include "pch.h"

extern int main()
{
    try {
        printf("Starting IFC engine C++ tests\n");
        ASSERT(0);
        printf("Finished IFC enginde C++ tests\n");
        return 0;
    }
    catch (int& code) {
        printf("!!!! IFC engine C++ tests are failed\n");
        return code;
    }
}

