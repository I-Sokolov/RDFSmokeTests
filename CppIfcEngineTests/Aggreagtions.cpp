
#include "pch.h"

#define TEST_FILE_DEL   "..\\TestData\\Walls.ifc"
#define TEST_FILE_AGGR  "..\\TestData\\AggregationTest.ifc"
#define TEST_SAVE       "AggregationTest.ifc"


static void* Value2Ptr(SdaiPrimitiveType valueType, va_list value)
{
    switch (valueType) {
        case 0:
            return nullptr;

        case sdaiINTEGER:
        case sdaiREAL:
        case sdaiBOOLEAN:
            //need &
            return value;
            break;

        case sdaiLOGICAL:
        case sdaiSTRING:
        case sdaiBINARY:
        case sdaiENUM:
        case sdaiADB:
        case sdaiINSTANCE:
        case sdaiAGGR:
            //do not need &
            return va_arg(value, void*);
            break;

        default:
            assert(false); //not supported value type
            return nullptr;
    }
}


void sdaiAdd_(
    const SdaiAggr			aggregate,
    SdaiPrimitiveType		valueType,
    ...
)
{
    va_list value;
    va_start(value, valueType);

    sdaiAppend(aggregate, valueType, Value2Ptr(valueType, value));

    va_end(value);
}


static void DeleteWalls(bool subtypes)
{
    SdaiModel   model = sdaiOpenModelBN(0, TEST_FILE_DEL, "");

    sdaiSaveModelBN(model, TEST_SAVE);//to check backlinks consistency

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
            sdaiSaveModelBN(model, TEST_SAVE);//to check backlinks consistency
        }
        else {
            break;
        }
    }

    ASSERT(cnt == 14);

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
        sdaiSaveModelBN(model, TEST_SAVE);//to check backlinks consistency
        cnt++;
    }

    ASSERT(cnt == 14);

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
    ASSERT((SdaiInstance)ret == val && val);

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

    sdaiSaveModelBN(model, TEST_SAVE);//to check backlinks consistency

    SdaiInteger last = sdaiGetMemberCount(units) - 1;

    CheckAggrElem(unitAssmt, "Units", last, 858);

    xxxxDeleteFromAggregation(unitAssmt, units, last);

    ASSERT(sdaiGetMemberCount(units) == last);

    CheckAggrElem(unitAssmt, "Units", last-1, 857);

    sdaiSaveModelBN(model, TEST_SAVE);//to check backlinks consistency

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
        ASSERT(sdaiIsMember(aggr, m));
    }

    ASSERT(!sdaiIsMember(aggr, notMember));
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
static void TestIsMemberVal (SdaiAggr aggr, SdaiPrimitiveType sdaiType, T* rMembers, int nMembers, T notMember)
{
    for (int i = 0; i < nMembers; i++) {
        ASSERT(sdaiIsMember(aggr, sdaiType, &rMembers[i]));
    }

    ASSERT(!sdaiIsMember(aggr, sdaiType, &notMember));
}

template <typename T>
static void TestIsMemberPtr(SdaiAggr aggr, SdaiPrimitiveType sdaiType, T* rMembers, int nMembers, T notMember)
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

    ASSERT(sdaiGetAggrElementBoundByIndex(aggr, 0) == elementBound);
}

//
static double rd[] = { 1,2.5,5 };
SdaiInteger lat[] = { 20,2,3,1 };
SdaiString rNames[] = { "Domingo", "Felipe", "Jacinto" };
SdaiString rBin[] = { "31", "23B", "092A" };

static void ExplicitAggregationsVariousTypes_Test4(SdaiModel model)
{
    ExpressID id = 1;

    SdaiInstance pt = internalGetInstanceFromP21Line(model, id++);

    SdaiAggr aggr = NULL;
    sdaiGetAttrBN(pt, "Coordinates", sdaiAGGR, &aggr);
    TestIsMemberVal(aggr, sdaiREAL, rd, 3, 8 + 1e-11);
    TestAggregationFunctions(aggr, 3, 12);

    auto site = internalGetInstanceFromP21Line(model, id++);
    aggr = 0;
    sdaiGetAttrBN(site, "RefLatitude", sdaiAGGR, &aggr);
    TestIsMemberVal(aggr, sdaiINTEGER, lat, 4, lat[0] + 1);
    TestAggregationFunctions(aggr, 4, -1);

    auto person = internalGetInstanceFromP21Line(model, id++);
    aggr = 0;
    sdaiGetAttrBN(person, "MiddleNames", sdaiAGGR, &aggr);
    TestIsMemberPtr(aggr, sdaiSTRING, rNames, 3, "Salvador");
    TestAggregationFunctions(aggr, 3, strlen(rNames[0]));

    auto texture = internalGetInstanceFromP21Line(model, id++);
    aggr = 0;
    sdaiGetAttrBN(texture, "Pixel", sdaiAGGR, &aggr);
    TestIsMemberPtr(aggr, sdaiBINARY, rBin, 3, "30");
    TestAggregationFunctions(aggr, 3, strlen(rBin[0]));
}

static void ExplicitAggregationsVariousTypes4()
{
    SdaiModel   model = sdaiCreateModelBN(0, "test IFC4", "IFC4");

    //
    auto pt = IFC4::IfcCartesianPoint::Create(model);
    pt.put_Coordinates(rd, 3);

    auto site = IFC4::IfcSite::Create(model);
    site.put_RefLatitude(lat, 4);

    auto person = IFC4::IfcPerson::Create(model);
    person.put_MiddleNames(rNames, 3);

    //
    auto texture = IFC4::IfcPixelTexture::Create(model);
    texture.put_Pixel(rBin, 3);

    ExplicitAggregationsVariousTypes_Test4(model);

    const char* fileName = "ExplicitAggregationsVariousTypes.ifc";
    sdaiSaveModelBN(model, fileName);
    sdaiCloseModel(model);
    model = 0;

    model = sdaiOpenModelBN(0, fileName, "");
    ASSERT(model);
    ExplicitAggregationsVariousTypes_Test4(model);
    sdaiCloseModel(model);
}

