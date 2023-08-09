using RDF;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CsEngineTests  
{
    internal class Rename : CsTests.TestBase
    {
        public static void Run()
        {
            RenameClass(false);
            RenameClass(true);
            RenameProperty(false);
            RenameProperty(true);
        }

        private static void RenameClass (bool w)
        {
            ENTER_TEST(w ? "SetNameOfClassW" : "SetNameOfClass");

            var model = engine.OpenModel(null as byte[]);

            RenameClass(model, "Box", "RenameBox", 2, w);
            if (w)
            {
                RenameClass(model, "Box", "Юникод", 2, w);
            }

            engine.CreateClass(model, "UsedName");
            engine.CreateProperty(model, 1, "UsedProp");

            engine.CreateClass(model, "CustomClass");
            RenameClass(model, "CustomClass", "UsedName", 3, w);
            RenameClass(model, "CustomClass", "Box", 3, w);
            RenameClass(model, "CustomClass", "length", 3, w);
            RenameClass(model, "CustomClass", "UsedProp", 3, w);
            RenameClass(model, "CustomClass", "NewName", 0, w);

            if (w)
            {
                RenameClass(model, "NewName", "Юникод", 0, w);
                RenameClass(model, "UsedName", "Юникод", 3, w);
            }

            engine.CloseModel(model);
        }

        private static void RenameProperty(bool w)
        {
            ENTER_TEST(w ? "SetNameOfPropertyW" : "SetNameOfProperty");

            var model = engine.OpenModel(null as byte[]);

            RenameProperty(model, "length", "RenameLen", 2, w);
            if (w)
            {
                RenameProperty(model, "length", "Юникод", 2, w);
            }

            engine.CreateClass(model, "UsedClass");
            engine.CreateProperty(model, 1, "UsedProp");

            for (int type = 1; type < 3; type++)
            {
                var propName = string.Format("CustomProp_{0}", type);
                
                engine.CreateProperty(model, type, propName);
                
                RenameProperty(model, propName, "UsedClass", 3, w);
                RenameProperty(model, propName, "Box", 3, w);
                RenameProperty(model, propName, "length", 3, w);
                RenameProperty(model, propName, "UsedProp", 3, w);
                RenameProperty(model, propName, "NewName" + type.ToString(), 0, w);

                if (w)
                {
                    RenameProperty(model, "NewName" + type.ToString(), "Юникод" + type.ToString(), 0, w);
                    RenameProperty(model, "UsedProp", "Юникод" + type.ToString(), 3, w);
                }
            }

            engine.CloseModel(model);
        }

        private static void RenameClass (Int64 model, string oldName, string newName, int expect, bool w)
        {
            var cls = engine.GetClassByName(model, oldName);
            ASSERT(cls != 0);

            byte[] ucodeName = Encoding.Unicode.GetBytes(newName);

            long res = 9999;
            if (w)
            {
                res = engine.SetNameOfClassW(cls, ucodeName);
            }
            else
            {
                res = engine.SetNameOfClass(cls, newName);
            }
            ASSERT(res == expect);

            if (res != 0)
                return;

            Int64 cls2 = 0;
            if (w)
            {
                cls2 = engine.GetClassByNameW(model, ucodeName);
            }
            else
            {
                cls2 = engine.GetClassByName(model, newName);
            }
            ASSERT(cls2 == cls);

            string name2;
            if (w)
            {
                name2 = engine.GetNameOfClassW(cls2);
            }
            else
            {
                name2 = engine.GetNameOfClass(cls2);
            }
            ASSERT(name2 == newName);

            cls = engine.GetClassByName(model, oldName);
            ASSERT (cls == 0);
        }

        private static void RenameProperty(Int64 model, string oldName, string newName, int expect, bool w)
        {
            var prp = engine.GetPropertyByName(model, oldName);
            ASSERT(prp != 0);

            byte[] ucodeName = Encoding.Unicode.GetBytes(newName);

            long res = 9999;
            if (w)
            {
                res = engine.SetNameOfPropertyW(prp, ucodeName);
            }
            else
            {
                res = engine.SetNameOfProperty(prp, newName);
            }
            ASSERT(res == expect);

            if (res != 0)
                return;

            Int64 prp2 = 0;
            if (w)
            {
                prp2 = engine.GetPropertyByNameW(model, ucodeName);
            }
            else
            {
                prp2 = engine.GetPropertyByName(model, newName);
            }
            ASSERT(prp2 == prp);

            string name2;
            if (w)
            {
                name2 = engine.GetNameOfPropertyW(prp2);
            }
            else
            {
                name2 = engine.GetNameOfProperty(prp2);
            }
            ASSERT(name2 == newName);

            prp = engine.GetPropertyByName(model, oldName);
            ASSERT(prp == 0);
        }

    }
}
