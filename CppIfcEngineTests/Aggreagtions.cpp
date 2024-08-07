
#include "pch.h"

#define TEST_FILE "..\\TestData\\Walls.ifc"

static void DeleteWalls(bool subtypes)
{
    SdaiModel   model = sdaiOpenModelBN(0, TEST_FILE, "");

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
    SdaiModel   model = sdaiOpenModelBN(0, TEST_FILE, "");

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
    SdaiModel   model = sdaiOpenModelBN(0, TEST_FILE, "");

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


extern void AggregationTests()
{
    ENTER_TEST;

    DeleteWalls(true);
    DeleteWalls(false);

    DeleteWallsByIndex(true);
    DeleteWallsByIndex(false);

    DeleteAttrAggrByIndex();

}