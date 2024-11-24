#include "pch.h"

#define TEST_FILE "DeriveAttr.ifc"

static void Ifc4test_Check(SdaiModel model, ExpressID extID, bool getOn, bool all1, bool all2)
{
    IFC4::IfcSIUnit lengthUnit = internalGetInstanceFromP21Line(model, 391);
    ASSERT(lengthUnit);

    IFC4::IfcSIUnit areaUnit = internalGetInstanceFromP21Line(model, 392);
    ASSERT(areaUnit);

    IFC4::IfcGeometricRepresentationSubContext sctx = internalGetInstanceFromP21Line(model, 33);
    ASSERT(sctx);

    IFC4::IfcAxis2Placement3D a2p3d = internalForceInstanceFromP21Line(model, 91);
    ASSERT(a2p3d);

    IFC4::IfcCartesianPoint pt = internalGetInstanceFromP21Line(model, 100);
    ASSERT(pt);

    IFC4::IfcCartesianTransformationOperator3DnonUniform oper = internalForceInstanceFromP21Line(model, extID);
    ASSERT(oper);

    IFC4::IfcCompositeCurve composite = internalGetInstanceFromP21Line(model, extID + 2);
    ASSERT(composite);

    if (getOn || all1 || all2) {
        auto exponents = lengthUnit.get_Dimensions();
        ASSERT(exponents);

        auto exp = exponents.get_LengthExponent();
        ASSERT(!exp.IsNull() && exp.Value() == 1);

        exp = exponents.get_TimeExponent();
        ASSERT(!exp.IsNull() && exp.Value() == 0);

        auto dim = sctx.get_CoordinateSpaceDimension();
        ASSERT(!dim.IsNull() && dim.Value() == 3);
    }
    else {
        auto exponents = lengthUnit.get_Dimensions();
        ASSERT(!exponents);

        auto dim = sctx.get_CoordinateSpaceDimension();
        ASSERT(dim.IsNull());
    }

    if (all2) {
        auto exponents = areaUnit.get_Dimensions();
        ASSERT(exponents);

        auto exp = exponents.get_LengthExponent();
        ASSERT(!exp.IsNull() && exp.Value() == 2);

        exp = exponents.get_TimeExponent();
        ASSERT(!exp.IsNull() && exp.Value() == 0);
    }
    else {
        auto exponents = areaUnit.get_Dimensions();
        ASSERT(!exponents);
    }

    if (getOn) {
        SdaiInteger dm = 0;
        auto res = sdaiGetAttrBN(pt, "Dim", sdaiINTEGER, &dm);
        ASSERT(res && dm == 3);

        SdaiAggr P = NULL;
        res = sdaiGetAttrBN(a2p3d, "P", sdaiAGGR, &P);
        ASSERT(res && P);

        auto cnt = sdaiGetMemberCount(P);
        ASSERT(cnt == 3);
        for (int_t i = 0; i < 3; i++) {
            SdaiInstance dir = NULL;
            res = sdaiGetAggrByIndex(P, i, sdaiINSTANCE, &dir);
            ASSERT(res && dir);
            IFC4::IfcDirection dir_(dir);
            ASSERT(dir_);
            std::vector<double> comps;
            dir_.get_DirectionRatios(comps);
            ASSERT(comps.size() == 3);
            for (int j = 0; j < 3; j++) {
                ASSERT(fabs(comps[j] - (i == j) ? 1 : 0) < 1e-8);
            }
        }

        SdaiReal scl = 0;
        res = sdaiGetAttrBN(oper, "Scl2", sdaiREAL, &scl);
        ASSERT(res && fabs(scl - 1.5) < 1e-11);

        const char* closed = nullptr;
        res = sdaiGetAttrBN(composite, "ClosedCurve", sdaiLOGICAL, &closed);
        ASSERT(res && *closed == 'T');
    }
    else {
        //
        SdaiInteger dm = 0;
        auto res = sdaiGetAttrBN(pt, "Dim", sdaiINTEGER, &dm);
        ASSERT(!res);

        SdaiAggr P = NULL;
        res = sdaiGetAttrBN(a2p3d, "P", sdaiAGGR, &P);
        ASSERT(!res && !P);

        SdaiReal scl = 0;
        res = sdaiGetAttrBN(oper, "Scl2", sdaiREAL, &scl);
        ASSERT(!res);

        const char* closed = nullptr;
        res = sdaiGetAttrBN(composite, "ClosedCurve", sdaiLOGICAL, &closed);
        ASSERT(!res);
    }
}

ExpressID Ifc4test()
{
    auto model = sdaiOpenModelBN(0, "..\\TestData\\AggregationTest.ifc", "");
    ASSERT(model);

    auto oper = IFC4::IfcCartesianTransformationOperator3DnonUniform::Create(model);
    auto extID = internalGetP21Line(oper);
    oper.put_Scale(1.5);

    IFC4::IfcCurve curve = internalGetInstanceFromP21Line(model, 79);
    ASSERT(curve);

    auto seg = IFC4::IfcCompositeCurveSegment::Create(model);
    seg.put_ParentCurve(curve);
    seg.put_Transition(IFC4::IfcTransitionCode::CONTINUOUS);

    auto composite = IFC4::IfcCompositeCurve::Create(model);
    IFC4::ListOfIfcCompositeCurveSegment lstSeg;
    lstSeg.push_back(seg);
    composite.put_Segments(lstSeg);

    //
    Ifc4test_Check(model, extID, false, false, false);

    bool ok = engiEnableDerivedAttributes(model, true);
    ASSERT(ok);

    Ifc4test_Check(model, extID, true, false, false);

    ok = engiEnableDerivedAttributes(model, false);
    ASSERT(ok);

    Ifc4test_Check(model, extID, false, false, false);
    //

    sdaiSaveModelBN(model, TEST_FILE);
    sdaiCloseModel(model);
    return extID;
}

