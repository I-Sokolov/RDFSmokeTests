using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Configurator
{
    static class Program
    {
        internal class ExitCode
        {
            public int value = -1;
        }

        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [STAThread]
        static int Main()
        {
            try
            {
                var exitCode = new ExitCode();

                Application.SetHighDpiMode(HighDpiMode.SystemAware);
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run(new Configurator(exitCode));

                return exitCode.value;
            }
            catch (Exception e)
            {
                System.Windows.Forms.MessageBox.Show (e.ToString(), e.Message);
                return 13;
            }
        }
    }
}
