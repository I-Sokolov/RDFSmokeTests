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

            var model = ifcengine.sdaiCreateModelBN("IFC4");
            ASSERT(model);

            var entity = ifcengine.sdaiGetEntity(model, "IfcWall");

            var ok = ifcengine.engiSetMappingSupport(entity, true);
            ASSERT(ok);
            ok = ifcengine.engiGetMappingSupport(entity);
            ASSERT(ok);
            ok = ifcengine.engiSetMappingSupport(entity, false);
            ASSERT(ok);
            ok = ifcengine.engiGetMappingSupport(entity);
            ASSERT(!ok);

            ifcengine.sdaiCloseModel(model);
        }
    }
}
