
#include "pch.h"


static void DeleteWalls(bool subtypes)
{
    SdaiModel   model = sdaiOpenModelBN(0, "Walls.ifc", "");

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
    SdaiModel   model = sdaiOpenModelBN(0, "Walls.ifc", "");

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


extern void AggregationTests()
{
    ENTER_TEST;

    DeleteWalls(true);
    DeleteWalls(false);

    DeleteWallsByIndex(true);
    DeleteWallsByIndex(false);

}