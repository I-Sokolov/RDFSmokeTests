
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

        auto a3 = sdaiGetEntityExtentBN(model, "IfcWall");
        ASSERT(a3 != entityAggr);

        auto a2 = xxxxGetEntityAndSubTypesExtentBN(model, "IfcWall");
        ASSERT(a2 == entityAggr);
        auto a4 = sdaiGetEntityExtentBN(model, "IfcWall");
        ASSERT(a4 == a3);
    }
    else {
        entityAggr = sdaiGetEntityExtentBN(model, "IfcWallStandardCase");

        auto a2 = sdaiGetEntityExtentBN(model, "IfcWallStandardCase");
        ASSERT(a2 == entityAggr);

        auto a3 = xxxxGetEntityAndSubTypesExtentBN(model, "IfcWall");
        ASSERT(a3 != a2);
        auto a4 = xxxxGetEntityAndSubTypesExtentBN(model, "IfcWall");
        ASSERT(a4 == a3);
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

    SdaiAggr a2 = 0;
    sdaiGetAttrBN(inst, attr, sdaiAGGR, &a2);
    ASSERT(a2 == aggr);

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
    ASSERT(sdaiGetAggrElementBoundByItr(it) == -1);

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

    SdaiAggr aggrDefinedBy = NULL;
    lstInst.clear();
    wall.get_IsDefinedBy(lstInst);
    sdaiGetAttrBN(wall, "IsDefinedBy", sdaiAGGR, &aggrDefinedBy);
    TestIterators(aggrDefinedBy, lstInst);
    TestIsMember(aggrDefinedBy, lstInst, wall);

    //test inverse aggregation reallocation
    SdaiAggr aggr2 = NULL;
    sdaiGetAttrBN(wall, "HasAssociations", sdaiAGGR, &aggr2);
    ASSERT(aggr2 == aggr);
    sdaiGetAttrBN(wall, "IsDefinedBy", sdaiAGGR, &aggr2);
    ASSERT(aggr2 == aggrDefinedBy);


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
    ASSERT(aggr == xxxxGetAllInstances(model));
    lstInst.clear();
    for (int i = 10; i < 420; i++) {
        auto inst = internalForceInstanceFromP21Line(model, i);
        if (inst) {
            lstInst.push_back(inst);
        }
    }
    TestIterators(aggr, lstInst);
    TestIsMember(aggr, lstInst, 0);

    //
    aggr = NULL;
    engiGetNotReferedAggr(model, (int_t*) & aggr);
    auto cnt = sdaiGetMemberCount(aggr);
    ASSERT(cnt == 31);

    aggr = NULL;
    engiGetAttributeAggr(wall, (int_t*)&aggr);
    cnt = sdaiGetMemberCount(aggr);
    ASSERT(cnt == 9);

    aggr = xxxxGetInstancesUsing(pset);
    cnt = sdaiGetMemberCount(aggr);
    ASSERT(cnt == 1);

    sdaiCloseModel(model);
}

template <typename T>
static void TestIsMember(SdaiAggr aggr, SdaiPrimitiveType sdaiType, T* rMembers, int nMembers, T notMember)
{
    for (int i = 0; i < nMembers; i++) {
        ASSERT(sdaiIsMember(aggr, sdaiType, rMembers[i]));
    }

    ASSERT(!sdaiIsMember(aggr, sdaiType, notMember));
}

static void TestAggregationFunctions(SdaiAggr aggr, SdaiInteger numElems, SdaiInteger elementBound)
{
    ASSERT(sdaiGetLowerBound(aggr) == 0);
    ASSERT(sdaiGetUpperBound(aggr) == numElems-1);

    SdaiIterator it = sdaiCreateIterator(aggr);
    sdaiNext(it);
    ASSERT(sdaiGetAggrElementBoundByItr(it) == elementBound);
    sdaiDeleteIterator(it);
}

