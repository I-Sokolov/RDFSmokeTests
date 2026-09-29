using System;
using System.Collections.Generic;
using System.IO;
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

            string basePath = AppContext.BaseDirectory;

            string fullPath = Path.GetFullPath(Path.Combine(basePath, "..\\TestData\\schemas\\IFC4_ADD2_TC1.exp"));
            ParseExpressFile(fullPath);

            fullPath = Path.GetFullPath(Path.Combine(basePath, "..\\TestData\\schemas\\IFC2X3_TC1.exp"));
            ParseExpressFile(fullPath);

            fullPath = Path.GetFullPath(Path.Combine(basePath, "..\\TestData\\schemas\\IFC4x1.exp"));
            ParseExpressFile(fullPath);

            fullPath = Path.GetFullPath(Path.Combine(basePath, "..\\TestData\\schemas\\IFC4x2.exp"));
            ParseExpressFile(fullPath);

            //fullPath = Path.GetFullPath(Path.Combine(basePath, "..\\TestData\\schemas\\IFC4X3_ADD2.exp"));
            //ParseExpressFile("fullPath");

            fullPath = Path.GetFullPath(Path.Combine(basePath, "..\\TestData\\schemas\\IFC4_ADD2_TC1.exp"));
            ParseExpressFile(fullPath);

            fullPath = Path.GetFullPath(Path.Combine(basePath, "..\\TestData\\schemas\\IFC4x4.exp"));
            ParseExpressFile(fullPath);

            fullPath = Path.GetFullPath(Path.Combine(basePath, "..\\TestData\\schemas\\structural_frame_schema.exp"));
            ParseExpressFile(fullPath);

            fullPath = Path.GetFullPath(Path.Combine(basePath, "..\\TestData\\schemas\\ap242ed2_mim_lf_v1.101.exp"));
            ParseExpressFile(fullPath);
        }

        private static void ParseExpressFile (string strExpFile)
        {
            var model = RDF.IFCEngine.sdaiCreateModelBN(1, "",strExpFile);
            ASSERT(model!= 0);

            RDF.IFCEngine.sdaiCloseModel(model);
        }

    }
}
