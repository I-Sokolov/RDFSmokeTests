
#include "pch.h"

#define TEST_FILE_DEL "..\\TestData\\Walls.ifc"
#define TEST_FILE_AGGR "..\\TestData\\AggregationTest.ifc"

static void DeleteWalls(bool subtypes)
{
    SdaiModel   model = sdaiOpenModelBN(0, TEST_FILE_DEL, "");

    int cnt = 0;

    SdaiAggr entityAggr = 0;
    if (subtypes) {
        entityAggr = xxxxGetEntityAndSubTypesExtentBN(model, "IfcWall");
    }
    else {
        entityAggr = sdaiGetEntityExtentBN(model, "IfcWallStandardCase");
    }

    while (true) {
        SdaiInstance    ifcInstance = 0;
        engiGetAggrElement(entityAggr, 0, sdaiINSTANCE, &ifcInstance);
        if (ifcInstance) {
            sdaiDeleteInstance(ifcInstance);
            cnt++;
        }
        else {
            break;
        }
    }

    ASSERT(cnt == 14);

    //sdaiSaveModelBN(model, "Walls_del.ifc");
    sdaiCloseModel(model);
}

static void DeleteWallsByIndex(bool subtypes)
{
    SdaiModel   model = sdaiOpenModelBN(0, TEST_FILE_DEL, "");

    int cnt = 0;

    SdaiAggr entityAggr = 0;
    if (subtypes) {
        entityAggr = xxxxGetEntityAndSubTypesExtentBN(model, "IfcWall");
    }
    else {
        entityAggr = sdaiGetEntityExtentBN(model, "IfcWallStandardCase");
    }

    SdaiInstance    ifcInstance = 0;
    while (engiGetAggrElement(entityAggr, 0, sdaiINSTANCE, &ifcInstance)) {
        sdaiDeleteInstance(ifcInstance);
        cnt++;
    }

    ASSERT(cnt == 14);

    //sdaiSaveModelBN(model, "Walls_del.ifc");
    sdaiCloseModel(model);
}

static void CheckAggrElem(SdaiInstance inst, const char* attr, SdaiInteger index, int stepId)
{
    SdaiAggr aggr = 0;
    auto ret = sdaiGetAttrBN(inst, attr, sdaiAGGR, &aggr);
    ASSERT(ret == aggr && aggr);

    auto cnt = sdaiGetMemberCount(aggr);
    ASSERT(index < cnt);

    SdaiInstance val = 0;
    ret = sdaiGetAggrByIndex(aggr, index, sdaiINSTANCE, &val);
    ASSERT(ret == (void*)val && val);

    auto sid = internalGetP21Line(val);
    ASSERT(sid == stepId);
}


static void DeleteAttrAggrByIndex()
{
    SdaiModel   model = sdaiOpenModelBN(0, TEST_FILE_DEL, "");

    auto unitAssmt = internalGetInstanceFromP21Line(model, 975);

    CheckAggrElem (unitAssmt, "Units", 0, 947);

    SdaiAggr units = NULL;
    sdaiGetAttrBN(unitAssmt, "Units", sdaiAGGR, &units);
    ASSERT(units);

    xxxxDeleteFromAggregation(unitAssmt, units, 0);

    CheckAggrElem(unitAssmt, "Units", 0, 948);
 
    CheckAggrElem(unitAssmt, "Units", 2, 950);

    xxxxDeleteFromAggregation(unitAssmt, units, 2);

    CheckAggrElem(unitAssmt, "Units", 2, 951);

    SdaiInteger last = sdaiGetMemberCount(units) - 1;

    CheckAggrElem(unitAssmt, "Units", last, 858);

    xxxxDeleteFromAggregation(unitAssmt, units, last);

    ASSERT(sdaiGetMemberCount(units) == last);

    CheckAggrElem(unitAssmt, "Units", last-1, 857);

    //sdaiSaveModelBN(model, "Walls_del.ifc");
    sdaiCloseModel(model);
}

static void Delete()
{
    DeleteWalls(true);
    DeleteWalls(false);

    DeleteWallsByIndex(true);
    DeleteWallsByIndex(false);

    DeleteAttrAggrByIndex();
}

template <typename TIterator> static void TestAndNextExpected(SdaiIterator it, TIterator& expected)
{
    SdaiInstance inst = NULL;
    auto ret = sdaiGetAggrByIterator(it, sdaiINSTANCE, &inst);
    ASSERT(ret && inst == *expected);
    expected++;
}

