using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CsTests
{
    class TestBase
    {
        protected static void ENTER_TEST(string testName = "")
        {
            var t = new System.Diagnostics.StackTrace();
            var f = t.GetFrame(1);
            var n = f.GetMethod().ReflectedType.Name + "." + f.GetMethod().Name;
            
            System.Console.WriteLine("Running {0} {1}", n, testName);
        }

        protected static void ASSERT(bool condition)
        {
            if (!condition)
            {
                throw new ApplicationException("ASSERT failed");
            }
        }

        protected static void ASSERT(Int64 condition) 
        {
            if (condition==0)
            {
                throw new ApplicationException("ASSERT failed");
            }
        }

        protected static void ASSERT_EQ(IEnumerable lst1, IEnumerable lst2)
        {
            var it1 = lst1.GetEnumerator();
            var it2 = lst2.GetEnumerator();

            bool m1 = it1.MoveNext();
            bool m2 = it2.MoveNext();
            while (m1 && m2)
            {
                var cmp1 = it1.Current as IComparable;
                var cmp2 = it2.Current as IComparable;
                if (cmp1 != null && cmp2 != null)
                {
                    ASSERT(cmp1.Equals(cmp2));
                }
                else
                {
                    var lst11 = it1.Current as IEnumerable;
                    var lst22 = it2.Current as IEnumerable;
                    if (lst11 != null && lst22 != null)
                    {
                        ASSERT_EQ(lst11, lst22);
                    }
                    else
                    {
                        ASSERT(false); //no comparision is implemented
                    }
                }

                m1 = it1.MoveNext();
                m2 = it2.MoveNext();
            }

            ASSERT(!m1 && !m2);
        }
    }
}
