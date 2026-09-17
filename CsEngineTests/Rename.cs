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

            var model = Engine.OpenModel(null as byte[]);

            RenameClass(model, "Box", "RenameBox", enum_error_code_set_uri_LOCKED_NAME, w);
            if (w)
            {
                RenameClass(model, "Box", "Юникод", enum_error_code_set_uri_LOCKED_NAME, w);
            }

            Engine.CreateClass(model, "UsedName");
            Engine.CreateProperty(model, 1, "UsedProp");

            Engine.CreateClass(model, "CustomClass");
            RenameClass(model, "CustomClass", "UsedName", enum_error_code_set_uri_NAME_USED_BY_CLASS, w);
            RenameClass(model, "CustomClass", "Box", enum_error_code_set_uri_NAME_USED_BY_CLASS, w);
            RenameClass(model, "CustomClass", "length", enum_error_code_set_uri_NAME_USED_BY_PROPERTY, w);
            RenameClass(model, "CustomClass", "UsedProp", enum_error_code_set_uri_NAME_USED_BY_PROPERTY, w);
            RenameClass(model, "CustomClass", "NewName", enum_error_code_set_uri_SUCCESSFUL, w);

            if (w)
            {
                RenameClass(model, "NewName", "Юникод", enum_error_code_set_uri_SUCCESSFUL, w);
                RenameClass(model, "UsedName", "Юникод", enum_error_code_set_uri_NAME_USED_BY_CLASS, w);
            }

            Engine.CloseModel(model);
        }

        const int enum_error_code_set_uri_SUCCESSFUL = 0;	//successful
        //const int //		1	argument owlClass is incorrect (not a proper handle to an active class)
        //const int //		2	argument name is incorrect (nullptr or zero length name)
        const long enum_error_code_set_uri_LOCKED_NAME = 3;	//the name of owlClass is locked
        const long enum_error_code_set_uri_NAME_USED_BY_CLASS = 4;	//name is already used by another class
        const long enum_error_code_set_uri_NAME_USED_BY_PROPERTY = 5;	//name is already used by a property
        //		6	name is already used by an instance
        const long enum_error_code_set_uri_OTHER_ERROR = 7;	//undefined error

        private static void RenameProperty(bool w)
        {
            ENTER_TEST(w ? "SetNameOfPropertyW" : "SetNameOfProperty");

            var model = Engine.OpenModel(null as byte[]);

            RenameProperty(model, "length", "RenameLen", enum_error_code_set_uri_LOCKED_NAME, w);
            if (w)
            {
                RenameProperty(model, "length", "Юникод", enum_error_code_set_uri_LOCKED_NAME, w);
            }

            Engine.CreateClass(model, "UsedClass");
            Engine.CreateProperty(model, 1, "UsedProp");

            for (int type = 1; type < 3; type++)
            {
                var propName = string.Format("CustomProp_{0}", type);

                Engine.CreateProperty(model, type, propName);
                
                RenameProperty(model, propName, "UsedClass", enum_error_code_set_uri_NAME_USED_BY_CLASS, w);
                RenameProperty(model, propName, "Box", enum_error_code_set_uri_NAME_USED_BY_CLASS, w);
                RenameProperty(model, propName, "length", enum_error_code_set_uri_NAME_USED_BY_PROPERTY, w);
                RenameProperty(model, propName, "UsedProp", enum_error_code_set_uri_NAME_USED_BY_PROPERTY, w);
                RenameProperty(model, propName, "NewName" + type.ToString(), enum_error_code_set_uri_SUCCESSFUL, w);

                if (w)
                {
                    RenameProperty(model, "NewName" + type.ToString(), "Юникод" + type.ToString(), enum_error_code_set_uri_SUCCESSFUL, w); ;
                    RenameProperty(model, "UsedProp", "Юникод" + type.ToString(), enum_error_code_set_uri_NAME_USED_BY_PROPERTY, w);
                }
            }

            Engine.CloseModel(model);
        }

        private static void RenameClass (Int64 model, string oldName, string newName, long expect, bool w)
        {
            var cls = Engine.GetClassByName(model, oldName);
            ASSERT(cls != 0);

            byte[] ucodeName = Encoding.Unicode.GetBytes(newName);

            long res = enum_error_code_set_uri_OTHER_ERROR;
            if (w)
            {
                res = Engine.SetNameOfClassW(cls, ucodeName);
            }
            else
            {
                res = Engine.SetNameOfClass(cls, newName);
            }
            ASSERT(res == expect);

            if (res != 0)
                return;

            Int64 cls2 = 0;
            if (w)
            {
                cls2 = Engine.GetClassByNameW(model, ucodeName);
            }
            else
            {
                cls2 = Engine.GetClassByName(model, newName);
            }
            ASSERT(cls2 == cls);

            string name2;
            if (w)
            {
                name2 = Engine.GetNameOfClassW(cls2);
            }
            else
            {
                name2 = Engine.GetNameOfClass(cls2);
            }
            ASSERT(name2 == newName);

            cls = Engine.GetClassByName(model, oldName);
            ASSERT (cls == 0);
        }

        private static void RenameProperty(Int64 model, string oldName, string newName, long expect, bool w)
        {
            var prp = Engine.GetPropertyByName(model, oldName);
            ASSERT(prp != 0);

            byte[] ucodeName = Encoding.Unicode.GetBytes(newName);

            long res = enum_error_code_set_uri_OTHER_ERROR;
            if (w)
            {
                res = Engine.SetNameOfPropertyW(prp, ucodeName);
            }
            else
            {
                res = Engine.SetNameOfProperty(prp, newName);
            }
            ASSERT(res == expect);

            if (res != 0)
                return;

            Int64 prp2 = 0;
            if (w)
            {
                prp2 = Engine.GetPropertyByNameW(model, ucodeName);
            }
            else
            {
                prp2 = Engine.GetPropertyByName(model, newName);
            }
            ASSERT(prp2 == prp);

            string name2;
            if (w)
            {
                name2 = Engine.GetNameOfPropertyW(prp2);
            }
            else
            {
                name2 = Engine.GetNameOfProperty(prp2);
            }
            ASSERT(name2 == newName);

            prp = Engine.GetPropertyByName(model, oldName);
            ASSERT(prp == 0);
        }

    }
}
