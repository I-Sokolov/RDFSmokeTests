using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using RDF;
using IFC4;

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
            
            public PrimitiveValues adbVal; //TODO subject to delete when other TODOs fixed: what we get through sdaiGetADBValue should mutch with sdaiGetAttr
            
            public int aggrLevel;
        };

        public static void Test()
        {
            Test(true);
            Test(false);
        }

        static void Test (bool unicode)
        {
            TestPrimitiveValue(unicode);
            TestADBPrimitive(unicode);
            TestAggregationPrimitive(unicode);

            TestADBAggregation(unicode);
            TestAggregationADB(unicode);

            TestAggregationAggregation(unicode);
        }


        static void TestPrimitiveValue(bool unicode)
        {
            ENTER_TEST(unicode ? "Unicode" : "Ascii");

            var model = CreateModel("", "IFC4", unicode);
            ASSERT(model != 0);

            var wall = IfcWall.Create(model);

            CheckValues(wall, "Name", null);

            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiSTRING, "1234");
            //TODO should sdaiGetAttr (sdaiADB) return ADB value after sdaiPutAttr (sdaiSTRING)? other primitive types? aggregations?
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234" });

            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiSTRING, "T");
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "T", expressStringVal = "T" });

            Int64 i = 1234;
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiINTEGER, ref i);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234 });

            double d = 12.34;
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiREAL, ref d);
            //TODO: not sure it is correct to return sdaiINTEGER when data value is real 
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34 });

            bool b = true;
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiBOOLEAN, ref b);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T.", binVal = "T" });

            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiLOGICAL, "U");
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U.", binVal = "U" });

            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiENUM, "EEE"); 
            //TODO - should not return sdaiLOGICAL
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "EEE", logicalVal="EEE", stringVal = ".EEE.", expressStringVal = ".EEE.", binVal = "EEE" });

            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiENUM, "F"); 
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F.", binVal = "F" });

            var typ = IfcWallType.Create(model);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiINSTANCE, typ);
            //TODO ifcengine allows get ADB when data is INSTANCE but I think it should work for any type
            CheckValues(wall, "Name", new PrimitiveValues 
            {
                adbVal = new PrimitiveValues { instVal = typ },
                instVal = typ 
            });

            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiBINARY, "0AF");
            //TODO - should sdaiGetAttr(sdaiENUM) return value for binary data?
            //TODO - should sdaiGetAttr(sdaiLOGICAL) return value for binary data?
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "0AF", logicalVal = "0AF", stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF" });

            ifcengine.sdaiCloseModel(model);

        }

        static void TestADBPrimitive(bool unicode)
        {
            ENTER_TEST(unicode ? "Unicode" : "Ascii");

            var model = CreateModel("", "IFC4", unicode);
            ASSERT(model != 0);

            var wall = IfcWall.Create(model);

            var adb = ifcengine.sdaiCreateADB(ifcengine.sdaiSTRING, "1234");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            //TODO sdaiPutAttrBN (sdaiSTRING) does not allow to get sdaiBINARY
            //TODO sdaiCreateADB and out it gets binart from ADB but does not get from attr (similar to firts one)
            CheckValues(wall, "Name",
                new PrimitiveValues
                {
                    adbVal = new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", binVal = "1234" },
                    stringVal = "1234",
                    expressStringVal = "1234"
                });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiSTRING, "T");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name",
                new PrimitiveValues
                {
                    adbVal = new PrimitiveValues { stringVal = "T", expressStringVal = "T", binVal = "T" },
                    stringVal = "T",
                    expressStringVal = "T"
                });

            Int64 i = 1234;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiINTEGER, ref i);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            //TODO: sdaiGetADBValue (string) does not work for ADB with integers but sdaiGetAttr(string) works
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { /*stringVal = "1234", expressStringVal = "1234",*/ intVal = 1234, realVal = 1234 },
                stringVal = "1234",
                expressStringVal = "1234",
                intVal = 1234,
                realVal = 1234
            });

            double d = 12.34;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiREAL, ref d);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            //TODO: sdaiGetADBValue (string) does not work for ADB with real but sdaiGetAttr(string) works
            //TODO: sdaiGetADBValue (int) does not work for ADB with real but sdaiGetAttr(string) works
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { /*stringVal = "12.340000", expressStringVal = "12.340000",*/ /*intVal = 12,*/ realVal = 12.34 },
                stringVal = "12.340000",
                expressStringVal = "12.340000",
                intVal = 12,
                realVal = 12.34
            });

            bool b = true;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiBOOLEAN, ref b);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            //TODO: sdaiGetADBValue returns T but sdaiGetAttr returns .T.
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = "T", expressStringVal = "T", binVal = "T" },
                boolVal = true,
                enumVal = "T",
                logicalVal = "T",
                stringVal = ".T.",
                expressStringVal = ".T.",
                binVal = "T"
            });

            b = false;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiBOOLEAN, ref b);
            ifcengine.sdaiPutADBTypePath(adb, 1, "IFCBOOLEAN");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { boolVal = false, enumVal = "F", logicalVal = "F", stringVal = "F", expressStringVal = "F", binVal = "F" },
                boolVal = false,
                enumVal = "F",
                logicalVal = "F",
                stringVal = ".F.",
                expressStringVal = ".F.",
                binVal = "F"
            });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiLOGICAL, "U");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            //TODO: sdaiGetADBValue returns U but sdaiGetAttr returns .U.
            //TODO: sdaiGetADBValue(sdaiBOOLEAN) returns false but sdaiGetAttr returns null
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { boolVal = false, enumVal = "U", logicalVal = "U", stringVal = "U", expressStringVal = "U", binVal = "U" },
                enumVal = "U",
                logicalVal = "U",
                stringVal = ".U.",
                expressStringVal = ".U.",
                binVal = "U"
            });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiENUM, "F");
            ifcengine.sdaiPutADBTypePath(adb, 1, "IFCLOGICAL");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { boolVal = false, enumVal = "F", logicalVal = "F", stringVal = "F", expressStringVal = "F", binVal = "F" },
                boolVal = false,
                enumVal = "F",
                logicalVal = "F",
                stringVal = ".F.",
                expressStringVal = ".F.",
                binVal = "F"
            });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiENUM, "EEE");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            //TODO - should not return sdaiLOGICAL
            //TODO: sdaiGetADBValue(sdaiBOOLEAN) returns false but sdaiGetAttr returns null
            //TODO: sdaiGetADBValue(sdaiSTRING) returns EEE but sdaiGetAttr .EEE.
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { boolVal = false, enumVal = "EEE", logicalVal = "EEE", stringVal = "EEE", expressStringVal = "EEE", binVal = "EEE" },
                enumVal = "EEE",
                logicalVal = "EEE",
                stringVal = ".EEE.",
                expressStringVal = ".EEE.",
                binVal = "EEE"
            });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiENUM, "F");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = "F", expressStringVal = "F", binVal = "F" },
                enumVal = "F",
                logicalVal = "F",
                boolVal = false,
                stringVal = ".F.",
                expressStringVal = ".F.",
                binVal = "F"
            });

            var typ = IfcWallType.Create(model);
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiINSTANCE, typ);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { instVal = typ },
                instVal = typ
            });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiBINARY, "0AF");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            //TODO differencies with sdaiGetAttr, see commented values
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { /*enumVal = "0AF", logicalVal = "0AF",*/ stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF" },
                enumVal = "0AF",
                logicalVal = "0AF",
                stringVal = "0AF",
                expressStringVal = "0AF",
                binVal = "0AF"
            });

            ifcengine.sdaiCloseModel(model);
        }

        static void TestAggregationPrimitive(bool unicode)
        {
            ENTER_TEST(unicode ? "Unicode" : "Ascii");

            var model = CreateModel("", "IFC4", unicode);
            ASSERT(model != 0);

            var wall = IfcWall.Create(model);

            var entity = ifcengine.sdaiGetInstanceType(wall);
            ASSERT(entity != 0);

            var attr = ifcengine.sdaiGetAttrDefinition(entity, "Name");
            ASSERT(attr != 0);

            var aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiSTRING, "1234");
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", aggrLevel = 1 });

            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiSTRING, "T");
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "T", expressStringVal = "T", aggrLevel = 1 });

            Int64 i = 1234;
            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiINTEGER, ref i);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234, aggrLevel = 1 });

            double d = 12.34;
            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiREAL, ref d);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34, aggrLevel = 1 });

            bool b = true;
            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiBOOLEAN, ref b);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T.", binVal = "T", aggrLevel = 1 });

            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiLOGICAL, "U");
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U.", binVal = "U", aggrLevel = 1 });

            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiENUM, "EEE");
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "EEE", logicalVal = "EEE", stringVal = ".EEE.", expressStringVal = ".EEE.", binVal = "EEE", aggrLevel = 1 });

            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiENUM, "F");
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F.", binVal = "F", aggrLevel = 1 });

            var typ = IfcWallType.Create(model);
            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiINSTANCE, typ);
            CheckValues(wall, "Name", new PrimitiveValues { instVal = typ, adbVal = new PrimitiveValues { instVal = typ }, aggrLevel = 1 });

            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiBINARY, "0AF");
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "0AF", logicalVal = "0AF", stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF", aggrLevel = 1 });

            ifcengine.sdaiCloseModel(model);
        }

        static void TestADBAggregation(bool unicode)
        {
            ENTER_TEST(unicode ? "Unicode" : "Ascii");

            var model = CreateModel("", "IFC4", unicode);
            ASSERT(model != 0);

            var wall = IfcWall.Create(model);

            var entity = ifcengine.sdaiGetInstanceType(wall);
            ASSERT(entity != 0);

            var attr = ifcengine.sdaiGetAttrDefinition(entity, "Name");
            ASSERT(attr != 0);

            var aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiSTRING, "1234");
            var adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            //TODO: when ADB contains string you can call sdaiGetAttr but when ABD contains aggr it does not work
            CheckValues(wall, "Name", new PrimitiveValues 
            { 
                adbVal = new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", aggrLevel= 1}
                //stringVal = "1234", expressStringVal = "1234", aggrLevel=1
            });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiSTRING, "T");
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { adbVal = new PrimitiveValues { stringVal = "T", expressStringVal = "T", aggrLevel = 1 } });

            Int64 i = 1234;
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiINTEGER, ref i);
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { adbVal = new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234, aggrLevel = 1 } });

            double d = 12.34;
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiREAL, ref d);
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { adbVal = new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34, aggrLevel = 1 } });

            bool b = true;
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiBOOLEAN, ref b);
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { adbVal = new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T.", binVal = "T", aggrLevel = 1 } });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiLOGICAL, "U");
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { adbVal = new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U.", binVal = "U", aggrLevel = 1 } });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiENUM, "EEE");
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { adbVal = new PrimitiveValues { enumVal = "EEE", logicalVal = "EEE", stringVal = ".EEE.", expressStringVal = ".EEE.", binVal = "EEE", aggrLevel = 1 } });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiENUM, "F");
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { adbVal = new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F.", binVal = "F", aggrLevel = 1 } });

            var typ = IfcWallType.Create(model);
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiINSTANCE, typ);
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { adbVal = new PrimitiveValues { instVal = typ, adbVal = new PrimitiveValues { instVal = typ }, aggrLevel = 1 } });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiBINARY, "0AF");
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { adbVal = new PrimitiveValues { enumVal = "0AF", logicalVal = "0AF", stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF", aggrLevel = 1 } });

            ifcengine.sdaiCloseModel(model);
        }

        static void TestAggregationADB(bool unicode)
        {
            ENTER_TEST(unicode ? "Unicode" : "Ascii");

            var model = CreateModel("", "IFC4", unicode);
            ASSERT(model != 0);

            var wall = IfcWall.Create(model);

            var adb = ifcengine.sdaiCreateADB(ifcengine.sdaiSTRING, "1234");
            var aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name",
                new PrimitiveValues
                {
                    adbVal = new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", binVal = "1234" },
                    stringVal = "1234",
                    expressStringVal = "1234",
                    aggrLevel = 1
                });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiSTRING, "T");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name",
                new PrimitiveValues
                {
                    adbVal = new PrimitiveValues { stringVal = "T", expressStringVal = "T", binVal = "T" },
                    stringVal = "T",
                    expressStringVal = "T",
                    aggrLevel = 1
                });

            Int64 i = 1234;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiINTEGER, ref i);
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { /*stringVal = "1234", expressStringVal = "1234",*/ intVal = 1234, realVal = 1234 },
                stringVal = "1234",
                expressStringVal = "1234",
                intVal = 1234,
                realVal = 1234,
                aggrLevel = 1
            });

            double d = 12.34;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiREAL, ref d);
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { /*stringVal = "12.340000", expressStringVal = "12.340000",*/ /*intVal = 12,*/ realVal = 12.34 },
                stringVal = "12.340000",
                expressStringVal = "12.340000",
                intVal = 12,
                realVal = 12.34,
                aggrLevel = 1
            });

            bool b = true;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiBOOLEAN, ref b);
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = "T", expressStringVal = "T", binVal = "T" },
                boolVal = true,
                enumVal = "T",
                logicalVal = "T",
                stringVal = ".T.",
                expressStringVal = ".T.",
                binVal = "T",
                aggrLevel = 1
            });

            b = false;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiBOOLEAN, ref b);
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { boolVal = false, enumVal = "F", logicalVal = "F", stringVal = "F", expressStringVal = "F", binVal = "F" },
                boolVal = false,
                enumVal = "F",
                logicalVal = "F",
                stringVal = ".F.",
                expressStringVal = ".F.",
                binVal = "F",
                aggrLevel = 1
            });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiLOGICAL, "U");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { boolVal = false, enumVal = "U", logicalVal = "U", stringVal = "U", expressStringVal = "U", binVal = "U" },
                enumVal = "U",
                logicalVal = "U",
                stringVal = ".U.",
                expressStringVal = ".U.",
                binVal = "U",
                aggrLevel = 1
            });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiENUM, "F");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { boolVal = false, enumVal = "F", logicalVal = "F", stringVal = "F", expressStringVal = "F", binVal = "F" },
                boolVal = false,
                enumVal = "F",
                logicalVal = "F",
                stringVal = ".F.",
                expressStringVal = ".F.",
                binVal = "F",
                aggrLevel = 1
            });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiENUM, "EEE");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { boolVal = false, enumVal = "EEE", logicalVal = "EEE", stringVal = "EEE", expressStringVal = "EEE", binVal = "EEE" },
                enumVal = "EEE",
                logicalVal = "EEE",
                stringVal = ".EEE.",
                expressStringVal = ".EEE.",
                binVal = "EEE",
                aggrLevel = 1
            });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiENUM, "F");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = "F", expressStringVal = "F", binVal = "F" },
                enumVal = "F",
                logicalVal = "F",
                boolVal = false,
                stringVal = ".F.",
                expressStringVal = ".F.",
                binVal = "F",
                aggrLevel = 1
            });

            var typ = IfcWallType.Create(model);
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiINSTANCE, typ);
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { instVal = typ },
                instVal = typ,
                aggrLevel = 1
            });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiBINARY, "0AF");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            //TODO this case returns sdaiGetADBValue (sdaiBOOLEAN) but other binary variants does not
            //TODO engiGetElement(sdaiSTRING) returns with .. while sdaiGetADBValue does not
            CheckValues(wall, "Name", new PrimitiveValues
            {
                adbVal = new PrimitiveValues { boolVal = false, enumVal = "0AF", logicalVal = "0AF", stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF" },
                enumVal = "0AF",
                logicalVal = "0AF",
                stringVal = ".0AF.",
                expressStringVal = ".0AF.",
                binVal = "0AF",
                aggrLevel = 1
            });

            ifcengine.sdaiCloseModel(model);
        }

        static void TestAggregationAggregation(bool unicode)
        {
            ENTER_TEST(unicode ? "Unicode" : "Ascii");

            var model = CreateModel("", "IFC4", unicode);
            ASSERT(model != 0);

            var wall = IfcWall.Create(model);

            var entity = ifcengine.sdaiGetInstanceType(wall);
            ASSERT(entity != 0);

            var attr = ifcengine.sdaiGetAttrDefinition(entity, "Name");
            ASSERT(attr != 0);

            var aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiSTRING, "1234");
            var aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", aggrLevel = 2 });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiSTRING, "T");
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "T", expressStringVal = "T", aggrLevel = 2 });

            Int64 i = 1234;
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiINTEGER, ref i);
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234, aggrLevel = 2 });

            double d = 12.34;
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiREAL, ref d);
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34, aggrLevel = 2 });

            bool b = true;
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiBOOLEAN, ref b);
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T.", binVal = "T", aggrLevel = 2 });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiLOGICAL, "U");
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U.", binVal = "U", aggrLevel = 2 });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiENUM, "EEE");
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "EEE", logicalVal = "EEE", stringVal = ".EEE.", expressStringVal = ".EEE.", binVal = "EEE", aggrLevel = 2 });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiENUM, "F");
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F.", binVal = "F", aggrLevel = 2 });

            var typ = IfcWallType.Create(model);
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiINSTANCE, typ);
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            //TODO other data allows to get sdaiINSTANCE ad sdaiADB
            CheckValues(wall, "Name", new PrimitiveValues { instVal = typ, adbVal = new PrimitiveValues { instVal = typ }, aggrLevel = 2 });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiBINARY, "0AF");
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "0AF", logicalVal = "0AF", stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF", aggrLevel = 2 });

            ifcengine.sdaiCloseModel(model);
        }


        static void CheckValues
            (Int64 inst,
             string attrName,
             PrimitiveValues expected
            )
        {
            var entity = ifcengine.sdaiGetInstanceType(inst);
            ASSERT(entity != 0);

            var attr = ifcengine.sdaiGetAttrDefinition(entity, attrName);
            ASSERT(attr != 0);

            Int64 aggrVal = 1;
            var res = ifcengine.sdaiGetAttr(inst, attr, ifcengine.sdaiAGGR, out aggrVal);
            if (expected != null && expected.aggrLevel > 0)
            {
                ASSERT(res != 0);
                expected.aggrLevel--;
                CheckAggrValues(aggrVal, expected);
                expected = null; //>>>>>> all other get attempts should fail
            }
            else
            {
                ASSERT(res == 0 && aggrVal == 0);
            }

            Int64 adbVal = 1;
            res = ifcengine.sdaiGetAttr(inst, attr, ifcengine.sdaiADB, out adbVal);
            if (expected != null &&  expected.adbVal != null)
            {
                ASSERT(res != 0);
                CheckADBValues(adbVal, expected.adbVal);
            }
            else
            {
                ASSERT(res == 0 && adbVal == 0);
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

        static void CheckADBValues(Int64 adb, PrimitiveValues expected)
        {
            Int64 adbVal = 1;
            var res = ifcengine.sdaiGetADBValue(adb, ifcengine.sdaiADB, out adbVal);
            if (expected != null && expected.adbVal != null && expected.aggrLevel == 0)
            {
                ASSERT(res != 0);
                CheckADBValues(adbVal, expected.adbVal);
            }
            else
            {
                ASSERT(res == 0 && adbVal == 0);
            }

            Int64 aggrVal = 1;
            res = ifcengine.sdaiGetADBValue(adb, ifcengine.sdaiAGGR, out aggrVal);
            if (expected != null && expected.aggrLevel > 0)
            {
                ASSERT(res != 0);
                expected.aggrLevel--;
                CheckAggrValues(aggrVal, expected);
                expected = null; //>>>>>>>>>> all other attempts to get should return NULL
            }
            else
            {
                ASSERT(res == 0 && aggrVal == 0);
            }

            IntPtr ptrVal = IntPtr.MaxValue;
            res = ifcengine.sdaiGetADBValue(adb, ifcengine.sdaiBINARY, out ptrVal);
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
            res = ifcengine.sdaiGetADBValue(adb, ifcengine.sdaiBOOLEAN, out intVal);
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
            res = ifcengine.sdaiGetADBValue(adb, ifcengine.sdaiENUM, out ptrVal);
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
            res = ifcengine.sdaiGetADBValue(adb, ifcengine.sdaiINSTANCE, out intVal);
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
            res = ifcengine.sdaiGetADBValue(adb, ifcengine.sdaiINTEGER, out intVal);
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
            res = ifcengine.sdaiGetADBValue(adb, ifcengine.sdaiLOGICAL, out ptrVal);
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
            res = ifcengine.sdaiGetADBValue(adb, ifcengine.sdaiREAL, out realVal);
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
            res = ifcengine.sdaiGetADBValue(adb, ifcengine.sdaiSTRING, out ptrVal);
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
            res = ifcengine.sdaiGetADBValue(adb, ifcengine.sdaiUNICODE, out ptrVal);
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
            res = ifcengine.sdaiGetADBValue(adb, ifcengine.sdaiEXPRESSSTRING, out ptrVal);
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

        static void CheckAggrValues(Int64 aggr, PrimitiveValues expected)
        {
            var cnt = ifcengine.sdaiGetMemberCount(aggr);
            ASSERT(cnt == 1);

            Int64 adbVal = 1;
            var res = ifcengine.engiGetAggrElement (aggr, 0, ifcengine.sdaiADB, out adbVal);
            if (expected != null && expected.adbVal != null && expected.aggrLevel == 0)
            {
                ASSERT(res != 0);
                CheckADBValues(adbVal, expected.adbVal);
            }
            else
            {
                ASSERT(res == 0 && adbVal == 0);
            }

            Int64 aggrVal = 1;
            res = ifcengine.engiGetAggrElement(aggr, 0, ifcengine.sdaiAGGR, out aggrVal);
            if (expected != null && expected.aggrLevel > 0)
            {
                ASSERT(res != 0);
                expected.aggrLevel--;
                CheckAggrValues(aggrVal, expected);
                expected = null; //>>>>>>>>>> all other attempts to get should return NULL
            }
            else
            {
                ASSERT(res == 0 && aggrVal == 0);
            }

            IntPtr ptrVal = IntPtr.MaxValue;
            res = ifcengine.engiGetAggrElement(aggr, 0, ifcengine.sdaiBINARY, out ptrVal);
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
            res = ifcengine.engiGetAggrElement(aggr, 0, ifcengine.sdaiBOOLEAN, out intVal);
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
            res = ifcengine.engiGetAggrElement(aggr, 0, ifcengine.sdaiENUM, out ptrVal);
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
            res = ifcengine.engiGetAggrElement(aggr, 0, ifcengine.sdaiINSTANCE, out intVal);
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
            res = ifcengine.engiGetAggrElement(aggr, 0, ifcengine.sdaiINTEGER, out intVal);
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
            res = ifcengine.engiGetAggrElement(aggr, 0, ifcengine.sdaiLOGICAL, out ptrVal);
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
            res = ifcengine.engiGetAggrElement(aggr, 0, ifcengine.sdaiREAL, out realVal);
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
            res = ifcengine.engiGetAggrElement(aggr, 0, ifcengine.sdaiSTRING, out ptrVal);
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
            res = ifcengine.engiGetAggrElement(aggr, 0, ifcengine.sdaiUNICODE, out ptrVal);
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
            res = ifcengine.engiGetAggrElement(aggr, 0, ifcengine.sdaiEXPRESSSTRING, out ptrVal);
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

    }
}