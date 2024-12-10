using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using RDF;

namespace CsIfcEngineTests
{
    internal class MixedTests : CsTests.TestBase
    {
        public static void Run()
        {
            ENTER_TEST();
            GetSetGlobalPlacement();
        }

        private static void GetSetGlobalPlacement()
        {
            var model = ifcengine.sdaiOpenModelBN(0, "..\\TestData\\walls.ifc", "");
            ASSERT(model != 0);

            //expected identity placement
            double[] expectedOrigin = new double[12];
            expectedOrigin[0] = expectedOrigin[4] = expectedOrigin[8] = 1.0;

            double[] getOrigin = new double[12];
            ifcengine.getGlobalPlacement(model, out getOrigin[0]);
            ASSERT_EQ(getOrigin, expectedOrigin);

            //now set translation (1,2,3)
            expectedOrigin[9] = 1.0;
            expectedOrigin[10] = 2.0;
            expectedOrigin[11] = 3.0;
            ifcengine.setGlobalPlacement(model, ref expectedOrigin[0], false);

            //check, use second form 
            ifcengine.getGlobalPlacement(model, getOrigin);
            ASSERT_EQ(getOrigin, expectedOrigin);

            ifcengine.sdaiCloseModel(model);
        }

    }
}
