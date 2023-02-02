using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using RDF;
using IFC4;
using System.Runtime.InteropServices;

namespace CsIfcEngineTests
{
    internal class PutGetAttr : CsTests.TestBase
    {
        class PrimitiveValues
        {
            public Int64? intVal;
            public double? realVal;
            public Int64? instVal;
            public string enumVal;
            public bool? boolVal;
            public string logicalVal;
            public string stringVal;
            public string expressStringVal;
            public string binVal;
        };

        public static void Test()
        {
            Test(true);
            Test(false);
        }

        static void Test (bool unicode)
        {
            TestGetPrimitiveValue(unicode);
            /*
            sdaiADB					1
            sdaiAGGR				sdaiADB + 1
            sdaiBINARY				sdaiAGGR + 1
            sdaiUNICODE				sdaiSTRING + 1
            sdaiEXPRESSSTRING		sdaiUNICODE + 1
            engiGLOBALID			sdaiEXPRESSSTRING + 1
            sdaiNUMBER				engiGLOBALID + 1
            */

        }


        static void TestGetPrimitiveValue(bool unicode)
        {
            ENTER_TEST(unicode ? "Unicode" : "Ascii");

            var model = CreateModel("", "IFC4", unicode);
            ASSERT(model != 0);

            var wall = IfcWall.Create(model);

            CheckValues(wall, "Name", null, null, null);

            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiSTRING, "1234");
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234" }, null, null);

            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiSTRING, "T");
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "T", expressStringVal = "T" }, null, null);

            Int64 i = 1234;
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiINTEGER, ref i);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234 }, null, null);

            double d = 12.34;
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiREAL, ref d);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34 }, null, null);

            bool b = true;
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiBOOLEAN, ref b);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T.", binVal = "T" }, null, null);

            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiLOGICAL, "U");
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U.", binVal = "U" }, null, null);

            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiENUM, "EEE"); //TODO - should not return sdaiLOGICAL
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "EEE", logicalVal="EEE", stringVal = ".EEE.", expressStringVal = ".EEE.", binVal = "EEE" }, null, null);

            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiENUM, "F"); 
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F.", binVal = "F" }, null, null);

            var typ = IfcWallType.Create(model);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiINSTANCE, typ);
            CheckValues(wall, "Name", new PrimitiveValues { instVal = typ }, null, null);

            ifcengine.sdaiCloseModel(model);

        }

        static void CheckValues
            (Int64 inst,
             string attrName,
             PrimitiveValues expected,
             PrimitiveValues expectedAdb,
             List<PrimitiveValues> expectedAggr
            )
        {
            var entity = ifcengine.sdaiGetInstanceType(inst);
            ASSERT(entity != 0);

            var attr = ifcengine.sdaiGetAttrDefinition(entity, attrName);
            ASSERT(attr != 0);

            Int64 adbVal = 1;
            var res = ifcengine.sdaiGetAttr(inst, attr, ifcengine.sdaiADB, out adbVal);
            if (expectedAdb != null)
            {
                ASSERT(res != 0);
                CheckADBValues(adbVal, expectedAdb);
            }
            else
            {
                ASSERT(res == 0 && adbVal == 0);
            }

            Int64 aggrVal = 1;
            res = ifcengine.sdaiGetAttr(inst, attr, ifcengine.sdaiAGGR, out aggrVal);
            if (expectedAggr != null)
            {
                ASSERT(res != 0);
                CheckAggrValues(aggrVal, expectedAggr);
            }
            else
            {
                ASSERT(res == 0 && aggrVal == 0);
            }

            IntPtr ptrVal = IntPtr.MaxValue;
            res = ifcengine.sdaiGetAttr(inst, attr, ifcengine.sdaiBINARY, out ptrVal);
            if (expected != null && expected.binVal != null)
            {
                ASSERT(res != 0);
                string str = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(ptrVal);
                ASSERT(str.Equals(expected.binVal));
            }
            else
            {
                ASSERT(res == 0 && ptrVal == IntPtr.Zero);
            }

            Int64 intVal = 1;
            res = ifcengine.sdaiGetAttr(inst, attr, ifcengine.sdaiBOOLEAN, out intVal);
            if (expected != null && expected.boolVal != null)
            {
                ASSERT(res != 0);
                ASSERT((intVal != 0) == expected.boolVal);
            }
            else
            {
                ASSERT(res == 0 && intVal == 0);
            }

            ptrVal = IntPtr.MaxValue;
            res = ifcengine.sdaiGetAttr(inst, attr, ifcengine.sdaiENUM, out ptrVal);
            if (expected != null && expected.enumVal != null)
            {
                ASSERT(res != 0);
                string str = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(ptrVal);
                ASSERT(str.Equals(expected.enumVal));
            }
            else
            {
                ASSERT(res == 0 && ptrVal == IntPtr.Zero);
            }

            intVal = 1;
            res = ifcengine.sdaiGetAttr(inst, attr, ifcengine.sdaiINSTANCE, out intVal);
            if (expected != null && expected.instVal != null)
            {
                ASSERT(res != 0);
                ASSERT(intVal == expected.instVal.Value);
            }
            else
            {
                ASSERT(res == 0 && intVal == 0);
            }

            intVal = 1;
            res = ifcengine.sdaiGetAttr(inst, attr, ifcengine.sdaiINTEGER, out intVal);
            if (expected != null && expected.intVal != null)
            {
                ASSERT(res != 0);
                ASSERT(intVal == expected.intVal.Value);
            }
            else
            {
                ASSERT(res == 0 && intVal == 0);
            }

            ptrVal = IntPtr.MaxValue;
            res = ifcengine.sdaiGetAttr(inst, attr, ifcengine.sdaiLOGICAL, out ptrVal);
            if (expected != null && expected.logicalVal != null)
            {
                ASSERT(res != 0);
                string str = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(ptrVal);
                ASSERT(str.Equals(expected.logicalVal));
            }
            else
            {
                ASSERT(res == 0 && ptrVal == IntPtr.Zero);
            }

            double realVal = 1.1;
            res = ifcengine.sdaiGetAttr(inst, attr, ifcengine.sdaiREAL, out realVal);
            if (expected != null && expected.realVal != null)
            {
                ASSERT(res != 0);
                ASSERT(realVal == expected.realVal.Value);
            }
            else
            {
                ASSERT(res == 0 && realVal == 0);
            }

            ptrVal = IntPtr.MaxValue;
            res = ifcengine.sdaiGetAttr(inst, attr, ifcengine.sdaiSTRING, out ptrVal);
            if (expected != null && expected.stringVal != null)
            {
                ASSERT(res != 0);
                string str = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(ptrVal);
                ASSERT(str.Equals(expected.stringVal));
            }
            else
            {
                ASSERT(res == 0 && ptrVal == IntPtr.Zero);
            }

            ptrVal = IntPtr.MaxValue;
            res = ifcengine.sdaiGetAttr(inst, attr, ifcengine.sdaiUNICODE, out ptrVal);
            if (expected != null && expected.stringVal != null)
            {
                ASSERT(res != 0);
                string str = System.Runtime.InteropServices.Marshal.PtrToStringUni(ptrVal);
                ASSERT(str.Equals(expected.stringVal));
            }
            else
            {
                ASSERT(res == 0 && ptrVal == IntPtr.Zero);
            }

            ptrVal = IntPtr.MaxValue;
            res = ifcengine.sdaiGetAttr(inst, attr, ifcengine.sdaiEXPRESSSTRING, out ptrVal);
            if (expected != null && expected.expressStringVal != null)
            {
                ASSERT(res != 0);
                string str = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(ptrVal);
                ASSERT(str.Equals(expected.expressStringVal));
            }
            else
            {
                ASSERT(res == 0 && ptrVal == IntPtr.Zero);
            }
        }

        static void CheckADBValues(Int64 adbVal, PrimitiveValues expectedAdb)
        {
            ASSERT(false); //todo
        }

        static void CheckAggrValues(Int64 aggrVal, List<PrimitiveValues> expectedAggr)
        {
            ASSERT(false); //todo
        }

    }
}