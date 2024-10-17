using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static RDF.ifcengine;
using IFC4;

/*
            sdaiPutAttrBN
            sdaiGetAttrBN
            sdaiPutAttr
            sdaiGetAttr

            sdaiCreateADB
            sdaiGetADBValue

            sdaiAppend
            sdaiPrepend
            sdaiAdd
            sdaiInsertByIndex
            sdaiPutAggrByIterator

            sdaiGetAggrByIndex
            sdaiGetAggrByIterator
* 
 */

namespace CsIfcEngineTests
{
    internal class RegionalChars : CsTests.TestBase
    {
        const string TEXT = "ABC ÜüÖöÄäẞß ЩяЪ 好";
        const string STEP =
            "ABC \\X\\DC\\X\\FC\\X\\D6\\X\\F6\\X\\C4\\X\\E4\\X2\\1E9E\\X0\\\\X\\DF \\X2\\0429\\X0\\\\X2\\044F\\X0\\\\X2\\042A\\X0\\ \\X2\\597D\\X0\\";

        public static void Run()
        {
            ENTER_TEST();

            var model = sdaiCreateModelBN(0, "", "IFC4");
            ASSERT(model != 0);

            var wall = IfcWall.Create(model);

            //
            sdaiPutAttrBN(wall, "Name", sdaiSTRING, TEXT);

            string str;
            sdaiGetAttrBN(wall, "Name", sdaiSTRING, out str);
            ASSERT(str == TEXT);

            sdaiPutAttrBN(wall, "Name", sdaiUNICODE, TEXT);

            sdaiGetAttrBN(wall, "Name", sdaiUNICODE, out str);
            ASSERT(str == TEXT);

            sdaiGetAttrBN(wall, "Name", sdaiEXPRESSSTRING, out str);
            ASSERT(str == STEP);

            //
            var entity = sdaiGetInstanceType(wall);
            var attr = sdaiGetAttrDefinition(entity, "Description");
            
            sdaiPutAttr(wall, attr, sdaiSTRING, TEXT);
            
            sdaiGetAttr(wall, attr, sdaiSTRING, out str);
            ASSERT(str == TEXT);

            sdaiPutAttr(wall, attr, sdaiUNICODE, TEXT);

            sdaiGetAttr(wall, attr, sdaiUNICODE, out str);
            ASSERT(str == TEXT);

            sdaiGetAttr(wall, attr, sdaiEXPRESSSTRING, out str);
            ASSERT(str == STEP);

            //
            var adb = sdaiCreateADB(sdaiSTRING, TEXT);
            CheckADB(adb);

            adb = sdaiCreateADB(sdaiUNICODE, TEXT);
            CheckADB(adb);

            //
            var person = IfcPerson.Create(model);
            var aggr = sdaiCreateAggrBN(person, "MiddleNames");

            sdaiAppend(aggr, sdaiSTRING, TEXT);
            sdaiAppend(aggr, sdaiUNICODE, TEXT);

            //not implemented
            //sdaiPrepend(aggr,sdaiUNICODE, TEXT + "0");
            //sdaiAdd(aggr, sdaiUNICODE, TEXT + "3");
            //sdaiInsertByIndex(aggr, 1, sdaiSTRING, TEXT + "1");

            //not implemented
            //var iter = sdaiCreateIterator(aggr);
            //sdaiPutAggrByIterator(iter, sdaiUNICODE, TEXT);

            var N = sdaiGetMemberCount(aggr);
            ASSERT(N == 2);

            for (Int64 i = 0; i<N; i++)
            {
                sdaiGetAggrByIndex(aggr, i, sdaiSTRING, out str);
                ASSERT(str == TEXT);

                sdaiGetAggrByIndex(aggr, i, sdaiUNICODE, out str);
                ASSERT(str == TEXT);

                sdaiGetAggrByIndex(aggr, i, sdaiEXPRESSSTRING, out str);
                ASSERT(str == STEP);
            }

            //not implemented
            //sdaiGetAggrByIterator(iter, sdaiSTRING, out str);
            //ASSERT(str == TEXT);
            //sdaiGetAggrByIterator(iter, sdaiUNICODE, out str);
            //ASSERT(str == TEXT);
            //sdaiGetAggrByIterator(iter, sdaiEXPRESSSTRING, out str);
            //ASSERT(str == STEP);
            //sdaiDeleteIterator(iter);

            //other string types
            sdaiPutAttrBN(wall, "GlobalId", sdaiEXPRESSSTRING, "T");
            sdaiGetAttrBN(wall, "GlobalId", sdaiEXPRESSSTRING, out str);
            ASSERT(str == "T");

            sdaiPutAttrBN(wall, "GlobalId", sdaiENUM, "T");
            sdaiGetAttrBN(wall, "GlobalId", sdaiENUM, out str);
            ASSERT(str == "T");

            sdaiPutAttrBN(wall, "GlobalId", sdaiLOGICAL, "T");
            sdaiGetAttrBN(wall, "GlobalId", sdaiLOGICAL, out str);
            ASSERT(str == "T");

            sdaiPutAttrBN(wall, "GlobalId", sdaiBINARY, "0");
            sdaiGetAttrBN(wall, "GlobalId", sdaiBINARY, out str);
            ASSERT(str == "0");

            sdaiCloseModel(model);
        }

        private static void CheckADB(Int64 adb)
        {
            string str;
            sdaiGetADBValue(adb, sdaiSTRING, out str);
            ASSERT(str == TEXT);

            sdaiGetADBValue(adb, sdaiUNICODE, out str);
            ASSERT(str == TEXT);

            sdaiGetADBValue(adb, sdaiEXPRESSSTRING, out str);
            ASSERT(str == STEP);
        }
    }
}