static void CheckDerivedCache(ExpressID operId)
{
    auto model = sdaiOpenModelBN(0, TEST_FILE, "");
    ASSERT(model);

    IFC4::IfcCartesianTransformationOperator3DnonUniform oper = internalForceInstanceFromP21Line(model, operId);
    ASSERT(oper);

    bool ok = engiEnableDerivedAttributes(model, true);
    ASSERT(ok);

    SdaiReal scl = 0;
    auto res = sdaiGetAttrBN(oper, "Scl2", sdaiREAL, &scl);
    ASSERT(res && fabs(scl - 1.5) < 1e-11);

    res = sdaiGetAttrBN(oper, "Scl2", sdaiREAL, &scl);
    ASSERT(res && fabs(scl - 1.5) < 1e-11);

    oper.put_Scale2(2.3);

    res = sdaiGetAttrBN(oper, "Scl2", sdaiREAL, &scl);
    ASSERT(res && fabs(scl - 2.3) < 1e-11);

    sdaiCloseModel(model);
}

static void TestEvaluateAll(ExpressID operId)
{
    SdaiModel model = sdaiOpenModelBN(0, TEST_FILE, "");
    ASSERT(model);

    Ifc4test_Check(model, operId, false, false, false);

    //
    engiEvaluateAllDerivedAttributes(model, false);
    sdaiSaveModelBN(model, "engiEvaluateAllDerivedAttributes_0.ifc");

    Ifc4test_Check(model, operId, false, true, false);

    //
    engiEvaluateAllDerivedAttributes(model, true);
    sdaiSaveModelBN(model, "engiEvaluateAllDerivedAttributes_1.ifc");

    Ifc4test_Check(model, operId, false, true, true);

    sdaiCloseModel(model);

    //
    model = sdaiOpenModelBN(0, "engiEvaluateAllDerivedAttributes_0.ifc", "");
    Ifc4test_Check(model, operId, false, true, false);
    sdaiCloseModel(model);

    model = sdaiOpenModelBN(0, "engiEvaluateAllDerivedAttributes_1.ifc", "");
    Ifc4test_Check(model, operId, false, true, true);
    sdaiCloseModel(model);
}

static void TestAttributes(SdaiEntity entity, int nAttr, const char* rNames[], const char* rDef[], SdaiBoolean derived[])
{
    SdaiAttr attr = 0;
    int i = 0;
    while (attr = engiGetEntityAttributeByIterator(entity, attr)) {
        ASSERT(i < nAttr);

        const char* name = 0;
        SdaiEntity def = 0;
        engiGetAttributeTraits(attr, &name, &def, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
        ASSERT(0 == strcmp(name, rNames[i]));
        ASSERT(def);

        const char* defName = engiGetEntityName(def, sdaiSTRING);
        ASSERT(0 == strcmp(defName, rDef[i]));

        auto der = engiGetAttrDerived(entity, attr);
        ASSERT(der == derived[i]);

        i++;
    }
    ASSERT(i == nAttr);
}

static void IterateAttributes()
{
    auto model = sdaiCreateModelBN("IFC4");

    auto ent = sdaiGetEntity(model, "IfcSIUnit");
    const char* rNames[] = { "Dimensions", "UnitType", "Prefix", "Name" };
    const char* rDef[] = { "IfcNamedUnit", "IfcNamedUnit", "IfcSIUnit", "IfcSIUnit" };
    SdaiBoolean derived[] = { sdaiTRUE, false, false, false };
    TestAttributes(ent, 4, rNames, rDef, derived);

    ent = sdaiGetEntity(model, "IfcNamedUnit");
    derived[0] = sdaiFALSE;
    TestAttributes(ent, 2, rNames, rDef, derived);

    ent = sdaiGetEntity(model, "IfcCartesianPoint");
    const char* rNames2[] = { "LayerAssignment", "StyledByItem", "Coordinates", "Dim" };
    const char* rDef2[] = { "IfcRepresentationItem", "IfcRepresentationItem", "IfcCartesianPoint", "IfcCartesianPoint" };
    SdaiBoolean rDerv2[] = { sdaiFALSE, sdaiFALSE, sdaiFALSE, sdaiTRUE };
    TestAttributes(ent, 4, rNames2, rDef2, rDerv2);

    ent = sdaiGetEntity(model, "IfcLine");
    const char* rNames3[] = { "LayerAssignment", "StyledByItem", "Pnt", "Dir", "Dim" };
    SdaiBoolean rDerv3[] = { sdaiFALSE, sdaiFALSE, sdaiFALSE, sdaiFALSE, sdaiTRUE };
    const char* rDef3[] = { "IfcRepresentationItem", "IfcRepresentationItem", "IfcLine", "IfcLine", "IfcCurve"};
    TestAttributes(ent, 5, rNames3, rDef3, rDerv3);

    sdaiCloseModel(model);
}


extern void DeriveAttrTests()
{
    ENTER_TEST;

    auto operId = Ifc4test();

    CheckDerivedCache(operId);

    TestEvaluateAll(operId);

    IterateAttributes();
}
