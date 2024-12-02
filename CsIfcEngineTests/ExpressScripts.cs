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

            EnumerateGlobalScripts(model);

            ifcengine.sdaiCloseModel(model);

            //
            model = ifcengine.sdaiOpenModelBN(0, "..\\TestData\\AggregationTest.ifc", "");

            TestEvaluateDerivedByScript(model);

            TestWhereRulesScript(model);

            TestUniqueRules(model);

            ifcengine.sdaiCloseModel(model);
        }

        static void TestUniqueRules(Int64 model)
        {
            var app = ifcengine.sdaiGetEntity(model, "IfcApplication");
            ASSERT(app);

            string label;
            Int64 rule = ifcengine.engiGetEntityUniqueRuleByIterator(app, 0, out label);
            ASSERT(rule);
            ASSERT(label == "UR1");

            rule = ifcengine.engiGetEntityUniqueRuleByIterator(app, rule, out label);
            ASSERT(rule);
            ASSERT(label == "UR2");

            rule = ifcengine.engiGetEntityUniqueRuleByIterator(app, rule, out label);
            ASSERT(rule==0);
        }

        static void TestWhereRulesScript(Int64 model)
        {
            long typeAngle = ifcengine.sdaiGetEntity(model, "IfcCompoundPlaneAngleMeasure");
            ASSERT(typeAngle != 0);

            string label;
            string text;
            Int64 rule = ifcengine.engiGetEntityWhereRuleByIterator(typeAngle, 0, out label);
            ASSERT(rule != 0 && label == "MinutesInRange");

            ifcengine.engiGetScriptText(rule, out _, out text);
            ASSERT(text == "ABS(SELF[2]) < 60;");

            string[] rLabels = { "SecondsInRange", "MicrosecondsInRange", "ConsistentSign" };

            int i = 0;
            while (0!=(rule = ifcengine.engiGetEntityWhereRuleByIterator(typeAngle, rule, out IntPtr _)))
            {
                ifcengine.engiGetScriptText(rule, out label, out _);
                ASSERT(label == rLabels[i]);
                i++;
            }
            ASSERT(i == 3);

            //
            IFC4.IfcCartesianPoint pt = ifcengine.internalForceInstanceFromP21Line(model, 99);
            var entity = ifcengine.sdaiGetInstanceType(pt);
            
            rule = ifcengine.engiGetEntityWhereRuleByIterator (entity, 0, out label);
            ASSERT(rule != 0 && label == "CP2Dor3D");

            string logval;
            var r = ifcengine.engiEvaluateScriptExpression (model, pt, rule, ifcengine.sdaiLOGICAL, out logval);
            ASSERT(r && logval == "T");
        }

        static void TestEvaluateDerivedByScript(Int64 model)
        {
            IFC4.IfcSIUnit lengthUnit = ifcengine.internalGetInstanceFromP21Line(model, 391);
            ASSERT(lengthUnit!=0);

            var entityNamedUnit = ifcengine.sdaiGetEntity(model, "IfcNamedUnit");
            ASSERT(entityNamedUnit!=0);

            var entitySIUnit = ifcengine.sdaiGetEntity(model, "IfcSIUnit");
            ASSERT(entitySIUnit != 0);

            var dimAttr = ifcengine.sdaiGetAttrDefinition(entityNamedUnit, "Dimensions");
            ASSERT(dimAttr != 0);

            var derivedScript = ifcengine.engiGetAttrDerived (entityNamedUnit, dimAttr);
            ASSERT(derivedScript == 0);

            derivedScript = ifcengine.engiGetAttrDerived(entitySIUnit, dimAttr);
            ASSERT(derivedScript != 0);

            string label;
            string text;
            ifcengine.engiGetScriptText(derivedScript, out label, out text);
            ASSERT(label == null && text == "IfcDimensionsForSiUnit (SELF.Name);");

            Int64 dim;
            var res = ifcengine.engiEvaluateScriptExpression (model, lengthUnit, derivedScript, ifcengine.sdaiINSTANCE, out dim);
            ASSERT(!res);

            var ok = ifcengine.engiEnableExpressScript(model, true);
            ASSERT(ok);

            res = ifcengine.engiEvaluateScriptExpression(model, lengthUnit, derivedScript, ifcengine.sdaiINSTANCE, out dim);
            ASSERT(res && dim != 0);

            IFC4.IfcDimensionalExponents ddim = dim;
            ASSERT(ddim != 0);
            ASSERT(ddim.LengthExponent.Value == 1);
            ASSERT(ddim.LuminousIntensityExponent.Value == 0);

            //
            //
            IFC4.IfcCartesianPoint pt = ifcengine.internalGetInstanceFromP21Line(model, 100);
            ASSERT(pt!=0);
            var entity = ifcengine.sdaiGetInstanceType(pt);
            derivedScript = ifcengine.engiGetAttrDerivedBN(entity, "Dim");
            ASSERT(derivedScript != 0);

            var derivedScript2 = ifcengine.engiGetAttrDerivedBN(entity, Encoding.ASCII.GetBytes("Dim"));
            ASSERT(derivedScript2 == derivedScript);

            res = ifcengine.engiEvaluateScriptExpression(model, pt, derivedScript, ifcengine.sdaiINSTANCE, out dim);
            ASSERT(!res);

            res = ifcengine.engiEvaluateScriptExpression(model, pt, derivedScript, ifcengine.sdaiINTEGER, out dim);
            ASSERT(res && dim == 3);

            double v;
            res = ifcengine.engiEvaluateScriptExpression(model, pt, derivedScript, ifcengine.sdaiREAL, out v);
            ASSERT(res && v == 3);

            bool b;
            res = ifcengine.engiEvaluateScriptExpression(model, pt, derivedScript, ifcengine.sdaiBOOLEAN, out b);
            ASSERT(!res);

            string s;
            res = ifcengine.engiEvaluateScriptExpression(model, pt, derivedScript, ifcengine.sdaiSTRING, out s);
            ASSERT(!res);
        }

        static void EnumerateGlobalScripts(Int64 model)
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
