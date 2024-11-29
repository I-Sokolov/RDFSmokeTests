using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

using RDF;

namespace CsIfcEngineTests
{
    internal class DerivedAttributes : CsTests.TestBase
    {
        public static void Run()
        {
            ENTER_TEST();
            TestSIUnits();
        }

        static void TestSIUnits()
        {
            var model = ifcengine.sdaiOpenModelBN(0, "..\\TestData\\Wall_SweptSolid.ifc", "");
            ASSERT(model!=0);

            var ok = ifcengine.engiEnableEvaluatingDerivedAttributes(model, 1);
            ASSERT(ok!=0);

            var units = ifcengine.sdaiGetEntityExtentBN(model, "IfcSIUnit");
            ASSERT(units!=0);

            var it = ifcengine.sdaiCreateIterator(units);
            ASSERT(it!=0);

            while (ifcengine.sdaiNext(it) != 0)
            {
                Int64 instance = 0;

                IFC4.IfcSIUnit unit = ifcengine.sdaiGetAggrByIterator(it, ifcengine.sdaiINSTANCE, out instance);
                ASSERT(unit != 0);


                IFC4.IfcDimensionalExponents dim = unit.Dimensions;
                IFC4.IfcUnitEnum? unitType        = unit.UnitType;
                IFC4.IfcSIUnitName? name          = unit.Name;
                string prefix                     = unit.Prefix.HasValue ? unit.Prefix.Value.ToString() : "";

                //print
                System.Console.Write($" {unitType.Value} {prefix} {name.Value}  ");

                System.Console.WriteLine("{0} {1} {2} {3} {4} {5} {6}",
                        dim.LengthExponent,
                        dim.MassExponent,
                        dim.TimeExponent,
                        dim.ElectricCurrentExponent,
                        dim.ThermodynamicTemperatureExponent,
                        dim.AmountOfSubstanceExponent,
                        dim.LuminousIntensityExponent
                    );             
            }
            ifcengine.sdaiDeleteIterator(it);

            ifcengine.sdaiCloseModel(model);
        }
    }
}
