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

            public int aggrLevel;
            public bool complexArg;
            public bool complexArgAggregated;
        };


        public static void Test ()
        {
            TestPrimitiveValue();
            TestADBPrimitive();
            TestAggregationPrimitive();

            TestADBAggregation();
            TestAggregationADB();

            TestAggregationAggregation();
        }


        static void TestPrimitiveValue()
        {
            ENTER_TEST();

            var model = ifcengine.sdaiCreateModelBN(0,"", "IFC4");
            ASSERT(model != 0);

            var wall = IfcWall.Create(model);

            CheckValues(wall, "Name", null);

            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiSTRING, "1234");
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234" });

            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiSTRING, "T");
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "T", expressStringVal = "T" });

            Int64 i = 1234;
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiINTEGER, ref i);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234 });

            double d = 12.34;
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiREAL, ref d);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34 });

            bool b = true;
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiBOOLEAN, ref b);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T." });

            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiLOGICAL, "U");
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U." });

            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiENUM, "EEE"); 
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "EEE", stringVal = ".EEE.", expressStringVal = ".EEE." });

            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiENUM, "F"); 
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F." });

            var typ = IfcWallType.Create(model);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiINSTANCE, typ);
            CheckValues(wall, "Name", new PrimitiveValues { instVal = typ });

            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiBINARY, "0AF");
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF" });

            ifcengine.sdaiCloseModel(model);

        }

        static void TestADBPrimitive()
        {
            ENTER_TEST();

            var model = ifcengine.sdaiCreateModelBN(0,"", "IFC4");
            ASSERT(model != 0);

            var wall = IfcWall.Create(model);

            var adb = ifcengine.sdaiCreateADB(ifcengine.sdaiSTRING, "1234kll");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234kll", expressStringVal = "1234kll" });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiSTRING, "T");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name",new PrimitiveValues { stringVal = "T", expressStringVal = "T" });

            Int64 i = 1234;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiINTEGER, ref i);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234 });

            double d = 12.34;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiREAL, ref d);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34 });

            bool b = true;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiBOOLEAN, ref b);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T."});

            b = false;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiBOOLEAN, ref b);
            ifcengine.sdaiPutADBTypePath(adb, 1, "IFCBOOLEAN"); //adding typePath changes ADB to complex
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = false, enumVal = "F", logicalVal = "F", stringVal = ".F.", expressStringVal = ".F.", complexArg = true });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiLOGICAL, "U");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U." });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiENUM, "F");
            ifcengine.sdaiPutADBTypePath(adb, 1, "IFCLOGICAL");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);//adding typePath changes ADB to complex
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = false, enumVal = "F", logicalVal = "F", stringVal = ".F.", expressStringVal = ".F.", complexArg = true });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiENUM, "EEE");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "EEE", stringVal = ".EEE.", expressStringVal = ".EEE." });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiENUM, "F");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F." });

            var typ = IfcWallType.Create(model);
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiINSTANCE, typ);
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { instVal = typ });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiBINARY, "0AF");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF" });

            ifcengine.sdaiCloseModel(model);
        }

      
        static void TestAggregationPrimitive()
        {
            ENTER_TEST();

            var model = ifcengine.sdaiCreateModelBN(0,"", "IFC4");
            ASSERT(model != 0);

            var wall = IfcWall.Create(model);

            var entity = ifcengine.sdaiGetInstanceType(wall);
            ASSERT(entity != 0);

            var attr = ifcengine.sdaiGetAttrDefinition(entity, "Name");
            ASSERT(attr != 0);

            var aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiSTRING, "1234");
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234",  aggrLevel = 1 });

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
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T.", aggrLevel = 1 });

            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiLOGICAL, "U");
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U.", aggrLevel = 1 });

            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiENUM, "EEE");
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "EEE", stringVal = ".EEE.", expressStringVal = ".EEE.", aggrLevel = 1 });

            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiENUM, "F");
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F.", aggrLevel = 1 });

            var typ = IfcWallType.Create(model);
            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiINSTANCE, typ);
            CheckValues(wall, "Name", new PrimitiveValues { instVal = typ, aggrLevel = 1 });

            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiBINARY, "0AF");
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF", aggrLevel = 1 });

            ifcengine.sdaiCloseModel(model);
        }

        static void TestADBAggregation()
        {
            ENTER_TEST();

            var model = ifcengine.sdaiCreateModelBN(0,"", "IFC4");
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
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", aggrLevel = 1 });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiSTRING, "T");
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutADBTypePath(adb, 1, "TypePath");  //put typePath makes complex
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "T", expressStringVal = "T", aggrLevel=1, complexArg=true });

            Int64 i = 1234;
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiINTEGER, ref i);
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutADBTypePath(adb, 1, "TypePath");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234, aggrLevel=1, complexArg=true });

            double d = 12.34;
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiREAL, ref d);
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutADBTypePath(adb, 1, "TypePath");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34, aggrLevel=1, complexArg=true });

            bool b = true;
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiBOOLEAN, ref b);
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutADBTypePath(adb, 1, "TypePath");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T.", aggrLevel=1, complexArg=true });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiLOGICAL, "U");
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutADBTypePath(adb, 1, "TypePath");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U.", aggrLevel=1, complexArg=true });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiENUM, "EEE");
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutADBTypePath(adb, 1, "TypePath");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "EEE", stringVal = ".EEE.", expressStringVal = ".EEE.", aggrLevel=1, complexArg=true });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiENUM, "F");
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutADBTypePath(adb, 1, "TypePath");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F.", aggrLevel=1, complexArg=true });

            var typ = IfcWallType.Create(model);
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiINSTANCE, typ);
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutADBTypePath(adb, 1, "TypePath");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { instVal = typ, aggrLevel=1, complexArg=true });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiBINARY, "0AF");
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutADBTypePath(adb, 1, "TypePath");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF", aggrLevel=1, complexArg=true });

            ifcengine.sdaiCloseModel(model);
        }

        static void TestAggregationADB()
        {
            ENTER_TEST();

            var model = ifcengine.sdaiCreateModelBN(0,"", "IFC4");
            ASSERT(model != 0);

            var wall = IfcWall.Create(model);

            var adb = ifcengine.sdaiCreateADB(ifcengine.sdaiSTRING, "1234");
            ifcengine.sdaiPutADBTypePath(adb, 1, "testPath");
            var aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", aggrLevel=1, complexArgAggregated=true });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiSTRING, "T");
            ifcengine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "T", expressStringVal = "T", aggrLevel=1, complexArgAggregated=true });

            Int64 i = 1234;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiINTEGER, ref i);
            ifcengine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234, aggrLevel=1, complexArgAggregated=true });

            double d = 12.34;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiREAL, ref d);
            ifcengine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34, aggrLevel=1, complexArgAggregated=true });

            bool b = true;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiBOOLEAN, ref b);
            ifcengine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T.", aggrLevel=1, complexArgAggregated=true });

            b = false;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiBOOLEAN, ref b);
            ifcengine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = false, enumVal = "F", logicalVal = "F", stringVal = ".F.", expressStringVal = ".F.", aggrLevel=1, complexArgAggregated=true });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiLOGICAL, "U");
            ifcengine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U.", aggrLevel=1, complexArgAggregated=true });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiENUM, "F");
            ifcengine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = false, enumVal = "F", logicalVal = "F", stringVal = ".F.", expressStringVal = ".F.", aggrLevel=1, complexArgAggregated=true });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiENUM, "EEE");
            ifcengine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "EEE", stringVal = ".EEE.", expressStringVal = ".EEE.", aggrLevel=1, complexArgAggregated=true });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiENUM, "F");
            ifcengine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F.", aggrLevel=1, complexArgAggregated=true });

            var typ = IfcWallType.Create(model);
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiINSTANCE, typ);
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { instVal = typ, aggrLevel=1 });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiBINARY, "0AF");
            ifcengine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF", aggrLevel=1, complexArgAggregated=true });

            ifcengine.sdaiCloseModel(model);
        }

        static void TestAggregationAggregation()
        {
            ENTER_TEST();

            var model = ifcengine.sdaiCreateModelBN(0, "", "IFC4");
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
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", aggrLevel=2 });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiSTRING, "T");
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "T", expressStringVal = "T", aggrLevel=2 });

            Int64 i = 1234;
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiINTEGER, ref i);
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234, aggrLevel=2 });

            double d = 12.34;
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiREAL, ref d);
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34, aggrLevel=2 });

            bool b = true;
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiBOOLEAN, ref b);
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T.", aggrLevel=2 });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiLOGICAL, "U");
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U.", aggrLevel=2 });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiENUM, "EEE");
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "EEE", stringVal = ".EEE.", expressStringVal = ".EEE.", aggrLevel=2 });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiENUM, "F");
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F.", aggrLevel=2 });

            var typ = IfcWallType.Create(model);
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiINSTANCE, typ);
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { instVal = typ, aggrLevel=2 });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiBINARY, "0AF");
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF", aggrLevel=2 });

            ifcengine.sdaiCloseModel(model);
        }

        static void CheckValues
            (Int64 inst,
             string attrName,
             PrimitiveValues expected
            )
        {
            CheckValuesInModel(inst, attrName, expected);

            var fileName = "PutGetAttrTest.ifc";
            var stepId = ifcengine.internalGetP21Line(inst);

            var entity = ifcengine.sdaiGetInstanceType(inst);
            var model = ifcengine.engiGetEntityModel(entity);

            ifcengine.SetSPFFHeaderItem(model, 9, 0, ifcengine.sdaiSTRING, "IFC4");
            ifcengine.SetSPFFHeaderItem(model, 9, 1, ifcengine.sdaiSTRING, (string)null);

            ifcengine.sdaiSaveModelBN(model, fileName);

            var model2 = ifcengine.sdaiOpenModelBN(0, fileName, "IFC4");
            ASSERT(model2 != 0);

            var inst2 = ifcengine.internalGetInstanceFromP21Line(model2, stepId);
            ASSERT(inst2 != 0);

            if (expected != null && expected.instVal != null)
            {
                var stepIdRef = ifcengine.internalGetP21Line(expected.instVal.Value);
                expected.instVal = ifcengine.internalGetInstanceFromP21Line(model2, stepIdRef);
                ASSERT(expected.instVal.Value != 0);
            }

            CheckValuesInModel(inst2, attrName, expected);

            ifcengine.sdaiCloseModel(model2);
        }

        static void CheckValuesInModel
            (Int64 inst,
             string attrName,
             PrimitiveValues expected
            )
        {
            var entity = ifcengine.sdaiGetInstanceType(inst);
            ASSERT(entity != 0);

            var attr = ifcengine.sdaiGetAttrDefinition(entity, attrName);
            ASSERT(attr != 0);

            Int64 adbVal;
            var res = ifcengine.sdaiGetAttr(inst, attr, ifcengine.sdaiADB, out adbVal);
            if (expected != null)
            {
                ASSERT(res != 0);
                CheckADBValues(adbVal, expected);
            }
            else
            {
                ASSERT(res == 0 && adbVal == 0);
            }

            Int64 aggrVal = 1;
            res = ifcengine.sdaiGetAttr(inst, attr, ifcengine.sdaiAGGR, out aggrVal);
            if (expected != null && expected.aggrLevel > 0)
            {
                ASSERT(res != 0);
                expected.aggrLevel--;
                CheckAggrValues(aggrVal, expected);
                expected.aggrLevel++;
                expected = null; //>>>>>> all other get attempts should fail
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

        static void CheckADBValues(Int64 adb, PrimitiveValues expected)
        {
            Int64 adbVal;
            var res = ifcengine.sdaiGetADBValue(adb, ifcengine.sdaiADB, out adbVal);
            if (expected != null && expected.complexArg)
            {
                ASSERT(res != 0);
                expected.complexArg = false;
                CheckADBValues(adbVal, expected);
                expected.complexArg = true;
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
                expected.aggrLevel++;
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

        static void CheckAggrValues
            (Int64 aggr,
             PrimitiveValues expected
            )
        {
            var cnt = ifcengine.sdaiGetMemberCount(aggr);
            ASSERT(cnt == 1);

            Int64 adbVal;
            var res = ifcengine.engiGetAggrElement(aggr, 0, ifcengine.sdaiADB, out adbVal);
            if (expected != null)
            {
                ASSERT(res != 0);

                bool save = expected.complexArg;
                expected.complexArg = false;
                if (expected.complexArgAggregated && expected.aggrLevel == 0)
                    expected.complexArg = true;
                else
                    expected.complexArg = false;

                CheckADBValues(adbVal, expected);

                expected.complexArg = save;
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
                expected.aggrLevel++;
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