using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using GEOM;
using RDF;

namespace CsEngineTests
    {
    internal class MixedTests : CsTests.TestBase
        {
        public static void Run()
            {
            RemoveInstance();
            RemoveInstanceRecursively();
            UpwardDelete();

            GetNameOfClassAndProperty();
            }

        private static void CreateCollection (out long model, out Collection collection) 
            {
            model = Engine.OpenModel(null as byte[]);

            var material = Material.Create(model);

            var lst = new List<GeometricItem>();
            for (int i = 0; i < 2; i++)
                {
                var box = Box.Create(model);
                box.material = material;
                lst.Add(box);
                }

            collection = Collection.Create(model);
            collection.objects = lst.ToArray();
            }

        private static void RemoveInstance() { 
            ENTER_TEST();

            long model = 0;
            Collection collection = null;
            CreateCollection(out model, out collection);

            var items = collection.objects;
            var material = items[0].material;

            var cnt = InstanceCount(model);
            ASSERT(cnt == 4);

            ASSERT(Engine.RemoveInstance(collection)==0);
            cnt = InstanceCount(model);
            ASSERT(cnt == 3);

            //double delete
            ASSERT(Engine.RemoveInstance(collection) == 0);
            cnt = InstanceCount(model);
            ASSERT(cnt == 3);

            ASSERT(Engine.RemoveInstance(items[0])==0);
            cnt = InstanceCount(model);
            ASSERT(cnt == 2);

            ASSERT(Engine.RemoveInstance(material) != 0);
            cnt = InstanceCount(model);
            ASSERT(cnt == 2);

            ASSERT(Engine.RemoveInstance(items[1]) == 0);
            cnt = InstanceCount(model);
            ASSERT(cnt == 1);

            ASSERT(Engine.RemoveInstance(material) == 0);
            cnt = InstanceCount(model);
            ASSERT(cnt == 0);

            Engine.CloseModel(model);
            }

        private static void RemoveInstanceRecursively()
            {
            ENTER_TEST();

            long model = 0;
            Collection collection = null;
            CreateCollection(out model, out collection);

            var items = collection.objects;
            var material = items[0].material;

            var cnt = InstanceCount(model);
            ASSERT(cnt == 4);

            ASSERT(Engine.RemoveInstanceRecursively(collection) == 4);
            cnt = InstanceCount(model);
            ASSERT(cnt == 0);

            //double delete
            ASSERT(Engine.RemoveInstanceRecursively(collection) == 0);

            ASSERT(Engine.RemoveInstance(items[0]) == 0);
            ASSERT(Engine.RemoveInstanceRecursively(items[1]) == 0);
            ASSERT(Engine.RemoveInstance(material) == 0);

            Engine.CloseModel(model);
            }

        static private void UpwardDelete()
            {
            ENTER_TEST();

            long model = 0;
            Collection collection = null;
            CreateCollection(out model, out collection);

            var items = collection.objects;
            var material = items[0].material;

            var cnt = InstanceCount(model);
            ASSERT(cnt == 4);

            ASSERT(Engine.RemoveInstance(items[0]) != 0);
            ASSERT(Engine.RemoveInstanceRecursively(items[1]) == 0);

            ASSERT(Engine.RemoveInstance(collection) == 0);
            cnt = InstanceCount(model);
            ASSERT(cnt == 3);

            ASSERT(Engine.RemoveInstance(items[0]) == 0);
            cnt = InstanceCount(model);
            ASSERT(cnt == 2);

            ASSERT(Engine.RemoveInstanceRecursively(items[1]) == 2);
            cnt = InstanceCount(model);
            ASSERT(cnt == 0);

            Engine.CloseModel(model);
            }

        private static int InstanceCount(long model)
            {
            int cnt = 0;
            long inst = 0;
            while ((inst = Engine.GetInstancesByIterator(model, inst))!=0)
                {
                cnt++;
                }
            return cnt;
            }

        private static void GetNameOfClassAndProperty()
            {
            ENTER_TEST();

            var model = Engine.OpenModel(null as byte[]);
            
            var box = Engine.GetClassByName(model, "Box");
            ASSERT(box != 0);
            var length = Engine.GetPropertyByName (model, "length");
            ASSERT(length != 0);
            var material = Engine.GetPropertyByName(model, "material");
            ASSERT(material != 0);
            
            var res = Engine.GetNameOfClass(model);
            ASSERT(res == null);

            res = Engine.GetNameOfClass (box);
            ASSERT(res == "Box");

            res = Engine.GetNameOfClassW(model);
            ASSERT(res == null);

            res = Engine.GetNameOfClassW(box);
            ASSERT(res == "Box");

            res = Engine.GetNameOfClassEx(model, model);
            ASSERT(res == null);

            res = Engine.GetNameOfClassEx(model, box);
            ASSERT(res == "Box");

            res = Engine.GetNameOfClassWEx(model, model);
            ASSERT(res == null);

            res = Engine.GetNameOfClassWEx(model, box);
            ASSERT(res == "Box");

            res = Engine.GetNameOfProperty(model);
            ASSERT(res == null);

            res = Engine.GetNameOfProperty(length);
            ASSERT(res == "length");
            res = Engine.GetNameOfProperty(material);
            ASSERT(res == "material");

            res = Engine.GetNameOfPropertyW(model);
            ASSERT(res == null);

            res = Engine.GetNameOfPropertyW(length);
            ASSERT(res == "length");
            res = Engine.GetNameOfPropertyW(material);
            ASSERT(res == "material");

            res = Engine.GetNameOfPropertyEx(model, model);
            ASSERT(res == null);

            res = Engine.GetNameOfPropertyEx(model, length);
            ASSERT(res == "length");
            res = Engine.GetNameOfPropertyEx(model, material);
            ASSERT(res == "material");

            res = Engine.GetNameOfPropertyWEx(model, model);
            ASSERT(res == null);

            res = Engine.GetNameOfPropertyWEx(model, length);
            ASSERT(res == "length");
            res = Engine.GetNameOfPropertyWEx(model, material);
            ASSERT(res == "material");

            Engine.CloseModel(model);
            }
        }
    }