static void TestIteratorsForward(SdaiIterator it1, SdaiIterator it2, std::list<SdaiInstance>& expected)
{
    auto exp1 = expected.begin();
    auto exp2 = expected.begin();

    SdaiInstance inst = 0;
    ASSERT(!sdaiGetAggrByIterator(it1, sdaiINSTANCE, &inst));

    ASSERT(sdaiNext(it1));
    TestAndNextExpected(it1, exp1);

    while (sdaiNext(it1)) {

        sdaiPrevious(it1);
        exp1--;
        TestAndNextExpected(it1, exp1);

        sdaiNext(it1);
        TestAndNextExpected(it1, exp1);

        ASSERT(sdaiNext(it2));
        TestAndNextExpected(it2, exp2);
    }

    ASSERT(!sdaiNext(it1));

    ASSERT(sdaiNext(it2));
    TestAndNextExpected(it2, exp2);

    ASSERT(!sdaiNext(it2));
}

static void TestIteratorsReverse(SdaiIterator it1, SdaiIterator it2, std::list<SdaiInstance>& expected)
{
    auto exp1 = expected.begin();
    auto exp2 = expected.rbegin();

    SdaiInstance inst = 0;
    ASSERT(!sdaiGetAggrByIterator(it1, sdaiINSTANCE, &inst));

    ASSERT(sdaiNext(it1));
    TestAndNextExpected(it1, exp1);

    while (sdaiNext(it1)) {

        TestAndNextExpected(it1, exp1);

        ASSERT(sdaiPrevious(it2));
        TestAndNextExpected(it2, exp2);
    }

    ASSERT(!sdaiNext(it1));

    ASSERT(sdaiPrevious(it2));
    TestAndNextExpected(it2, exp2);

    ASSERT(!sdaiPrevious(it2));
}

static void TestIterators(SdaiAggr aggr, std::list<SdaiInstance>& expected)
{
    auto cnt = sdaiGetMemberCount(aggr);
    ASSERT(cnt == expected.size());

    auto it1 = sdaiCreateIterator(aggr);    
    auto it2 = sdaiCreateIterator(aggr);
    TestIteratorsForward(it1, it2, expected);

    sdaiBeginning(it1);
    sdaiBeginning(it2);
    TestIteratorsForward(it1, it2, expected);

    sdaiBeginning(it1);
    sdaiEnd(it2);
    TestIteratorsReverse(it1, it2, expected);

    sdaiDeleteIterator(it1);
    sdaiDeleteIterator(it2);
}

static void TestIsMember(SdaiAggr aggr, std::list<SdaiInstance>& members, SdaiInstance notMember)
{
    for (auto& m : members) {
        ASSERT(sdaiIsMember(aggr, sdaiINSTANCE, m));
    }

    ASSERT(!sdaiIsMember(aggr, sdaiINSTANCE, notMember));
}

static void Iterators()
{
    SdaiModel   model = sdaiOpenModelBN(0, TEST_FILE_AGGR, "");

    //arguments
    IFC4::IfcPropertySet pset = internalGetInstanceFromP21Line(model, 124);
    std::list<SdaiInstance> lstInst;
    pset.get_HasProperties(lstInst);
    SdaiAggr aggr = 0;
    sdaiGetAttrBN(pset, "HasProperties", sdaiAGGR, &aggr);
    TestIterators(aggr, lstInst);
    TestIsMember(aggr, lstInst, pset);

    //inverse
    IFC4::IfcWall wall = internalGetInstanceFromP21Line(model, 68);
    lstInst.clear();
    aggr = NULL;
    wall.get_HasAssociations(lstInst);
    sdaiGetAttrBN(wall, "HasAssociations", sdaiAGGR, &aggr);
    TestIterators(aggr, lstInst);
    TestIsMember(aggr, lstInst, wall);

    lstInst.clear();
    aggr = NULL;
    wall.get_IsDefinedBy(lstInst);
    sdaiGetAttrBN(wall, "IsDefinedBy", sdaiAGGR, &aggr);
    TestIterators(aggr, lstInst);
    TestIsMember(aggr, lstInst, wall);

    //entity range
    aggr = sdaiGetEntityExtentBN(model, "IfcWall");
    lstInst.clear();
    lstInst.push_back(wall);
    TestIterators(aggr, lstInst);
    TestIsMember(aggr, lstInst, pset);

    //entity range with subtypes
    aggr = xxxxGetEntityAndSubTypesExtentBN(model, "IfcElement");
    lstInst.push_front(internalGetInstanceFromP21Line(model, 13));
    TestIterators(aggr, lstInst);
    TestIsMember(aggr, lstInst, pset);

    //all entities
    aggr = xxxxGetAllInstances(model);
    lstInst.clear();
    for (int i = 10; i < 420; i++) {
        auto inst = internalForceInstanceFromP21Line(model, i);
        if (inst) {
            lstInst.push_back(inst);
        }
    }
    TestIterators(aggr, lstInst);
    TestIsMember(aggr, lstInst, 0);

    sdaiCloseModel(model);
}

extern void AggregationTests()
{
    ENTER_TEST;

    Iterators();
    Delete();
}