static void ExplicitAggregationsVariousTypes()
{
    SdaiModel   model = sdaiCreateModelBN(0, "test IFC4", "IFC4");
    SdaiModel   model4x4 = sdaiCreateModelBN(0, "test IFC4x4", "IFC4x4");
    SdaiModel   model242 = sdaiCreateModelBN(0, "test AP242", "AP242");

    //
    double rd[] = { 1,2.5,5 };
    auto pt = IFC4::IfcCartesianPoint::Create(model);
    pt.put_Coordinates(rd, 3);
    
    SdaiAggr aggr = NULL;
    sdaiGetAttrBN(pt, "Coordinates", sdaiAGGR, &aggr);
    TestIsMember(aggr, sdaiREAL, rd, 3, 8 + 1e-11);
    TestAggregationFunctions(aggr, 3, 12);

    //
    SdaiInteger lat [] = { 20,2,3,1 };
    auto site = IFC4::IfcSite::Create(model);
    site.put_RefLatitude(lat, 4);

    aggr = 0;
    sdaiGetAttrBN(site, "RefLatitude", sdaiAGGR, &aggr);
    TestIsMember(aggr, sdaiINTEGER, lat, 4, lat[0]+1);
    TestAggregationFunctions(aggr, 4, -1);

    //
    SdaiBoolean voxels[] = { true };
    auto voxelGrid = IFC4x4::IfcVoxelGrid::Create(model4x4);
    voxelGrid.put_Voxels(voxels, 1);

    aggr = 0;
    sdaiGetAttrBN(voxelGrid, "Voxels", sdaiAGGR, &aggr);
    TestIsMember(aggr, sdaiBOOLEAN, voxels, 1, sdaiFALSE);
    TestAggregationFunctions(aggr, 1, -1);

    //
    IFC4x4::IfcLogical voxelDataValues[] = { IFC4x4::IfcLogical::False, IFC4x4::IfcLogical::Unknown };
    SdaiString voxelDataValues_s[] = { "F", "U" };
    auto voxelData = IFC4x4::IfcLogicalVoxelData::Create(model4x4);
    voxelData.put_ValueData(voxelDataValues, 2);

    aggr = 0;
    sdaiGetAttrBN(voxelData, "ValueData", sdaiAGGR, &aggr);
    TestIsMember(aggr, sdaiLOGICAL, voxelDataValues_s, 2, "T");
    TestAggregationFunctions(aggr, 2, -1);

    //
    SdaiString rNames[] = { "Domingo", "Felipe", "Jacinto" };
    auto person = IFC4::IfcPerson::Create(model);
    person.put_MiddleNames(rNames, 3);

    aggr = 0;
    sdaiGetAttrBN(person, "MiddleNames", sdaiAGGR, &aggr);
    TestIsMember(aggr, sdaiSTRING, rNames, 3, "Salvador");
    TestAggregationFunctions(aggr, 3, strlen(rNames[0]));

    //
    SdaiString rBin[] = { "31", "23B", "092A" };
    auto texture = IFC4::IfcPixelTexture::Create(model);
    texture.put_Pixel(rBin, 3);

    aggr = 0;
    sdaiGetAttrBN(texture, "Pixel", sdaiAGGR, &aggr);
    TestIsMember(aggr, sdaiBINARY, rBin, 3, "30");
    TestAggregationFunctions(aggr, 3, strlen(rBin[0]));

    //
    AP242::a3m_element_type_name a3mtypes[] = { AP242::a3m_element_type_name::etna_shape_representation, AP242::a3m_element_type_name::etns_point_cloud_dataset };
    SdaiString a3mtypes_s[] = { "etna_shape_representation", "etns_point_cloud_dataset"};

    auto criterion = AP242::different_component_identification_via_multi_level_reference::Create(model242);
    criterion.put_compared_element_types(a3mtypes, 2);

    aggr = 0;
    sdaiGetAttrBN(criterion, "compared_element_types", sdaiAGGR, &aggr);
    TestIsMember(aggr, sdaiENUM, a3mtypes_s, 2, "eeee");
    TestAggregationFunctions(aggr, 2, -1);

    //
    sdaiCloseModel(model);
    sdaiCloseModel(model4x4);
    sdaiCloseModel(model242);
}

static IFC4::IfcCartesianPointList2D CreatePointList(SdaiModel model, double* rpt, int npt)
{
    auto points = IFC4::IfcCartesianPointList2D::Create(model);

    IFC4::ListOfListOfIfcLengthMeasure lstCoords;
    for (int i = 0; i < npt; i++) {
        lstCoords.push_back(IFC4::ListOfIfcLengthMeasure());
        for (int j = 0; j < 2; j++) {
            lstCoords.back().push_back(rpt[2 * i + j]);
        }
    }

    points.put_CoordList(lstCoords);

    return points;
}

static void IsMemberADB(SdaiAggr aggr, SdaiADB notMember)
{
    SdaiADB chADB = NULL;
    sdaiGetAggrByIndex(aggr, 1, sdaiADB, &chADB);
    ASSERT(sdaiIsMember(aggr, sdaiADB, chADB));

    ASSERT(!sdaiIsMember(aggr, sdaiADB, notMember));

    notMember = sdaiCreateADB(sdaiSTRING, "not a member");
    ASSERT(!sdaiIsMember(aggr, sdaiADB, notMember));

    sdaiDeleteADB(notMember);
}

