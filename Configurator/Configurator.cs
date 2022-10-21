using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Configurator
{
    public partial class Configurator : Form
    {
        Program.ExitCode exitCode;

        internal Configurator(Program.ExitCode exitCode)
        {
            this.exitCode = exitCode;

            InitializeComponent();

            DataExchange(true);
        }

        private void OnClick(object sender, EventArgs e)
        {
            DataExchange(false);
            
            exitCode.value = 0;
            Close();
        }

        private void DataExchange(bool load)
        {
            

        }
    }
}
