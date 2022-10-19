using System;

namespace CsIfcEngineTests
{
    class Program
    {
        static int Main(string[] args)
        {
            try
            {
                Console.WriteLine("--------- Starting IFC engine C# tests");

                EarlyBinding_IFC.Test();

                Console.WriteLine("--------- Finished IFC engine C# tests");
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
