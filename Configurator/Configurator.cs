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

        string ENV_ONLY_KERNEL = "RDF_TEST_ONLY_KERNEL";
        string ENV_INCLUDE_PATH = "RDF_TEST_INCLUDE_PATH";
        string ENV_LIB = "RDF_TEST_LIB";
        string ENV_DLL = "RDF_TEST_DLL";
        string ENV_ENGINE_CS = "RDF_TEST_ENGINE_CS";
        string ENV_IFCENGINE_CS = "RDF_TEST_IFCENGINE_CS";
        string ENV_GEOM_CS = "RDF_TEST_GEOM_CS";
        string ENV_IFC4_CS = "RDF_TEST_IFC4_CS";
        string ENV_AP242_CS = "RDF_TEST_AP242_CS";
        string ENV_CSIFC_CMDLINE = "RDF_TEST_CSIFC_CMDLINE";

        Dictionary<string, string> config = new Dictionary<string, string>();
        
        HashSet<object> modifiedCtrls = new HashSet<object>();
        bool loadingSettings = false;

        internal Configurator(Program.ExitCode exitCode)
        {
            this.exitCode = exitCode;

            InitializeComponent();

            SettingsExchange(true);
        }

        private void OnClick(object sender, EventArgs e)
        {
            if (IsValid())
            {
                SettingsExchange(false);
                WriteConfig();
                exitCode.value = 0;
                Close();
            }
        }

        void WriteConfig ()
        {
            var exepath = System.Reflection.Assembly.GetEntryAssembly().Location;
            var folder = System.IO.Path.GetDirectoryName(exepath);
            var cfgFile = System.IO.Path.Combine(folder, "SetConfig.bat");

            using (var writer = new System.IO.StreamWriter(cfgFile))
            {
                writer.WriteLine("@echo on");
                foreach (var v in config)
                {
                    writer.WriteLine("SET {0}={1}", v.Key, v.Value);
                }
                writer.WriteLine("@echo off");
            }
        }

        private bool IsValid()
        {
            return CheckDirExist(cbIncludePath)
                && CheckFileExist(cbLibFile)
                && CheckFileExist(cbDllFile)
                && CheckDirExist(cbEngineCs)
                && CheckDirExist(cbGeomCs)
                && (chkOnlyKernel.Checked ||
                      CheckDirExist(cbIfcEngineCs)
                      && CheckDirExist(cbIFC4cs)
                      && CheckDirExist(cbAP242cs)
                );
        }

        private bool CheckDirExist(ComboBox dir)
        {
            if (dir.Text.Length > 0 && System.IO.Directory.Exists(dir.Text))
            {
                return true;
            }
            else
            {
                MessageBox.Show("Directory does not exist: " + dir.Text);
                dir.Focus();
                return false;
            }
        }

        private bool CheckFileExist(ComboBox file)
        {
            if (file.Text.Length > 0 && System.IO.File.Exists(file.Text))
            {
                return true;
            }
            else
            {
                MessageBox.Show("File does not exist: " + file.Text);
                file.Focus();
                return false;
            }
        }

        private void SettingsExchange(bool load)
        {
            loadingSettings = true;

            if (load)
            {
                Settings.Default.Reload();
            }

            SettingsExchange(cbIncludePath, ENV_INCLUDE_PATH, load);
            SettingsExchange(cbLibFile, ENV_LIB, load);
            SettingsExchange(cbDllFile, ENV_DLL, load);
            SettingsExchange(cbEngineCs, ENV_ENGINE_CS, load);
            SettingsExchange(cbGeomCs, ENV_GEOM_CS, load);
            SettingsExchange(cbIfcEngineCs, ENV_IFCENGINE_CS, load);
            SettingsExchange(cbIFC4cs, ENV_IFC4_CS, load);
            SettingsExchange(cbAP242cs, ENV_AP242_CS, load);

            SettingsExchange(chkOnlyKernel, load);
            SettingsExchange(сhkExpressParser, load);

            if (!load)
            {
                SetOptionsConfig();
            }

            if (load)
            {
                UpdateUI();
            }
            else
            {
                Settings.Default.Save();
            }

            loadingSettings = false;
        }

        void SetOptionsConfig ()
        {
            config.Add(ENV_ONLY_KERNEL, chkOnlyKernel.Checked ? "1" : "0");

            string cmdCsIfc = "";

            if (сhkExpressParser.Checked)
            {
                cmdCsIfc += " --ExpressParsing";
            }

            config.Add(ENV_CSIFC_CMDLINE, cmdCsIfc);
        }

        void SettingsExchange (CheckBox chk, bool load)
        {
            var name = chk.Name;

            if (load)
            {
                if (!modifiedCtrls.Contains(chk))
                    chk.Checked = (bool)Settings.Default[name];
            }
            else
            {
                Settings.Default[name] = chk.Checked;
            }
        }

        void SettingsExchange(ComboBox cb, string envVar, bool load)
        {
            string pfx;
            if (chkOnlyKernel.Checked)
                pfx = "KernelTest_";
            else
                pfx = "IFCTest_";

            var valueProp = pfx + cb.Name + "_value";
            var historyProp = cb.Name + "_history";

            if (load)
            {
                if (!modifiedCtrls.Contains(cb))
                {
                    var history = (System.Collections.Specialized.StringCollection)Settings.Default[historyProp];
                    if (history != null)
                    {
                        foreach (var item in history)
                        {
                            if (item.Length > 0)
                                cb.Items.Add(item);
                        }
                    }

                    var value = (string)Settings.Default[valueProp];
                    if (value != null)
                    {
                        cb.Text = value;
                    }
                }
            }
            else
            {
                var value = cb.Text;
                Settings.Default[valueProp] = value;
                config.Add(envVar, value);

                var historySet = new HashSet<string>();
                foreach (var item in cb.Items)
                {
                    historySet.Add((string)item);
                }
                var history = new System.Collections.Specialized.StringCollection();
                foreach (var item in historySet)
                {
                    history.Add(item);
                }
                history.Add(value);
                Settings.Default[historyProp] = history;
            }
        }


        private void UpdateUI()
        {
            cbIfcEngineCs.Enabled = !chkOnlyKernel.Checked;
            cbIFC4cs.Enabled = !chkOnlyKernel.Checked;
            cbAP242cs.Enabled = !chkOnlyKernel.Checked;
            сhkExpressParser.Enabled = !chkOnlyKernel.Checked;
        }

        private void chkOnlyKernel_CheckedChanged(object sender, EventArgs e)
        {
            if (!loadingSettings)
            {
                modifiedCtrls.Add(sender);
                SettingsExchange(true);
            }
        }

        private void OnSettingChanged(object sender, EventArgs e)
        {
            if (!loadingSettings)
            {
                modifiedCtrls.Add(sender);
            }
        }

    }
}
