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
        enum Nest
        {
            Aggregation, ADB
        };

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
                        
            public Stack<Nest> nests = new Stack<Nest>();
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
            //TODO - should sdaiGetAttr(sdaiENUM) return value for binary data?
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
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = false, enumVal = "F", logicalVal = "F", stringVal = ".F.", expressStringVal = ".F.", nests = Nests(Nest.ADB)});

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiLOGICAL, "U");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U." });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiENUM, "F");
            ifcengine.sdaiPutADBTypePath(adb, 1, "IFCLOGICAL");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);//adding typePath changes ADB to complex
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = false, enumVal = "F", logicalVal = "F", stringVal = ".F.", expressStringVal = ".F.", nests = Nests(Nest.ADB) });

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

        static Stack<Nest> Nests(params Nest[] nests)
        {
            var ret = new Stack<Nest>();
            foreach (var n in nests) {
                ret.Push(n);
            }
            return ret;
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
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", nests = Nests(Nest.Aggregation) });

            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiSTRING, "T");
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "T", expressStringVal = "T", nests = Nests(Nest.Aggregation) });

            Int64 i = 1234;
            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiINTEGER, ref i);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234, nests = Nests(Nest.Aggregation) });

            double d = 12.34;
            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiREAL, ref d);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34, nests = Nests(Nest.Aggregation) });

            bool b = true;
            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiBOOLEAN, ref b);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T.", nests = Nests(Nest.Aggregation) });

            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiLOGICAL, "U");
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U.", nests = Nests(Nest.Aggregation) });

            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiENUM, "EEE");
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "EEE", stringVal = ".EEE.", expressStringVal = ".EEE.", nests = Nests(Nest.Aggregation) });

            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiENUM, "F");
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F.", nests = Nests(Nest.Aggregation) });

            var typ = IfcWallType.Create(model);
            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiINSTANCE, typ);
            CheckValues(wall, "Name", new PrimitiveValues { instVal = typ, nests = Nests(Nest.Aggregation) });

            aggr = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiBINARY, "0AF");
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF", nests = Nests(Nest.Aggregation) });

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
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", nests = Nests(Nest.Aggregation) });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiSTRING, "T");
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutADBTypePath(adb, 1, "TypePath");  //put typePath makes complex
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "T", expressStringVal = "T", nests = Nests(Nest.Aggregation, Nest.ADB) });

            Int64 i = 1234;
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiINTEGER, ref i);
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutADBTypePath(adb, 1, "TypePath");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234, nests = Nests(Nest.Aggregation, Nest.ADB) });

            double d = 12.34;
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiREAL, ref d);
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutADBTypePath(adb, 1, "TypePath");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34, nests = Nests(Nest.Aggregation, Nest.ADB) });

            bool b = true;
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiBOOLEAN, ref b);
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutADBTypePath(adb, 1, "TypePath");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T.", nests = Nests(Nest.Aggregation, Nest.ADB) });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiLOGICAL, "U");
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutADBTypePath(adb, 1, "TypePath");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U.", nests = Nests(Nest.Aggregation, Nest.ADB) });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiENUM, "EEE");
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutADBTypePath(adb, 1, "TypePath");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "EEE", stringVal = ".EEE.", expressStringVal = ".EEE.", nests = Nests(Nest.Aggregation, Nest.ADB) });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiENUM, "F");
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutADBTypePath(adb, 1, "TypePath");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F.", nests = Nests(Nest.Aggregation, Nest.ADB) });

            var typ = IfcWallType.Create(model);
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiINSTANCE, typ);
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutADBTypePath(adb, 1, "TypePath");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { instVal = typ, nests = Nests(Nest.Aggregation, Nest.ADB) });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiBINARY, "0AF");
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiAGGR, aggr);
            ifcengine.sdaiPutADBTypePath(adb, 1, "TypePath");
            ifcengine.sdaiPutAttrBN(wall, "Name", ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF", nests = Nests(Nest.Aggregation, Nest.ADB) });

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
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", nests = Nests(Nest.ADB, Nest.Aggregation) });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiSTRING, "T");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "T", expressStringVal = "T", nests = Nests(Nest.ADB, Nest.Aggregation) });

            Int64 i = 1234;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiINTEGER, ref i);
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234, nests = Nests(Nest.ADB, Nest.Aggregation) });

            double d = 12.34;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiREAL, ref d);
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34, nests = Nests(Nest.ADB, Nest.Aggregation) });

            bool b = true;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiBOOLEAN, ref b);
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T.", binVal = "T", nests = Nests(Nest.ADB, Nest.Aggregation) });

            b = false;
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiBOOLEAN, ref b);
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = false, enumVal = "F", logicalVal = "F", stringVal = ".F.", expressStringVal = ".F.", binVal = "F", nests = Nests(Nest.ADB, Nest.Aggregation) });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiLOGICAL, "U");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U.", binVal = "U", nests = Nests(Nest.ADB, Nest.Aggregation) });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiENUM, "F");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = false, enumVal = "F", logicalVal = "F", stringVal = ".F.", expressStringVal = ".F.", binVal = "F", nests = Nests(Nest.ADB, Nest.Aggregation) });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiENUM, "EEE");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "EEE", logicalVal = "EEE", stringVal = ".EEE.", expressStringVal = ".EEE.", binVal = "EEE", nests = Nests(Nest.ADB, Nest.Aggregation) });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiENUM, "F");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F.", binVal = "F", nests = Nests(Nest.ADB, Nest.Aggregation) });

            var typ = IfcWallType.Create(model);
            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiINSTANCE, typ);
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { instVal = typ, nests = Nests(Nest.ADB, Nest.Aggregation) });

            adb = ifcengine.sdaiCreateADB(ifcengine.sdaiBINARY, "0AF");
            aggr = ifcengine.sdaiCreateAggrBN(wall, "Name");
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF", nests = Nests(Nest.ADB, Nest.Aggregation) });

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
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", nests = Nests(Nest.Aggregation, Nest.Aggregation) });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiSTRING, "T");
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "T", expressStringVal = "T", nests = Nests(Nest.Aggregation, Nest.Aggregation) });

            Int64 i = 1234;
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiINTEGER, ref i);
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234, nests = Nests(Nest.Aggregation, Nest.Aggregation) });

            double d = 12.34;
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiREAL, ref d);
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34, nests = Nests(Nest.Aggregation, Nest.Aggregation) });

            bool b = true;
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiBOOLEAN, ref b);
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T.", binVal = "T", nests = Nests(Nest.Aggregation, Nest.Aggregation) });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiLOGICAL, "U");
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U.", binVal = "U", nests = Nests(Nest.Aggregation, Nest.Aggregation) });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiENUM, "EEE");
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "EEE", logicalVal = "EEE", stringVal = ".EEE.", expressStringVal = ".EEE.", binVal = "EEE", nests = Nests(Nest.Aggregation, Nest.Aggregation) });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiENUM, "F");
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F.", binVal = "F", nests = Nests(Nest.Aggregation, Nest.Aggregation) });

            var typ = IfcWallType.Create(model);
            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiINSTANCE, typ);
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { instVal = typ, nests = Nests(Nest.Aggregation, Nest.Aggregation) });

            aggr = ifcengine.sdaiCreateAggr(wall, 0);
            ifcengine.sdaiAppend(aggr, ifcengine.sdaiBINARY, "0AF");
            aggr2 = ifcengine.sdaiCreateAggr(wall, attr);
            ifcengine.sdaiAppend(aggr2, ifcengine.sdaiAGGR, aggr);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "0AF", logicalVal = "0AF", stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF", nests = Nests(Nest.Aggregation, Nest.Aggregation) });

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

            Int64 adbVal = 1;
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
            if (expected != null && expected.nests.Count>0 && expected.nests.First()==Nest.Aggregation)
            {
                ASSERT(res != 0);
                expected.nests.Pop();
                CheckAggrValues(aggrVal, expected);
                expected.nests.Push(Nest.Aggregation);
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
            Int64 adbVal = 1;
            var res = ifcengine.sdaiGetADBValue(adb, ifcengine.sdaiADB, out adbVal);
            if (expected != null && expected.nests.Count > 0 && expected.nests.First() == Nest.ADB)
            {
                ASSERT(res != 0);
                expected.nests.Pop();
                CheckADBValues(adbVal, expected);
                //do not push
            }
            else
            {
                ASSERT(res == 0 && adbVal == 0);
            }

            Int64 aggrVal = 1;
            res = ifcengine.sdaiGetADBValue(adb, ifcengine.sdaiAGGR, out aggrVal);
            if (expected != null && expected.nests.Count > 0 && expected.nests.First()==Nest.Aggregation)
            {
                ASSERT(res != 0);
                expected.nests.Pop();
                CheckAggrValues(aggrVal, expected);
                expected.nests.Push(Nest.Aggregation);
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

            Int64 adbVal = 1;
            var res = ifcengine.engiGetAggrElement(aggr, 0, ifcengine.sdaiADB, out adbVal);
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
            res = ifcengine.engiGetAggrElement(aggr, 0, ifcengine.sdaiAGGR, out aggrVal);
            if (expected != null && expected.nests.Count > 0 && expected.nests.First()==Nest.Aggregation)
            {
                ASSERT(res != 0);
                expected.nests.Pop();
                CheckAggrValues(aggrVal, expected);
                expected.nests.Push(Nest.Aggregation);
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