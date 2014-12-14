namespace HomeMediaCenterGUI
{
    partial class ParametersControl
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.othersGroupBox = new System.Windows.Forms.GroupBox();
            this.outBufSizeLabel = new System.Windows.Forms.Label();
            this.outBufSizeTextBox = new System.Windows.Forms.TextBox();
            this.subtitlesIntCheckBox = new System.Windows.Forms.CheckBox();
            this.endTimeTextBox = new System.Windows.Forms.TextBox();
            this.startTimeTextBox = new System.Windows.Forms.TextBox();
            this.startEndTimeLabel = new System.Windows.Forms.Label();
            this.fpsComboBox = new System.Windows.Forms.ComboBox();
            this.fpsLabel = new System.Windows.Forms.Label();
            this.audioGroupBox = new System.Windows.Forms.GroupBox();
            this.bitrateAudioComboBox = new System.Windows.Forms.ComboBox();
            this.bitrateAudioLabel = new System.Windows.Forms.Label();
            this.bitrateVideoLabel = new System.Windows.Forms.Label();
            this.bitrateVideoComboBox = new System.Windows.Forms.ComboBox();
            this.videoGroupBox = new System.Windows.Forms.GroupBox();
            this.scanLabel = new System.Windows.Forms.Label();
            this.scanComboBox = new System.Windows.Forms.ComboBox();
            this.resolutionLabel = new System.Windows.Forms.Label();
            this.resolutionComboBox = new System.Windows.Forms.ComboBox();
            this.keepAspectCheckBox = new System.Windows.Forms.CheckBox();
            this.containerGroupBox = new System.Windows.Forms.GroupBox();
            this.audioCheckBox = new System.Windows.Forms.CheckBox();
            this.videoCheckBox = new System.Windows.Forms.CheckBox();
            this.containerComboBox = new System.Windows.Forms.ComboBox();
            this.outputGroupBox = new System.Windows.Forms.GroupBox();
            this.outputTextBox = new System.Windows.Forms.ComboBox();
            this.browseOutputButton = new System.Windows.Forms.Button();
            this.inputGroupBox = new System.Windows.Forms.GroupBox();
            this.browseInputButton = new System.Windows.Forms.Button();
            this.inputTextBox = new System.Windows.Forms.ComboBox();
            this.paramGroupBox = new System.Windows.Forms.GroupBox();
            this.editCheckBox = new System.Windows.Forms.CheckBox();
            this.paramBox = new System.Windows.Forms.TextBox();
            this.infoTextBox = new System.Windows.Forms.RichTextBox();
            this.infoMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.copyMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.othersGroupBox.SuspendLayout();
            this.audioGroupBox.SuspendLayout();
            this.videoGroupBox.SuspendLayout();
            this.containerGroupBox.SuspendLayout();
            this.outputGroupBox.SuspendLayout();
            this.inputGroupBox.SuspendLayout();
            this.paramGroupBox.SuspendLayout();
            this.infoMenuStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // othersGroupBox
            // 
            this.othersGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.othersGroupBox.Controls.Add(this.outBufSizeLabel);
            this.othersGroupBox.Controls.Add(this.outBufSizeTextBox);
            this.othersGroupBox.Controls.Add(this.subtitlesIntCheckBox);
            this.othersGroupBox.Controls.Add(this.endTimeTextBox);
            this.othersGroupBox.Controls.Add(this.startTimeTextBox);
            this.othersGroupBox.Controls.Add(this.startEndTimeLabel);
            this.othersGroupBox.Location = new System.Drawing.Point(2, 249);
            this.othersGroupBox.Name = "othersGroupBox";
            this.othersGroupBox.Size = new System.Drawing.Size(605, 75);
            this.othersGroupBox.TabIndex = 50;
            this.othersGroupBox.TabStop = false;
            this.othersGroupBox.Text = "Others";
            // 
            // outBufSizeLabel
            // 
            this.outBufSizeLabel.Location = new System.Drawing.Point(4, 47);
            this.outBufSizeLabel.Name = "outBufSizeLabel";
            this.outBufSizeLabel.Size = new System.Drawing.Size(180, 13);
            this.outBufSizeLabel.TabIndex = 55;
            this.outBufSizeLabel.Text = "Output buffer size (kbyte):";
            // 
            // outBufSizeTextBox
            // 
            this.outBufSizeTextBox.Location = new System.Drawing.Point(189, 44);
            this.outBufSizeTextBox.MaxLength = 10;
            this.outBufSizeTextBox.Name = "outBufSizeTextBox";
            this.outBufSizeTextBox.Size = new System.Drawing.Size(90, 20);
            this.outBufSizeTextBox.TabIndex = 54;
            this.outBufSizeTextBox.EnabledChanged += new System.EventHandler(this.outBufSizeTextBox_Changed);
            this.outBufSizeTextBox.TextChanged += new System.EventHandler(this.outBufSizeTextBox_Changed);
            // 
            // subtitlesIntCheckBox
            // 
            this.subtitlesIntCheckBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.subtitlesIntCheckBox.AutoSize = true;
            this.subtitlesIntCheckBox.Location = new System.Drawing.Point(490, 20);
            this.subtitlesIntCheckBox.Name = "subtitlesIntCheckBox";
            this.subtitlesIntCheckBox.Size = new System.Drawing.Size(109, 17);
            this.subtitlesIntCheckBox.TabIndex = 53;
            this.subtitlesIntCheckBox.Text = "Integrate subtitles";
            this.subtitlesIntCheckBox.UseVisualStyleBackColor = true;
            this.subtitlesIntCheckBox.CheckedChanged += new System.EventHandler(this.subtitlesIntCheckBox_CheckedChanged);
            // 
            // endTimeTextBox
            // 
            this.endTimeTextBox.Location = new System.Drawing.Point(285, 18);
            this.endTimeTextBox.Name = "endTimeTextBox";
            this.endTimeTextBox.Size = new System.Drawing.Size(90, 20);
            this.endTimeTextBox.TabIndex = 52;
            this.endTimeTextBox.Text = "00:00:00";
            this.endTimeTextBox.TextChanged += new System.EventHandler(this.endTimeTextBox_TextChanged);
            // 
            // startTimeTextBox
            // 
            this.startTimeTextBox.Location = new System.Drawing.Point(189, 18);
            this.startTimeTextBox.Name = "startTimeTextBox";
            this.startTimeTextBox.Size = new System.Drawing.Size(90, 20);
            this.startTimeTextBox.TabIndex = 51;
            this.startTimeTextBox.Text = "00:00:00";
            this.startTimeTextBox.TextChanged += new System.EventHandler(this.startTimeTextBox_TextChanged);
            // 
            // startEndTimeLabel
            // 
            this.startEndTimeLabel.Location = new System.Drawing.Point(3, 21);
            this.startEndTimeLabel.Name = "startEndTimeLabel";
            this.startEndTimeLabel.Size = new System.Drawing.Size(180, 13);
            this.startEndTimeLabel.TabIndex = 4;
            this.startEndTimeLabel.Text = "Start time / end time (hh:mm:ss):";
            // 
            // fpsComboBox
            // 
            this.fpsComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.fpsComboBox.FormattingEnabled = true;
            this.fpsComboBox.Items.AddRange(new object[] {
            "Same as source",
            "5",
            "10",
            "15",
            "20",
            "25",
            "30",
            "40",
            "50",
            "60"});
            this.fpsComboBox.Location = new System.Drawing.Point(340, 42);
            this.fpsComboBox.Name = "fpsComboBox";
            this.fpsComboBox.Size = new System.Drawing.Size(120, 21);
            this.fpsComboBox.TabIndex = 35;
            this.fpsComboBox.SelectedIndexChanged += new System.EventHandler(this.fpsComboBox_Changed);
            this.fpsComboBox.EnabledChanged += new System.EventHandler(this.fpsComboBox_Changed);
            // 
            // fpsLabel
            // 
            this.fpsLabel.Location = new System.Drawing.Point(194, 47);
            this.fpsLabel.Name = "fpsLabel";
            this.fpsLabel.Size = new System.Drawing.Size(140, 13);
            this.fpsLabel.TabIndex = 8;
            this.fpsLabel.Text = "Frames per second:";
            // 
            // audioGroupBox
            // 
            this.audioGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.audioGroupBox.Controls.Add(this.bitrateAudioComboBox);
            this.audioGroupBox.Controls.Add(this.bitrateAudioLabel);
            this.audioGroupBox.Location = new System.Drawing.Point(2, 193);
            this.audioGroupBox.Name = "audioGroupBox";
            this.audioGroupBox.Size = new System.Drawing.Size(605, 50);
            this.audioGroupBox.TabIndex = 40;
            this.audioGroupBox.TabStop = false;
            this.audioGroupBox.Text = "Audio";
            // 
            // bitrateAudioComboBox
            // 
            this.bitrateAudioComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.bitrateAudioComboBox.FormattingEnabled = true;
            this.bitrateAudioComboBox.Items.AddRange(new object[] {
            "320 kbps",
            "256 kbps",
            "192 kbps",
            "128 kbps",
            "64 kbps"});
            this.bitrateAudioComboBox.Location = new System.Drawing.Point(108, 17);
            this.bitrateAudioComboBox.Name = "bitrateAudioComboBox";
            this.bitrateAudioComboBox.Size = new System.Drawing.Size(80, 21);
            this.bitrateAudioComboBox.TabIndex = 41;
            this.bitrateAudioComboBox.SelectedIndexChanged += new System.EventHandler(this.bitrateAudioComboBox_Changed);
            this.bitrateAudioComboBox.EnabledChanged += new System.EventHandler(this.bitrateAudioComboBox_Changed);
            // 
            // bitrateAudioLabel
            // 
            this.bitrateAudioLabel.Location = new System.Drawing.Point(3, 22);
            this.bitrateAudioLabel.Name = "bitrateAudioLabel";
            this.bitrateAudioLabel.Size = new System.Drawing.Size(100, 16);
            this.bitrateAudioLabel.TabIndex = 5;
            this.bitrateAudioLabel.Text = "Bit rate:";
            // 
            // bitrateVideoLabel
            // 
            this.bitrateVideoLabel.Location = new System.Drawing.Point(3, 22);
            this.bitrateVideoLabel.Name = "bitrateVideoLabel";
            this.bitrateVideoLabel.Size = new System.Drawing.Size(100, 13);
            this.bitrateVideoLabel.TabIndex = 4;
            this.bitrateVideoLabel.Text = "Bit rate:";
            // 
            // bitrateVideoComboBox
            // 
            this.bitrateVideoComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.bitrateVideoComboBox.FormattingEnabled = true;
            this.bitrateVideoComboBox.Items.AddRange(new object[] {
            "9000 kbps",
            "8000 kbps",
            "5000 kbps",
            "3000 kbps",
            "1500 kbps",
            "1000 kbps",
            "800 kbps",
            "384 kbps"});
            this.bitrateVideoComboBox.Location = new System.Drawing.Point(108, 17);
            this.bitrateVideoComboBox.Name = "bitrateVideoComboBox";
            this.bitrateVideoComboBox.Size = new System.Drawing.Size(80, 21);
            this.bitrateVideoComboBox.TabIndex = 31;
            this.bitrateVideoComboBox.SelectedIndexChanged += new System.EventHandler(this.bitrateVideoComboBox_Changed);
            this.bitrateVideoComboBox.EnabledChanged += new System.EventHandler(this.bitrateVideoComboBox_Changed);
            // 
            // videoGroupBox
            // 
            this.videoGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.videoGroupBox.Controls.Add(this.scanLabel);
            this.videoGroupBox.Controls.Add(this.scanComboBox);
            this.videoGroupBox.Controls.Add(this.resolutionLabel);
            this.videoGroupBox.Controls.Add(this.fpsComboBox);
            this.videoGroupBox.Controls.Add(this.bitrateVideoLabel);
            this.videoGroupBox.Controls.Add(this.fpsLabel);
            this.videoGroupBox.Controls.Add(this.bitrateVideoComboBox);
            this.videoGroupBox.Controls.Add(this.resolutionComboBox);
            this.videoGroupBox.Controls.Add(this.keepAspectCheckBox);
            this.videoGroupBox.Location = new System.Drawing.Point(2, 115);
            this.videoGroupBox.Name = "videoGroupBox";
            this.videoGroupBox.Size = new System.Drawing.Size(605, 72);
            this.videoGroupBox.TabIndex = 30;
            this.videoGroupBox.TabStop = false;
            this.videoGroupBox.Text = "Video";
            // 
            // scanLabel
            // 
            this.scanLabel.Location = new System.Drawing.Point(3, 47);
            this.scanLabel.Name = "scanLabel";
            this.scanLabel.Size = new System.Drawing.Size(100, 15);
            this.scanLabel.TabIndex = 11;
            this.scanLabel.Text = "Scan type:";
            // 
            // scanComboBox
            // 
            this.scanComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.scanComboBox.FormattingEnabled = true;
            this.scanComboBox.Items.AddRange(new object[] {
            "Interlaced",
            "Progressive"});
            this.scanComboBox.Location = new System.Drawing.Point(108, 42);
            this.scanComboBox.Name = "scanComboBox";
            this.scanComboBox.Size = new System.Drawing.Size(80, 21);
            this.scanComboBox.TabIndex = 34;
            this.scanComboBox.SelectedIndexChanged += new System.EventHandler(this.scanComboBox_Changed);
            this.scanComboBox.EnabledChanged += new System.EventHandler(this.scanComboBox_Changed);
            // 
            // resolutionLabel
            // 
            this.resolutionLabel.Location = new System.Drawing.Point(194, 22);
            this.resolutionLabel.Name = "resolutionLabel";
            this.resolutionLabel.Size = new System.Drawing.Size(140, 13);
            this.resolutionLabel.TabIndex = 9;
            this.resolutionLabel.Text = "Resolution:";
            // 
            // resolutionComboBox
            // 
            this.resolutionComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.resolutionComboBox.FormattingEnabled = true;
            this.resolutionComboBox.Items.AddRange(new object[] {
            "Same as source",
            "1920x1080",
            "1280x768",
            "720x576",
            "640x480",
            "320x240",
            "160x160"});
            this.resolutionComboBox.Location = new System.Drawing.Point(340, 17);
            this.resolutionComboBox.Name = "resolutionComboBox";
            this.resolutionComboBox.Size = new System.Drawing.Size(120, 21);
            this.resolutionComboBox.TabIndex = 32;
            this.resolutionComboBox.SelectedIndexChanged += new System.EventHandler(this.resolutionComboBox_Changed);
            this.resolutionComboBox.EnabledChanged += new System.EventHandler(this.resolutionComboBox_Changed);
            // 
            // keepAspectCheckBox
            // 
            this.keepAspectCheckBox.Location = new System.Drawing.Point(476, 21);
            this.keepAspectCheckBox.Name = "keepAspectCheckBox";
            this.keepAspectCheckBox.Size = new System.Drawing.Size(120, 17);
            this.keepAspectCheckBox.TabIndex = 33;
            this.keepAspectCheckBox.Text = "Keep aspect ratio";
            this.keepAspectCheckBox.UseVisualStyleBackColor = true;
            this.keepAspectCheckBox.CheckedChanged += new System.EventHandler(this.keepAspectCheckBox_Changed);
            this.keepAspectCheckBox.EnabledChanged += new System.EventHandler(this.keepAspectCheckBox_Changed);
            // 
            // containerGroupBox
            // 
            this.containerGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.containerGroupBox.Controls.Add(this.audioCheckBox);
            this.containerGroupBox.Controls.Add(this.videoCheckBox);
            this.containerGroupBox.Controls.Add(this.containerComboBox);
            this.containerGroupBox.Location = new System.Drawing.Point(3, 59);
            this.containerGroupBox.Name = "containerGroupBox";
            this.containerGroupBox.Size = new System.Drawing.Size(605, 50);
            this.containerGroupBox.TabIndex = 20;
            this.containerGroupBox.TabStop = false;
            this.containerGroupBox.Text = "Container - codec";
            // 
            // audioCheckBox
            // 
            this.audioCheckBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.audioCheckBox.Checked = true;
            this.audioCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.audioCheckBox.Location = new System.Drawing.Point(528, 22);
            this.audioCheckBox.Name = "audioCheckBox";
            this.audioCheckBox.Size = new System.Drawing.Size(70, 17);
            this.audioCheckBox.TabIndex = 23;
            this.audioCheckBox.Text = "Audio";
            this.audioCheckBox.UseVisualStyleBackColor = true;
            this.audioCheckBox.CheckedChanged += new System.EventHandler(this.audioCheckBox_CheckedChanged);
            // 
            // videoCheckBox
            // 
            this.videoCheckBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.videoCheckBox.Checked = true;
            this.videoCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.videoCheckBox.Location = new System.Drawing.Point(452, 22);
            this.videoCheckBox.Name = "videoCheckBox";
            this.videoCheckBox.Size = new System.Drawing.Size(70, 17);
            this.videoCheckBox.TabIndex = 22;
            this.videoCheckBox.Text = "Video";
            this.videoCheckBox.UseVisualStyleBackColor = true;
            this.videoCheckBox.CheckedChanged += new System.EventHandler(this.videoCheckBox_CheckedChanged);
            // 
            // containerComboBox
            // 
            this.containerComboBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.containerComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.containerComboBox.FormattingEnabled = true;
            this.containerComboBox.Location = new System.Drawing.Point(6, 19);
            this.containerComboBox.Name = "containerComboBox";
            this.containerComboBox.Size = new System.Drawing.Size(430, 21);
            this.containerComboBox.TabIndex = 21;
            this.containerComboBox.SelectedIndexChanged += new System.EventHandler(this.containerComboBox_SelectedIndexChanged);
            // 
            // outputGroupBox
            // 
            this.outputGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.outputGroupBox.Controls.Add(this.outputTextBox);
            this.outputGroupBox.Controls.Add(this.browseOutputButton);
            this.outputGroupBox.Location = new System.Drawing.Point(2, 386);
            this.outputGroupBox.Name = "outputGroupBox";
            this.outputGroupBox.Size = new System.Drawing.Size(605, 50);
            this.outputGroupBox.TabIndex = 70;
            this.outputGroupBox.TabStop = false;
            this.outputGroupBox.Text = "Output";
            // 
            // outputTextBox
            // 
            this.outputTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.outputTextBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.outputTextBox.FormattingEnabled = true;
            this.outputTextBox.Items.AddRange(new object[] {
            ".."});
            this.outputTextBox.Location = new System.Drawing.Point(6, 18);
            this.outputTextBox.Name = "outputTextBox";
            this.outputTextBox.Size = new System.Drawing.Size(512, 21);
            this.outputTextBox.TabIndex = 71;
            // 
            // browseOutputButton
            // 
            this.browseOutputButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.browseOutputButton.Location = new System.Drawing.Point(524, 17);
            this.browseOutputButton.Name = "browseOutputButton";
            this.browseOutputButton.Size = new System.Drawing.Size(75, 23);
            this.browseOutputButton.TabIndex = 72;
            this.browseOutputButton.Text = "Browse";
            this.browseOutputButton.UseVisualStyleBackColor = true;
            this.browseOutputButton.Click += new System.EventHandler(this.browseOutputButton_Click);
            // 
            // inputGroupBox
            // 
            this.inputGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.inputGroupBox.Controls.Add(this.browseInputButton);
            this.inputGroupBox.Controls.Add(this.inputTextBox);
            this.inputGroupBox.Location = new System.Drawing.Point(2, 330);
            this.inputGroupBox.Name = "inputGroupBox";
            this.inputGroupBox.Size = new System.Drawing.Size(605, 50);
            this.inputGroupBox.TabIndex = 60;
            this.inputGroupBox.TabStop = false;
            this.inputGroupBox.Text = "Input";
            // 
            // browseInputButton
            // 
            this.browseInputButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.browseInputButton.Location = new System.Drawing.Point(524, 18);
            this.browseInputButton.Name = "browseInputButton";
            this.browseInputButton.Size = new System.Drawing.Size(75, 23);
            this.browseInputButton.TabIndex = 62;
            this.browseInputButton.Text = "Browse";
            this.browseInputButton.UseVisualStyleBackColor = true;
            this.browseInputButton.Click += new System.EventHandler(this.browseInputButton_Click);
            // 
            // inputTextBox
            // 
            this.inputTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.inputTextBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.inputTextBox.FormattingEnabled = true;
            this.inputTextBox.Items.AddRange(new object[] {
            ".."});
            this.inputTextBox.Location = new System.Drawing.Point(6, 19);
            this.inputTextBox.Name = "inputTextBox";
            this.inputTextBox.Size = new System.Drawing.Size(512, 21);
            this.inputTextBox.TabIndex = 61;
            this.inputTextBox.SelectedIndexChanged += new System.EventHandler(this.inputTextBox_SelectedIndexChanged);
            // 
            // paramGroupBox
            // 
            this.paramGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.paramGroupBox.Controls.Add(this.editCheckBox);
            this.paramGroupBox.Controls.Add(this.paramBox);
            this.paramGroupBox.Location = new System.Drawing.Point(2, 3);
            this.paramGroupBox.Name = "paramGroupBox";
            this.paramGroupBox.Size = new System.Drawing.Size(605, 50);
            this.paramGroupBox.TabIndex = 10;
            this.paramGroupBox.TabStop = false;
            this.paramGroupBox.Text = "Parameters";
            // 
            // editCheckBox
            // 
            this.editCheckBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.editCheckBox.Location = new System.Drawing.Point(529, 21);
            this.editCheckBox.Name = "editCheckBox";
            this.editCheckBox.Size = new System.Drawing.Size(70, 17);
            this.editCheckBox.TabIndex = 12;
            this.editCheckBox.Text = "Edit";
            this.editCheckBox.UseVisualStyleBackColor = true;
            this.editCheckBox.CheckedChanged += new System.EventHandler(this.editCheckBox_CheckedChanged);
            // 
            // paramBox
            // 
            this.paramBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.paramBox.Location = new System.Drawing.Point(7, 19);
            this.paramBox.Name = "paramBox";
            this.paramBox.ReadOnly = true;
            this.paramBox.Size = new System.Drawing.Size(516, 20);
            this.paramBox.TabIndex = 11;
            // 
            // infoTextBox
            // 
            this.infoTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.infoTextBox.ContextMenuStrip = this.infoMenuStrip;
            this.infoTextBox.Location = new System.Drawing.Point(3, 59);
            this.infoTextBox.Name = "infoTextBox";
            this.infoTextBox.ReadOnly = true;
            this.infoTextBox.Size = new System.Drawing.Size(604, 320);
            this.infoTextBox.TabIndex = 17;
            this.infoTextBox.TabStop = false;
            this.infoTextBox.Text = "";
            this.infoTextBox.Visible = false;
            // 
            // infoMenuStrip
            // 
            this.infoMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.copyMenuItem});
            this.infoMenuStrip.Name = "infoMenuStrip";
            this.infoMenuStrip.Size = new System.Drawing.Size(153, 48);
            // 
            // copyMenuItem
            // 
            this.copyMenuItem.Name = "copyMenuItem";
            this.copyMenuItem.Size = new System.Drawing.Size(152, 22);
            this.copyMenuItem.Text = "Copy";
            this.copyMenuItem.Click += new System.EventHandler(this.copyMenuItem_Click);
            // 
            // ParametersControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.containerGroupBox);
            this.Controls.Add(this.outputGroupBox);
            this.Controls.Add(this.inputGroupBox);
            this.Controls.Add(this.othersGroupBox);
            this.Controls.Add(this.videoGroupBox);
            this.Controls.Add(this.audioGroupBox);
            this.Controls.Add(this.paramGroupBox);
            this.Controls.Add(this.infoTextBox);
            this.MinimumSize = new System.Drawing.Size(610, 440);
            this.Name = "ParametersControl";
            this.Size = new System.Drawing.Size(610, 440);
            this.Load += new System.EventHandler(this.ParametersControl_Load);
            this.othersGroupBox.ResumeLayout(false);
            this.othersGroupBox.PerformLayout();
            this.audioGroupBox.ResumeLayout(false);
            this.videoGroupBox.ResumeLayout(false);
            this.containerGroupBox.ResumeLayout(false);
            this.outputGroupBox.ResumeLayout(false);
            this.inputGroupBox.ResumeLayout(false);
            this.paramGroupBox.ResumeLayout(false);
            this.paramGroupBox.PerformLayout();
            this.infoMenuStrip.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox othersGroupBox;
        private System.Windows.Forms.ComboBox fpsComboBox;
        private System.Windows.Forms.Label fpsLabel;
        private System.Windows.Forms.CheckBox subtitlesIntCheckBox;
        private System.Windows.Forms.TextBox endTimeTextBox;
        private System.Windows.Forms.TextBox startTimeTextBox;
        private System.Windows.Forms.Label startEndTimeLabel;
        private System.Windows.Forms.GroupBox audioGroupBox;
        private System.Windows.Forms.ComboBox bitrateAudioComboBox;
        private System.Windows.Forms.Label bitrateAudioLabel;
        private System.Windows.Forms.Label bitrateVideoLabel;
        private System.Windows.Forms.ComboBox bitrateVideoComboBox;
        private System.Windows.Forms.GroupBox videoGroupBox;
        private System.Windows.Forms.ComboBox resolutionComboBox;
        private System.Windows.Forms.CheckBox keepAspectCheckBox;
        private System.Windows.Forms.GroupBox containerGroupBox;
        private System.Windows.Forms.CheckBox audioCheckBox;
        private System.Windows.Forms.CheckBox videoCheckBox;
        private System.Windows.Forms.ComboBox containerComboBox;
        private System.Windows.Forms.Label scanLabel;
        private System.Windows.Forms.ComboBox scanComboBox;
        private System.Windows.Forms.Label resolutionLabel;
        private System.Windows.Forms.GroupBox outputGroupBox;
        private System.Windows.Forms.ComboBox outputTextBox;
        private System.Windows.Forms.Button browseOutputButton;
        private System.Windows.Forms.GroupBox inputGroupBox;
        private System.Windows.Forms.Button browseInputButton;
        private System.Windows.Forms.ComboBox inputTextBox;
        private System.Windows.Forms.GroupBox paramGroupBox;
        private System.Windows.Forms.TextBox paramBox;
        private System.Windows.Forms.CheckBox editCheckBox;
        private System.Windows.Forms.RichTextBox infoTextBox;
        private System.Windows.Forms.Label outBufSizeLabel;
        private System.Windows.Forms.TextBox outBufSizeTextBox;
        private System.Windows.Forms.ContextMenuStrip infoMenuStrip;
        private System.Windows.Forms.ToolStripMenuItem copyMenuItem;
    }
}
