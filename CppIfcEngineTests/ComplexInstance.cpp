#include "pch.h"

#define STEP_TEST1 "..\\TestData\\ComplexInstance1.step"
#define STEP_TEST2 "..\\TestData\\ComplexInstance2.step"
#define STEP_TEST_SAVED "ComplexInstance_saved.step"

#define TEST_SCHEMA         "..\\TestData\\schemas\\smoke_test.exp"
#define TEST_SCHEMA_MODEL   "..\\TestData\\smoke_test.txt"
#define TEST_SCHEMA_SAVED   "SmokeTestSaved.txt"

struct Instance
{
    const char* entityName = NULL;
    int narg = 0;
    const char** args = NULL;
};

static void CheckComplex(SdaiModel model, ExpressID id, Instance rInst[], int nInst)
{
    auto inst = internalGetInstanceFromP21Line(model, id);

    int i = 0;
    while (inst) {
        ASSERT(i < nInst);

        auto entity = sdaiGetInstanceType(inst);
        auto name = engiGetEntityName(entity, sdaiSTRING);
        ASSERT(0 == _stricmp(name, rInst[i].entityName));
        
        SdaiAggr args = NULL;
        engiGetAttributeAggr(inst, (int_t*)&args);
        ASSERT(sdaiGetMemberCount(args) == rInst[i].narg);

        inst = engiGetComplexInstanceNextPart(inst);
        i++;
    }

    ASSERT(i == nInst);
}

//
const int_t BLOCK_LENGTH_WRITE = 20000; //  no maximum limit
static FILE* myFileWrite = nullptr;
static void    __stdcall   WriteCallBackFunction(unsigned char* content, int64_t size)
{
    fwrite(content, (size_t)size, 1, myFileWrite);
}


static void CheckComplex(const char* testFile, ExpressID id, Instance rInst[], int nInst)
{

    //
    SdaiModel model = sdaiOpenModelBN(0, testFile, "");

    CheckComplex(model, id, rInst, nInst);

    sdaiSaveModelBN(model, STEP_TEST_SAVED);

    sdaiCloseModel(model);

    //
    model = sdaiOpenModelBN(0, STEP_TEST_SAVED, "");
    
    CheckComplex(model, id, rInst, nInst);

    fopen_s(&myFileWrite, STEP_TEST_SAVED "_S", "wb");
    ASSERT(myFileWrite);
    if (myFileWrite) {
        engiSaveModelByStream(model, WriteCallBackFunction, BLOCK_LENGTH_WRITE);
        fclose(myFileWrite);
    }

    sdaiCloseModel(model);

    //
    model = sdaiOpenModelBN(0, STEP_TEST_SAVED "_S", "");

    CheckComplex(model, id, rInst, nInst);

    sdaiCloseModel(model);
}

static void CheckComplex()
{
    ENTER_TEST;

    Instance inst1[] = {
        {"Set of Entity Instances", 0},
        {"NAMED_UNIT", 1},
        {"SI_UNIT", 2},
        {"SOLID_ANGLE_UNIT", 0}
    };

    Instance inst2[] = {
        {"Set of Entity Instances", 0},
        {"PART", 2},
        {"PART_PRISMATIC", 0},
        {"PART_PRISMATIC_SIMPLE", 4},
        {"STRUCTURAL_FRAME_ITEM", 3},
        {"STRUCTURAL_FRAME_PRODUCT", 1},
        {"STRUCTURAL_FRAME_PRODUCT_WITH_MATERIAL", 3}
    };

    CheckComplex(STEP_TEST1, 1007, inst1, 4);
    CheckComplex(STEP_TEST2, 1233, inst2, 7);
   
}

typedef std::map<const char*, const char*> AttrVal;

static void ASSERT_STR_VAL(const char* v1, const char* v2)
{
    if (v1 == NULL) {
        ASSERT(v2 == NULL);
    }
    else {
        ASSERT(v2 && 0 == strcmp(v1, v2));
    }
}

