using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using RDF;

namespace CsIfcEngineTests
{
    internal class SchemaReadingAPI : CsTests.TestBase
    {
        public static void Run()
        {
            ENTER_TEST();

            var model = ifcengine.sdaiCreateModelBN("IFC4");

            IterateEntities(model);

            ifcengine.sdaiCloseModel(model);

        }

        static void IterateEntities(Int64 model)
        {
            int definedTypes = 0;
            int enumTypes = 0;
            int selectTypes = 0;
            int entities = 0;

            Int64 it = 0;
            while (0 != (it = ifcengine.engiGetNextTypeDeclarationIterator(model, it)))
            {
                Int64 decl = ifcengine.engiGetTypeDeclarationFromIterator(model, it);
                ASSERT(decl);

                var type = ifcengine.engiGetDeclarationType(decl);
                switch (type) 
                {
                    case enum_express_declaration.__DEFINED_TYPE:
                        definedTypes++;
                        break;
                    case enum_express_declaration.__SELECT:
                        selectTypes++;
                        break;
                    case enum_express_declaration.__ENUM:
                        enumTypes++;
                        break;
                    case enum_express_declaration.__ENTITY:
                        entities++;
                        break;
                    default:
                        ASSERT(false);
                        break;
                }
            }
            ASSERT(definedTypes == 130 && selectTypes == 60 && enumTypes == 207 & entities == 776);            
        }
    }
}