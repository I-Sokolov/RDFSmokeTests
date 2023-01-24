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
        public static void Test ()
        {
            ENTER_TEST();

            ThreadProc(0);//do once in single thread

            var threads = new List<Thread>();
            const int N = 50;

            for (int i = 0; i < N; i++)
            {
                threads.Add (new Thread(ThreadProc));
            }

            var rand = new Random();
            foreach (var th in threads)
            {
                th.Start(rand.Next(5));
            }

            int cnt = -1;
            while (cnt != 0)
            {
                cnt = 0;
                foreach (var th in threads)
                {
                    if (th.IsAlive)
                        cnt++;
                }
                Console.WriteLine("Numeber of active threads: {0}", cnt);
            }

        }

        public static void ThreadProc(object delay)
        {
            Thread.Sleep((int)delay);
            //System.Console.WriteLine("Delay: {0}", delay);

            long model = 0;
            model = RDF.ifcengine.sdaiOpenModelBN(0, "mmmTest.ifc", "IFC4");
            ASSERT(model == 0);

            model = RDF.ifcengine.sdaiOpenModelBN(0, "..\\TestData\\ModelCheckerTESTSWE_UT_LP_4.ifc", "IFC4x3");
            ASSERT(model != 0);

            //RDF.ifcengine.sdaiCloseModel(model);           
        }
    }
}