static void CheckInst(SdaiInstance inst, const char* entName, AttrVal& attrVal)
{
    ASSERT(inst);

    for (auto& av : attrVal) {

        const char* val = NULL;
        auto ret = sdaiGetAttrBN(inst, av.first, sdaiSTRING, &val);
        ASSERT_STR_VAL(val , av.second);
        ASSERT((ret!=NULL) == (val!=NULL));

        auto ent = sdaiGetInstanceType(inst);
        auto name = engiGetEntityName(ent, sdaiSTRING);
        ASSERT(0==_stricmp(name, entName));

        auto attr = sdaiGetAttrDefinition(ent, av.first);
        ASSERT(attr);

        val = NULL;
        ret = sdaiGetAttr(inst, attr, sdaiSTRING, &val);
        ASSERT_STR_VAL(val, av.second);
        ASSERT((ret != NULL) == (val != NULL));
    }
}

static void CheckDiamond(SdaiInstance inst,
    const char* valAttrBase,
    const char* valAttrCommonNameLeft, const char* valAttrCommonNameRight,
    const char* valAttrRight, const char* valAttrChild)
{
    AttrVal av;
    av["AttrBase"] = valAttrBase;
    av["DiamondBase.AttrBase"] = valAttrBase;
    av["DiamondLeft.AttrBase"] = valAttrBase;
    av["DiamondRight.AttrBase"] = valAttrBase;
    av["DiamondRight2.AttrBase"] = valAttrBase;
    av["Diamond.AttrBase"] = valAttrBase;
    av["AttrCommonName"] = valAttrCommonNameLeft;
    av["Diamond.AttrCommonName"] = valAttrCommonNameLeft;
    av["DiamondLeft.AttrCommonName"] = valAttrCommonNameLeft;
    av["DiamondRight.AttrCommonName"] = valAttrCommonNameRight;
    av["DiamondRight2.AttrCommonName"] = valAttrCommonNameRight;
    av["AttrRight"] = valAttrRight;
    av["DiamondRight2.AttrRight"] = valAttrRight;
    av["Diamond.AttrRight"] = valAttrRight;
    av["AttrChild"] = valAttrChild;
    av["Diamond.AttrChild"] = valAttrChild;

    //TODO add test with getting attr from supertype

    CheckInst(inst, "Diamond", av);
}

