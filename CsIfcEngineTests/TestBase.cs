﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CsIfcEngineTests
{
    class TestBase
    {
        protected static void ASSERT (bool condition)
        {
            if (!condition)
            {
                throw new ApplicationException ("ASSERT failed");
            }
        }

        protected static void ENTER_TEST()
        {
            var t = new System.Diagnostics.StackTrace();
            var f = t.GetFrame(t.FrameCount - 2);
            var n = f.GetMethod().ReflectedType.Name + "." + f.GetMethod().Name;
            
            System.Console.WriteLine("Running {0}.", n);
        }
    }
}
