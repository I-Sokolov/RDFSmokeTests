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

            TestGlobalRules(model);

            ifcengine.sdaiCloseModel(model);
        }

        static void TestGlobalRules(Int64 model)
        {
            var ok = ifcengine.engiEnableExpressScript(model, true);
            ASSERT(ok);

            var funcNames = new HashSet<string>();
            funcNames.Add("IfcAssociatedSurface");
            funcNames.Add("IfcBaseAxis");
            funcNames.Add("IfcBooleanChoose");

            var ruleNames = new HashSet<string>();
            ruleNames.Add("IfcSingleProjectInstance");
            ruleNames.Add("IfcRepresentationContextSameWCS");

            Int64 script = 0;
            while (0!=(script = ifcengine.engiGetSchemaScriptDeclarationByIterator(model, script)))
            {
                string label;
                string text;

                var type = ifcengine.engiGetDeclarationType(script);
                switch (type)
                {
                    case enum_express_declaration.__PROCEDURE:
                        ASSERT(false); //not expected in IFC4
                        break;

                    case enum_express_declaration.__FUNCTION:
                        ifcengine.engiGetScriptText(script, out label, out _);
                        funcNames.Remove(label);
                        break;

                    case enum_express_declaration.__GLOBAL_RULE:
                        ifcengine.engiGetScriptText(script, out label, out text);
                        ASSERT(ruleNames.Contains(label));
                        ruleNames.Remove(label);
                        break;

                    default:
                        ASSERT(false); //not expected
                        break;
                } 
            }

            ASSERT(funcNames.Count == 0 && ruleNames.Count == 0);
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