static void SmokeTestModelPopulate(SdaiModel model)
{
    auto diamondEntity = sdaiGetEntity(model, "Diamond");
    ASSERT(diamondEntity);

    //--------------------------------------------------------------------------
    //diamond1
    auto inst = sdaiCreateInstance(model, diamondEntity);
    sdaiPutAttrBN(inst, "AttrBase", sdaiSTRING, "Diamond1-AttrBase");
    sdaiPutAttrBN(inst, "AttrCommonName", sdaiSTRING, "Diamond1-ValueCommonName");
    sdaiPutAttrBN(inst, "AttrChild", sdaiSTRING, "Diamond1-ValueAttrChild");

    CheckDiamond(inst, "Diamond1-AttrBase", "Diamond1-ValueCommonName", NULL, NULL, "Diamond1-ValueAttrChild");

    //diamond2
    inst = sdaiCreateInstance(model, diamondEntity);
    sdaiPutAttrBN(inst, "DiamondBase.AttrBase", sdaiSTRING, "d2-AttrBase");
    sdaiPutAttrBN(inst, "DiamondLeft.AttrCommonName", sdaiSTRING, "d2-L-CN");
    sdaiPutAttrBN(inst, "DiamondRight.AttrCommonName", sdaiSTRING, "d2-R-CN");
    sdaiPutAttrBN(inst, "DiamondRight2.AttrRight", sdaiSTRING, "d2-AR");
    sdaiPutAttrBN(inst, "Diamond.AttrChild", sdaiSTRING, "d2-Ch");

    CheckDiamond(inst, "d2-AttrBase", "d2-L-CN", "d2-R-CN", "d2-AR", "d2-Ch");

    //diamond3
    inst = sdaiCreateInstance(model, diamondEntity);
    sdaiPutAttrBN(inst, "DiamondRight.AttrBase", sdaiSTRING, "d3-AttrBase");
    sdaiPutAttrBN(inst, "DiamondLeft.AttrCommonName", sdaiSTRING, "d3-L-CN");
    sdaiPutAttrBN(inst, "DiamondRight2.AttrCommonName", sdaiSTRING, "d3-R-CN");
    sdaiPutAttrBN(inst, "AttrRight", sdaiSTRING, "d3-AR");

    CheckDiamond(inst, "d3-AttrBase", "d3-L-CN", "d3-R-CN", "d3-AR", NULL);

    //----------------------------------------------------------------------------------------
    // get complex entities
    //
    const char* strParentAndChild[] = { "Parent", "Child", "Person"};
    const char* strPersonAndDiamond[] = { "Person", "Diamond" };
    const char* strPersonAndDiamondBase[] = { "Person", "Diamond", "DiamondBase"};

    auto entityParenyAndChild = sdaiGetComplexEntityBN(model, _countof(strParentAndChild), strParentAndChild);
    auto entityPersonAndDiamond = sdaiGetComplexEntityBN(model, _countof(strPersonAndDiamond), strPersonAndDiamond);
    auto entityPersonAndDiamondBase = sdaiGetComplexEntityBN(model, _countof(strPersonAndDiamondBase), strPersonAndDiamondBase);
    ASSERT(entityParenyAndChild != entityPersonAndDiamond);
    ASSERT(entityParenyAndChild != entityPersonAndDiamondBase);
    ASSERT(entityPersonAndDiamondBase != entityPersonAndDiamond);

    auto entity = sdaiGetComplexEntityBN(model, _countof(strParentAndChild), strParentAndChild);
    ASSERT(entity == entityParenyAndChild);

    entity = sdaiGetComplexEntityBN(model, _countof(strPersonAndDiamond), strPersonAndDiamond);
    ASSERT(entity == entityPersonAndDiamond);

    entity = sdaiGetComplexEntityBN(model, _countof(strPersonAndDiamondBase), strPersonAndDiamondBase);
    ASSERT(entity == entityPersonAndDiamondBase);

    //TODO - derived attribute with qualified name

    //TODO - qualified inverse attributes
}

static void SmokeTestModelCheckContent(SdaiModel model)
{
    auto inst = internalGetInstanceFromP21Line(model, 1);
    CheckDiamond(inst, "Diamond1-AttrBase", "Diamond1-ValueCommonName", NULL, NULL, "Diamond1-ValueAttrChild");

    inst = internalGetInstanceFromP21Line(model, 2);
    CheckDiamond(inst, "d2-AttrBase", "d2-L-CN", "d2-R-CN", "d2-AR", "d2-Ch");

    inst = internalGetInstanceFromP21Line(model, 3);
    CheckDiamond(inst, "d3-AttrBase", "d3-L-CN", "d3-R-CN", "d3-AR", NULL);
}

static void SmokeTestSchema()
{
    ENTER_TEST;

    auto model = sdaiCreateModelBN(TEST_SCHEMA);
    ASSERT(model);
    SmokeTestModelPopulate(model);
    SmokeTestModelCheckContent(model);
    sdaiSaveModelBN(model, TEST_SCHEMA_SAVED);
    sdaiCloseModel(model);
    model = NULL;

    model = sdaiOpenModelBN(0, TEST_SCHEMA_SAVED, TEST_SCHEMA);
    ASSERT(model);
    SmokeTestModelCheckContent(model);
    sdaiCloseModel(model);
    model = NULL;

/*
    model = sdaiOpenModelBN(0, TEST_SCHEMA_MODEL, TEST_SCHEMA);
    ASSERT(model);
    SmokeTestModelCheckContent(model);
    sdaiCloseModel(model);
    model = NULL;
    */
}

extern void ComplexInstance()
{
    CheckComplex();
    SmokeTestSchema();
}

#if 0
if (auto list = sdaiCreateNPL__internal()) {
    for (int_t i = 0; i < nameNumber; i++) {
        if (auto entity = sdaiGetEntity__UNICODE__internal(session, nameVector[i])) {
            SdaiInteger val = (SdaiInteger)entity;
            sdaiAppend__internal(list, sdaiINTEGER, &val);
        }
        else {
            return NULL;
        }
    }
#endif
