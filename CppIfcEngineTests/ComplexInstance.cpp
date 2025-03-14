#include "pch.h"

#define STEP_TEST1 "..\\TestData\\ComplexInstance1.step"
#define STEP_TEST2 "..\\TestData\\ComplexInstance2.step"
#define STEP_TEST_SAVED "ComplexInstance_saved.step"

#define TEST_SCHEMA         "..\\TestData\\schemas\\smoke_test.exp"
#define TEST_MODEL_SAVED    "SmokeTest.txt"
#define TEST_MODEL          "..\\TestData\\" TEST_MODEL_SAVED

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
        {"+NAMED_UNIT+SI_UNIT+SOLID_ANGLE_UNIT", 0},
        {"NAMED_UNIT", 1},
        {"SI_UNIT", 2},
        {"SOLID_ANGLE_UNIT", 0}
    };

    Instance inst2[] = {
        {"+PART+PART_PRISMATIC+PART_PRISMATIC_SIMPLE+STRUCTURAL_FRAME_ITEM+STRUCTURAL_FRAME_PRODUCT+STRUCTURAL_FRAME_PRODUCT_WITH_MATERIAL", 0},
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
        //s name = engiGetEntityName(ent, sdaiSTRING);
        //ASSERT(0==strstr(name, entName));

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
    av["AttrRight"] = valAttrRight;
    av["DiamondRight.AttrRight"] = valAttrRight;
    av["DiamondRight2.AttrRight"] = valAttrRight;
    av["Diamond.AttrRight"] = valAttrRight;
    av["AttrChild"] = valAttrChild;
    av["Diamond.AttrChild"] = valAttrChild;

    CheckInst(inst, "Diamond", av);

    //
    auto model = sdaiGetInstanceModel(inst);

    auto left = sdaiGetEntity(model, "DiamondLeft");
    auto attrLeft = sdaiGetAttrDefinition(left, "AttrCommonName");
    ASSERT(attrLeft);

    SdaiString val = NULL;;
    auto ret = sdaiGetAttr(inst, attrLeft, sdaiSTRING, &val);
    ASSERT_STR_VAL(val, valAttrCommonNameLeft);
    ASSERT((ret != NULL) == (val != NULL));

    auto right = sdaiGetEntity(model, "DiamondRight");
    auto attrRight = sdaiGetAttrDefinition(right, "AttrCommonName");
    ASSERT(attrRight);

    val = NULL;;
    ret = sdaiGetAttr(inst, attrRight, sdaiSTRING, &val);
    ASSERT_STR_VAL(val, valAttrCommonNameRight);
    ASSERT((ret != NULL) == (val != NULL));

}

static SdaiNPL CreateEntitiesList(SdaiModel model, int_t numComponents, SdaiString* components)
{
    auto list = sdaiCreateNPL();

    for (int i = 0; i < numComponents; i++) {
        if (i % 2) {
            sdaiAppend(list, sdaiSTRING, components[i]);
        }
        else {
            auto e = sdaiGetEntity(model, components[i]);
            ASSERT(e);
            SdaiInteger ival = e;
            sdaiAppend(list, sdaiINTEGER, &ival);
        }
    }

    return list;
}

static void CheckComplexEntity(SdaiEntity entity, int_t numComponents, SdaiString* components)
{
    auto model = engiGetEntityModel(entity);

    auto getentity = sdaiGetComplexEntityBN(model, numComponents, components);
    ASSERT(entity == getentity);

    //
    auto list = CreateEntitiesList(model, numComponents, components);

    getentity = sdaiGetComplexEntity(model, list);
    ASSERT(entity == getentity);

    sdaiDeleteNPL(list);
}

static void PutSet1(SdaiInstance inst)
{
    sdaiPutAttrBN(inst, "AttrBase", sdaiSTRING, "Diamond1-AttrBase");
    sdaiPutAttrBN(inst, "AttrCommonName", sdaiSTRING, "Diamond1-ValueCommonName");
    sdaiPutAttrBN(inst, "AttrChild", sdaiSTRING, "Diamond1-ValueAttrChild");
}

static void PutSet2(SdaiInstance inst)
{
    sdaiPutAttrBN(inst, "DiamondBase.AttrBase", sdaiSTRING, "d2-AttrBase");
    sdaiPutAttrBN(inst, "DiamondLeft.AttrCommonName", sdaiSTRING, "d2-L-CN");
    sdaiPutAttrBN(inst, "DiamondRight.AttrCommonName", sdaiSTRING, "d2-R-CN");
    sdaiPutAttrBN(inst, "DiamondRight2.AttrRight", sdaiSTRING, "d2-AR");
    sdaiPutAttrBN(inst, "Diamond.AttrChild", sdaiSTRING, "d2-Ch");
}

