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

            var model = IFCEngine.sdaiOpenModelBN(0, "..\\TestData\\DataFiles\\Wall_SweptSolid.ifc", "");
            ASSERT(model != 0);

            TestSIUnitsDerivedDim(model);

            EnumerateGlobalScripts(model);

            IFCEngine.sdaiCloseModel(model);

            //
            model = IFCEngine.sdaiOpenModelBN(0, "..\\TestData\\AggregationTest.ifc", "");

            TestEvaluateDerivedByScript(model);

            TestWhereRulesScript(model);

            TestUniqueRules(model);

            IFCEngine.sdaiCloseModel(model);
        }

        static void TestUniqueRules(Int64 model)
        {
            var app = IFCEngine.sdaiGetEntity(model, "IfcApplication");
            ASSERT(app);

            string label;
            Int64 rule = IFCEngine.engiGetEntityUniqueRuleByIterator(app, 0, out label);
            ASSERT(rule);
            ASSERT(label == "UR1");

            string domain;
            string attrName = IFCEngine.engiGetEntityUniqueRuleAttributeByIterator(rule, null, out domain);
            ASSERT(attrName == "ApplicationIdentifier" && domain == null);

            attrName = IFCEngine.engiGetEntityUniqueRuleAttributeByIterator(rule, attrName, out domain);
            ASSERT(attrName == null && domain == null);

            rule = IFCEngine.engiGetEntityUniqueRuleByIterator(app, rule, out label);
            ASSERT(rule);
            ASSERT(label == "UR2");

            attrName = IFCEngine.engiGetEntityUniqueRuleAttributeByIterator(rule, null, out domain);
            ASSERT(attrName == "ApplicationFullName" && domain == null);

            attrName = IFCEngine.engiGetEntityUniqueRuleAttributeByIterator(rule, attrName, out domain);
            ASSERT(attrName == "Version" && domain == null);

            attrName = IFCEngine.engiGetEntityUniqueRuleAttributeByIterator(rule, attrName, out domain);
            ASSERT(attrName == null && domain == null);

            rule = IFCEngine.engiGetEntityUniqueRuleByIterator(app, rule, out label);
            ASSERT(rule==0);
        }

        static void TestWhereRulesScript(Int64 model)
        {
            long typeAngle = IFCEngine.sdaiGetEntity(model, "IfcCompoundPlaneAngleMeasure");
            ASSERT(typeAngle != 0);

            string label;
            string text;
            Int64 rule = IFCEngine.engiGetEntityWhereRuleByIterator(typeAngle, 0, out label);
            ASSERT(rule != 0 && label == "MinutesInRange");

            IFCEngine.engiGetScriptText(rule, out _, out text);
            ASSERT(text == "ABS(SELF[2]) < 60;");

            string[] rLabels = { "SecondsInRange", "MicrosecondsInRange", "ConsistentSign" };

            int i = 0;
            while (0!=(rule = IFCEngine.engiGetEntityWhereRuleByIterator(typeAngle, rule, out IntPtr _)))
            {
                IFCEngine.engiGetScriptText(rule, out label, out _);
                ASSERT(label == rLabels[i]);
                i++;
            }
            ASSERT(i == 3);

            //
            IFC4.IfcCartesianPoint pt = IFCEngine.internalForceInstanceFromP21Line(model, 99);
            var entity = IFCEngine.sdaiGetInstanceType(pt);
            
            rule = IFCEngine.engiGetEntityWhereRuleByIterator (entity, 0, out label);
            ASSERT(rule != 0 && label == "CP2Dor3D");

            string logval;
            var r = IFCEngine.engiEvaluateScriptExpression (model, pt, rule, IFCEngine.sdaiLOGICAL, out logval);
            ASSERT(r != 0 && logval == "T");
        }

        static void TestEvaluateDerivedByScript(Int64 model)
        {
            IFC4.IfcSIUnit lengthUnit = IFCEngine.internalGetInstanceFromP21Line(model, 391);
            ASSERT(lengthUnit!=0);

            var entityNamedUnit = IFCEngine.sdaiGetEntity(model, "IfcNamedUnit");
            ASSERT(entityNamedUnit!=0);

            var entitySIUnit = IFCEngine.sdaiGetEntity(model, "IfcSIUnit");
            ASSERT(entitySIUnit != 0);

            var dimAttr = IFCEngine.sdaiGetAttrDefinition(entityNamedUnit, "Dimensions");
            ASSERT(dimAttr != 0);

            var derivedScript = IFCEngine.engiGetAttrDerived (entityNamedUnit, dimAttr);
            ASSERT(derivedScript == 0);

            derivedScript = IFCEngine.engiGetAttrDerived(entitySIUnit, dimAttr);
            ASSERT(derivedScript != 0);

            string label;
            string text;
            IFCEngine.engiGetScriptText(derivedScript, out label, out text);
            ASSERT(label == null && text == "IfcDimensionsForSiUnit (SELF.Name);");

            Int64 dim;
            var res = IFCEngine.engiEvaluateScriptExpression(model, lengthUnit, derivedScript, IFCEngine.sdaiINSTANCE, out dim);
            ASSERT(res != 0 && dim != 0);

            var ok = IFCEngine.engiEnableDerivedAttributes(model, true);
            ASSERT(ok);

            IFC4.IfcDimensionalExponents ddim = dim;
            ASSERT(ddim != 0);
            ASSERT(ddim.LengthExponent.Value == 1);
            ASSERT(ddim.LuminousIntensityExponent.Value == 0);

            //
            //
            IFC4.IfcCartesianPoint pt = IFCEngine.internalGetInstanceFromP21Line(model, 100);
            ASSERT(pt!=0);
            var entity = IFCEngine.sdaiGetInstanceType(pt);
            derivedScript = IFCEngine.engiGetAttrDerivedBN(entity, "Dim");
            ASSERT(derivedScript != 0);

            var derivedScript2 = IFCEngine.engiGetAttrDerivedBN(entity, Encoding.ASCII.GetBytes("Dim"));
            ASSERT(derivedScript2 == derivedScript);

            res = IFCEngine.engiEvaluateScriptExpression(model, pt, derivedScript, IFCEngine.sdaiINSTANCE, out dim);
            ASSERT(res == 0);

            res = IFCEngine.engiEvaluateScriptExpression(model, pt, derivedScript, IFCEngine.sdaiINTEGER, out dim);
            ASSERT(res != 0 && dim == 3);

            double v;
            res = IFCEngine.engiEvaluateScriptExpression(model, pt, derivedScript, IFCEngine.sdaiREAL, out v);
            ASSERT(res != 0 && v == 3);

            bool b;
            res = IFCEngine.engiEvaluateScriptExpression(model, pt, derivedScript, IFCEngine.sdaiBOOLEAN, out b);
            ASSERT(res == 0);

            string s;
            res = IFCEngine.engiEvaluateScriptExpression(model, pt, derivedScript, IFCEngine.sdaiSTRING, out s);
            ASSERT(res == 0);
        }

        static void EnumerateGlobalScripts(Int64 model)
        {
            var ok = IFCEngine.engiEnableDerivedAttributes(model, true);
            ASSERT(ok);

            var funcNames = new HashSet<string>();
            funcNames.Add("IfcAssociatedSurface");
            funcNames.Add("IfcBaseAxis");
            funcNames.Add("IfcBooleanChoose");

            var ruleNames = new HashSet<string>();
            ruleNames.Add("IfcSingleProjectInstance");
            ruleNames.Add("IfcRepresentationContextSameWCS");

            Int64 script = 0;
            while (0!=(script = IFCEngine.engiGetSchemaScriptDeclarationByIterator(model, script)))
            {
                string label;
                string text;

                var type = IFCEngine.engiGetDeclarationType(script);
                switch (type)
                {
                    case enum_express_declaration.__PROCEDURE:
                        ASSERT(false); //not expected in IFC4
                        break;

                    case enum_express_declaration.__FUNCTION:
                        IFCEngine.engiGetScriptText(script, out label, out _);
                        funcNames.Remove(label);
                        break;

                    case enum_express_declaration.__GLOBAL_RULE:
                        IFCEngine.engiGetScriptText(script, out label, out text);
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

            var ok = IFCEngine.engiEnableDerivedAttributes(model, true);
            ASSERT(ok);
            TestSIUnitsDerivedDim(model, true);

            ok = IFCEngine.engiEnableDerivedAttributes(model, false);
            ASSERT(ok);
            TestSIUnitsDerivedDim(model, false);

            ok = IFCEngine.engiEnableDerivedAttributes(model, true);
            ASSERT(ok);
            TestSIUnitsDerivedDim(model, true);
        }

        static void TestSIUnitsDerivedDim (Int64 model, bool scriptEnabled)
        {
            var units = IFCEngine.sdaiGetEntityExtentBN(model, "IfcSIUnit");
            ASSERT(units != 0);

            var it = IFCEngine.sdaiCreateIterator(units);
            ASSERT(it != 0);

            while (IFCEngine.sdaiNext(it))
            {
                Int64 instance = 0;

                IFC4.IfcSIUnit unit = IFCEngine.sdaiGetAggrByIterator(it, IFCEngine.sdaiINSTANCE, out instance);
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
            IFCEngine.sdaiDeleteIterator(it);
        }
    }
}
