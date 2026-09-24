using RDF;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CsIfcEngineTests
{
    internal class TestBool : CsTests.TestBase
    {
        public static void Run()
        {
            ENTER_TEST();

            var model = IFCEngine.sdaiCreateModelBN("IFC4");
            ASSERT(model);

            var entity = IFCEngine.sdaiGetEntity(model, "IfcWall");

            var ok = IFCEngine.engiSetMappingSupport(entity, true);
            ASSERT(ok);
            ok = IFCEngine.engiGetMappingSupport(entity);
            ASSERT(ok);
            ok = IFCEngine.engiSetMappingSupport(entity, false);
            ASSERT(ok);
            ok = IFCEngine.engiGetMappingSupport(entity);
            ASSERT(!ok);

            IFCEngine.sdaiCloseModel(model);
        }
    }
}
