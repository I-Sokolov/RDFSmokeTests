using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using RDF;
using IFC4;

/*
sdaiADB					1
sdaiAGGR				sdaiADB + 1
sdaiBINARY				sdaiAGGR + 1
sdaiBOOLEAN				sdaiBINARY + 1
sdaiENUM				sdaiBOOLEAN + 1
sdaiINSTANCE			sdaiENUM + 1
sdaiINTEGER				sdaiINSTANCE + 1
sdaiLOGICAL				sdaiINTEGER + 1
sdaiREAL				sdaiLOGICAL + 1
sdaiSTRING				sdaiREAL + 1
sdaiUNICODE				sdaiSTRING + 1
sdaiEXPRESSSTRING		sdaiUNICODE + 1
engiGLOBALID			sdaiEXPRESSSTRING + 1
sdaiNUMBER				engiGLOBALID + 1
*/

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
            TestGetDollarValue(unicode);
        }


        static void TestGetDollarValue (bool unicode)
        {
            ENTER_TEST(unicode ? "Unicode" : "Ascii");

            var model = CreateModel("", "IFC4", unicode);
            ASSERT(model != 0);

            var wall = IfcWall.Create(model);

            CheckValues(wall, "Name", null, null, null);

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