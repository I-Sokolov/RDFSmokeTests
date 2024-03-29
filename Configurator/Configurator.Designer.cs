﻿
namespace Configurator
{
    partial class Configurator
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.btnRun = new System.Windows.Forms.Button();
            this.chkOnlyKernel = new System.Windows.Forms.CheckBox();
            this.cbIncludePath = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.cbLibFile = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.cbDllFile = new System.Windows.Forms.ComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this.cbEngineCs = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.cbIfcEngineCs = new System.Windows.Forms.ComboBox();
            this.label6 = new System.Windows.Forms.Label();
            this.cbEbApi = new System.Windows.Forms.ComboBox();
            this.label8 = new System.Windows.Forms.Label();
            this.cbGeomCs = new System.Windows.Forms.ComboBox();
            this.chkToolBoxEx = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // btnRun
            // 
            this.btnRun.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.btnRun.Location = new System.Drawing.Point(383, 563);
            this.btnRun.Name = "btnRun";
            this.btnRun.Size = new System.Drawing.Size(74, 29);
            this.btnRun.TabIndex = 0;
            this.btnRun.Text = "Run";
            this.btnRun.UseVisualStyleBackColor = true;
            this.btnRun.Click += new System.EventHandler(this.OnOK);
            // 
            // chkOnlyKernel
            // 
            this.chkOnlyKernel.AutoSize = true;
            this.chkOnlyKernel.Location = new System.Drawing.Point(12, 24);
            this.chkOnlyKernel.Name = "chkOnlyKernel";
            this.chkOnlyKernel.Size = new System.Drawing.Size(172, 24);
            this.chkOnlyKernel.TabIndex = 1;
            this.chkOnlyKernel.Text = "Test Geometry Kernel";
            this.chkOnlyKernel.UseVisualStyleBackColor = true;
            this.chkOnlyKernel.CheckedChanged += new System.EventHandler(this.chkOnlyKernel_CheckedChanged);
            // 
            // cbIncludePath
            // 
            this.cbIncludePath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cbIncludePath.FormattingEnabled = true;
            this.cbIncludePath.Location = new System.Drawing.Point(12, 84);
            this.cbIncludePath.Name = "cbIncludePath";
            this.cbIncludePath.Size = new System.Drawing.Size(829, 28);
            this.cbIncludePath.TabIndex = 2;
            this.cbIncludePath.TextChanged += new System.EventHandler(this.OnSettingChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 61);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(133, 20);
            this.label1.TabIndex = 3;
            this.label1.Text = "C++ includes path:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 115);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(103, 20);
            this.label3.TabIndex = 5;
            this.label3.Text = "LIB pathname:";
            // 
            // cbLibFile
            // 
            this.cbLibFile.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cbLibFile.FormattingEnabled = true;
            this.cbLibFile.Location = new System.Drawing.Point(12, 138);
            this.cbLibFile.Name = "cbLibFile";
            this.cbLibFile.Size = new System.Drawing.Size(829, 28);
            this.cbLibFile.TabIndex = 4;
            this.cbLibFile.TextChanged += new System.EventHandler(this.OnSettingChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(13, 174);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(108, 20);
            this.label2.TabIndex = 7;
            this.label2.Text = "DLL pathname:";
            // 
            // cbDllFile
            // 
            this.cbDllFile.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cbDllFile.FormattingEnabled = true;
            this.cbDllFile.Location = new System.Drawing.Point(13, 197);
            this.cbDllFile.Name = "cbDllFile";
            this.cbDllFile.Size = new System.Drawing.Size(829, 28);
            this.cbDllFile.TabIndex = 6;
            this.cbDllFile.TextChanged += new System.EventHandler(this.OnSettingChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(14, 239);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(125, 20);
            this.label4.TabIndex = 9;
            this.label4.Text = "path to engine.cs:";
            // 
            // cbEngineCs
            // 
            this.cbEngineCs.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cbEngineCs.FormattingEnabled = true;
            this.cbEngineCs.Location = new System.Drawing.Point(14, 262);
            this.cbEngineCs.Name = "cbEngineCs";
            this.cbEngineCs.Size = new System.Drawing.Size(829, 28);
            this.cbEngineCs.TabIndex = 8;
            this.cbEngineCs.TextChanged += new System.EventHandler(this.OnSettingChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(14, 359);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(141, 20);
            this.label5.TabIndex = 11;
            this.label5.Text = "path to ifcengine.cs:";
            // 
            // cbIfcEngineCs
            // 
            this.cbIfcEngineCs.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cbIfcEngineCs.FormattingEnabled = true;
            this.cbIfcEngineCs.Location = new System.Drawing.Point(14, 382);
            this.cbIfcEngineCs.Name = "cbIfcEngineCs";
            this.cbIfcEngineCs.Size = new System.Drawing.Size(829, 28);
            this.cbIfcEngineCs.TabIndex = 10;
            this.cbIfcEngineCs.TextChanged += new System.EventHandler(this.OnSettingChanged);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(13, 425);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(236, 20);
            this.label6.TabIndex = 13;
            this.label6.Text = "path to early-bound EXPRESS APIs";
            // 
            // cbEbApi
            // 
            this.cbEbApi.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cbEbApi.FormattingEnabled = true;
            this.cbEbApi.Location = new System.Drawing.Point(13, 452);
            this.cbEbApi.Name = "cbEbApi";
            this.cbEbApi.Size = new System.Drawing.Size(829, 28);
            this.cbEbApi.TabIndex = 12;
            this.cbEbApi.TextChanged += new System.EventHandler(this.OnSettingChanged);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(14, 294);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(136, 20);
            this.label8.TabIndex = 17;
            this.label8.Text = "path to geom.cs/.h:";
            // 
            // cbGeomCs
            // 
            this.cbGeomCs.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cbGeomCs.FormattingEnabled = true;
            this.cbGeomCs.Location = new System.Drawing.Point(14, 317);
            this.cbGeomCs.Name = "cbGeomCs";
            this.cbGeomCs.Size = new System.Drawing.Size(829, 28);
            this.cbGeomCs.TabIndex = 16;
            this.cbGeomCs.TextChanged += new System.EventHandler(this.OnSettingChanged);
            // 
            // chkToolBoxEx
            // 
            this.chkToolBoxEx.AutoSize = true;
            this.chkToolBoxEx.Location = new System.Drawing.Point(14, 512);
            this.chkToolBoxEx.Name = "chkToolBoxEx";
            this.chkToolBoxEx.Size = new System.Drawing.Size(134, 24);
            this.chkToolBoxEx.TabIndex = 18;
            this.chkToolBoxEx.Text = "ToolBoxEx tests";
            this.chkToolBoxEx.UseVisualStyleBackColor = true;
            this.chkToolBoxEx.CheckedChanged += new System.EventHandler(this.OnSettingChanged);
            // 
            // Configurator
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(853, 604);
            this.Controls.Add(this.chkToolBoxEx);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.cbGeomCs);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.cbEbApi);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.cbIfcEngineCs);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.cbEngineCs);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.cbDllFile);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.cbLibFile);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.cbIncludePath);
            this.Controls.Add(this.chkOnlyKernel);
            this.Controls.Add(this.btnRun);
            this.Name = "Configurator";
            this.Text = "Configurator";
            this.Load += new System.EventHandler(this.OnFormLoad);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnRun;
        private System.Windows.Forms.CheckBox chkOnlyKernel;
        private System.Windows.Forms.ComboBox cbIncludePath;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox cbLibFile;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox cbDllFile;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox cbEngineCs;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ComboBox cbIfcEngineCs;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ComboBox cbEbApi;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.ComboBox cbGeomCs;
        private System.Windows.Forms.CheckBox chkToolBoxEx;
    }
}

