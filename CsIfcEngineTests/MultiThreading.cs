using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace CsIfcEngineTests 
{
    class MultiThreading : CsTests.TestBase
    {
        public static void Test()
        {
            ENTER_TEST();

            ThreadProc(-1);//do once in single thread to check all assertions correct

            var threads = new List<Thread>();

            int N = 10;
            while (N-->0)
            {
                threads.Add(new Thread(ThreadProc));
            }

            foreach (var th in threads)
            {
                th.Start(++N);
            }

            /*
            int cnt = -1;
            while (cnt != 0)
            {
                cnt = 0;
                foreach (var th in threads)
                {
                    if (th.IsAlive)
                        cnt++;
                }
            }*/

        }

        public static void ThreadProc(object onum)
        {
            int num = (int)onum;
            if (num >= 0)
                Thread.Sleep(10 * (10 - num % 10));

            long model = 0;

            //open model test
            model = RDF.ifcengine.sdaiOpenModelBN(0, "NotExist.ifc", "IFC4");
            ASSERT(model == 0);

            model = RDF.ifcengine.sdaiOpenModelBN(0, "..\\TestData\\ModelCheckerTESTSWE_UT_LP_4.ifc", "IFC4x3");
            ASSERT(model != 0);

            //get data test
            for (int i = 0; i < 3; i++)
            {
                IFC4x3.IfcUnitAssignment unitsAssmt = RDF.ifcengine.internalGetInstanceFromP21Line(model, 9);
                ASSERT(unitsAssmt != 0);

                var units = unitsAssmt.get_Units();
                ASSERT(units.Count == 6);
                int cntNamed = 0;
                foreach (var unit in units)
                {
                    var named = unit.get_IfcNamedUnit();
                    if (named != 0)
                    {
                        named.get_UnitType();
                        cntNamed++;
                    }
                }
                ASSERT(cntNamed == 6);
            }

            //modify data test
            for (int i = 0; i < 3; i++)
            {
                IFC4x3.IfcPropertySet pset = RDF.ifcengine.internalGetInstanceFromP21Line(model, 84);
                ASSERT(pset != 0);

                pset.put_Name("NewName");

                var props = pset.get_HasProperties();
                ASSERT(props.Count == 7 - i);
                props.RemoveAt(0);
                pset.put_HasProperties(props);
            }

            if (num >= 0)
                Console.WriteLine("Multi-thrad test: thread #{0} finished successfully", num);
        }
    }
}
