#include "pch.h"

#define STEP_TEST "..\\TestData\\ComplexInstance.step"
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


extern void ComplexInstance()
{
    const char* entityNames[] = {
        "Set of Entity Instances",
        "NAMED_UNIT",
        "SI_UNIT",
        "SOLID_ANGLE_UNIT"
    };

    //
    SdaiModel model = sdaiOpenModelBN(0, STEP_TEST, "");

    CheckComplex(model, 1007, entityNames, 4);

    sdaiSaveModelBN(model, STEP_TEST_SAVED);

    sdaiCloseModel(model);

    //
    model = sdaiOpenModelBN(0, STEP_TEST_SAVED, "");
    
    CheckComplex(model, 1007, entityNames, 4);

    fopen_s(&myFileWrite, STEP_TEST_SAVED "_S", "wb");
    engiSaveModelByStream(model, WriteCallBackFunction, BLOCK_LENGTH_WRITE);
    fclose(myFileWrite);

    sdaiCloseModel(model);

    //
    model = sdaiOpenModelBN(0, STEP_TEST_SAVED "_S", "");

    CheckComplex(model, 1007, entityNames, 4);

    sdaiCloseModel(model);
}