static void PutByAttr(SdaiInstance inst, const char* entityName, const char* attrName, const char* value)
{
    SdaiEntity entity = NULL;
    if (entityName) {
        auto model = sdaiGetInstanceModel(inst);
        entity = sdaiGetEntity(model, entityName);
    }
    else {
        entity = sdaiGetInstanceType(inst);
    }
    ASSERT(entity);

    auto attr = sdaiGetAttrDefinition(entity, attrName);
    ASSERT(attr);

    sdaiPutAttr(inst, attr, sdaiSTRING, value);
}

static void PutSet3(SdaiInstance inst)
{
    PutByAttr(inst, "DiamondRight", "AttrBase", "d3-AttrBase");
    PutByAttr(inst, "DiamondLeft", "AttrCommonName", "d3-L-CN");
    PutByAttr(inst, "DiamondRight", "AttrCommonName", "d3-R-CN");
    PutByAttr(inst, NULL, "AttrRight", "d3-AR");
}

static void SmokeTestModelPopulate(SdaiModel model)
{
    sdaiCreateInstanceBN(model, "DiamondBase");
    sdaiCreateInstanceBN(model, "DiamondLeft");
    sdaiCreateInstanceBN(model, "DiamondRight");

    //TODO - derived attribute with qualified name

    //TODO - qualified inverse attributes

    //----------------------------------------------------------------------------------------
    // get complex entities
    //
    const char* strParentAndChild[] = { "Parent", "Child", "Person"};
    const char* strPersonAndDiamond[] = { "Person", "Diamond" };
    const char* strTest[] = { "Person", "Diamond", "DiamondBase", "DiamondLeft", "DiamondRight", "DiamondRight2"};

    auto entityParenyAndChild = sdaiGetComplexEntityBN(model, _countof(strParentAndChild), strParentAndChild);
    auto entityPersonAndDiamond = sdaiGetComplexEntityBN(model, _countof(strPersonAndDiamond), strPersonAndDiamond);
    auto entityTest = sdaiGetComplexEntityBN(model, _countof(strTest), strTest);
    ASSERT(entityParenyAndChild != entityPersonAndDiamond);
    ASSERT(entityParenyAndChild != entityTest);
    ASSERT(entityTest != entityPersonAndDiamond);

    CheckComplexEntity (entityParenyAndChild, _countof(strParentAndChild), strParentAndChild);
    CheckComplexEntity (entityPersonAndDiamond, _countof(strPersonAndDiamond), strPersonAndDiamond);
    CheckComplexEntity(entityTest, _countof(strTest), strTest);

    //---------------------------------------------------------------------------------------------
    // creare complex instances
    auto inst = sdaiCreateInstance(model, entityTest);
    PutSet1(inst);
    CheckDiamond(inst, "Diamond1-AttrBase", "Diamond1-ValueCommonName", NULL, NULL, "Diamond1-ValueAttrChild");

    inst = sdaiCreateComplexInstanceBN(model, _countof(strTest), strTest);
    PutSet2(inst);
    CheckDiamond(inst, "d2-AttrBase", "d2-L-CN", "d2-R-CN", "d2-AR", "d2-Ch");

    auto list = CreateEntitiesList(model, _countof(strTest), strTest);
    inst = sdaiCreateComplexInstance(model, list);
    sdaiDeleteNPL(list);

    PutSet3(inst);
    CheckDiamond(inst, "d3-AttrBase", "d3-L-CN", "d3-R-CN", "d3-AR", NULL);

    //--------------------------------------------------------------------------
    // diamond inheritance
    auto diamondEntity = sdaiGetEntity(model, "Diamond");
    ASSERT(diamondEntity);

    inst = sdaiCreateInstance(model, diamondEntity);
    PutSet1(inst);
    CheckDiamond(inst, "Diamond1-AttrBase", "Diamond1-ValueCommonName", NULL, NULL, "Diamond1-ValueAttrChild");

    inst = sdaiCreateInstance(model, diamondEntity);
    PutSet2(inst);
    CheckDiamond(inst, "d2-AttrBase", "d2-L-CN", "d2-R-CN", "d2-AR", "d2-Ch");

    inst = sdaiCreateInstance(model, diamondEntity);
    PutSet3(inst);
    CheckDiamond(inst, "d3-AttrBase", "d3-L-CN", "d3-R-CN", "d3-AR", NULL);

    //---------------------------------------------------------------------------------------------
    // diamond sdaiUnsetAttr
    inst = sdaiCreateInstance(model, diamondEntity);
    PutSet2(inst);
    sdaiUnsetAttrBN(inst, "AttrCommonName");
    CheckDiamond(inst, "d2-AttrBase", NULL, "d2-R-CN", "d2-AR", "d2-Ch");

    inst = sdaiCreateInstance(model, diamondEntity);
    PutSet2(inst);
    sdaiUnsetAttrBN(inst, "DiamondLeft.AttrCommonName");
    CheckDiamond(inst, "d2-AttrBase", NULL, "d2-R-CN", "d2-AR", "d2-Ch");

    inst = sdaiCreateInstance(model, diamondEntity);
    PutSet2(inst);
    sdaiUnsetAttrBN(inst, "DiamondRight.AttrCommonName");
    CheckDiamond(inst, "d2-AttrBase", "d2-L-CN", NULL, "d2-AR", "d2-Ch");

    //---------------------------------------------------------------------------------------------
    // compplex instance unset sdaiUnsetAttr
    inst = sdaiCreateInstance(model, entityTest);
    PutSet2(inst);
    sdaiUnsetAttrBN(inst, "AttrCommonName");
    CheckDiamond(inst, "d2-AttrBase", NULL, "d2-R-CN", "d2-AR", "d2-Ch");

    inst = sdaiCreateInstance(model, entityTest);
    PutSet2(inst);
    sdaiUnsetAttrBN(inst, "DiamondLeft.AttrCommonName");
    CheckDiamond(inst, "d2-AttrBase", NULL, "d2-R-CN", "d2-AR", "d2-Ch");

    inst = sdaiCreateInstance(model, entityTest);
    PutSet2(inst);
    sdaiUnsetAttrBN(inst, "DiamondRight.AttrCommonName");
    CheckDiamond(inst, "d2-AttrBase", "d2-L-CN", NULL, "d2-AR", "d2-Ch");

    //    
    sdaiCreateInstance(model, entityParenyAndChild);
    sdaiCreateInstanceBN(model, "DiamondRight");
    sdaiCreateInstanceBN(model, "DiamondLeft");
    sdaiCreateInstanceBN(model, "DiamondBase");

}

