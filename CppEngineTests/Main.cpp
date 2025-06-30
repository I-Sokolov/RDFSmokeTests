
#include "pch.h"

extern void InstancePropertiesTests();
extern void EarlyBindingTests();
extern void MetaPropertiesTest();
extern void ExtAppMemTest();
extern void EntitiesTests();
extern void Test_Example_RDFGeomGet();

extern int main()
{
    try {
        printf("--------- Starting Geometry engine C++ tests\n");

#ifdef VLD_ON
        VLDEnable();
#endif

        EntitiesTests();
        EarlyBindingTests();
        InstancePropertiesTests();
        MetaPropertiesTest();
        ExtAppMemTest();
        Test_Example_RDFGeomGet();

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
