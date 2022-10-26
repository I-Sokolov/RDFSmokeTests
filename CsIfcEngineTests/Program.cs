using System;
using CommandLine;

namespace CsIfcEngineTests
{
    class Program
    {
        public class Options
        {
            [CommandLine.Option("ExpressParsing", Default = false, HelpText = "Include EXPRESS schema parsing tests")]
            public bool ExpressParsing { get; set; }
        }

        static int Main(string[] args)
        {
            try
            {
                Options options = null;
                Parser.Default.ParseArguments<Options>(args).WithParsed<Options>(o => { options = o; });
                if (options == null)
                {
                    return -1;//--help option
                }

                Console.WriteLine("Perform EXPRESS parsing tests: {0}", options.ExpressParsing);

                Console.WriteLine("--------- Starting IFC engine C# tests");

                EarlyBinding_IFC.Test();
                EarlyBinding_AP242.Run();
                EarlyBinding_GuideExample.Run();

                if (options.ExpressParsing)
                {
                    ExpressParser.Run();
                }

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
