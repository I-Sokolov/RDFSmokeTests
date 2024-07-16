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
            model = engine.OpenModel(null as byte[]);

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

            ASSERT(engine.RemoveInstance(collection)==0);
            cnt = InstanceCount(model);
            ASSERT(cnt == 3);

            //double delete
            ASSERT(engine.RemoveInstance(collection) == 0);
            cnt = InstanceCount(model);
            ASSERT(cnt == 3);

            ASSERT(engine.RemoveInstance(items[0])==0);
            cnt = InstanceCount(model);
            ASSERT(cnt == 2);

            ASSERT(engine.RemoveInstance(material) != 0);
            cnt = InstanceCount(model);
            ASSERT(cnt == 2);

            ASSERT(engine.RemoveInstance(items[1]) == 0);
            cnt = InstanceCount(model);
            ASSERT(cnt == 1);

            ASSERT(engine.RemoveInstance(material) == 0);
            cnt = InstanceCount(model);
            ASSERT(cnt == 0);

            engine.CloseModel(model);
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

            ASSERT(engine.RemoveInstanceRecursively(collection) == 4);
            cnt = InstanceCount(model);
            ASSERT(cnt == 0);

            //double delete
            ASSERT(engine.RemoveInstanceRecursively(collection) == 0);

            ASSERT(engine.RemoveInstance(items[0]) == 0);
            ASSERT(engine.RemoveInstanceRecursively(items[1]) == 0);
            ASSERT(engine.RemoveInstance(material) == 0);

            engine.CloseModel(model);
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

            ASSERT(engine.RemoveInstance(items[0]) != 0);
            ASSERT(engine.RemoveInstanceRecursively(items[1]) == 0);

            ASSERT(engine.RemoveInstance(collection) == 0);
            cnt = InstanceCount(model);
            ASSERT(cnt == 3);

            ASSERT(engine.RemoveInstance(items[0]) == 0);
            cnt = InstanceCount(model);
            ASSERT(cnt == 2);

            ASSERT(engine.RemoveInstanceRecursively(items[1]) == 2);
            cnt = InstanceCount(model);
            ASSERT(cnt == 0);

            engine.CloseModel(model);
            }

        private static int InstanceCount(long model)
            {
            int cnt = 0;
            long inst = 0;
            while ((inst = engine.GetInstancesByIterator(model, inst))!=0)
                {
                cnt++;
                }
            return cnt;
            }

        private static void GetNameOfClassAndProperty()
            {
            ENTER_TEST();

            var model = engine.OpenModel(null as byte[]);
            
            var box = engine.GetClassByName(model, "Box");
            ASSERT(box != 0);
            var length = engine.GetPropertyByName (model, "length");
            ASSERT(length != 0);
            var material = engine.GetPropertyByName(model, "material");
            ASSERT(material != 0);
            
            var res = engine.GetNameOfClass(model);
            ASSERT(res == null);

            res = engine.GetNameOfClass (box);
            ASSERT(res == "Box");

            res = engine.GetNameOfClassW(model);
            ASSERT(res == null);

            res = engine.GetNameOfClassW(box);
            ASSERT(res == "Box");

            res = engine.GetNameOfClassEx(model, model);
            ASSERT(res == null);

            res = engine.GetNameOfClassEx(model, box);
            ASSERT(res == "Box");

            res = engine.GetNameOfClassWEx(model, model);
            ASSERT(res == null);

            res = engine.GetNameOfClassWEx(model, box);
            ASSERT(res == "Box");

            res = engine.GetNameOfProperty(model);
            ASSERT(res == null);

            res = engine.GetNameOfProperty(length);
            ASSERT(res == "length");
            res = engine.GetNameOfProperty(material);
            ASSERT(res == "material");

            res = engine.GetNameOfPropertyW(model);
            ASSERT(res == null);

            res = engine.GetNameOfPropertyW(length);
            ASSERT(res == "length");
            res = engine.GetNameOfPropertyW(material);
            ASSERT(res == "material");

            res = engine.GetNameOfPropertyEx(model, model);
            ASSERT(res == null);

            res = engine.GetNameOfPropertyEx(model, length);
            ASSERT(res == "length");
            res = engine.GetNameOfPropertyEx(model, material);
            ASSERT(res == "material");

            res = engine.GetNameOfPropertyWEx(model, model);
            ASSERT(res == null);

            res = engine.GetNameOfPropertyWEx(model, length);
            ASSERT(res == "length");
            res = engine.GetNameOfPropertyWEx(model, material);
            ASSERT(res == "material");

            engine.CloseModel(model);
            }
        }
    }
