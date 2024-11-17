#include "pch.h"

#define TEST_FILE "DeriveAttr.ifc"

static void DimensionalExponents_Check(SdaiModel model, bool calculated)
{
    IFC4::IfcSIUnit lengthUnit = internalGetInstanceFromP21Line(model, 391);
    ASSERT(lengthUnit);

    if (calculated) {
        auto exponents = lengthUnit.get_Dimensions();
        ASSERT(exponents);

        auto exp = exponents.get_LengthExponent();
        ASSERT(!exp.IsNull() && exp.Value() == 1);

        exp = exponents.get_TimeExponent();
        ASSERT(!exp.IsNull() && exp.Value() == 0);
    }
    else {
        //
        auto exponents = lengthUnit.get_Dimensions();
        ASSERT(!exponents);
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

    sdaiSaveModelBN(model, TEST_FILE);
    sdaiCloseModel(model);

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
