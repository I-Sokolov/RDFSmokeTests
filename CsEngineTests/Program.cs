using System;

namespace CsEngineTests
{
    class Program
    {
        static int Main(string[] args)
        {
            try
            {
                Console.WriteLine("--------- Starting Geometry Kernel C# tests");

                EarlyBinding.Run();

                Console.WriteLine("--------- Finished Geometry Kernel C# tests");
                return 0;
            }
            catch (System.ApplicationException e)
            {
                System.Console.WriteLine(e.ToString());
                System.Console.WriteLine(e.Message);
                return 13;
            }
        }
    }
}