static void ExplicitAggregationsVariousTypes()
{
    //
    SdaiModel   model4x4 = sdaiCreateModelBN(0, "test IFC4x4", "IFC4x4");
    SdaiModel   model242 = sdaiCreateModelBN(0, "test AP242", "AP242");

    //
    SdaiBoolean voxels[] = { true };
    auto voxelGrid = IFC4x4::IfcVoxelGrid::Create(model4x4);
    voxelGrid.put_Voxels(voxels, 1);

    SdaiAggr aggr = 0;
    sdaiGetAttrBN(voxelGrid, "Voxels", sdaiAGGR, &aggr);
    TestIsMemberVal<SdaiBoolean>(aggr, sdaiBOOLEAN, voxels, 1, sdaiFALSE);
    TestAggregationFunctions(aggr, 1, -1);

    //
    IFC4x4::IfcLogical voxelDataValues[] = { IFC4x4::IfcLogical::False, IFC4x4::IfcLogical::Unknown };
    SdaiString voxelDataValues_s[] = { "F", "U" };
    auto voxelData = IFC4x4::IfcLogicalVoxelData::Create(model4x4);
    voxelData.put_ValueData(voxelDataValues, 2);

    aggr = 0;
    sdaiGetAttrBN(voxelData, "ValueData", sdaiAGGR, &aggr);
    TestIsMemberPtr(aggr, sdaiLOGICAL, voxelDataValues_s, 2, "T");
    TestAggregationFunctions(aggr, 2, -1);

    //
    AP242::a3m_element_type_name a3mtypes[] = { AP242::a3m_element_type_name::etna_shape_representation, AP242::a3m_element_type_name::etns_point_cloud_dataset };
    SdaiString a3mtypes_s[] = { "etna_shape_representation", "etns_point_cloud_dataset"};

    auto criterion = AP242::different_component_identification_via_multi_level_reference::Create(model242);
    criterion.put_compared_element_types(a3mtypes, 2);

    aggr = 0;
    sdaiGetAttrBN(criterion, "compared_element_types", sdaiAGGR, &aggr);
    TestIsMemberPtr(aggr, sdaiENUM, a3mtypes_s, 2, "eeee");
    TestAggregationFunctions(aggr, 2, -1);

    //
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

    //Segments is member of complex aggregation
    SdaiAggr allArgs = NULL;
    engiGetAttributeAggr(poly, (int_t*)&allArgs);
    ASSERT(sdaiIsMember(allArgs, sdaiAGGR, aggr)); 
    IsMemberADB(allArgs, notMember);

    //
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

static void CheckAdd(SdaiModel model, int64_t oid, bool xml)
{
    IFC4::IfcCartesianPointList2D points = internalGetInstanceFromP21Line(model, oid);

    //aggrgation of aggregation
    SdaiAggr coordList = NULL;
    auto res = sdaiGetAttrBN(points, "CoordList", sdaiAGGR, &coordList);
    ASSERT(res && sdaiGetMemberCount(coordList) == 6);

    for (int i = 0; i < 6; i++) {

        SdaiAggr pt = NULL;
        res = sdaiGetAggrByIndex(coordList, i, sdaiAGGR, &pt);
        ASSERT(res && sdaiGetMemberCount(pt) == 2);

        double dval = 0;
        res = sdaiGetAggrByIndex(pt, 0, sdaiREAL, &dval);
        ASSERT(fabs(dval - 10) < 1e-5);

        res = sdaiGetAggrByIndex(pt, 1, sdaiREAL, &dval);
        ASSERT(fabs(dval - (i+1)*10) < 1e-5);
    }

    if (xml) {
        return; //xml not to test
    }

    //shared aggregation
    for (int i = 1; i <= 4; i++) {
        auto person = internalGetInstanceFromP21Line(model, ++oid);
        SdaiAggr names = NULL;
        sdaiGetAttrBN(person, "MiddleNames", sdaiAGGR, &names);
        ASSERT(sdaiGetMemberCount(names) == 3);
        for (int j = 0; j < 3; j++) {
            SdaiString name = NULL;
            sdaiGetAggrByIndex(names, j, sdaiSTRING, &name);
            ASSERT(!strcmp(name, "Name"));
        }
    }

    //array of ADB of array
    for (int n = 0; n < 2; n++) {
        IFC4::IfcIndexedPolyCurve poly = internalGetInstanceFromP21Line(model, ++oid);
        SdaiAggr segments = NULL;
        res = sdaiGetAttrBN(poly, "Segments", sdaiAGGR, &segments);
        ASSERT(res && segments && sdaiGetMemberCount(segments) == 2);
        for (int i = 0; i < 2; i++) {
            SdaiADB adb = NULL;
            res = sdaiGetAggrByIndex(segments, i, sdaiADB, &adb);
            ASSERT(res && adb);
            auto type = sdaiGetADBTypePath(adb, 1);
            ASSERT(0 == strcmp(type, i ? "IFCARCINDEX" : "IFCLINEINDEX"));
            SdaiAggr inds = 0;
            res = sdaiGetADBValue(adb, sdaiAGGR, &inds);
            auto cnt = sdaiGetMemberCount(inds);
            ASSERT(res &&  cnt== 6);
            for (int j = 0; j < cnt; j++) {
                SdaiInteger v = 0;
                res = sdaiGetAggrByIndex(inds, j, sdaiINTEGER, &v);
                ASSERT(res && v == j);
            }
        }
    }
}

static void Add()
{
    SdaiModel   model = sdaiCreateModelBN("IFC4");

    // array of array
    auto points = IFC4::IfcCartesianPointList2D::Create(model);
    int64_t pointsId = internalGetP21Line(points);

    SdaiAggr coordList = sdaiCreateAggrBN(points, "CoordList");

    SdaiAggr pt1 = sdaiCreateNestedAggr(coordList);

    double dval = 10.0;
    SdaiADB adbval = sdaiCreateADB(sdaiREAL, &dval);

    sdaiAdd_(pt1, sdaiREAL, dval);
    sdaiAdd_(pt1, sdaiADB, adbval);

    for (int i = 0; i < 5; i++) {
        SdaiAggr pt2 = sdaiCreateNestedAggr(coordList);
        sdaiAdd_(pt2, sdaiADB, adbval);
        dval = (i + 2) * 10;
        sdaiAdd_(pt2, sdaiREAL, dval);
    }

    // shared aggregation
    auto person1 = IFC4::IfcPerson::Create(model);
    auto person2 = IFC4::IfcPerson::Create(model);
    auto person3 = IFC4::IfcPerson::Create(model);
    auto person4 = IFC4::IfcPerson::Create(model);
    auto aggrNames = sdaiCreateAggrBN(person1, "MiddleNames");
    sdaiPutAttrBN(person2, "MiddleNames", sdaiAGGR, aggrNames);
    sdaiPutAttrBN(person3, "MiddleNames", sdaiAGGR, aggrNames);

    sdaiAdd_(aggrNames, sdaiSTRING, "Name");
    
    SdaiAggr aggrNames2 = NULL;
    sdaiGetAttrBN(person2, "MiddleNames", sdaiAGGR, &aggrNames2);
    sdaiPutAttrBN(person2, "MiddleNames", sdaiAGGR, aggrNames2);//self-put
    sdaiPutAttrBN(person4, "MiddleNames", sdaiAGGR, aggrNames2);

    sdaiAdd_(aggrNames, sdaiSTRING, "Name");
    sdaiAdd_(aggrNames2, sdaiSTRING, "Name");

    //array of ADB of array
    auto poly = IFC4::IfcIndexedPolyCurve::Create(model);
    poly.put_Points(points);
    SdaiAggr segments = sdaiCreateAggrBN(poly, "Segments");

    SdaiAggr indecies = sdaiCreateAggr(poly, NULL);
    sdaiAdd_(indecies, sdaiINTEGER, 2);

    SdaiADB adb = sdaiCreateADB(sdaiAGGR, indecies);

    sdaiPutADBTypePath(adb, 1, "IFCLINEINDEX");
    sdaiAdd_(segments, sdaiADB, adb);

    sdaiPutADBTypePath(adb, 1, "IFCARCINDEX");
    sdaiAdd_(segments, sdaiADB, adb);

    sdaiAdd_(indecies, sdaiINTEGER, 4);

    SdaiInteger ival = 0;
    sdaiInsertByIndex(indecies, 0, sdaiINTEGER, &ival);
    ival = 1;
    sdaiInsertByIndex(indecies, 1, sdaiINTEGER, &ival);
    ival = 3;
    sdaiInsertByIndex(indecies, 3, sdaiINTEGER, &ival);
    ival = 5;
    sdaiInsertByIndex(indecies, 5, sdaiINTEGER, &ival);

    sdaiDeleteADB(adb);

    //by SDAI standard
    poly = IFC4::IfcIndexedPolyCurve::Create(model);
    poly.put_Points(points);
    segments = sdaiCreateAggrBN(poly, "Segments");

    adb = sdaiCreateEmptyADB();
    sdaiPutADBTypePath(adb, 1, "IFCLINEINDEX");

    indecies = sdaiCreateNestedAggrADB(segments, adb);
    sdaiAdd_(indecies, sdaiINTEGER, 2);
    sdaiAdd_(indecies, sdaiINTEGER, 4);
    
    SdaiIterator iter = sdaiCreateIterator(indecies);
    sdaiNext(iter);
    
    ival = 0;
    sdaiInsertBefore(iter, sdaiINTEGER, &ival);

    ival = 1;
    sdaiInsertBefore(iter, sdaiINTEGER, &ival);
    
    ival = 3;
    sdaiInsertAfter(iter, sdaiINTEGER, &ival);

    sdaiNext(iter);
    sdaiNext(iter);

    ival = 5;
    sdaiInsertAfter(iter, sdaiINTEGER, &ival);
    
    sdaiDeleteIterator(iter);

    sdaiPutADBTypePath(adb, 1, "IFCARCINDEX");

    indecies = sdaiCreateNestedAggrADB(segments, adb);
    for (int i = 0; i < 6; i++) {
        sdaiAdd_(indecies, sdaiINTEGER, 9999);
    }
    iter = sdaiCreateIterator(indecies);
    for (SdaiInteger i = 0; i < 2; i++) {
        sdaiNext(iter);
        sdaiPutAggrByIterator(iter, sdaiINTEGER, &i);
        for (SdaiInteger j = 0; j < 6; j++) {
            sdaiPutAggrByIndex(indecies, j, sdaiINTEGER, &j);
        }
    }
    sdaiDeleteIterator(iter);

    sdaiDeleteADB(adb);

    //
    //
    CheckAdd(model, pointsId, false);

    const char* testFile = "TestAggregationAdd.ifc";
    sdaiSaveModelBN(model, testFile);

    const char* testFileXML = "TestAggregationAdd.xml";
    sdaiSaveModelAsSimpleXmlBN(model, testFileXML);

    sdaiCloseModel(model);

    model = sdaiOpenModelBN(0, testFile, "");
    CheckAdd(model, pointsId, false);
    sdaiCloseModel(model);

    model = sdaiOpenModelBN(0, testFileXML, "");
    CheckAdd(model, 1, true);
    sdaiCloseModel(model);
}

static void CheckPointList(IFC4::IfcCartesianPointList2D ptlst)
{
    SdaiAggr coordList = NULL;
    auto res = sdaiGetAttrBN(ptlst, "CoordList", sdaiAGGR, &coordList);
    ASSERT(res && coordList);

    auto cnt = sdaiGetMemberCount(coordList);
    ASSERT(cnt == 5);

    for (int i = 0; i < cnt; i++) {
        SdaiAggr coords = NULL;
        res = sdaiGetAggrByIndex(coordList, i, sdaiAGGR, &coords);
        ASSERT(res && coords);

        auto dim = sdaiGetMemberCount(coords);
        ASSERT(dim == 2);

        for (int j = 0; j < dim; j++) {
            double v = 0;
            res = sdaiGetAggrByIndex(coords, j, sdaiREAL, &v);
            ASSERT(res && fabs(v - i) < 1e-7);
        }
    }
}

static SdaiInstance TestCreateNestedArgByIndex_0(SdaiModel model)
{
    auto ptlst = IFC4::IfcCartesianPointList2D::Create(model);

    auto coordList = sdaiCreateAggrBN(ptlst, "CoordList");

    //works like add for new index
    for (int i = 0; i < 5; i++) {
        auto coords = sdaiCreateNestedAggrByIndex(coordList, i);
        sdaiAdd_(coords, sdaiREAL, double(i));
        sdaiAdd_(coords, sdaiREAL, double(i));
    }

    return ptlst;
}

static SdaiInstance TestCreateNestedArgByIndex(SdaiModel model)
{
    auto ptlst = IFC4::IfcCartesianPointList2D::Create(model);

    auto coordList = sdaiCreateAggrBN(ptlst, "CoordList");

    //works like add for new index
    for (int i = 0; i < 5; i++) {
        auto coords = sdaiCreateNestedAggrByIndex(coordList, i);
        sdaiAdd_(coords, sdaiREAL, 2.0);
        sdaiAdd_(coords, sdaiREAL, 2.0);
    }

    //works as replace for existing instances
    for (int i = 0; i < 5; i++) {
        if (i != 2) {
            auto coords = sdaiCreateNestedAggrByIndex(coordList, i);
            sdaiAdd_(coords, sdaiREAL, double(i));
            sdaiAdd_(coords, sdaiREAL, double(i));
        }
    }

    return ptlst;
}

static SdaiInstance TestCreateNestedArgByItr(SdaiModel model)
{
    auto ptlst = IFC4::IfcCartesianPointList2D::Create(model);

    auto coordList = sdaiCreateAggrBN(ptlst, "CoordList");

    //works like add for new index
    for (int i = 0; i < 5; i++) {
        auto coords = sdaiCreateNestedAggrByIndex(coordList, i);
        sdaiAdd_(coords, sdaiREAL, 2.0);
        sdaiAdd_(coords, sdaiREAL, 2.0);
    }

    //works as replace for existing instances
    SdaiIterator iter = sdaiCreateIterator(coordList);
    for (int i = 0; sdaiNext(iter); i++) {
        if (i != 2) {
            auto coords = sdaiCreateNestedAggrByItr(iter);
            sdaiAdd_(coords, sdaiREAL, double(i));
            sdaiAdd_(coords, sdaiREAL, double(i));
        }
    }
    sdaiDeleteIterator(iter);

    return ptlst;
}

static SdaiInstance TestInsertBeforeAfter(SdaiModel model)
{
    auto ptlst = IFC4::IfcCartesianPointList2D::Create(model);

    auto coordList = sdaiCreateAggrBN(ptlst, "CoordList");

    auto coords = sdaiCreateNestedAggr(coordList);
    sdaiAdd_(coords, sdaiREAL, 2.0);
    sdaiAdd_(coords, sdaiREAL, 2.0);

    //works as replace for existing instances
    SdaiIterator iter = sdaiCreateIterator(coordList);
    sdaiNext(iter);

    for (int i = 0; i < 2; i++) {
        coords = sdaiInsertNestedAggrBefore(iter);
        sdaiAdd_(coords, sdaiREAL, double(i));
        sdaiAdd_(coords, sdaiREAL, double(i));

        coords = sdaiInsertNestedAggrAfter(iter);
        sdaiAdd_(coords, sdaiREAL, 4.0 - i);
        sdaiAdd_(coords, sdaiREAL, 4.0 - i);
    }

    sdaiDeleteIterator(iter);

    return ptlst;

}

static SdaiInstance TestInsertByIndex(SdaiModel model)
{
    auto ptlst = IFC4::IfcCartesianPointList2D::Create(model);

    auto coordList = sdaiCreateAggrBN(ptlst, "CoordList");

    auto coords = sdaiCreateNestedAggr(coordList);
    sdaiAdd_(coords, sdaiREAL, 3.0);
    sdaiAdd_(coords, sdaiREAL, 3.0);

    //works as replace for existing instances
    SdaiIterator iter = sdaiCreateIterator(coordList);
    sdaiNext(iter);

    for (int i = 0; i < 2; i++) {
        coords = sdaiInsertNestedAggrByIndex(coordList, 0);
        sdaiAdd_(coords, sdaiREAL, 1. - i);
        sdaiAdd_(coords, sdaiREAL, 1. - i);
    }

    for (int i = 2; i < 5; i += 2) {
        coords = sdaiInsertNestedAggrByIndex(coordList, i);
        sdaiAdd_(coords, sdaiREAL, double(i));
        sdaiAdd_(coords, sdaiREAL, double(i));
    }

    sdaiDeleteIterator(iter);

    return ptlst;

}

static void AddIndecies(SdaiAggr inds, SdaiInteger v)
{
    for (int i = 0; i < 5; i++) {
        sdaiAdd_(inds, sdaiINTEGER, v);
    }
}

static void CheckPolyCurve(IFC4::IfcIndexedPolyCurve poly)
{
    SdaiAggr segs = NULL;
    auto res = sdaiGetAttrBN(poly, "Segments", sdaiAGGR, &segs);
    ASSERT(res && segs);

    auto NSeg = sdaiGetMemberCount(segs);
    ASSERT(NSeg == 5);

    for (int i = 0; i < NSeg; i++)         {
        SdaiADB adb = sdaiCreateEmptyADB();
        res = sdaiGetAggrByIndex(segs, i, sdaiADB, &adb);
        ASSERT(res && adb);

        auto type = sdaiGetADBTypePath(adb, 1);
        ASSERT(0 == strcmp(type, (i % 2) ? "IFCARCINDEX" : "IFCLINEINDEX"));

        SdaiAggr inds = NULL;
        res = sdaiGetADBValue(adb, sdaiAGGR, &inds);
        ASSERT(res && inds);

        auto Ni = sdaiGetMemberCount(inds);
        ASSERT(Ni == 5);
        for (int j = 0; j < Ni; j++) {
            SdaiInteger v = 0;
            res = sdaiGetAggrByIndex(inds, j, sdaiINTEGER, &v);
            ASSERT(res && v == i);
        }

        sdaiDeleteADB(adb);
    }
}

static SdaiInstance TestCreateNestedArgByIndexADB_0(SdaiModel model)
{
    auto poly = IFC4::IfcIndexedPolyCurve::Create(model);

    auto segments = sdaiCreateAggrBN(poly, "Segments");

    //works like add for new index
    for (int i = 0; i < 5; i++) {
        SdaiADB adb = sdaiCreateEmptyADB();
        sdaiPutADBTypePath(adb, 1, (i % 2) ? "IFCARCINDEX" : "IFCLINEINDEX");
        
        auto inds = sdaiCreateNestedAggrByIndexADB(segments, i, adb);
        
        if (i % 2) {
            AddIndecies(inds, i);
        }
        else {
            SdaiAggr aggr = 0;
            auto res = sdaiGetADBValue(adb, sdaiAGGR, &aggr);
            ASSERT(res && aggr);
            AddIndecies(aggr, i);
        }

        sdaiDeleteADB(adb);
    }

    return poly;
}


static SdaiInstance TestCreateNestedArgByIndexADB(SdaiModel model)
{
    auto poly = IFC4::IfcIndexedPolyCurve::Create(model);

    auto segments = sdaiCreateAggrBN(poly, "Segments");

    //works like add for new index
    for (int i = 0; i < 5; i++) {
        SdaiADB adb = sdaiCreateEmptyADB();
        sdaiPutADBTypePath(adb, 1, (2 % 2) ? "IFCARCINDEX" : "IFCLINEINDEX");

        auto inds = sdaiCreateNestedAggrByIndexADB(segments, i, adb);

        if (i % 2) {
            AddIndecies(inds, 2);
        }
        else {
            SdaiAggr aggr = 0;
            auto res = sdaiGetADBValue(adb, sdaiAGGR, &aggr);
            ASSERT(res && aggr);
            AddIndecies(aggr, 2);
        }

        sdaiDeleteADB(adb);
    }

    //
    for (int i = 0; i < 5; i++) {
        if (i == 2) {
            continue;
        }

        SdaiADB adb = sdaiCreateEmptyADB();
        sdaiPutADBTypePath(adb, 1, (i % 2) ? "IFCARCINDEX" : "IFCLINEINDEX");

        auto inds = sdaiCreateNestedAggrByIndexADB(segments, i, adb);

        if (i % 2) {
            AddIndecies(inds, i);
        }
        else {
            SdaiAggr aggr = 0;
            auto res = sdaiGetADBValue(adb, sdaiAGGR, &aggr);
            ASSERT(res && aggr);
            AddIndecies(aggr, i);
        }

        sdaiDeleteADB(adb);
    }

    return poly;
}

static SdaiInstance TestCreateNestedArgByItrADB(SdaiModel model)
{
    auto poly = IFC4::IfcIndexedPolyCurve::Create(model);

    auto segments = sdaiCreateAggrBN(poly, "Segments");

    //works like add for new index
    for (int i = 0; i < 5; i++) {
        SdaiADB adb = sdaiCreateEmptyADB();
        sdaiPutADBTypePath(adb, 1, (2 % 2) ? "IFCARCINDEX" : "IFCLINEINDEX");

        auto inds = sdaiCreateNestedAggrByIndexADB(segments, i, adb);

        if (i % 2) {
            AddIndecies(inds, 2);
        }
        else {
            SdaiAggr aggr = 0;
            auto res = sdaiGetADBValue(adb, sdaiAGGR, &aggr);
            ASSERT(res && aggr);
            AddIndecies(aggr, 2);
        }

        sdaiDeleteADB(adb);
    }


    //
    SdaiIterator iter = sdaiCreateIterator(segments);
    for (int i = 0; i < 5; i++) {
        ASSERT(sdaiNext(iter));

        if (i == 2) {
            continue;
        }

        SdaiADB adb = sdaiCreateEmptyADB();
        sdaiPutADBTypePath(adb, 1, (i % 2) ? "IFCARCINDEX" : "IFCLINEINDEX");

        auto inds = sdaiCreateNestedAggrByItrADB(iter, adb);

        if (i % 2) {
            AddIndecies(inds, i);
        }
        else {
            SdaiAggr aggr = 0;
            auto res = sdaiGetADBValue(adb, sdaiAGGR, &aggr);
            ASSERT(res && aggr);
            AddIndecies(aggr, i);
        }

        sdaiDeleteADB(adb);
    }
    ASSERT(!sdaiNext(iter));
    sdaiDeleteIterator(iter);

    return poly;
}

static SdaiInstance TestInsertBeforeAfterADB(SdaiModel model)
{
    auto poly = IFC4::IfcIndexedPolyCurve::Create(model);

    auto segments = sdaiCreateAggrBN(poly, "Segments");

    SdaiADB adb = sdaiCreateEmptyADB();
    sdaiPutADBTypePath(adb, 1, "IFCLINEINDEX");

    auto inds = sdaiCreateNestedAggrADB(segments, adb);
    AddIndecies(inds, 2);
        
    sdaiDeleteADB(adb);
    
    //
    SdaiIterator iter = sdaiCreateIterator(segments);
    ASSERT(sdaiNext(iter));
    for (int i = 0; i < 2; i++) {

        SdaiADB adb = sdaiCreateEmptyADB();
        sdaiPutADBTypePath(adb, 1, (i % 2) ? "IFCARCINDEX" : "IFCLINEINDEX");

        auto inds = sdaiInsertNestedAggrBeforeADB(iter, adb);

        if (i % 2) {
            AddIndecies(inds, i);
        }
        else {
            SdaiAggr aggr = 0;
            auto res = sdaiGetADBValue(adb, sdaiAGGR, &aggr);
            ASSERT(res && aggr);
            AddIndecies(aggr, i);
        }

        sdaiDeleteADB(adb);

        //
        auto j = 4 - i;
        adb = sdaiCreateEmptyADB();
        sdaiPutADBTypePath(adb, 1, (j % 2) ? "IFCARCINDEX" : "IFCLINEINDEX");

        inds = sdaiInsertNestedAggrAfterADB(iter, adb);

        if (j % 2) {
            AddIndecies(inds, j);
        }
        else {
            SdaiAggr aggr = 0;
            auto res = sdaiGetADBValue(adb, sdaiAGGR, &aggr);
            ASSERT(res && aggr);
            AddIndecies(aggr, j);
        }

        sdaiDeleteADB(adb);

    }
    sdaiDeleteIterator(iter);

    return poly;
}


static SdaiInstance TestInsertByIndexADB(SdaiModel model)
{
    auto poly = IFC4::IfcIndexedPolyCurve::Create(model);

    auto segments = sdaiCreateAggrBN(poly, "Segments");

    SdaiADB adb = sdaiCreateEmptyADB();
    sdaiPutADBTypePath(adb, 1, "IFCARCINDEX");

    auto inds = sdaiCreateNestedAggrADB(segments, adb);
    AddIndecies(inds, 3);

    sdaiDeleteADB(adb);

    //
    for (int i = 0; i < 2; i++) {

        auto j = 1 - i;
        SdaiADB adb = sdaiCreateEmptyADB();
        sdaiPutADBTypePath(adb, 1, (j % 2) ? "IFCARCINDEX" : "IFCLINEINDEX");
        auto inds = sdaiInsertNestedAggrByIndexADB(segments, 0, adb);
        AddIndecies(inds, j);
        sdaiDeleteADB(adb);
    }

    for (int j = 2; j < 5; j += 2) {
        adb = sdaiCreateEmptyADB();
        sdaiPutADBTypePath(adb, 1, (j % 2) ? "IFCARCINDEX" : "IFCLINEINDEX");
        inds = sdaiInsertNestedAggrByIndexADB(segments, j, adb);
        AddIndecies(inds, j);
        sdaiDeleteADB(adb);
    }
    return poly;
}


static void CreateNested()
{
    SdaiModel   model = sdaiCreateModelBN("IFC4");

    auto inst = TestCreateNestedArgByIndex_0(model);
    CheckPointList(inst);

    inst = TestCreateNestedArgByIndex(model);
    CheckPointList(inst);

    inst = TestCreateNestedArgByItr(model);
    CheckPointList(inst);

    inst = TestInsertBeforeAfter(model);
    CheckPointList(inst);

    inst = TestInsertByIndex(model);
    CheckPointList(inst);

    inst = TestCreateNestedArgByIndexADB_0(model);
    CheckPolyCurve(inst);

    inst = TestCreateNestedArgByIndexADB(model);
    CheckPolyCurve(inst);

    inst = TestCreateNestedArgByItrADB(model);
    CheckPolyCurve(inst);

    inst = TestInsertBeforeAfterADB(model);
    CheckPolyCurve(inst);

    inst = TestInsertByIndexADB(model);
    CheckPolyCurve(inst);

    sdaiCloseModel(model);
}

/// <summary>
/// insert/delete with backlinks check
/// </summary>

#define NPS 4

struct Property
{
    SdaiInstance inst;
    SdaiBoolean in[NPS];

    Property()
    {
        inst = NULL;
        for (int i = 0; i < NPS; i++) {
            in[i] = sdaiFALSE;
        }
    }
};

typedef std::list<Property> PropList;

static void CheckProperties(SdaiInstance allPsets[NPS], PropList& props)
{
    //direct links
    for (int i = 0; i < NPS; i++) {
        SdaiAggr hasProps = 0;
        auto res = sdaiGetAttrBN(allPsets[i], "HasProperties", sdaiAGGR, &hasProps);
        ASSERT(res && hasProps);

        int n = 0;
        for (auto& prop : props) {
            ASSERT(sdaiIsMember(hasProps, prop.inst) == prop.in[i]);
            if (prop.in[i]) {
                n++;
            }
        }

        ASSERT(n == sdaiGetMemberCount(hasProps));
    }
        
    //back links
    for (auto& prop : props) {
        SdaiAggr partOfPsets = NULL;
        auto ret = sdaiGetAttrBN(prop.inst, "PartOfPset", sdaiAGGR, &partOfPsets);
        ASSERT(ret && partOfPsets);

        int n = 0;
        for (int i = 0; i < NPS; i++) {
            ASSERT(sdaiIsMember(partOfPsets, allPsets[i])== prop.in[i]);
            if (prop.in[i]) {
                n++;
            }
        }

        ASSERT(n==0 || sdaiTRUE == sdaiTestArrayByIndex(partOfPsets, 0));

        ASSERT(sdaiGetMemberCount(partOfPsets) == n);
    }
}


static void AddByIndex(SdaiModel model, SdaiAggr aggr[NPS], PropList& props)
{
    for (int i = 0; i < 3; i++) {
        props.push_back(Property());
        auto& prop = props.back();
        prop.inst = IFC4::IfcPropertySingleValue::Create(model);

        for (int j = 0; j < NPS; j++) {
            sdaiPutAggrByIndex(aggr[j], i, prop.inst);
            prop.in[j] = sdaiTRUE;
        }
    }
}

static void AddByIterator(SdaiModel model, SdaiAggr aggr[NPS], PropList& props)
{
    for (int i = 0; i < 2; i++) {
        props.push_front(Property());
        auto& fr1 = props.front();
        fr1.inst = IFC4::IfcPropertySingleValue::Create(model);

        props.push_front(Property());
        auto& fr2 = props.front();
        fr2.inst = IFC4::IfcPropertySingleValue::Create(model);

        props.push_back(Property());
        auto& bk1 = props.back();
        bk1.inst = IFC4::IfcPropertySingleValue::Create(model);

        props.push_back(Property());
        auto& bk2 = props.back();
        bk2.inst = IFC4::IfcPropertySingleValue::Create(model);

        for (int j = 0; j < NPS; j++) {
            auto it = sdaiCreateIterator(aggr[j]);
            sdaiNext(it);
            sdaiInsertBefore(it, fr1.inst);
            fr1.in[j] = sdaiTRUE;
            sdaiDeleteIterator(it);

            sdaiInsertByIndex(aggr[j], 0, fr2.inst);
            fr2.in[j] = sdaiTRUE;

            it = sdaiCreateIterator(aggr[j]);
            sdaiEnd(it);
            sdaiPrevious(it);
            sdaiInsertAfter(it, bk1.inst);
            bk1.in[j] = sdaiTRUE;
            sdaiDeleteIterator(it);

            sdaiInsertByIndex(aggr[j], -1, bk2.inst);
            bk2.in[j] = sdaiTRUE;
        }
    }

}

static void RemoveProperties(SdaiAggr aggr[NPS], PropList& props)
{
    auto pit = props.begin();

    auto ait = sdaiCreateIterator(aggr[0]);
    sdaiBeginning(ait);
    sdaiNext(ait);
    sdaiRemoveByIterator(ait);
    pit->in[0] = sdaiFALSE;
    sdaiDeleteIterator(ait);

    pit++;

    sdaiRemoveByIndex(aggr[1], 1);
    pit->in[1] = sdaiFALSE;

    pit++;

    sdaiRemove(aggr[2], pit->inst);
    pit->in[2] = sdaiFALSE;
}

static void ReplaceProperties(SdaiModel model, SdaiAggr aggr[NPS], PropList& props)
{
    props.back().in[0] = sdaiFALSE;
    
    props.push_back(Property());
    auto& prop = props.back();
    prop.inst = IFC4::IfcPropertySingleValue::Create(model);

    SdaiIterator it = sdaiCreateIterator(aggr[0]);
    sdaiEnd(it);
    sdaiPrevious(it);
    sdaiPutAggrByIterator(it, prop.inst);
    prop.in[0] = sdaiTRUE;
    sdaiDeleteIterator(it);
}


static void InsertDeleteCheckBacklinks()
{
    SdaiModel   model = sdaiCreateModelBN("IFC4");

    SdaiInstance rpset[NPS];
    SdaiAggr raggr[NPS];
    for (int i = 0; i < NPS; i++) {
        rpset[i] = IFC4::IfcPropertySet::Create(model);
        raggr[i] = sdaiCreateAggrBN(rpset[i], "HasProperties");
    }

    PropList props;

    AddByIndex(model, raggr, props);
    CheckProperties(rpset, props);

    RemoveProperties(raggr, props);
    CheckProperties(rpset, props);

    AddByIterator(model, raggr, props);
    CheckProperties(rpset, props);

    ReplaceProperties(model, raggr, props);
    CheckProperties(rpset, props);

    sdaiCloseModel(model);
}

static void CheckNullsAndResize(SdaiArray  aggr, SdaiInteger size)
{
    ASSERT(size == sdaiGetMemberCount(aggr));

    for (SdaiInteger i = 0; i < size + 3; i++) { //want to check outside
        SdaiBoolean test = (i == 0 && size > 0 || i == 2 && size > 2) ? sdaiTRUE : sdaiFALSE;
        ASSERT(test == sdaiTestArrayByIndex(aggr, i));
    }

    if (size > 0) {
        auto iter = sdaiCreateIterator(aggr);
        sdaiBeginning(iter);

        ASSERT(sdaiNext(iter));
        ASSERT(sdaiTRUE == sdaiTestArrayByItr(iter));

        ASSERT(sdaiNext(iter));
        ASSERT(sdaiFALSE == sdaiTestArrayByItr(iter));

        sdaiDeleteIterator(iter);
    }
}

static void TestNullsAndResize()
{
    SdaiModel   model = sdaiCreateModelBN("IFC4");

    auto inst = IFC4::IfcMaterialProfileWithOffsets::Create(model);
    auto aggr = sdaiCreateAggrBN(inst, "OffsetValues");

    sdaiAdd_(aggr, sdaiREAL, 1.0);
    sdaiAdd_(aggr, 0, NULL);
    sdaiAdd_(aggr, sdaiREAL, 1.0);
    sdaiAdd_(aggr, sdaiREAL, 1.0);
    sdaiAdd_(aggr, sdaiREAL, 1.0);

    auto it = sdaiCreateIterator(aggr);
    sdaiEnd(it);
    sdaiPrevious(it);
    sdaiUnsetArrayByItr(it);
    sdaiDeleteIterator(it);

    sdaiUnsetArrayByIndex(aggr, 3);

    CheckNullsAndResize(aggr, 5);
    
    //reopen
    sdaiSaveModelBN(model, "NullInAggr.ifc");
    sdaiCloseModel(model);
    model = sdaiOpenModelBN(0, "NullInAggr.ifc", "");

    inst = internalGetInstanceFromP21Line(model, 1);
    aggr = NULL;
    auto ret = sdaiGetAttrBN(inst, "OffsetValues", sdaiAGGR, &aggr);
    ASSERT(ret && aggr);

    CheckNullsAndResize(aggr, 5);

    //expand
    sdaiResetArrayIndex(aggr, 0, 10);
    CheckNullsAndResize(aggr, 10);

    //truncate
    sdaiResetArrayIndex(aggr, 0, 8);
    CheckNullsAndResize(aggr, 8);

    sdaiReindexArray(aggr);
    CheckNullsAndResize(aggr, 2);

    sdaiResetArrayIndex(aggr, 0, 0);
    CheckNullsAndResize(aggr, 0);

    sdaiCloseModel(model);
}

static void UpdateCounters()
{
    SdaiModel   model = sdaiCreateModelBN("IFC4");

    auto aggrAll = xxxxGetAllInstances(model);
    auto itAll = sdaiCreateIterator(aggrAll);
    auto itAllBk = sdaiCreateIterator(aggrAll);

    auto aggrSVProps = sdaiGetEntityExtentBN(model, "IfcPropertySingleValue");
    auto itSVProps = sdaiCreateIterator(aggrSVProps);

    auto aggrPropsExtent = xxxxGetEntityAndSubTypesExtentBN(model, "IfcProperty");
    auto itPropsExtent = sdaiCreateIterator(aggrPropsExtent);

    //
    sdaiBeginning(itAll);
    ASSERT(!sdaiNext(itAll));
    sdaiEnd(itAllBk);
    ASSERT(!sdaiPrevious(itAllBk));
    sdaiBeginning(itSVProps);
    ASSERT(!sdaiNext(itSVProps));
    sdaiBeginning(itPropsExtent);
    ASSERT(!sdaiNext(itPropsExtent));

    //
    //
    auto pset = IFC4::IfcPropertySet::Create(model);
    auto prop = IFC4::IfcPropertySingleValue::Create(model);

    //
    sdaiBeginning(itAll);
    ASSERT(sdaiNext(itAll));
    sdaiEnd(itAllBk);
    ASSERT(sdaiPrevious(itAllBk));
    sdaiBeginning(itSVProps);
    ASSERT(sdaiNext(itSVProps));
    sdaiBeginning(itPropsExtent);
    ASSERT(sdaiNext(itPropsExtent));

    //
    //
    SdaiAggr aggrProps = sdaiCreateAggrBN (pset, "HasProperties");
    SdaiIterator itProps = sdaiCreateIterator(aggrProps);

    SdaiAggr aggrPsets;
    sdaiGetAttrBN(prop, "PartOfPset", sdaiAGGR, &aggrPsets);
    SdaiIterator itPsets = sdaiCreateIterator(aggrPsets);

    auto prop2 = IFC4::IfcPropertySingleValue::Create(model);
    sdaiAdd_(aggrProps, sdaiINSTANCE, (SdaiInstance)prop2);

    sdaiSaveModelBN(model, TEST_SAVE);//to check backlinks consistency
    
    //
    sdaiBeginning(itProps);
    ASSERT(sdaiNext(itProps));
    ASSERT(!sdaiNext(itProps));
    sdaiBeginning(itPsets);
    ASSERT(!sdaiNext(itPsets));

    //
    sdaiAdd_(aggrProps, sdaiINSTANCE, (SdaiInstance)prop);

    //
    sdaiBeginning(itProps);
    ASSERT(sdaiNext(itProps));
    ASSERT(sdaiNext(itProps));
    sdaiBeginning(itPsets);
    ASSERT(sdaiNext(itPsets));

    sdaiSaveModelBN(model, TEST_SAVE);//to check backlinks consistency

    //
    sdaiRemoveByIndex(aggrProps, 0);
    sdaiRemoveByIndex(aggrProps, 0);

    sdaiSaveModelBN(model, TEST_SAVE);//to check backlinks consistency

    //
    sdaiBeginning(itProps);
    ASSERT(!sdaiNext(itProps));
    sdaiBeginning(itPsets);
    ASSERT(!sdaiNext(itPsets));

    //
    sdaiDeleteInstance(prop);
    sdaiDeleteInstance(prop2);
    sdaiDeleteInstance(pset);

    sdaiSaveModelBN(model, TEST_SAVE);//to check backlinks consistency

    //
    sdaiBeginning(itAll);
    ASSERT(!sdaiNext(itAll));
    sdaiEnd(itAllBk);
    ASSERT(!sdaiPrevious(itAllBk));
    sdaiBeginning(itSVProps);
    ASSERT(!sdaiNext(itSVProps));
    sdaiBeginning(itPropsExtent);
    ASSERT(!sdaiNext(itPropsExtent));

    //
    sdaiCloseModel(model);
}

extern void AggregationTests()
{
    ENTER_TEST;

    UpdateCounters();
    Add();
    CreateNested();
    Iterators();
    ExplicitAggregationsVariousTypes4();
    ExplicitAggregationsVariousTypes();
    IsMemberComplex();
    Delete();
    InsertDeleteCheckBacklinks();
    TestNullsAndResize();
}


