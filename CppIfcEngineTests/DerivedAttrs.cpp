#include "pch.h"

#define TEST_FILE "DeriveAttr.ifc"

static void DimensionalExponents_Check(SdaiModel model, bool calculated)
{
    IFC4::IfcSIUnit lengthUnit = internalGetInstanceFromP21Line(model, 391);
    ASSERT(lengthUnit);

    IFC4::IfcGeometricRepresentationSubContext sctx = internalGetInstanceFromP21Line(model, 33);
    ASSERT(sctx);

    IFC4::IfcAxis2Placement3D a2p3d = internalForceInstanceFromP21Line(model, 91);
    ASSERT(a2p3d);

    IFC4::IfcCartesianPoint pt = internalGetInstanceFromP21Line(model, 100);
    ASSERT(pt);
/*
    IfcCartesianTransformationOperator3DnonUniform.
        Scl2
        IfcCompositeCurve.ClosedCurve
        IfcGeometricRepresentationContext.Precision
        IfcMaterialLayerSet.TotalThickness
        */
    if (calculated) {
        auto exponents = lengthUnit.get_Dimensions();
        ASSERT(exponents);

        auto exp = exponents.get_LengthExponent();
        ASSERT(!exp.IsNull() && exp.Value() == 1);

        exp = exponents.get_TimeExponent();
        ASSERT(!exp.IsNull() && exp.Value() == 0);

        auto dim = sctx.get_CoordinateSpaceDimension();
        ASSERT(!dim.IsNull() && dim.Value() == 3);

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
    }
    else {
        //
        auto exponents = lengthUnit.get_Dimensions();
        ASSERT(!exponents);

        auto dim = sctx.get_CoordinateSpaceDimension();
        ASSERT(dim.IsNull());

        SdaiInteger dm = 0;
        auto res = sdaiGetAttrBN(pt, "Dim", sdaiINTEGER, &dm);
        ASSERT(!res);

        SdaiAggr P = NULL;
        res = sdaiGetAttrBN(a2p3d, "P", sdaiAGGR, &P);
        ASSERT(!res && !P);
    }
}

//int
//ID 32
//CoordinateSpaceDimension

static void DimensionalExponents()
{
    auto model = sdaiOpenModelBN(0, "..\\TestData\\AggregationTest.ifc", "");
    ASSERT(model);

    DimensionalExponents_Check(model, false);

    bool ok = engiSetDerivedAttributesSupport(model, true, false);
    ASSERT(ok);

    DimensionalExponents_Check(model, true);

    ok = engiSetDerivedAttributesSupport(model, false, true);
    ASSERT(ok);

    DimensionalExponents_Check(model, false);

    //sdaiSaveModelBN(model, TEST_FILE);
    sdaiCloseModel(model);
    return;
    model = sdaiOpenModelBN(0, TEST_FILE, "");
    ASSERT(model);

    DimensionalExponents_Check(model, true);

    sdaiCloseModel(model);
}

extern void DeriveAttrTests()
{
    ENTER_TEST;

    DimensionalExponents();
}
