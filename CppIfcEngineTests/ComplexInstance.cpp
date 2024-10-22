#include "pch.h"

#define STEP_TEST1 "..\\TestData\\ComplexInstance1.step"
#define STEP_TEST2 "..\\TestData\\ComplexInstance2.step"
#define STEP_TEST_SAVED "ComplexInstance_saved.step"

static void CheckComplex(SdaiModel model, ExpressID id, const char* rNames[], int nNames)
{
    auto inst = internalGetInstanceFromP21Line(model, id);

    int i = 0;
    while (inst) {
        ASSERT(i < nNames);

        auto entity = sdaiGetInstanceType(inst);
        auto name = engiGetEntityName(entity, sdaiSTRING);
        ASSERT(0 == _stricmp(name, rNames[i]));
        
        inst = engiGetComplexInstanceNextPart(inst);
        i++;
    }

    ASSERT(i == nNames);
}

//
const int_t BLOCK_LENGTH_WRITE = 20000; //  no maximum limit
static FILE* myFileWrite = nullptr;
static void    __stdcall   WriteCallBackFunction(unsigned char* content, int64_t size)
{
    fwrite(content, (size_t)size, 1, myFileWrite);
}


static void ComplexInstance(const char* testFile, ExpressID id, const char* rNames[], int nNames)
{

    //
    SdaiModel model = sdaiOpenModelBN(0, testFile, "");

    CheckComplex(model, id, rNames, nNames);

    sdaiSaveModelBN(model, STEP_TEST_SAVED);

    sdaiCloseModel(model);

    //
    model = sdaiOpenModelBN(0, STEP_TEST_SAVED, "");
    
    CheckComplex(model, id, rNames, nNames);

    fopen_s(&myFileWrite, STEP_TEST_SAVED "_S", "wb");
    engiSaveModelByStream(model, WriteCallBackFunction, BLOCK_LENGTH_WRITE);
    fclose(myFileWrite);

    sdaiCloseModel(model);

    //
    model = sdaiOpenModelBN(0, STEP_TEST_SAVED "_S", "");

    CheckComplex(model, id, rNames, nNames);

    sdaiCloseModel(model);
}

extern void ComplexInstance()
{
    ENTER_TEST;

    const char* entityNames1[] = {
    "Set of Entity Instances",
    "NAMED_UNIT",
    "SI_UNIT",
    "SOLID_ANGLE_UNIT"
    };

    const char* entityNames2[] = {
    "Set of Entity Instances",
    "PART",
    "PART_PRISMATIC",
    "PART_PRISMATIC_SIMPLE",
    "STRUCTURAL_FRAME_ITEM",
    "STRUCTURAL_FRAME_PRODUCT",
    "STRUCTURAL_FRAME_PRODUCT_WITH_MATERIAL"
    };

    ComplexInstance(STEP_TEST1, 1007, entityNames1, 4);
    ComplexInstance(STEP_TEST2, 1233, entityNames2, 7);
}