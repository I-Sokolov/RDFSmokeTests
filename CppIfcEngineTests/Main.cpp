
#include "pch.h"

extern void EngineTests();
extern void EarlyBound_IFC4_test();
extern void EarlyBound_IFC4x4_test();
extern void EarlyBound_AP242_test();
extern void EarlyBound_GuideExamples();



extern int main()
{
    try {
        printf("--------- Starting IFC engine C++ tests\n");
        
        EngineTests();
        EarlyBound_IFC4_test();
        EarlyBound_IFC4x4_test();
        EarlyBound_AP242_test();
        EarlyBound_GuideExamples();

        printf("---------- Finished IFC enginde C++ tests\n");
        return 0;
    }
    catch (int& code) {
        printf("!!!! IFC engine C++ tests are failed\n");
        return code;
    }
}

