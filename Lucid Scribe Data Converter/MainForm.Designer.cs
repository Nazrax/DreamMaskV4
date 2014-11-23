namespace Lucid_Scribe_Data_Converter
{
  partial class MainForm
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
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
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
      this.tmrLoad = new System.Windows.Forms.Timer(this.components);
      this.pgbSatus = new System.Windows.Forms.ProgressBar();
      this.SuspendLayout();
      // 
      // tmrLoad
      // 
      this.tmrLoad.Enabled = true;
      this.tmrLoad.Interval = 1;
      this.tmrLoad.Tick += new System.EventHandler(this.tmrLoad_Tick);
      // 
      // pgbSatus
      // 
      this.pgbSatus.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.pgbSatus.Location = new System.Drawing.Point(12, 12);
      this.pgbSatus.Name = "pgbSatus";
      this.pgbSatus.Size = new System.Drawing.Size(360, 28);
      this.pgbSatus.TabIndex = 0;
      // 
      // MainForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.BackColor = System.Drawing.Color.LightSteelBlue;
      this.ClientSize = new System.Drawing.Size(384, 52);
      this.Controls.Add(this.pgbSatus);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "MainForm";
      this.Text = "Lucid Scribe Data Converter";
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.Timer tmrLoad;
    private System.Windows.Forms.ProgressBar pgbSatus;
  }
}

