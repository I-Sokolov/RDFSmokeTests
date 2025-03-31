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

static void CheckDiamondAttrs(SdaiInstance inst, AttrVal& attrVal)
{
    ASSERT(inst);

    for (auto& av : attrVal) {

        const char* val = NULL;
        auto ret = sdaiGetAttrBN(inst, av.first, sdaiSTRING, &val);
        ASSERT_STR_VAL(val , av.second);
        ASSERT((ret!=NULL) == (val!=NULL));

        auto ent = sdaiGetInstanceType(inst);
        auto name = engiGetEntityName(ent, sdaiSTRING);
        ASSERT(0==strcmp(name, "Diamond") || NULL != strstr(name, "+DIAMOND"));

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
    const char* valAttrRight, const char* valAttrChild,
    bool hasOwner = false
    )
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
    
    av["Drv"] = "Derived_DiamondLeft";
    av["DiamondLeft.Drv"] = "Derived_DiamondLeft";
    av["DiamondRight.Drv"] = "Derived_DiamondRight";

    CheckDiamondAttrs(inst, av);

    //
    auto model = sdaiGetInstanceModel(inst);

    auto left = sdaiGetEntity(model, "DiamondLeft");
    auto attrLeft = sdaiGetAttrDefinition(left, "AttrCommonName");
    auto attrDrvLeft = sdaiGetAttrDefinition(left, "Drv");
    ASSERT(attrLeft && attrDrvLeft);

    SdaiString val = NULL;;
    auto ret = sdaiGetAttr(inst, attrLeft, sdaiSTRING, &val);
    ASSERT_STR_VAL(val, valAttrCommonNameLeft);
    ASSERT((ret != NULL) == (val != NULL));

    val = NULL;
    ret = sdaiGetAttr(inst, attrDrvLeft, sdaiSTRING, &val);
    ASSERT(ret && !strcmp(val, "Derived_DiamondLeft"));

    auto right = sdaiGetEntity(model, "DiamondRight");
    auto attrRight = sdaiGetAttrDefinition(right, "AttrCommonName");
    auto attrDrvRight = sdaiGetAttrDefinition(right, "Drv");
    auto attrOwner = sdaiGetAttrDefinition(right, "Owner");
    ASSERT(attrRight && attrDrvRight && attrOwner);

    val = NULL;
    ret = sdaiGetAttr(inst, attrRight, sdaiSTRING, &val);
    ASSERT_STR_VAL(val, valAttrCommonNameRight);
    ASSERT((ret != NULL) == (val != NULL));

    val = NULL;
    ret = sdaiGetAttr(inst, attrDrvRight, sdaiSTRING, &val);
    ASSERT(ret && !strcmp(val, "Derived_DiamondRight"));

    //
    SdaiInstance owner = NULL;
    ret = sdaiGetAttrBN(inst, "Owner", sdaiINSTANCE, &owner);
    if (hasOwner) {
        ASSERT(ret && owner);
        ASSERT(internalGetP21Line(owner) == 1);
    }
    else {
        ASSERT(!ret && !owner);
    }

    SdaiInstance owner2 = NULL;
    auto ret2 = sdaiGetAttrBN(inst, "DiamondRight.Owner", sdaiINSTANCE, &owner2);
    ASSERT(owner2 == owner && ret2 == ret);
    owner2 = NULL;
    ret2 = sdaiGetAttr(inst, attrOwner, sdaiINSTANCE, &owner2);
    ASSERT(owner2 == owner && ret2 == ret);
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

static void PutSet2(SdaiInstance inst, SdaiInstance owner)
{
    sdaiPutAttrBN(inst, "DiamondBase.AttrBase", sdaiSTRING, "d2-AttrBase");
    sdaiPutAttrBN(inst, "DiamondLeft.AttrCommonName", sdaiSTRING, "d2-L-CN");
    sdaiPutAttrBN(inst, "DiamondRight.AttrCommonName", sdaiSTRING, "d2-R-CN");
    sdaiPutAttrBN(inst, "DiamondRight2.AttrRight", sdaiSTRING, "d2-AR");
    sdaiPutAttrBN(inst, "Diamond.AttrChild", sdaiSTRING, "d2-Ch");

    ///
    if (owner) {
        SdaiAggr aggr = NULL;
        sdaiGetAttrBN(owner, "HasDiamonds", sdaiAGGR, &aggr);
        auto N = sdaiGetMemberCount(aggr);

        sdaiPutAttrBN(inst, "Owner", owner);

        auto N2 = sdaiGetMemberCount(aggr);
        ASSERT(N + 1 == N2);
    }
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
    auto owner = sdaiCreateInstanceBN(model, "Person");
    sdaiCreateInstanceBN(model, "DiamondLeft");
    sdaiCreateInstanceBN(model, "DiamondRight");

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
    PutSet2(inst, owner);
    CheckDiamond(inst, "d2-AttrBase", "d2-L-CN", "d2-R-CN", "d2-AR", "d2-Ch", true);

    auto list = CreateEntitiesList(model, _countof(strTest), strTest);
    auto complexDiamond = sdaiCreateComplexInstance(model, list);
    sdaiDeleteNPL(list);

    PutSet3(complexDiamond);
    CheckDiamond(complexDiamond, "d3-AttrBase", "d3-L-CN", "d3-R-CN", "d3-AR", NULL);

    //--------------------------------------------------------------------------
    // diamond inheritance
    auto diamondEntity = sdaiGetEntity(model, "Diamond");
    ASSERT(diamondEntity);

    inst = sdaiCreateInstance(model, diamondEntity);
    PutSet1(inst);
    CheckDiamond(inst, "Diamond1-AttrBase", "Diamond1-ValueCommonName", NULL, NULL, "Diamond1-ValueAttrChild");

    inst = sdaiCreateInstance(model, diamondEntity);
    PutSet2(inst, owner);
    CheckDiamond(inst, "d2-AttrBase", "d2-L-CN", "d2-R-CN", "d2-AR", "d2-Ch", true);

    auto simpleDiamond = sdaiCreateInstance(model, diamondEntity);
    PutSet3(simpleDiamond);
    CheckDiamond(simpleDiamond, "d3-AttrBase", "d3-L-CN", "d3-R-CN", "d3-AR", NULL);

    //---------------------------------------------------------------------------------------------
    // diamond sdaiUnsetAttr
    inst = sdaiCreateInstance(model, diamondEntity);
    PutSet2(inst, NULL);
    sdaiUnsetAttrBN(inst, "AttrCommonName");
    CheckDiamond(inst, "d2-AttrBase", NULL, "d2-R-CN", "d2-AR", "d2-Ch");

    inst = sdaiCreateInstance(model, diamondEntity);
    PutSet2(inst, owner);
    sdaiUnsetAttrBN(inst, "DiamondLeft.AttrCommonName");
    sdaiUnsetAttrBN(inst, "Owner");
    CheckDiamond(inst, "d2-AttrBase", NULL, "d2-R-CN", "d2-AR", "d2-Ch");

    inst = sdaiCreateInstance(model, diamondEntity);
    PutSet2(inst, owner);
    sdaiUnsetAttrBN(inst, "DiamondRight.AttrCommonName");
    sdaiUnsetAttrBN(inst, "DiamondRight.Owner");
    CheckDiamond(inst, "d2-AttrBase", "d2-L-CN", NULL, "d2-AR", "d2-Ch");

    //---------------------------------------------------------------------------------------------
    // compplex instance unset sdaiUnsetAttr
    inst = sdaiCreateInstance(model, entityTest);
    PutSet2(inst, owner);
    sdaiUnsetAttrBN(inst, "AttrCommonName");
    sdaiUnsetAttrBN(inst, "Owner");
    CheckDiamond(inst, "d2-AttrBase", NULL, "d2-R-CN", "d2-AR", "d2-Ch");

    inst = sdaiCreateInstance(model, entityTest);
    PutSet2(inst, owner);
    sdaiUnsetAttrBN(inst, "DiamondLeft.AttrCommonName");
    sdaiUnsetAttrBN(inst, "DiamondRight.Owner");
    CheckDiamond(inst, "d2-AttrBase", NULL, "d2-R-CN", "d2-AR", "d2-Ch");

    inst = sdaiCreateInstance(model, entityTest);
    PutSet2(inst, owner);
    sdaiUnsetAttrBN(inst, "DiamondRight.AttrCommonName");
    sdaiUnsetAttrBN(inst, "DiamondRight.Owner");
    CheckDiamond(inst, "d2-AttrBase", "d2-L-CN", NULL, "d2-AR", "d2-Ch");

    //    
    sdaiCreateInstance(model, entityParenyAndChild);
    sdaiCreateInstanceBN(model, "DiamondRight");
    sdaiCreateInstanceBN(model, "DiamondLeft");
    sdaiCreateInstanceBN(model, "DiamondBase");

    //
    inst = sdaiCreateInstanceBN(model, "Child");
    sdaiPutAttrBN(inst, "LDiamond", sdaiINSTANCE, complexDiamond);
    inst = sdaiCreateInstanceBN(model, "Child");
    sdaiPutAttrBN(inst, "LDiamond", sdaiINSTANCE, simpleDiamond);

    inst = sdaiCreateInstanceBN(model, "Parent");
    sdaiPutAttrBN(inst, "RDiamond", sdaiINSTANCE, complexDiamond);
    inst = sdaiCreateInstanceBN(model, "Parent");
    sdaiPutAttrBN(inst, "RDiamond", sdaiINSTANCE, simpleDiamond);

    inst = sdaiCreateInstance(model, entityParenyAndChild);
    sdaiPutAttrBN(inst, "RDiamond", sdaiINSTANCE, complexDiamond);
    sdaiPutAttrBN(inst, "LDiamond", sdaiINSTANCE, complexDiamond);

    inst = sdaiCreateInstance(model, entityParenyAndChild);
    sdaiPutAttrBN(inst, "RDiamond", sdaiINSTANCE, simpleDiamond);
    sdaiPutAttrBN(inst, "LDiamond", sdaiINSTANCE, simpleDiamond);
}

static void CheckInstAggr(SdaiAggr aggr, int_t N, ExpressID rid[])
{
    auto n = sdaiGetMemberCount(aggr);
    ASSERT(n >= N);//igor.sokolov 16.03.2025 NEED SWITCH FROM attrIndex TO attrPtr uncomment after unset fix //ASSERT(N == _countof(ids2));
    for (SdaiAggrIndex i = 0; i < N; i++) {
        SdaiInstance inst = 0;
        sdaiGetAggrByIndex(aggr, i, &inst);
        auto id = internalGetP21Line(inst);
        ASSERT(id == rid[i]);
    }

}

static void     CheckInv(SdaiInstance inst, ExpressID rInstLeft[], ExpressID rInstRight[])
{
    auto model = sdaiGetInstanceModel(inst);

    SdaiAggr aggr = NULL;
    auto ret = sdaiGetAttrBN(inst, "Inv", sdaiAGGR, &aggr);
    CheckInstAggr(aggr, 2, rInstLeft);

    aggr = NULL;
    ret = sdaiGetAttrBN(inst, "DiamondLeft.Inv", sdaiAGGR, &aggr);
    CheckInstAggr(aggr, 2, rInstLeft);

    aggr = NULL;
    ret = sdaiGetAttrBN(inst, "DiamondRight.Inv", sdaiAGGR, &aggr);
    CheckInstAggr(aggr, 2, rInstRight);

    auto entityRight = sdaiGetEntity(model, "DiamondRight");
    auto attrInvRight = sdaiGetAttrDefinition(entityRight, "Inv");
    ASSERT(attrInvRight);

    aggr = NULL;
    ret = sdaiGetAttr(inst, attrInvRight, sdaiAGGR, &aggr);
    CheckInstAggr(aggr, 2, rInstRight);
}

static void SmokeTestModelCheckContent(SdaiModel model)
{
    int_t i = 4;

    //complex instance
    auto inst = internalGetInstanceFromP21Line(model, i++);
    CheckDiamond(inst, "Diamond1-AttrBase", "Diamond1-ValueCommonName", NULL, NULL, "Diamond1-ValueAttrChild");

    inst = internalGetInstanceFromP21Line(model, i++);
    CheckDiamond(inst, "d2-AttrBase", "d2-L-CN", "d2-R-CN", "d2-AR", "d2-Ch", true);

    inst = internalGetInstanceFromP21Line(model, i++);
    CheckDiamond(inst, "d3-AttrBase", "d3-L-CN", "d3-R-CN", "d3-AR", NULL);

    //diamond
    inst = internalGetInstanceFromP21Line(model, i++);
    CheckDiamond(inst, "Diamond1-AttrBase", "Diamond1-ValueCommonName", NULL, NULL, "Diamond1-ValueAttrChild");

    inst = internalGetInstanceFromP21Line(model, i++);
    CheckDiamond(inst, "d2-AttrBase", "d2-L-CN", "d2-R-CN", "d2-AR", "d2-Ch", true);

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

    //
    ExpressID ids[] = { 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 18 };
    auto extent = xxxxGetEntityAndSubTypesExtentBN(model, "DIAMONDLEFT");
    CheckInstAggr(extent, _countof(ids), ids);

    //inverse to complex/diamond
    ExpressID ids2[] =  { 5, 8 };
    auto owner = internalGetInstanceFromP21Line(model, 1);
    SdaiAggr aggr = NULL;
    auto ret = sdaiGetAttrBN(owner, "HasDiamonds", sdaiAGGR, &aggr);
    ASSERT(ret && aggr && ret == aggr);
    CheckInstAggr(aggr, _countof(ids2), ids2);

    //inverse from complex/diamond    
    ExpressID rInst6Left[] = { 20, 24 };
    ExpressID rInst6Right[] = { 22, 24 };
    inst = internalGetInstanceFromP21Line(model, 6);
    CheckInv(inst, rInst6Left, rInst6Right);
    
    ExpressID rInst9Right[] = { 23, 25 };
    ExpressID rInst9Left[] = { 21, 25 };
    inst = internalGetInstanceFromP21Line(model, 9);
    CheckInv(inst, rInst9Left, rInst9Right);
}

static void DeleteInstance(SdaiModel model, ExpressID id)
{
    auto inst = internalGetInstanceFromP21Line(model, id);
    ASSERT(inst);

    sdaiDeleteInstance(inst);
}

static void SmokeTestSchema()
{
    ENTER_TEST;

    auto model = sdaiCreateModelBN(TEST_SCHEMA);
    ASSERT(model);
    engiEnableDerivedAttributes(model, sdaiTRUE);
    SmokeTestModelPopulate(model);
    SmokeTestModelCheckContent(model);
    sdaiSaveModelBN(model, TEST_MODEL_SAVED);
    sdaiCloseModel(model);
    model = NULL;

    model = sdaiOpenModelBN(0, TEST_MODEL_SAVED, TEST_SCHEMA);
    ASSERT(model);
    engiEnableDerivedAttributes(model, sdaiTRUE);
    SmokeTestModelCheckContent(model);
    sdaiCloseModel(model);
    model = NULL;

    model = sdaiOpenModelBN(0, TEST_MODEL, TEST_SCHEMA);
    ASSERT(model);
    engiEnableDerivedAttributes(model, sdaiTRUE);
    SmokeTestModelCheckContent(model);

    //backlinks consistency after removing instaces
    DeleteInstance(model, 5);
    DeleteInstance(model, 9);
    DeleteInstance(model, 6);
    sdaiSaveModelBN(model, TEST_MODEL_SAVED);//to check backlinks consistency

    sdaiCloseModel(model);
    model = NULL;
}

static void ReadWithoutSchema()
{
/*
    auto model = sdaiOpenModelBN(0, TEST_MODEL, NULL);
    ASSERT(model);

    Instance inst11[] = { {"DIAMOND", 6} };
    CheckComplex(model, 11, inst11, 1);

    sdaiCloseModel(model);
    */
}

extern void ComplexInstance()
{
    CheckComplex();
    SmokeTestSchema();
    ReadWithoutSchema();
}


