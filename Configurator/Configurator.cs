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
        
        HashSet<object> modifiedCtrls = new HashSet<object>();
        bool loadingSettings = false;

        internal Configurator(Program.ExitCode exitCode)
        {
            this.exitCode = exitCode;

            InitializeComponent();

            SettingsExchange(true);
        }

        private void OnOK(object sender, EventArgs e)
        {
            if (IsValid())
            {
                SettingsExchange(false);
                CreateConfiguration();
                Close();
            }
        }

        void CreateConfiguration()
        {
            try
            {
                //var exepath = System.Reflection.Assembly.GetEntryAssembly().Location;
                //var folder = System.IO.Path.GetDirectoryName(exepath);
                var folder = System.IO.Directory.GetCurrentDirectory();
                var cfgFile = System.IO.Path.Combine(folder, "SetConfig.bat");

                using (var writer = new System.IO.StreamWriter(cfgFile))
                {
                    CreateConfiguration(folder, writer);
                }

                exitCode.value = 0;
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
                Console.WriteLine();
                Console.WriteLine(e.Message);
                System.Windows.Forms.MessageBox.Show (e.ToString(), e.Message);
                exitCode.value = 13;
            }
        }

        [System.Runtime.InteropServices.DllImport("kernel32.dll")]
        static extern bool CreateSymbolicLink(string lpSymlinkFileName, string lpTargetFileName, SymbolicLink dwFlags);
        enum SymbolicLink
        {
            File = 0,
            Directory = 1
        }

        void CopyFile (string srcPath, string srcFile, string targetFolder, string targetFile, System.IO.StreamWriter cfgFile)
        {
            if (srcFile != null)
            {
                srcPath = System.IO.Path.Combine(srcPath, srcFile);
            }

            if (targetFile == null)
            {
                targetFile = System.IO.Path.GetFileName(srcPath);
            }

            string targetPath = System.IO.Path.Combine(targetFolder, targetFile);

            if (System.IO.File.Exists (targetPath))
            {
                System.IO.File.Delete(targetPath);
                cfgFile.WriteLine("echo Deleted {0}" + targetPath);
            }

            if (CreateSymbolicLink(targetPath, srcPath, SymbolicLink.File))
                cfgFile.WriteLine("echo SYMLINK {0} to {1}", srcPath, targetPath);
            else
                throw new System.ApplicationException("Failed CreateSymbolicLink(" + targetPath + ", " + srcPath + ", SymbolicLink.File)");
        }

        void CreateConfiguration(string folder, System.IO.StreamWriter cfgFile)
        {
            cfgFile.WriteLine("@echo on");

            cfgFile.WriteLine("@echo ************************ Configuration *************************");
            cfgFile.WriteLine("@echo .");

            cfgFile.WriteLine("SET RDF_TEST_ONLY_KERNEL={0}", chkOnlyKernel.Checked ? "1" : "0");
            cfgFile.WriteLine("SET RDF_TEST_INCLUDE_PATH={0}", cbIncludePath.Text);
            cfgFile.WriteLine("SET RDF_TEST_LIB={0}", cbLibFile.Text);

            string cmdLineCsIfc = "";
            if (сhkExpressParser.Checked)
            {
                cmdLineCsIfc += " --ExpressParsing";
            }
            cfgFile.WriteLine("SET RDF_TEST_CSIFC_CMDLINE={0}", cmdLineCsIfc);

            cfgFile.WriteLine("@echo off");

            CopyFile(cbDllFile.Text, null, folder, null, cfgFile);
            CopyFile(cbEngineCs.Text, "engine.cs", folder, null, cfgFile);
            CopyFile(cbGeomCs.Text, "geom.cs", folder, null, cfgFile);

            if (!chkOnlyKernel.Checked)
            {
                CopyFile(cbIfcEngineCs.Text, "ifcengine.cs", folder, null, cfgFile);
                CopyFile(cbIFC4cs.Text, "IFC4.cs", folder, null, cfgFile);
                CopyFile(cbAP242cs.Text, "AP242.cs", folder, null, cfgFile);

                CopyFile(".", "ifcEngine.dll", ".", "engine.dll", cfgFile); //trick because engine.cs refers to engine.dll, not ifcengine.dll
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

            SettingsExchange(cbIncludePath, load);
            SettingsExchange(cbLibFile, load);
            SettingsExchange(cbDllFile, load);
            SettingsExchange(cbEngineCs, load);
            SettingsExchange(cbGeomCs, load);
            SettingsExchange(cbIfcEngineCs, load);
            SettingsExchange(cbIFC4cs, load);
            SettingsExchange(cbAP242cs, load);

            SettingsExchange(chkOnlyKernel, load);
            SettingsExchange(сhkExpressParser, load);

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

        void SettingsExchange(ComboBox cb, bool load)
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

        //[System.Runtime.InteropServices.DllImport("kernel32.dll", SetLastError = true)]
        //static extern bool AllocConsole();

        private void OnFormLoad(object sender, EventArgs e)
        {
            /*if (!AllocConsole())
            {
                System.Windows.Forms.MessageBox.Show("Can not allow console!");
            }*/
        }
    }
}
