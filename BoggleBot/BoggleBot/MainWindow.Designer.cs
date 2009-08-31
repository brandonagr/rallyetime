namespace BoggleBot
{
    partial class BoggleBotWindow
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
			this.statusStrip1 = new System.Windows.Forms.StatusStrip();
			this.listBox1 = new System.Windows.Forms.ListBox();
			this.gridSize_ComboBox = new System.Windows.Forms.ComboBox();
			this.randomGrid_Button = new System.Windows.Forms.Button();
			this.menuStrip1 = new System.Windows.Forms.MenuStrip();
			this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.dictionaryToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.loadNewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.emptyExistingToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.statsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.panel1 = new BoggleBot.DoubleBufferPanel();
			this.menuStrip1.SuspendLayout();
			this.SuspendLayout();
			// 
			// statusStrip1
			// 
			this.statusStrip1.Location = new System.Drawing.Point(0, 532);
			this.statusStrip1.Name = "statusStrip1";
			this.statusStrip1.Size = new System.Drawing.Size(682, 22);
			this.statusStrip1.TabIndex = 0;
			this.statusStrip1.Text = "statusStrip1";
			// 
			// listBox1
			// 
			this.listBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.listBox1.FormattingEnabled = true;
			this.listBox1.Location = new System.Drawing.Point(506, 55);
			this.listBox1.Name = "listBox1";
			this.listBox1.Size = new System.Drawing.Size(172, 472);
			this.listBox1.TabIndex = 1;
			// 
			// gridSize_ComboBox
			// 
			this.gridSize_ComboBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.gridSize_ComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.gridSize_ComboBox.FormattingEnabled = true;
			this.gridSize_ComboBox.Items.AddRange(new object[] {
            "4x4",
            "5x5",
            "6x6",
            "7x7",
            "8x8"});
			this.gridSize_ComboBox.Location = new System.Drawing.Point(506, 27);
			this.gridSize_ComboBox.Name = "gridSize_ComboBox";
			this.gridSize_ComboBox.Size = new System.Drawing.Size(86, 21);
			this.gridSize_ComboBox.TabIndex = 2;
			this.gridSize_ComboBox.SelectedIndexChanged += new System.EventHandler(this.gridSize_ComboBox_SelectedIndexChanged);
			// 
			// randomGrid_Button
			// 
			this.randomGrid_Button.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.randomGrid_Button.Location = new System.Drawing.Point(598, 26);
			this.randomGrid_Button.Name = "randomGrid_Button";
			this.randomGrid_Button.Size = new System.Drawing.Size(80, 21);
			this.randomGrid_Button.TabIndex = 3;
			this.randomGrid_Button.Text = "Random Grid";
			this.randomGrid_Button.UseVisualStyleBackColor = true;
			this.randomGrid_Button.Click += new System.EventHandler(this.randomGrid_Button_Click);
			// 
			// menuStrip1
			// 
			this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.dictionaryToolStripMenuItem});
			this.menuStrip1.Location = new System.Drawing.Point(0, 0);
			this.menuStrip1.Name = "menuStrip1";
			this.menuStrip1.Size = new System.Drawing.Size(682, 24);
			this.menuStrip1.TabIndex = 5;
			this.menuStrip1.Text = "menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.exitToolStripMenuItem});
			this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
			this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
			this.fileToolStripMenuItem.Text = "File";
			// 
			// exitToolStripMenuItem
			// 
			this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
			this.exitToolStripMenuItem.Size = new System.Drawing.Size(103, 22);
			this.exitToolStripMenuItem.Text = "Exit";
			// 
			// dictionaryToolStripMenuItem
			// 
			this.dictionaryToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.loadNewToolStripMenuItem,
            this.emptyExistingToolStripMenuItem,
            this.statsToolStripMenuItem});
			this.dictionaryToolStripMenuItem.Name = "dictionaryToolStripMenuItem";
			this.dictionaryToolStripMenuItem.Size = new System.Drawing.Size(67, 20);
			this.dictionaryToolStripMenuItem.Text = "Dictionary";
			// 
			// loadNewToolStripMenuItem
			// 
			this.loadNewToolStripMenuItem.Name = "loadNewToolStripMenuItem";
			this.loadNewToolStripMenuItem.Size = new System.Drawing.Size(155, 22);
			this.loadNewToolStripMenuItem.Text = "Load New";
			// 
			// emptyExistingToolStripMenuItem
			// 
			this.emptyExistingToolStripMenuItem.Name = "emptyExistingToolStripMenuItem";
			this.emptyExistingToolStripMenuItem.Size = new System.Drawing.Size(155, 22);
			this.emptyExistingToolStripMenuItem.Text = "Empty Existing";
			// 
			// statsToolStripMenuItem
			// 
			this.statsToolStripMenuItem.Name = "statsToolStripMenuItem";
			this.statsToolStripMenuItem.Size = new System.Drawing.Size(155, 22);
			this.statsToolStripMenuItem.Text = "Stats";
			// 
			// panel1
			// 
			this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.panel1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(224)))), ((int)(((byte)(192)))));
			this.panel1.Location = new System.Drawing.Point(0, 27);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(500, 500);
			this.panel1.TabIndex = 6;
			this.panel1.Paint += new System.Windows.Forms.PaintEventHandler(this.panel1_Paint);
			// 
			// BoggleBotWindow
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(682, 554);
			this.Controls.Add(this.panel1);
			this.Controls.Add(this.randomGrid_Button);
			this.Controls.Add(this.gridSize_ComboBox);
			this.Controls.Add(this.listBox1);
			this.Controls.Add(this.statusStrip1);
			this.Controls.Add(this.menuStrip1);
			this.MainMenuStrip = this.menuStrip1;
			this.Name = "BoggleBotWindow";
			this.Text = "BoggleBot";
			this.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.BoggleBotWindow_KeyPress);
			this.Resize += new System.EventHandler(this.BoggleBotWindow_Resize);
			this.menuStrip1.ResumeLayout(false);
			this.menuStrip1.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ListBox listBox1;
        private System.Windows.Forms.ComboBox gridSize_ComboBox;
        private System.Windows.Forms.Button randomGrid_Button;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem dictionaryToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem loadNewToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem emptyExistingToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem statsToolStripMenuItem;
        private DoubleBufferPanel panel1;
    }
}

