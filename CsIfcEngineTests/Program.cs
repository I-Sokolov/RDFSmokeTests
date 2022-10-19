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
                EarlyBinding_AP242.Run();
                EarlyBinding_GuideExample.Run();

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
