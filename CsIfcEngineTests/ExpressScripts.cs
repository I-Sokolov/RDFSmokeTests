using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

using RDF;

namespace CsIfcEngineTests
{
    internal class ExpressScripts : CsTests.TestBase
    {
        public static void Run()
        {
            ENTER_TEST();

            var model = ifcengine.sdaiOpenModelBN(0, "..\\TestData\\Wall_SweptSolid.ifc", "");
            ASSERT(model != 0);

            TestSIUnitsDerivedDim(model);

            

            ifcengine.sdaiCloseModel(model);
        }

        static void TestSIUnitsDerivedDim(Int64 model)
        {
            TestSIUnitsDerivedDim(model, false);

            var ok = ifcengine.engiEnableExpressScript(model, true);
            ASSERT(ok);
            TestSIUnitsDerivedDim(model, true);

            ok = ifcengine.engiEnableExpressScript(model, false);
            ASSERT(ok);
            TestSIUnitsDerivedDim(model, false);

            ok = ifcengine.engiEnableExpressScript(model, true);
            ASSERT(ok);
            TestSIUnitsDerivedDim(model, true);
        }

        static void TestSIUnitsDerivedDim (Int64 model, bool scriptEnabled)
        {
            var units = ifcengine.sdaiGetEntityExtentBN(model, "IfcSIUnit");
            ASSERT(units != 0);

            var it = ifcengine.sdaiCreateIterator(units);
            ASSERT(it != 0);

            while (ifcengine.sdaiNext(it) != 0)
            {
                Int64 instance = 0;

                IFC4.IfcSIUnit unit = ifcengine.sdaiGetAggrByIterator(it, ifcengine.sdaiINSTANCE, out instance);
                ASSERT(unit != 0);


                IFC4.IfcDimensionalExponents dim = unit.Dimensions;
                ASSERT(scriptEnabled == (dim != 0));

                IFC4.IfcUnitEnum? unitType = unit.UnitType;
                ASSERT(unitType != null);

                if (unitType.Value == IFC4.IfcUnitEnum.MASSUNIT)
                {

                    IFC4.IfcSIUnitName? name = unit.Name;
                    ASSERT(name.HasValue);
                    ASSERT(name.Value == IFC4.IfcSIUnitName.GRAM);

                    if (dim != 0)
                    {
                        ASSERT(dim.LengthExponent == 0);
                        ASSERT(dim.MassExponent == 1);
                        ASSERT(dim.TimeExponent == 0);
                        ASSERT(dim.ElectricCurrentExponent == 0);
                        ASSERT(dim.ThermodynamicTemperatureExponent == 0);
                        ASSERT(dim.AmountOfSubstanceExponent == 0);
                        ASSERT(dim.LuminousIntensityExponent == 0);
                    }
                }
            }
            ifcengine.sdaiDeleteIterator(it);
        }
    }
}
