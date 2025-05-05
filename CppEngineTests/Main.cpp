
#include "pch.h"

extern void InstancePropertiesTests();
extern void EarlyBindingTests();
extern void MetaPropertiesTest();
extern void ExtAppMemTest();
extern void TriangulationTest();

extern int main()
{
    try {
        printf("--------- Starting Geometry engine C++ tests\n");

#ifdef VLD_ON
        VLDEnable();
#endif

        TriangulationTest();
        EarlyBindingTests();
        InstancePropertiesTests();
        MetaPropertiesTest();
        ExtAppMemTest();

#ifdef VLD_ON
        VLDReportLeaks();
#endif

        printf("--------- Finished Geometry enginde C++ tests\n");
        return 0;
    }
    catch (int& code) {
        printf("!!!! Geometry engine C++ tests are failed\n");
        return code;
    }
}
