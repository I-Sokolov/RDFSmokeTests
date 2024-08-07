﻿using Microsoft.VisualBasic;
using RDF;
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
        class ThreadInfo
        {
            public string type;
            public int num;
            public ThreadInfo(string type, int num) { this.type = type; this.num = num; }
        }

        public static void Test()
        {
            ENTER_TEST();

            ThreadProc(new ThreadInfo (null, 0));//do once in single thread to check all assertions correct

            const int N = 10;
            for (int i = 0; i < N; i++)
            {
                var th = new Thread(ThreadProc);

                //test working thread
                th.Start(new ThreadInfo("working thread", i));

                //test thread pull
                System.Threading.ThreadPool.QueueUserWorkItem(ThreadProc, new ThreadInfo("pulled thread", i));

                //test task
                Task.Factory.StartNew(ThreadProc, new ThreadInfo("task", i));
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

        static void ThreadProc(object oti)
        {
            var ti = (ThreadInfo)oti;
            if (ti.type != null)
                Thread.Sleep(10 * (10 - ti.num % 10));

            long model = 0;

            //open null-model test
            model = ifcengine.sdaiOpenModelBN(0,"NotExist", "IFC4");
            ASSERT(model == 0);

            //
            model = ifcengine.sdaiOpenModelBN(0,"..\\TestData\\ModelCheckerIFC4x3.ifc", "IFC4x3");
            ASSERT(model != 0);

            //get data test
            for (int i = 0; i < 3; i++)
            {
                IFC4x3.IfcUnitAssignment unitsAssmt = RDF.ifcengine.internalGetInstanceFromP21Line(model, 9);
                ASSERT(unitsAssmt != 0);

                var units = unitsAssmt.Units;
                ASSERT(units.Count == 6);
                int cntNamed = 0;
                foreach (var unit in units)
                {
                    var named = unit.IfcNamedUnit;
                    if (named != 0)
                    {
                        var a = named.UnitType; 
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

                pset.Name = ("NewName");

                var props = pset.HasProperties;
                ASSERT(props.Count == 7 - i);
                props.RemoveAt(0);
                pset.put_HasProperties(props);
            }

            if (ti.type!=null)
                Console.WriteLine("\t\t\tmulti-thread test: {0} #{1} finished successfully", ti.type, ti.num);
        }
    }
}
