using System;
using CommandLine;

namespace CsIfcEngineTests
{
    class Program
    {
        /*public class Options
        {
            [CommandLine.Option("ExpressParsing", Default = false, HelpText = "Include EXPRESS schema parsing tests")]
            public bool ExpressParsing { get; set; }
        }*/

        static int Main(string[] args)
        {
            try
            {
           /*     Options options = null;
                Parser.Default.ParseArguments<Options>(args).WithParsed<Options>(o => { options = o; });
                if (options == null)
                {
                    return -1;//--help option
                }

                Console.WriteLine("Perform EXPRESS parsing tests: {0}", options.ExpressParsing);
           */
                Console.WriteLine("--------- Starting IFC engine C# tests");
                //RDF.ifcengine.setStringUnicode(1);`

                MixedTests.Run();
                RegionalChars.Run();
                ExpressParser.Run();
                PutGetAttr.Run();
                MultiThreading.Test();
                EarlyBinding_IFC4.Test();
                EarlyBinding_IFC4x4.Test();
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
