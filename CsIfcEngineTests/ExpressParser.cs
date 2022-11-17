using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CsIfcEngineTests
{
    class ExpressParser : CsTests.TestBase
    {
        public static void Run ()
        {
            ENTER_TEST();

            ParseExpressFile("..\\TestData\\schemas\\IFC4_ADD2_TC1.exp");
            ParseExpressFile("..\\TestData\\schemas\\IFC2X3_TC1.exp");
            ParseExpressFile("..\\TestData\\schemas\\IFC4x1.exp");
            ParseExpressFile("..\\TestData\\schemas\\IFC4x2.exp");
            ParseExpressFile("..\\TestData\\schemas\\IFC4X3_ADD1.exp");
            ParseExpressFile("..\\TestData\\schemas\\IFC4_ADD2_TC1.exp");
            ParseExpressFile("..\\TestData\\schemas\\IFC4x4.exp");
            ParseExpressFile("..\\TestData\\schemas\\structural_frame_schema.exp");
            ParseExpressFile("..\\TestData\\schemas\\ap242ed2_mim_lf_v1.101.exp");
        }

        private static void ParseExpressFile (string strExpFile)
        {
            var model = RDF.ifcengine.sdaiCreateModelBN(1, "",strExpFile);
            ASSERT(model!= 0);

            RDF.ifcengine.sdaiCloseModel(model);
        }
    }
}