static void IsMemberComplex()
{
    SdaiModel   model = sdaiCreateModelBN(0, "test IFC4", "IFC4");

    //
    // aggrgation of aggregations - points list
    // 
 
    double rpt[] = {
        0,0,
        1,0,
        1,1,
        0,1
    };
    
    auto points = CreatePointList(model, rpt, 4);

    double check[] = {
        1 + 1e-11,1 - 1e-11,
        0,0.5
    };

    auto checks = CreatePointList(model, check, 2);

    SdaiAggr aggr = NULL;
    sdaiGetAttrBN(points, "CoordList", sdaiAGGR, &aggr);

    SdaiAggr acheck = NULL;
    sdaiGetAttrBN(checks, "CoordList", sdaiAGGR, &acheck);

    SdaiAggr ch = NULL;
    sdaiGetAggrByIndex(acheck, 0, sdaiAGGR, &ch);
    ASSERT(sdaiIsMember(aggr, sdaiAGGR, ch));

    SdaiPrimitiveType valueType = 0;
    engiGetAggrUnknownElement(acheck, 0, &valueType, &ch);
    ASSERT(valueType == sdaiAGGR);
    ASSERT(sdaiIsMember(aggr, sdaiAGGR, ch));

    ch = NULL;
    sdaiGetAggrByIndex(acheck, 1, sdaiAGGR, &ch);
    ASSERT( ! sdaiIsMember(aggr, sdaiAGGR, ch));

    //
    //create IfcIndexedPolyCurve
    //
    auto poly = IFC4::IfcIndexedPolyCurve::Create(model);


    //indexes of line and arc;
    IFC4::IfcPositiveInteger line[] = { 0,1 };
    IFC4::IfcPositiveInteger arc[] = { 1,2,3 };

    //create points list
    //

    //create segments list
    //
    IFC4::ListOfIfcSegmentIndexSelect segments;

    IFC4::IfcSegmentIndexSelect segment(poly);
    segment.put_IfcLineIndex(line, 2);
    segments.push_back(segment);

    segment.put_IfcArcIndex(arc, 3);
    segments.push_back(segment);

    //
    poly.put_Segments(segments);
    poly.put_Points(points);
    poly.put_SelfIntersect(false);

    //
    auto aTest = sdaiCreateAggr(poly, 0);
    int_t iTest = 0;
    sdaiAppend(aTest, sdaiINTEGER, &iTest);
    auto notMember = sdaiCreateADB(sdaiAGGR, aTest);
    sdaiPutADBTypePath(notMember, 1, "IFCLINEINDEX");

    //aggregation of ADB
    aggr = NULL;
    sdaiGetAttrBN(poly, "Segments", sdaiAGGR, &aggr);
    IsMemberADB(aggr, notMember);

    valueType = 0;
    int_t ttt = NULL;
    engiGetAggrUnknownElement(aggr, 0, &valueType, &ttt);
    ASSERT(valueType == sdaiAGGR);
    ASSERT(sdaiGetMemberCount((SdaiAggr)ttt)==1);

    aggr = NULL;
    sdaiGetAttrBN(points, "CoordList", sdaiAGGR, &aggr);
    IsMemberADB(aggr, notMember);

    //sdaiSaveModelBN(model, "testIsMebber.ifc");
    sdaiCloseModel(model);
}

static void CheckAdd(SdaiModel model, int64_t pointsId)
{
    IFC4::IfcCartesianPointList2D points = internalGetInstanceFromP21Line(model, pointsId);

    SdaiAggr coordList = NULL;
    auto res = sdaiGetAttrBN(points, "CoordList", sdaiAGGR, &coordList);
    ASSERT(res && sdaiGetMemberCount(coordList) == 2);

    for (int i = 0; i < 2; i++) {

        SdaiAggr pt = NULL;
        res = sdaiGetAggrByIndex(coordList, i, sdaiAGGR, &pt);
        ASSERT(res && sdaiGetMemberCount(pt) == 2);

        for (int j = 0; j < 2; j++) {

            double dval = 0;
            res = sdaiGetAggrByIndex(pt, j, sdaiREAL, &dval);
            ASSERT(fabs(dval - 1.23) < 1e-5);
        }
    }
}

static void Add()
{
    SdaiModel   model = sdaiCreateModelBN(0, "test IFC4", "IFC4");

    auto points = IFC4::IfcCartesianPointList2D::Create(model);
    int64_t pointsId = internalGetP21Line(points);

    SdaiAggr coordList = sdaiCreateAggrBN(points, "CoordList");

    SdaiAggr pt1 = sdaiCreateNestedAggr(coordList);

    double dval = 1.23;
    SdaiADB adbval = sdaiCreateADB(sdaiREAL, &dval);

    sdaiAdd(pt1, sdaiREAL, &dval);
    sdaiAdd(pt1, sdaiADB, adbval);
    
    SdaiAggr pt2 = sdaiCreateNestedAggrADB(coordList, adbval);
    sdaiAdd(pt2, sdaiREAL, &dval);

    const char* testFile = "TestAggregationAdd.ifc";
    sdaiSaveModelBN(model, testFile);

    CheckAdd(model, pointsId);

    sdaiCloseModel(model);

    //TODO sdaiOpenModelBN (0, )
}


extern void AggregationTests()
{
    ENTER_TEST;

    Add();
    Iterators();
    ExplicitAggregationsVariousTypes();
    IsMemberComplex();
    Delete();
}