static void SmokeTestModelCheckContent(SdaiModel model)
{
    int_t i = 4;

    //complex instance
    auto inst = internalGetInstanceFromP21Line(model, i++);
    CheckDiamond(inst, "Diamond1-AttrBase", "Diamond1-ValueCommonName", NULL, NULL, "Diamond1-ValueAttrChild");

    inst = internalGetInstanceFromP21Line(model, i++);
    CheckDiamond(inst, "d2-AttrBase", "d2-L-CN", "d2-R-CN", "d2-AR", "d2-Ch");

    inst = internalGetInstanceFromP21Line(model, i++);
    CheckDiamond(inst, "d3-AttrBase", "d3-L-CN", "d3-R-CN", "d3-AR", NULL);

    //diamond
    inst = internalGetInstanceFromP21Line(model, i++);
    CheckDiamond(inst, "Diamond1-AttrBase", "Diamond1-ValueCommonName", NULL, NULL, "Diamond1-ValueAttrChild");

    inst = internalGetInstanceFromP21Line(model, i++);
    CheckDiamond(inst, "d2-AttrBase", "d2-L-CN", "d2-R-CN", "d2-AR", "d2-Ch");

    inst = internalGetInstanceFromP21Line(model, i++);
    CheckDiamond(inst, "d3-AttrBase", "d3-L-CN", "d3-R-CN", "d3-AR", NULL);

    //diamond unset
    inst = internalGetInstanceFromP21Line(model, i++);
    CheckDiamond(inst, "d2-AttrBase", NULL, "d2-R-CN", "d2-AR", "d2-Ch");

    inst = internalGetInstanceFromP21Line(model, i++);
    CheckDiamond(inst, "d2-AttrBase", NULL, "d2-R-CN", "d2-AR", "d2-Ch");

    inst = internalGetInstanceFromP21Line(model, i++);
    CheckDiamond(inst, "d2-AttrBase", "d2-L-CN", NULL, "d2-AR", "d2-Ch");

    //complex unset
    inst = internalGetInstanceFromP21Line(model, i++);
    CheckDiamond(inst, "d2-AttrBase", NULL, "d2-R-CN", "d2-AR", "d2-Ch");

    inst = internalGetInstanceFromP21Line(model, i++);
    CheckDiamond(inst, "d2-AttrBase", NULL, "d2-R-CN", "d2-AR", "d2-Ch");

    inst = internalGetInstanceFromP21Line(model, i++);
    CheckDiamond(inst, "d2-AttrBase", "d2-L-CN", NULL, "d2-AR", "d2-Ch");
}

static void SmokeTestSchema()
{
    ENTER_TEST;

    auto model = sdaiCreateModelBN(TEST_SCHEMA);
    ASSERT(model);
    SmokeTestModelPopulate(model);
    SmokeTestModelCheckContent(model);
    sdaiSaveModelBN(model, TEST_MODEL_SAVED);
    sdaiCloseModel(model);
    model = NULL;

    model = sdaiOpenModelBN(0, TEST_MODEL_SAVED, TEST_SCHEMA);
    ASSERT(model);
    SmokeTestModelCheckContent(model);
    sdaiCloseModel(model);
    model = NULL;

    model = sdaiOpenModelBN(0, TEST_MODEL, TEST_SCHEMA);
    ASSERT(model);
    SmokeTestModelCheckContent(model);
    sdaiCloseModel(model);
    model = NULL;
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
