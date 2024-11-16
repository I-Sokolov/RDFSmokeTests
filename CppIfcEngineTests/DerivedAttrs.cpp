#include "pch.h"

static void DimensionalExponents()
{
    auto model = sdaiOpenModelBN(0, "..\\TestData\\AggregationTest.ifc", "");
    ASSERT(model);

    IFC4::IfcSIUnit lengthUnit = internalGetInstanceFromP21Line(model, 391);
    ASSERT(lengthUnit);

    auto exponents = lengthUnit.get_Dimensions();
    ASSERT(exponents);

    auto exp = exponents.get_LengthExponent();
    ASSERT(!exp.IsNull() && exp.Value() == 1);

    exp = exponents.get_TimeExponent();
    ASSERT(!exp.IsNull() && exp.Value() == 0);

    sdaiCloseModel(model);
}

extern void DeriveAttrTests()
{
    ENTER_TEST;

    DimensionalExponents();
}
