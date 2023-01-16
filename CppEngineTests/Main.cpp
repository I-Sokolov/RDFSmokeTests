
#include "pch.h"

extern void InstancePropertiesTests();
extern void EarlyBindingTests();

extern int main()
{
    try {
        printf("--------- Starting Geometry engine C++ tests\n");

        InstancePropertiesTests();
        EarlyBindingTests();

        printf("--------- Finished Geometry enginde C++ tests\n");
        return 0;
    }
    catch (int& code) {
        printf("!!!! Geometry engine C++ tests are failed\n");
        return code;
    }
}
