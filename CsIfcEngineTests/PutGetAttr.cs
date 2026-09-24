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
        const string REGIONAL_CHARS_TXT = "ABC ÜüÖöÄäẞß ЩяЪ 好";
        const string REGIONAL_CHARS_EXPR = 
            "ABC \\X\\DC\\X\\FC\\X\\D6\\X\\F6\\X\\C4\\X\\E4\\X2\\1E9E\\X0\\\\X\\DF \\X2\\0429\\X0\\\\X2\\044F\\X0\\\\X2\\042A\\X0\\ \\X2\\597D\\X0\\";

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


        public static void Run ()
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

            var model = IFCEngine.sdaiCreateModelBN(0,"", "IFC4");
            ASSERT(model != 0);

            var wall = IfcWall.Create(model);

            CheckValues(wall, "Name", null);

            var text = "ANSI text";
            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiSTRING, text);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = text, expressStringVal = text });

            text = REGIONAL_CHARS_TXT;
            var bytes = Encoding.Unicode.GetBytes(text);
            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiUNICODE, bytes);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = text, expressStringVal = REGIONAL_CHARS_EXPR });

            Int64 i = 1234;
            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiINTEGER, ref i);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234 });

            double d = 12.34;
            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiREAL, ref d);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34 });

            bool b = true;
            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiBOOLEAN, ref b);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T." });

            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiLOGICAL, "U");
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U." });

            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiENUM, "EEE"); 
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "EEE", stringVal = ".EEE.", expressStringVal = ".EEE." });

            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiENUM, "F"); 
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F." });

            var typ = IfcWallType.Create(model);
            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiINSTANCE, typ);
            CheckValues(wall, "Name", new PrimitiveValues { instVal = typ });

            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiBINARY, "0AF");
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF" });

            IFCEngine.sdaiCloseModel(model);

        }

        static void TestADBPrimitive()
        {
            ENTER_TEST();

            var model = IFCEngine.sdaiCreateModelBN(0,"", "IFC4");
            ASSERT(model != 0);

            var wall = IfcWall.Create(model);

            var adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiSTRING, "1234kll");
            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234kll", expressStringVal = "1234kll" });

            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiSTRING, "T");
            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiADB, adb);
            CheckValues(wall, "Name",new PrimitiveValues { stringVal = "T", expressStringVal = "T" });

            Int64 i = 1234;
            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiINTEGER, ref i);
            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234 });

            double d = 12.34;
            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiREAL, ref d);
            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34 });

            bool b = true;
            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiBOOLEAN, ref b);
            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T."});

            b = false;
            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiBOOLEAN, ref b);
            IFCEngine.sdaiPutADBTypePath(adb, 1, "IFCBOOLEAN"); //adding typePath changes ADB to complex
            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = false, enumVal = "F", logicalVal = "F", stringVal = ".F.", expressStringVal = ".F.", complexArg = true });

            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiLOGICAL, "U");
            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U." });

            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiENUM, "F");
            IFCEngine.sdaiPutADBTypePath(adb, 1, "IFCLOGICAL");
            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiADB, adb);//adding typePath changes ADB to complex
            CheckValues(wall, "Name", new PrimitiveValues { boolVal = false, enumVal = "F", logicalVal = "F", stringVal = ".F.", expressStringVal = ".F.", complexArg = true });

            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiENUM, "EEE");
            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "EEE", stringVal = ".EEE.", expressStringVal = ".EEE." });

            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiENUM, "F");
            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F." });

            var typ = IfcWallType.Create(model);
            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiINSTANCE, typ);
            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { instVal = typ });

            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiBINARY, "0AF");
            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiADB, adb);
            CheckValues(wall, "Name", new PrimitiveValues { stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF" });

            //empty ADB
            adb = IFCEngine.sdaiCreateEmptyADB();
            CheckADBValues(adb, null);
            IFCEngine.sdaiPutAttrBN(wall, "Name", IFCEngine.sdaiADB, adb);
            CheckValues(wall, "Name", null);

            IFCEngine.sdaiCloseModel(model);
        }

      
        static void TestAggregationPrimitive()
        {
            ENTER_TEST();

            var model = IFCEngine.sdaiCreateModelBN(0,"", "IFC4");
            ASSERT(model != 0);

            var person = IfcPerson.Create(model);

            var entity = IFCEngine.sdaiGetInstanceType(person);
            ASSERT(entity != 0);

            var attr = IFCEngine.sdaiGetAttrDefinition(entity, "MiddleNames");
            ASSERT(attr != 0);

            var aggr = IFCEngine.sdaiCreateAggr(person, attr);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiSTRING, "1234");
            CheckValues(person, "MiddleNames", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234",  aggrLevel = 1 });

            aggr = IFCEngine.sdaiCreateAggr(person, attr);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiSTRING, "T");
            CheckValues(person, "MiddleNames", new PrimitiveValues { stringVal = "T", expressStringVal = "T", aggrLevel = 1 });

            Int64 i = 1234;
            aggr = IFCEngine.sdaiCreateAggr(person, attr);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiINTEGER, ref i);
            CheckValues(person, "MiddleNames", new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234, aggrLevel = 1 });

            double d = 12.34;
            aggr = IFCEngine.sdaiCreateAggr(person, attr);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiREAL, ref d);
            CheckValues(person, "MiddleNames", new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34, aggrLevel = 1 });

            bool b = true;
            aggr = IFCEngine.sdaiCreateAggr(person, attr);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiBOOLEAN, ref b);
            CheckValues(person, "MiddleNames", new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T.", aggrLevel = 1 });

            aggr = IFCEngine.sdaiCreateAggr(person, attr);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiLOGICAL, "U");
            CheckValues(person, "MiddleNames", new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U.", aggrLevel = 1 });

            aggr = IFCEngine.sdaiCreateAggr(person, attr);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiENUM, "EEE");
            CheckValues(person, "MiddleNames", new PrimitiveValues { enumVal = "EEE", stringVal = ".EEE.", expressStringVal = ".EEE.", aggrLevel = 1 });

            aggr = IFCEngine.sdaiCreateAggr(person, attr);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiENUM, "F");
            CheckValues(person, "MiddleNames", new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F.", aggrLevel = 1 });

            var typ = IfcWallType.Create(model);
            aggr = IFCEngine.sdaiCreateAggr(person, attr);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiINSTANCE, typ);
            CheckValues(person, "MiddleNames", new PrimitiveValues { instVal = typ, aggrLevel = 1 });

            aggr = IFCEngine.sdaiCreateAggr(person, attr);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiBINARY, "0AF");
            CheckValues(person, "MiddleNames", new PrimitiveValues { stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF", aggrLevel = 1 });

            IFCEngine.sdaiCloseModel(model);
        }

        static void TestADBAggregation()
        {
            ENTER_TEST();

            var model = IFCEngine.sdaiCreateModelBN(0,"", "IFC4");
            ASSERT(model != 0);

            var person = IfcPerson.Create(model);

            var entity = IFCEngine.sdaiGetInstanceType(person);
            ASSERT(entity != 0);

            const string attrName = "MiddleNames";

            var attr = IFCEngine.sdaiGetAttrDefinition(entity, attrName);
            ASSERT(attr != 0);

            var aggr = IFCEngine.sdaiCreateAggr(person, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiSTRING, "1234");
            var adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiAGGR, aggr);
            IFCEngine.sdaiPutAttrBN(person, attrName, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", aggrLevel = 1 });

            aggr = IFCEngine.sdaiCreateAggr(person, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiSTRING, "T");
            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiAGGR, aggr);
            IFCEngine.sdaiPutADBTypePath(adb, 1, "TypePath");  //put typePath makes complex
            IFCEngine.sdaiPutAttrBN(person, attrName, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { stringVal = "T", expressStringVal = "T", aggrLevel=1, complexArg=true });

            Int64 i = 1234;
            aggr = IFCEngine.sdaiCreateAggr(person, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiINTEGER, ref i);
            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiAGGR, aggr);
            IFCEngine.sdaiPutADBTypePath(adb, 1, "TypePath");
            IFCEngine.sdaiPutAttrBN(person, attrName, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234, aggrLevel=1, complexArg=true });

            double d = 12.34;
            aggr = IFCEngine.sdaiCreateAggr(person, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiREAL, ref d);
            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiAGGR, aggr);
            IFCEngine.sdaiPutADBTypePath(adb, 1, "TypePath");
            IFCEngine.sdaiPutAttrBN(person, attrName, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34, aggrLevel=1, complexArg=true });

            bool b = true;
            aggr = IFCEngine.sdaiCreateAggr(person, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiBOOLEAN, ref b);
            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiAGGR, aggr);
            IFCEngine.sdaiPutADBTypePath(adb, 1, "TypePath");
            IFCEngine.sdaiPutAttrBN(person, attrName, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T.", aggrLevel=1, complexArg=true });

            aggr = IFCEngine.sdaiCreateAggr(person, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiLOGICAL, "U");
            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiAGGR, aggr);
            IFCEngine.sdaiPutADBTypePath(adb, 1, "TypePath");
            IFCEngine.sdaiPutAttrBN(person, attrName, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U.", aggrLevel=1, complexArg=true });

            aggr = IFCEngine.sdaiCreateAggr(person, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiENUM, "EEE");
            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiAGGR, aggr);
            IFCEngine.sdaiPutADBTypePath(adb, 1, "TypePath");
            IFCEngine.sdaiPutAttrBN(person, attrName, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { enumVal = "EEE", stringVal = ".EEE.", expressStringVal = ".EEE.", aggrLevel=1, complexArg=true });

            aggr = IFCEngine.sdaiCreateAggr(person, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiENUM, "F");
            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiAGGR, aggr);
            IFCEngine.sdaiPutADBTypePath(adb, 1, "TypePath");
            IFCEngine.sdaiPutAttrBN(person, attrName, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F.", aggrLevel=1, complexArg=true });

            var typ = IfcWallType.Create(model);
            aggr = IFCEngine.sdaiCreateAggr(person, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiINSTANCE, typ);
            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiAGGR, aggr);
            IFCEngine.sdaiPutADBTypePath(adb, 1, "TypePath");
            IFCEngine.sdaiPutAttrBN(person, attrName, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { instVal = typ, aggrLevel=1, complexArg=true });

            aggr = IFCEngine.sdaiCreateAggr(person, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiBINARY, "0AF");
            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiAGGR, aggr);
            IFCEngine.sdaiPutADBTypePath(adb, 1, "TypePath");
            IFCEngine.sdaiPutAttrBN(person, attrName, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF", aggrLevel=1, complexArg=true });

            IFCEngine.sdaiCloseModel(model);
        }

        static void TestAggregationADB()
        {
            ENTER_TEST();

            var model = IFCEngine.sdaiCreateModelBN(0,"", "IFC4");
            ASSERT(model != 0);

            var person = IfcPerson.Create(model);

            const string attrName = "MiddleNames";

            var adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiSTRING, "1234");
            IFCEngine.sdaiPutADBTypePath(adb, 1, "testPath");
            var aggr = IFCEngine.sdaiCreateAggrBN(person, attrName);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", aggrLevel=1, complexArgAggregated=true });

            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiSTRING, "T");
            IFCEngine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = IFCEngine.sdaiCreateAggrBN(person, attrName);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { stringVal = "T", expressStringVal = "T", aggrLevel=1, complexArgAggregated=true });

            Int64 i = 1234;
            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiINTEGER, ref i);
            IFCEngine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = IFCEngine.sdaiCreateAggrBN(person, attrName);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234, aggrLevel=1, complexArgAggregated=true });

            double d = 12.34;
            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiREAL, ref d);
            IFCEngine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = IFCEngine.sdaiCreateAggrBN(person, attrName);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34, aggrLevel=1, complexArgAggregated=true });

            bool b = true;
            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiBOOLEAN, ref b);
            IFCEngine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = IFCEngine.sdaiCreateAggrBN(person, attrName);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T.", aggrLevel=1, complexArgAggregated=true });

            b = false;
            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiBOOLEAN, ref b);
            IFCEngine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = IFCEngine.sdaiCreateAggrBN(person, attrName);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { boolVal = false, enumVal = "F", logicalVal = "F", stringVal = ".F.", expressStringVal = ".F.", aggrLevel=1, complexArgAggregated=true });

            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiLOGICAL, "U");
            IFCEngine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = IFCEngine.sdaiCreateAggrBN(person, attrName);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U.", aggrLevel=1, complexArgAggregated=true });

            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiENUM, "F");
            IFCEngine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = IFCEngine.sdaiCreateAggrBN(person, attrName);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { boolVal = false, enumVal = "F", logicalVal = "F", stringVal = ".F.", expressStringVal = ".F.", aggrLevel=1, complexArgAggregated=true });

            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiENUM, "EEE");
            IFCEngine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = IFCEngine.sdaiCreateAggrBN(person, attrName);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { enumVal = "EEE", stringVal = ".EEE.", expressStringVal = ".EEE.", aggrLevel=1, complexArgAggregated=true });

            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiENUM, "F");
            IFCEngine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = IFCEngine.sdaiCreateAggrBN(person, attrName);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F.", aggrLevel=1, complexArgAggregated=true });

            var typ = IfcWallType.Create(model);
            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiINSTANCE, typ);
            aggr = IFCEngine.sdaiCreateAggrBN(person, attrName);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { instVal = typ, aggrLevel=1 });

            adb = IFCEngine.sdaiCreateADB(IFCEngine.sdaiBINARY, "0AF");
            IFCEngine.sdaiPutADBTypePath(adb, 1, "testPath");
            aggr = IFCEngine.sdaiCreateAggrBN(person, attrName);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiADB, adb);
            CheckValues(person, attrName, new PrimitiveValues { stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF", aggrLevel=1, complexArgAggregated=true });

            //empty ADB
            /* creates empty aggregation
            adb = IFCEngine.sdaiCreateEmptyADB();
            aggr = IFCEngine.sdaiCreateAggrBN(wall, attrName);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiADB, adb);
            CheckValues(wall, attrName, new PrimitiveValues { aggrLevel = 1 });
            */

            IFCEngine.sdaiCloseModel(model);
        }

        static void TestAggregationAggregation()
        {
            ENTER_TEST();

            var model = IFCEngine.sdaiCreateModelBN(0, "", "IFC4");
            ASSERT(model != 0);

            var inst = IfcCartesianPointList2D.Create(model);

            var entity = IFCEngine.sdaiGetInstanceType(inst);
            ASSERT(entity != 0);

            const string attrName = "CoordList";

            var attr = IFCEngine.sdaiGetAttrDefinition(entity, attrName);
            ASSERT(attr != 0);

            var aggr = IFCEngine.sdaiCreateAggr(inst, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiSTRING, "1234");
            var aggr2 = IFCEngine.sdaiCreateAggr(inst, attr);
            IFCEngine.sdaiAppend(aggr2, IFCEngine.sdaiAGGR, aggr);
            CheckValues(inst, attrName, new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", aggrLevel=2 });

            aggr = IFCEngine.sdaiCreateAggr(inst, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiSTRING, "T");
            aggr2 = IFCEngine.sdaiCreateAggr(inst, attr);
            IFCEngine.sdaiAppend(aggr2, IFCEngine.sdaiAGGR, aggr);
            CheckValues(inst, attrName, new PrimitiveValues { stringVal = "T", expressStringVal = "T", aggrLevel=2 });

            Int64 i = 1234;
            aggr = IFCEngine.sdaiCreateAggr(inst, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiINTEGER, ref i);
            aggr2 = IFCEngine.sdaiCreateAggr(inst, attr);
            IFCEngine.sdaiAppend(aggr2, IFCEngine.sdaiAGGR, aggr);
            CheckValues(inst, attrName, new PrimitiveValues { stringVal = "1234", expressStringVal = "1234", intVal = 1234, realVal = 1234, aggrLevel=2 });

            double d = 12.34;
            aggr = IFCEngine.sdaiCreateAggr(inst, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiREAL, ref d);
            aggr2 = IFCEngine.sdaiCreateAggr(inst, attr);
            IFCEngine.sdaiAppend(aggr2, IFCEngine.sdaiAGGR, aggr);
            CheckValues(inst, attrName, new PrimitiveValues { stringVal = "12.340000", expressStringVal = "12.340000", intVal = 12, realVal = 12.34, aggrLevel=2 });

            bool b = true;
            aggr = IFCEngine.sdaiCreateAggr(inst, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiBOOLEAN, ref b);
            aggr2 = IFCEngine.sdaiCreateAggr(inst, attr);
            IFCEngine.sdaiAppend(aggr2, IFCEngine.sdaiAGGR, aggr);
            CheckValues(inst, attrName, new PrimitiveValues { boolVal = true, enumVal = "T", logicalVal = "T", stringVal = ".T.", expressStringVal = ".T.", aggrLevel=2 });

            aggr = IFCEngine.sdaiCreateAggr(inst, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiLOGICAL, "U");
            aggr2 = IFCEngine.sdaiCreateAggr(inst, attr);
            IFCEngine.sdaiAppend(aggr2, IFCEngine.sdaiAGGR, aggr);
            CheckValues(inst, attrName, new PrimitiveValues { enumVal = "U", logicalVal = "U", stringVal = ".U.", expressStringVal = ".U.", aggrLevel=2 });

            aggr = IFCEngine.sdaiCreateAggr(inst, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiENUM, "EEE");
            aggr2 = IFCEngine.sdaiCreateAggr(inst, attr);
            IFCEngine.sdaiAppend(aggr2, IFCEngine.sdaiAGGR, aggr);
            CheckValues(inst, attrName, new PrimitiveValues { enumVal = "EEE", stringVal = ".EEE.", expressStringVal = ".EEE.", aggrLevel=2 });

            aggr = IFCEngine.sdaiCreateAggr(inst, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiENUM, "F");
            aggr2 = IFCEngine.sdaiCreateAggr(inst, attr);
            IFCEngine.sdaiAppend(aggr2, IFCEngine.sdaiAGGR, aggr);
            CheckValues(inst, attrName, new PrimitiveValues { enumVal = "F", logicalVal = "F", boolVal = false, stringVal = ".F.", expressStringVal = ".F.", aggrLevel=2 });

            var typ = IfcWallType.Create(model);
            aggr = IFCEngine.sdaiCreateAggr(inst, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiINSTANCE, typ);
            aggr2 = IFCEngine.sdaiCreateAggr(inst, attr);
            IFCEngine.sdaiAppend(aggr2, IFCEngine.sdaiAGGR, aggr);
            CheckValues(inst, attrName, new PrimitiveValues { instVal = typ, aggrLevel=2 });

            aggr = IFCEngine.sdaiCreateAggr(inst, 0);
            IFCEngine.sdaiAppend(aggr, IFCEngine.sdaiBINARY, "0AF");
            aggr2 = IFCEngine.sdaiCreateAggr(inst, attr);
            IFCEngine.sdaiAppend(aggr2, IFCEngine.sdaiAGGR, aggr);
            CheckValues(inst, attrName, new PrimitiveValues { stringVal = "0AF", expressStringVal = "0AF", binVal = "0AF", aggrLevel=2 });

            IFCEngine.sdaiCloseModel(model);
        }

        static void CheckValues
            (Int64 inst,
             string attrName,
             PrimitiveValues expected
            )
        {
            CheckValuesInModel(inst, attrName, expected);

            var fileName = "PutGetAttrTest.ifc";
            var stepId = IFCEngine.internalGetP21Line(inst);

            var entity = IFCEngine.sdaiGetInstanceType(inst);
            var model = IFCEngine.engiGetEntityModel(entity);

            IFCEngine.SetSPFFHeaderItem(model, 9, 0, IFCEngine.sdaiSTRING, "IFC4");
            IFCEngine.SetSPFFHeaderItem(model, 9, 1, IFCEngine.sdaiSTRING, (string)null);

            IFCEngine.sdaiSaveModelBN(model, fileName);

            var model2 = IFCEngine.sdaiOpenModelBN(0, fileName, "IFC4");
            ASSERT(model2 != 0);

            var inst2 = IFCEngine.internalGetInstanceFromP21Line(model2, stepId);
            ASSERT(inst2 != 0);

            if (expected != null && expected.instVal != null)
            {
                var stepIdRef = IFCEngine.internalGetP21Line(expected.instVal.Value);
                expected.instVal = IFCEngine.internalGetInstanceFromP21Line(model2, stepIdRef);
                ASSERT(expected.instVal.Value != 0);
            }

            CheckValuesInModel(inst2, attrName, expected);

            IFCEngine.sdaiCloseModel(model2);
        }

        static void CheckValuesInModel
            (Int64 inst,
             string attrName,
             PrimitiveValues expected
            )
        {
            var entity = IFCEngine.sdaiGetInstanceType(inst);
            ASSERT(entity != 0);

            var attr = IFCEngine.sdaiGetAttrDefinition(entity, attrName);
            ASSERT(attr != 0);

            Int64 adbVal;
            var res = IFCEngine.sdaiGetAttr(inst, attr, IFCEngine.sdaiADB, out adbVal);
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
            res = IFCEngine.sdaiGetAttr(inst, attr, IFCEngine.sdaiAGGR, out aggrVal);
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

            string strVal;
            res = IFCEngine.sdaiGetAttr(inst, attr, IFCEngine.sdaiBINARY, out strVal);
            if (expected != null && expected.binVal != null)
            {
                ASSERT(res != 0);
                ASSERT(strVal.Equals(expected.binVal));
            }
            else
            {
                ASSERT(res == 0 && strVal == null);
            }

            Int64 intVal = 1;
            res = IFCEngine.sdaiGetAttr(inst, attr, IFCEngine.sdaiBOOLEAN, out intVal);
            if (expected != null && expected.boolVal != null)
            {
                ASSERT(res != 0);
                ASSERT((intVal != 0) == expected.boolVal);
            }
            else
            {
                ASSERT(res == 0 && intVal == 0);
            }

            res = IFCEngine.sdaiGetAttr(inst, attr, IFCEngine.sdaiENUM, out strVal);
            if (expected != null && expected.enumVal != null)
            {
                ASSERT(res != 0);
                ASSERT(strVal.Equals(expected.enumVal));
            }
            else
            {
                ASSERT(res == 0 && strVal == null);
            }

            intVal = 1;
            res = IFCEngine.sdaiGetAttr(inst, attr, IFCEngine.sdaiINSTANCE, out intVal);
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
            res = IFCEngine.sdaiGetAttr(inst, attr, IFCEngine.sdaiINTEGER, out intVal);
            if (expected != null && expected.intVal != null)
            {
                ASSERT(res != 0);
                ASSERT(intVal == expected.intVal.Value);
            }
            else
            {
                ASSERT(res == 0 && intVal == 0);
            }

            res = IFCEngine.sdaiGetAttr(inst, attr, IFCEngine.sdaiLOGICAL, out strVal);
            if (expected != null && expected.logicalVal != null)
            {
                ASSERT(res != 0);
                ASSERT(strVal.Equals(expected.logicalVal));
            }
            else
            {
                ASSERT(res == 0 && strVal == null);
            }

            double realVal = 1.1;
            res = IFCEngine.sdaiGetAttr(inst, attr, IFCEngine.sdaiREAL, out realVal);
            if (expected != null && expected.realVal != null)
            {
                ASSERT(res != 0);
                ASSERT(realVal == expected.realVal.Value);
            }
            else
            {
                ASSERT(res == 0 && realVal == 0);
            }

            res = IFCEngine.sdaiGetAttr(inst, attr, IFCEngine.sdaiSTRING, out strVal);
            if (expected != null && expected.stringVal != null)
            {
                ASSERT(res != 0);
                ASSERT(strVal.Equals(expected.stringVal));
            }
            else
            {
                ASSERT(res == 0 && strVal == null);
            }

            res = IFCEngine.sdaiGetAttr(inst, attr, IFCEngine.sdaiUNICODE, out strVal);
            if (expected != null && expected.stringVal != null)
            {
                ASSERT(res != 0);
                ASSERT(strVal.Equals(expected.stringVal));
            }
            else
            {
                ASSERT(res == 0 && strVal == null);
            }

            res = IFCEngine.sdaiGetAttr(inst, attr, IFCEngine.sdaiEXPRESSSTRING, out strVal);
            if (expected != null && expected.expressStringVal != null)
            {
                ASSERT(res != 0);
                ASSERT(strVal.Equals(expected.expressStringVal));
            }
            else
            {
                ASSERT(res == 0 && strVal == null);
            }
        }

        static void CheckADBValues(Int64 adb, PrimitiveValues expected)
        {
            Int64 adbVal;
            var res = IFCEngine.sdaiGetADBValue(adb, IFCEngine.sdaiADB, out adbVal);
            if (expected != null && expected.complexArg)
            {
                ASSERT(res != 0);

                var saveExpressString = expected.expressStringVal;
                expected.expressStringVal = expected.stringVal; //step in ADB
                expected.complexArg = false;

                CheckADBValues(adbVal, expected);

                expected.expressStringVal = saveExpressString;
                expected.complexArg = true;
            }
            else
            {
                ASSERT(res == 0 && adbVal == 0);
            }

            Int64 aggrVal = 1;
            res = IFCEngine.sdaiGetADBValue(adb, IFCEngine.sdaiAGGR, out aggrVal);
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

            string strVal;
            //IntPtr ptrVal = IntPtr.MaxValue;
            res = IFCEngine.sdaiGetADBValue(adb, IFCEngine.sdaiBINARY, out strVal);
            if (expected != null && expected.binVal != null)
            {
                ASSERT(res != 0);
                ASSERT(strVal.Equals(expected.binVal));
            }
            else
            {
                ASSERT(res == 0 && strVal == null);
            }

            Int64 intVal = 1;
            res = IFCEngine.sdaiGetADBValue(adb, IFCEngine.sdaiBOOLEAN, out intVal);
            if (expected != null && expected.boolVal != null)
            {
                ASSERT(res != 0);
                ASSERT((intVal != 0) == expected.boolVal);
            }
            else
            {
                ASSERT(res == 0 && intVal == 0);
            }

            res = IFCEngine.sdaiGetADBValue(adb, IFCEngine.sdaiENUM, out strVal);
            if (expected != null && expected.enumVal != null)
            {
                ASSERT(res != 0);
                ASSERT(strVal.Equals(expected.enumVal));
            }
            else
            {
                ASSERT(res == 0 && strVal == null); ;
            }

            intVal = 1;
            res = IFCEngine.sdaiGetADBValue(adb, IFCEngine.sdaiINSTANCE, out intVal);
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
            res = IFCEngine.sdaiGetADBValue(adb, IFCEngine.sdaiINTEGER, out intVal);
            if (expected != null && expected.intVal != null)
            {
                ASSERT(res != 0);
                ASSERT(intVal == expected.intVal.Value);
            }
            else
            {
                ASSERT(res == 0 && intVal == 0);
            }

            res = IFCEngine.sdaiGetADBValue(adb, IFCEngine.sdaiLOGICAL, out strVal);
            if (expected != null && expected.logicalVal != null)
            {
                ASSERT(res != 0);
                ASSERT(strVal.Equals(expected.logicalVal));
            }
            else
            {
                ASSERT(res == 0 && strVal == null);
            }

            double realVal = 1.1;
            res = IFCEngine.sdaiGetADBValue(adb, IFCEngine.sdaiREAL, out realVal);
            if (expected != null && expected.realVal != null)
            {
                ASSERT(res != 0);
                ASSERT(realVal == expected.realVal.Value);
            }
            else
            {
                ASSERT(res == 0 && realVal == 0);
            }

            res = IFCEngine.sdaiGetADBValue(adb, IFCEngine.sdaiSTRING, out strVal);
            if (expected != null && expected.stringVal != null)
            {
                ASSERT(res != 0);
                ASSERT(strVal.Equals(expected.stringVal));
            }
            else
            {
                ASSERT(res == 0 && strVal == null);
            }

            res = IFCEngine.sdaiGetADBValue(adb, IFCEngine.sdaiUNICODE, out strVal);
            if (expected != null && expected.stringVal != null)
            {
                ASSERT(res != 0);
                ASSERT(strVal.Equals(expected.stringVal));
            }
            else
            {
                ASSERT(res == 0 && strVal == null);
            }

            res = IFCEngine.sdaiGetADBValue(adb, IFCEngine.sdaiEXPRESSSTRING, out strVal);
            if (expected != null && expected.expressStringVal != null)
            {
                ASSERT(res != 0);
                ASSERT(strVal.Equals(expected.expressStringVal));
            }
            else
            {
                ASSERT(res == 0 && strVal == null);
            }
        }

        static void CheckAggrValues
            (Int64 aggr,
             PrimitiveValues expected
            )
        {
            var cnt = IFCEngine.sdaiGetMemberCount(aggr);
            ASSERT(cnt == 1);

            Int64 adbVal;
            var res = IFCEngine.sdaiGetAggrByIndex(aggr, 0, IFCEngine.sdaiADB, out adbVal);
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
            res = IFCEngine.sdaiGetAggrByIndex(aggr, 0, IFCEngine.sdaiAGGR, out aggrVal);
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

            string strVal;
            res = IFCEngine.sdaiGetAggrByIndex(aggr, 0, IFCEngine.sdaiBINARY, out strVal);
            if (expected != null && expected.binVal != null)
            {
                ASSERT(res != 0);
                ASSERT(strVal.Equals(expected.binVal));
            }
            else
            {
                ASSERT(res == 0 && strVal == null);
            }

            Int64 intVal = 1;
            res = IFCEngine.sdaiGetAggrByIndex(aggr, 0, IFCEngine.sdaiBOOLEAN, out intVal);
            if (expected != null && expected.boolVal != null)
            {
                ASSERT(res != 0);
                ASSERT((intVal != 0) == expected.boolVal);
            }
            else
            {
                ASSERT(res == 0 && intVal == 0);
            }

            res = IFCEngine.sdaiGetAggrByIndex(aggr, 0, IFCEngine.sdaiENUM, out strVal);
            if (expected != null && expected.enumVal != null)
            {
                ASSERT(res != 0);
                ASSERT(strVal.Equals(expected.enumVal));
            }
            else
            {
                ASSERT(res == 0 && strVal == null);
            }

            intVal = 1;
            res = IFCEngine.sdaiGetAggrByIndex(aggr, 0, IFCEngine.sdaiINSTANCE, out intVal);
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
            res = IFCEngine.sdaiGetAggrByIndex(aggr, 0, IFCEngine.sdaiINTEGER, out intVal);
            if (expected != null && expected.intVal != null)
            {
                ASSERT(res != 0);
                ASSERT(intVal == expected.intVal.Value);
            }
            else
            {
                ASSERT(res == 0 && intVal == 0);
            }

            res = IFCEngine.sdaiGetAggrByIndex(aggr, 0, IFCEngine.sdaiLOGICAL, out strVal);
            if (expected != null && expected.logicalVal != null)
            {
                ASSERT(res != 0);
                ASSERT(strVal.Equals(expected.logicalVal));
            }
            else
            {
                ASSERT(res == 0 && strVal == null);
            }

            double realVal = 1.1;
            res = IFCEngine.sdaiGetAggrByIndex(aggr, 0, IFCEngine.sdaiREAL, out realVal);
            if (expected != null && expected.realVal != null)
            {
                ASSERT(res != 0);
                ASSERT(realVal == expected.realVal.Value);
            }
            else
            {
                ASSERT(res == 0 && realVal == 0);
            }

            res = IFCEngine.sdaiGetAggrByIndex(aggr, 0, IFCEngine.sdaiSTRING, out strVal);
            if (expected != null && expected.stringVal != null)
            {
                ASSERT(res != 0);
                ASSERT(strVal.Equals(expected.stringVal));
            }
            else
            {
                ASSERT(res == 0 && strVal == null);
            }

            res = IFCEngine.sdaiGetAggrByIndex(aggr, 0, IFCEngine.sdaiUNICODE, out strVal);
            if (expected != null && expected.stringVal != null)
            {
                ASSERT(res != 0);
                ASSERT(strVal.Equals(expected.stringVal));
            }
            else
            {
                ASSERT(res == 0 && strVal == null);
            }

            res = IFCEngine.sdaiGetAggrByIndex(aggr, 0, IFCEngine.sdaiEXPRESSSTRING, out strVal);
            if (expected != null && expected.expressStringVal != null)
            {
                ASSERT(res != 0);
                ASSERT(strVal.Equals(expected.expressStringVal));
            }
            else
            {
                ASSERT(res == 0 && strVal == null);
            }
        }

    }
}