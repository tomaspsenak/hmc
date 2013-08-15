namespace HomeMediaCenterGUI
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
            this.startButton = new System.Windows.Forms.Button();
            this.dirListBox = new System.Windows.Forms.ListBox();
            this.addDirButton = new System.Windows.Forms.Button();
            this.removeDirButton = new System.Windows.Forms.Button();
            this.webLinkLabel = new System.Windows.Forms.LinkLabel();
            this.directoriesGroup = new System.Windows.Forms.GroupBox();
            this.buildDatabaseButton = new System.Windows.Forms.Button();
            this.mainTabControl = new System.Windows.Forms.TabControl();
            this.homeTabPage = new System.Windows.Forms.TabPage();
            this.codecsGroup = new System.Windows.Forms.GroupBox();
            this.hmcLabel = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.mpeg2Label = new System.Windows.Forms.Label();
            this.ffdshowLabel = new System.Windows.Forms.Label();
            this.wmvLabel = new System.Windows.Forms.Label();
            this.webmLabel = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.settingsTabPage = new System.Windows.Forms.TabPage();
            this.tryToForwardPortCheckBox = new System.Windows.Forms.CheckBox();
            this.streamGroupBox = new System.Windows.Forms.GroupBox();
            this.videoStreamParamEditButton = new System.Windows.Forms.Button();
            this.videoStreamParamAddButton = new System.Windows.Forms.Button();
            this.videoStreamParamRemoveButton = new System.Windows.Forms.Button();
            this.streamParamLabel = new System.Windows.Forms.Label();
            this.videoStreamParamCombo = new System.Windows.Forms.ComboBox();
            this.realTimeDatabaseRefreshCheckBox = new System.Windows.Forms.CheckBox();
            this.minimizeCheckBox = new System.Windows.Forms.CheckBox();
            this.startupCheckBox = new System.Windows.Forms.CheckBox();
            this.applySettingsButton = new System.Windows.Forms.Button();
            this.videoGroupBox = new System.Windows.Forms.GroupBox();
            this.videoParamEditButton = new System.Windows.Forms.Button();
            this.videoParamAddButton = new System.Windows.Forms.Button();
            this.videoParamRemoveButton = new System.Windows.Forms.Button();
            this.videoNativeCheck = new System.Windows.Forms.CheckBox();
            this.videoParamCombo = new System.Windows.Forms.ComboBox();
            this.videoParamLabel = new System.Windows.Forms.Label();
            this.imageGroupBox = new System.Windows.Forms.GroupBox();
            this.imageParamEditButton = new System.Windows.Forms.Button();
            this.imageParamAddButton = new System.Windows.Forms.Button();
            this.imageParamRemoveButton = new System.Windows.Forms.Button();
            this.imageNativeCheck = new System.Windows.Forms.CheckBox();
            this.imageParamCombo = new System.Windows.Forms.ComboBox();
            this.imageParamLabel = new System.Windows.Forms.Label();
            this.audioGroupBox = new System.Windows.Forms.GroupBox();
            this.audioParamEditButton = new System.Windows.Forms.Button();
            this.audioParamAddButton = new System.Windows.Forms.Button();
            this.audioParamRemoveButton = new System.Windows.Forms.Button();
            this.audioNativeCheck = new System.Windows.Forms.CheckBox();
            this.audioParamCombo = new System.Windows.Forms.ComboBox();
            this.audioParamLabel = new System.Windows.Forms.Label();
            this.portText = new System.Windows.Forms.TextBox();
            this.portLabel = new System.Windows.Forms.Label();
            this.friendlyNameText = new System.Windows.Forms.TextBox();
            this.friendlyNameLabel = new System.Windows.Forms.Label();
            this.logTabPage = new System.Windows.Forms.TabPage();
            this.requestLabel = new System.Windows.Forms.Label();
            this.requestCountLabel = new System.Windows.Forms.Label();
            this.logLengthLabel = new System.Windows.Forms.Label();
            this.logLengthUpDown = new System.Windows.Forms.NumericUpDown();
            this.logCheckBox = new System.Windows.Forms.CheckBox();
            this.logListBox = new System.Windows.Forms.ListBox();
            this.converterTabPage = new System.Windows.Forms.TabPage();
            this.convertButton = new System.Windows.Forms.Button();
            this.aboutTabPage = new System.Windows.Forms.TabPage();
            this.label5 = new System.Windows.Forms.Label();
            this.homepageLinkLabel = new System.Windows.Forms.LinkLabel();
            this.settingsLinkLabel = new System.Windows.Forms.LinkLabel();
            this.aboutLabel = new System.Windows.Forms.Label();
            this.statusLabel = new System.Windows.Forms.Label();
            this.statusTextLabel = new System.Windows.Forms.Label();
            this.mainNotifyIcon = new System.Windows.Forms.NotifyIcon(this.components);
            this.paramControl = new HomeMediaCenterGUI.ParametersControl();
            this.directoriesGroup.SuspendLayout();
            this.mainTabControl.SuspendLayout();
            this.homeTabPage.SuspendLayout();
            this.codecsGroup.SuspendLayout();
            this.settingsTabPage.SuspendLayout();
            this.streamGroupBox.SuspendLayout();
            this.videoGroupBox.SuspendLayout();
            this.imageGroupBox.SuspendLayout();
            this.audioGroupBox.SuspendLayout();
            this.logTabPage.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.logLengthUpDown)).BeginInit();
            this.converterTabPage.SuspendLayout();
            this.aboutTabPage.SuspendLayout();
            this.SuspendLayout();
            // 
            // startButton
            // 
            this.startButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.startButton.Location = new System.Drawing.Point(416, 12);
            this.startButton.Name = "startButton";
            this.startButton.Size = new System.Drawing.Size(120, 32);
            this.startButton.TabIndex = 0;
            this.startButton.Tag = "";
            this.startButton.Text = "Start";
            this.startButton.UseVisualStyleBackColor = true;
            this.startButton.Click += new System.EventHandler(this.startButton_Click);
            // 
            // dirListBox
            // 
            this.dirListBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.dirListBox.FormattingEnabled = true;
            this.dirListBox.HorizontalScrollbar = true;
            this.dirListBox.Location = new System.Drawing.Point(10, 20);
            this.dirListBox.Name = "dirListBox";
            this.dirListBox.Size = new System.Drawing.Size(331, 290);
            this.dirListBox.Sorted = true;
            this.dirListBox.TabIndex = 1;
            // 
            // addDirButton
            // 
            this.addDirButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.addDirButton.Location = new System.Drawing.Point(185, 330);
            this.addDirButton.Name = "addDirButton";
            this.addDirButton.Size = new System.Drawing.Size(75, 23);
            this.addDirButton.TabIndex = 2;
            this.addDirButton.Text = "Add";
            this.addDirButton.UseVisualStyleBackColor = true;
            this.addDirButton.Click += new System.EventHandler(this.addDirButton_Click);
            // 
            // removeDirButton
            // 
            this.removeDirButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.removeDirButton.Location = new System.Drawing.Point(266, 330);
            this.removeDirButton.Name = "removeDirButton";
            this.removeDirButton.Size = new System.Drawing.Size(75, 23);
            this.removeDirButton.TabIndex = 3;
            this.removeDirButton.Text = "Remove";
            this.removeDirButton.UseVisualStyleBackColor = true;
            this.removeDirButton.Click += new System.EventHandler(this.removeDirButton_Click);
            // 
            // webLinkLabel
            // 
            this.webLinkLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.webLinkLabel.AutoSize = true;
            this.webLinkLabel.Location = new System.Drawing.Point(469, 341);
            this.webLinkLabel.Name = "webLinkLabel";
            this.webLinkLabel.Size = new System.Drawing.Size(74, 13);
            this.webLinkLabel.TabIndex = 4;
            this.webLinkLabel.TabStop = true;
            this.webLinkLabel.Text = "Web interface";
            this.webLinkLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.webLinkLabel_LinkClicked);
            // 
            // directoriesGroup
            // 
            this.directoriesGroup.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.directoriesGroup.Controls.Add(this.dirListBox);
            this.directoriesGroup.Controls.Add(this.removeDirButton);
            this.directoriesGroup.Controls.Add(this.addDirButton);
            this.directoriesGroup.Location = new System.Drawing.Point(6, 6);
            this.directoriesGroup.Name = "directoriesGroup";
            this.directoriesGroup.Size = new System.Drawing.Size(351, 364);
            this.directoriesGroup.TabIndex = 5;
            this.directoriesGroup.TabStop = false;
            this.directoriesGroup.Text = "Directories";
            // 
            // buildDatabaseButton
            // 
            this.buildDatabaseButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buildDatabaseButton.Location = new System.Drawing.Point(542, 12);
            this.buildDatabaseButton.Name = "buildDatabaseButton";
            this.buildDatabaseButton.Size = new System.Drawing.Size(120, 32);
            this.buildDatabaseButton.TabIndex = 6;
            this.buildDatabaseButton.Text = "Refresh database";
            this.buildDatabaseButton.UseVisualStyleBackColor = true;
            this.buildDatabaseButton.Click += new System.EventHandler(this.buildDatabaseButton_Click);
            // 
            // mainTabControl
            // 
            this.mainTabControl.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mainTabControl.Controls.Add(this.homeTabPage);
            this.mainTabControl.Controls.Add(this.settingsTabPage);
            this.mainTabControl.Controls.Add(this.logTabPage);
            this.mainTabControl.Controls.Add(this.converterTabPage);
            this.mainTabControl.Controls.Add(this.aboutTabPage);
            this.mainTabControl.Location = new System.Drawing.Point(15, 47);
            this.mainTabControl.Name = "mainTabControl";
            this.mainTabControl.SelectedIndex = 0;
            this.mainTabControl.Size = new System.Drawing.Size(647, 402);
            this.mainTabControl.TabIndex = 7;
            this.mainTabControl.Selected += new System.Windows.Forms.TabControlEventHandler(this.mainTabControl_Selected);
            // 
            // homeTabPage
            // 
            this.homeTabPage.Controls.Add(this.codecsGroup);
            this.homeTabPage.Controls.Add(this.directoriesGroup);
            this.homeTabPage.Controls.Add(this.webLinkLabel);
            this.homeTabPage.Location = new System.Drawing.Point(4, 22);
            this.homeTabPage.Name = "homeTabPage";
            this.homeTabPage.Padding = new System.Windows.Forms.Padding(3);
            this.homeTabPage.Size = new System.Drawing.Size(639, 376);
            this.homeTabPage.TabIndex = 0;
            this.homeTabPage.Text = "Home";
            this.homeTabPage.UseVisualStyleBackColor = true;
            // 
            // codecsGroup
            // 
            this.codecsGroup.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.codecsGroup.Controls.Add(this.hmcLabel);
            this.codecsGroup.Controls.Add(this.label7);
            this.codecsGroup.Controls.Add(this.mpeg2Label);
            this.codecsGroup.Controls.Add(this.ffdshowLabel);
            this.codecsGroup.Controls.Add(this.wmvLabel);
            this.codecsGroup.Controls.Add(this.webmLabel);
            this.codecsGroup.Controls.Add(this.label4);
            this.codecsGroup.Controls.Add(this.label3);
            this.codecsGroup.Controls.Add(this.label2);
            this.codecsGroup.Controls.Add(this.label1);
            this.codecsGroup.Location = new System.Drawing.Point(363, 6);
            this.codecsGroup.Name = "codecsGroup";
            this.codecsGroup.Size = new System.Drawing.Size(268, 310);
            this.codecsGroup.TabIndex = 6;
            this.codecsGroup.TabStop = false;
            this.codecsGroup.Text = "Codecs";
            // 
            // hmcLabel
            // 
            this.hmcLabel.Location = new System.Drawing.Point(181, 118);
            this.hmcLabel.Name = "hmcLabel";
            this.hmcLabel.Size = new System.Drawing.Size(80, 13);
            this.hmcLabel.TabIndex = 9;
            // 
            // label7
            // 
            this.label7.Location = new System.Drawing.Point(9, 118);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(150, 13);
            this.label7.TabIndex = 8;
            this.label7.Text = "HMC Encoder";
            // 
            // mpeg2Label
            // 
            this.mpeg2Label.Location = new System.Drawing.Point(180, 94);
            this.mpeg2Label.Name = "mpeg2Label";
            this.mpeg2Label.Size = new System.Drawing.Size(80, 13);
            this.mpeg2Label.TabIndex = 7;
            // 
            // ffdshowLabel
            // 
            this.ffdshowLabel.Location = new System.Drawing.Point(180, 70);
            this.ffdshowLabel.Name = "ffdshowLabel";
            this.ffdshowLabel.Size = new System.Drawing.Size(80, 13);
            this.ffdshowLabel.TabIndex = 6;
            // 
            // wmvLabel
            // 
            this.wmvLabel.Location = new System.Drawing.Point(180, 46);
            this.wmvLabel.Name = "wmvLabel";
            this.wmvLabel.Size = new System.Drawing.Size(80, 13);
            this.wmvLabel.TabIndex = 5;
            // 
            // webmLabel
            // 
            this.webmLabel.Location = new System.Drawing.Point(180, 22);
            this.webmLabel.Name = "webmLabel";
            this.webmLabel.Size = new System.Drawing.Size(80, 13);
            this.webmLabel.TabIndex = 4;
            // 
            // label4
            // 
            this.label4.Location = new System.Drawing.Point(9, 22);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(150, 13);
            this.label4.TabIndex = 3;
            this.label4.Text = "WebM Splitter, VP8, Vorbis";
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(9, 94);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(150, 13);
            this.label3.TabIndex = 2;
            this.label3.Text = "Microsoft MPEG-2 Encoder";
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(9, 70);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(150, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "FFDshow Raw Video Filter";
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(9, 46);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(150, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "WM ASF Writer";
            // 
            // settingsTabPage
            // 
            this.settingsTabPage.AutoScroll = true;
            this.settingsTabPage.Controls.Add(this.tryToForwardPortCheckBox);
            this.settingsTabPage.Controls.Add(this.streamGroupBox);
            this.settingsTabPage.Controls.Add(this.realTimeDatabaseRefreshCheckBox);
            this.settingsTabPage.Controls.Add(this.minimizeCheckBox);
            this.settingsTabPage.Controls.Add(this.startupCheckBox);
            this.settingsTabPage.Controls.Add(this.applySettingsButton);
            this.settingsTabPage.Controls.Add(this.videoGroupBox);
            this.settingsTabPage.Controls.Add(this.imageGroupBox);
            this.settingsTabPage.Controls.Add(this.audioGroupBox);
            this.settingsTabPage.Controls.Add(this.portText);
            this.settingsTabPage.Controls.Add(this.portLabel);
            this.settingsTabPage.Controls.Add(this.friendlyNameText);
            this.settingsTabPage.Controls.Add(this.friendlyNameLabel);
            this.settingsTabPage.Location = new System.Drawing.Point(4, 22);
            this.settingsTabPage.Name = "settingsTabPage";
            this.settingsTabPage.Size = new System.Drawing.Size(639, 376);
            this.settingsTabPage.TabIndex = 2;
            this.settingsTabPage.Text = "Settings";
            this.settingsTabPage.UseVisualStyleBackColor = true;
            // 
            // tryToForwardPortCheckBox
            // 
            this.tryToForwardPortCheckBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tryToForwardPortCheckBox.Location = new System.Drawing.Point(175, 126);
            this.tryToForwardPortCheckBox.Name = "tryToForwardPortCheckBox";
            this.tryToForwardPortCheckBox.Size = new System.Drawing.Size(458, 17);
            this.tryToForwardPortCheckBox.TabIndex = 34;
            this.tryToForwardPortCheckBox.Text = "Enable UPnP port forwarding";
            this.tryToForwardPortCheckBox.UseVisualStyleBackColor = true;
            // 
            // streamGroupBox
            // 
            this.streamGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.streamGroupBox.Controls.Add(this.videoStreamParamEditButton);
            this.streamGroupBox.Controls.Add(this.videoStreamParamAddButton);
            this.streamGroupBox.Controls.Add(this.videoStreamParamRemoveButton);
            this.streamGroupBox.Controls.Add(this.streamParamLabel);
            this.streamGroupBox.Controls.Add(this.videoStreamParamCombo);
            this.streamGroupBox.Location = new System.Drawing.Point(6, 388);
            this.streamGroupBox.Name = "streamGroupBox";
            this.streamGroupBox.Size = new System.Drawing.Size(627, 50);
            this.streamGroupBox.TabIndex = 30;
            this.streamGroupBox.TabStop = false;
            this.streamGroupBox.Text = "Stream";
            // 
            // videoStreamParamEditButton
            // 
            this.videoStreamParamEditButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.videoStreamParamEditButton.Image = ((System.Drawing.Image)(resources.GetObject("videoStreamParamEditButton.Image")));
            this.videoStreamParamEditButton.Location = new System.Drawing.Point(532, 17);
            this.videoStreamParamEditButton.Name = "videoStreamParamEditButton";
            this.videoStreamParamEditButton.Size = new System.Drawing.Size(29, 24);
            this.videoStreamParamEditButton.TabIndex = 29;
            this.videoStreamParamEditButton.UseVisualStyleBackColor = true;
            this.videoStreamParamEditButton.Click += new System.EventHandler(this.videoStreamParamEditButton_Click);
            // 
            // videoStreamParamAddButton
            // 
            this.videoStreamParamAddButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.videoStreamParamAddButton.Image = ((System.Drawing.Image)(resources.GetObject("videoStreamParamAddButton.Image")));
            this.videoStreamParamAddButton.Location = new System.Drawing.Point(592, 17);
            this.videoStreamParamAddButton.Name = "videoStreamParamAddButton";
            this.videoStreamParamAddButton.Size = new System.Drawing.Size(29, 24);
            this.videoStreamParamAddButton.TabIndex = 28;
            this.videoStreamParamAddButton.UseVisualStyleBackColor = true;
            this.videoStreamParamAddButton.Click += new System.EventHandler(this.videoStreamParamAddButton_Click);
            // 
            // videoStreamParamRemoveButton
            // 
            this.videoStreamParamRemoveButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.videoStreamParamRemoveButton.Image = ((System.Drawing.Image)(resources.GetObject("videoStreamParamRemoveButton.Image")));
            this.videoStreamParamRemoveButton.Location = new System.Drawing.Point(562, 17);
            this.videoStreamParamRemoveButton.Name = "videoStreamParamRemoveButton";
            this.videoStreamParamRemoveButton.Size = new System.Drawing.Size(29, 24);
            this.videoStreamParamRemoveButton.TabIndex = 27;
            this.videoStreamParamRemoveButton.UseVisualStyleBackColor = true;
            this.videoStreamParamRemoveButton.Click += new System.EventHandler(this.videoStreamParamRemoveButton_Click);
            // 
            // streamParamLabel
            // 
            this.streamParamLabel.Location = new System.Drawing.Point(6, 22);
            this.streamParamLabel.Name = "streamParamLabel";
            this.streamParamLabel.Size = new System.Drawing.Size(160, 13);
            this.streamParamLabel.TabIndex = 24;
            this.streamParamLabel.Text = "Encode parameters:";
            // 
            // videoStreamParamCombo
            // 
            this.videoStreamParamCombo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.videoStreamParamCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.videoStreamParamCombo.FormattingEnabled = true;
            this.videoStreamParamCombo.Location = new System.Drawing.Point(172, 19);
            this.videoStreamParamCombo.Name = "videoStreamParamCombo";
            this.videoStreamParamCombo.Size = new System.Drawing.Size(353, 21);
            this.videoStreamParamCombo.TabIndex = 25;
            // 
            // realTimeDatabaseRefreshCheckBox
            // 
            this.realTimeDatabaseRefreshCheckBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.realTimeDatabaseRefreshCheckBox.Location = new System.Drawing.Point(175, 103);
            this.realTimeDatabaseRefreshCheckBox.Name = "realTimeDatabaseRefreshCheckBox";
            this.realTimeDatabaseRefreshCheckBox.Size = new System.Drawing.Size(458, 17);
            this.realTimeDatabaseRefreshCheckBox.TabIndex = 33;
            this.realTimeDatabaseRefreshCheckBox.Text = "Real time database refresh";
            this.realTimeDatabaseRefreshCheckBox.UseVisualStyleBackColor = true;
            // 
            // minimizeCheckBox
            // 
            this.minimizeCheckBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.minimizeCheckBox.Location = new System.Drawing.Point(175, 80);
            this.minimizeCheckBox.Name = "minimizeCheckBox";
            this.minimizeCheckBox.Size = new System.Drawing.Size(458, 17);
            this.minimizeCheckBox.TabIndex = 32;
            this.minimizeCheckBox.Text = "Minimize to tray";
            this.minimizeCheckBox.UseVisualStyleBackColor = true;
            this.minimizeCheckBox.CheckedChanged += new System.EventHandler(this.minimizeCheckBox_CheckedChanged);
            // 
            // startupCheckBox
            // 
            this.startupCheckBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.startupCheckBox.Location = new System.Drawing.Point(175, 57);
            this.startupCheckBox.Name = "startupCheckBox";
            this.startupCheckBox.Size = new System.Drawing.Size(458, 17);
            this.startupCheckBox.TabIndex = 31;
            this.startupCheckBox.Text = "Run at system startup";
            this.startupCheckBox.UseVisualStyleBackColor = true;
            this.startupCheckBox.CheckedChanged += new System.EventHandler(this.startupCheckBox_CheckedChanged);
            // 
            // applySettingsButton
            // 
            this.applySettingsButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.applySettingsButton.Location = new System.Drawing.Point(543, 5);
            this.applySettingsButton.Name = "applySettingsButton";
            this.applySettingsButton.Size = new System.Drawing.Size(90, 46);
            this.applySettingsButton.TabIndex = 30;
            this.applySettingsButton.Text = "Apply and restart";
            this.applySettingsButton.UseVisualStyleBackColor = true;
            this.applySettingsButton.Click += new System.EventHandler(this.applySettingsButton_Click);
            // 
            // videoGroupBox
            // 
            this.videoGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.videoGroupBox.Controls.Add(this.videoParamEditButton);
            this.videoGroupBox.Controls.Add(this.videoParamAddButton);
            this.videoGroupBox.Controls.Add(this.videoParamRemoveButton);
            this.videoGroupBox.Controls.Add(this.videoNativeCheck);
            this.videoGroupBox.Controls.Add(this.videoParamCombo);
            this.videoGroupBox.Controls.Add(this.videoParamLabel);
            this.videoGroupBox.Location = new System.Drawing.Point(6, 309);
            this.videoGroupBox.Name = "videoGroupBox";
            this.videoGroupBox.Size = new System.Drawing.Size(627, 73);
            this.videoGroupBox.TabIndex = 29;
            this.videoGroupBox.TabStop = false;
            this.videoGroupBox.Text = "Video";
            // 
            // videoParamEditButton
            // 
            this.videoParamEditButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.videoParamEditButton.Image = ((System.Drawing.Image)(resources.GetObject("videoParamEditButton.Image")));
            this.videoParamEditButton.Location = new System.Drawing.Point(532, 40);
            this.videoParamEditButton.Name = "videoParamEditButton";
            this.videoParamEditButton.Size = new System.Drawing.Size(29, 24);
            this.videoParamEditButton.TabIndex = 18;
            this.videoParamEditButton.UseVisualStyleBackColor = true;
            this.videoParamEditButton.Click += new System.EventHandler(this.videoParamEditButton_Click);
            // 
            // videoParamAddButton
            // 
            this.videoParamAddButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.videoParamAddButton.Image = ((System.Drawing.Image)(resources.GetObject("videoParamAddButton.Image")));
            this.videoParamAddButton.Location = new System.Drawing.Point(592, 40);
            this.videoParamAddButton.Name = "videoParamAddButton";
            this.videoParamAddButton.Size = new System.Drawing.Size(29, 24);
            this.videoParamAddButton.TabIndex = 19;
            this.videoParamAddButton.UseVisualStyleBackColor = true;
            this.videoParamAddButton.Click += new System.EventHandler(this.videoParamAddButton_Click);
            // 
            // videoParamRemoveButton
            // 
            this.videoParamRemoveButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.videoParamRemoveButton.Image = ((System.Drawing.Image)(resources.GetObject("videoParamRemoveButton.Image")));
            this.videoParamRemoveButton.Location = new System.Drawing.Point(562, 40);
            this.videoParamRemoveButton.Name = "videoParamRemoveButton";
            this.videoParamRemoveButton.Size = new System.Drawing.Size(29, 24);
            this.videoParamRemoveButton.TabIndex = 18;
            this.videoParamRemoveButton.UseVisualStyleBackColor = true;
            this.videoParamRemoveButton.Click += new System.EventHandler(this.videoParamRemoveButton_Click);
            // 
            // videoNativeCheck
            // 
            this.videoNativeCheck.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.videoNativeCheck.Location = new System.Drawing.Point(172, 19);
            this.videoNativeCheck.Name = "videoNativeCheck";
            this.videoNativeCheck.Size = new System.Drawing.Size(449, 17);
            this.videoNativeCheck.TabIndex = 18;
            this.videoNativeCheck.Text = "Allow native file";
            this.videoNativeCheck.UseVisualStyleBackColor = true;
            // 
            // videoParamCombo
            // 
            this.videoParamCombo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.videoParamCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.videoParamCombo.FormattingEnabled = true;
            this.videoParamCombo.Location = new System.Drawing.Point(172, 42);
            this.videoParamCombo.Name = "videoParamCombo";
            this.videoParamCombo.Size = new System.Drawing.Size(353, 21);
            this.videoParamCombo.TabIndex = 21;
            // 
            // videoParamLabel
            // 
            this.videoParamLabel.Location = new System.Drawing.Point(6, 45);
            this.videoParamLabel.Name = "videoParamLabel";
            this.videoParamLabel.Size = new System.Drawing.Size(160, 13);
            this.videoParamLabel.TabIndex = 24;
            this.videoParamLabel.Text = "Encode parameters:";
            // 
            // imageGroupBox
            // 
            this.imageGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.imageGroupBox.Controls.Add(this.imageParamEditButton);
            this.imageGroupBox.Controls.Add(this.imageParamAddButton);
            this.imageGroupBox.Controls.Add(this.imageParamRemoveButton);
            this.imageGroupBox.Controls.Add(this.imageNativeCheck);
            this.imageGroupBox.Controls.Add(this.imageParamCombo);
            this.imageGroupBox.Controls.Add(this.imageParamLabel);
            this.imageGroupBox.Location = new System.Drawing.Point(6, 230);
            this.imageGroupBox.Name = "imageGroupBox";
            this.imageGroupBox.Size = new System.Drawing.Size(627, 73);
            this.imageGroupBox.TabIndex = 28;
            this.imageGroupBox.TabStop = false;
            this.imageGroupBox.Text = "Image";
            // 
            // imageParamEditButton
            // 
            this.imageParamEditButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.imageParamEditButton.Image = ((System.Drawing.Image)(resources.GetObject("imageParamEditButton.Image")));
            this.imageParamEditButton.Location = new System.Drawing.Point(532, 40);
            this.imageParamEditButton.Name = "imageParamEditButton";
            this.imageParamEditButton.Size = new System.Drawing.Size(29, 24);
            this.imageParamEditButton.TabIndex = 15;
            this.imageParamEditButton.UseVisualStyleBackColor = true;
            this.imageParamEditButton.Click += new System.EventHandler(this.imageParamEditButton_Click);
            // 
            // imageParamAddButton
            // 
            this.imageParamAddButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.imageParamAddButton.Image = ((System.Drawing.Image)(resources.GetObject("imageParamAddButton.Image")));
            this.imageParamAddButton.Location = new System.Drawing.Point(592, 40);
            this.imageParamAddButton.Name = "imageParamAddButton";
            this.imageParamAddButton.Size = new System.Drawing.Size(29, 24);
            this.imageParamAddButton.TabIndex = 15;
            this.imageParamAddButton.UseVisualStyleBackColor = true;
            this.imageParamAddButton.Click += new System.EventHandler(this.imageParamAddButton_Click);
            // 
            // imageParamRemoveButton
            // 
            this.imageParamRemoveButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.imageParamRemoveButton.Image = ((System.Drawing.Image)(resources.GetObject("imageParamRemoveButton.Image")));
            this.imageParamRemoveButton.Location = new System.Drawing.Point(562, 40);
            this.imageParamRemoveButton.Name = "imageParamRemoveButton";
            this.imageParamRemoveButton.Size = new System.Drawing.Size(29, 24);
            this.imageParamRemoveButton.TabIndex = 14;
            this.imageParamRemoveButton.UseVisualStyleBackColor = true;
            this.imageParamRemoveButton.Click += new System.EventHandler(this.imageParamRemoveButton_Click);
            // 
            // imageNativeCheck
            // 
            this.imageNativeCheck.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.imageNativeCheck.Location = new System.Drawing.Point(172, 19);
            this.imageNativeCheck.Name = "imageNativeCheck";
            this.imageNativeCheck.Size = new System.Drawing.Size(449, 17);
            this.imageNativeCheck.TabIndex = 11;
            this.imageNativeCheck.Text = "Allow native file";
            this.imageNativeCheck.UseVisualStyleBackColor = true;
            // 
            // imageParamCombo
            // 
            this.imageParamCombo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.imageParamCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.imageParamCombo.FormattingEnabled = true;
            this.imageParamCombo.Location = new System.Drawing.Point(172, 42);
            this.imageParamCombo.Name = "imageParamCombo";
            this.imageParamCombo.Size = new System.Drawing.Size(353, 21);
            this.imageParamCombo.TabIndex = 14;
            // 
            // imageParamLabel
            // 
            this.imageParamLabel.Location = new System.Drawing.Point(6, 45);
            this.imageParamLabel.Name = "imageParamLabel";
            this.imageParamLabel.Size = new System.Drawing.Size(160, 13);
            this.imageParamLabel.TabIndex = 17;
            this.imageParamLabel.Text = "Encode parameters:";
            // 
            // audioGroupBox
            // 
            this.audioGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.audioGroupBox.Controls.Add(this.audioParamEditButton);
            this.audioGroupBox.Controls.Add(this.audioParamAddButton);
            this.audioGroupBox.Controls.Add(this.audioParamRemoveButton);
            this.audioGroupBox.Controls.Add(this.audioNativeCheck);
            this.audioGroupBox.Controls.Add(this.audioParamCombo);
            this.audioGroupBox.Controls.Add(this.audioParamLabel);
            this.audioGroupBox.Location = new System.Drawing.Point(6, 149);
            this.audioGroupBox.Name = "audioGroupBox";
            this.audioGroupBox.Size = new System.Drawing.Size(627, 75);
            this.audioGroupBox.TabIndex = 27;
            this.audioGroupBox.TabStop = false;
            this.audioGroupBox.Text = "Audio";
            // 
            // audioParamEditButton
            // 
            this.audioParamEditButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.audioParamEditButton.Image = ((System.Drawing.Image)(resources.GetObject("audioParamEditButton.Image")));
            this.audioParamEditButton.Location = new System.Drawing.Point(532, 40);
            this.audioParamEditButton.Name = "audioParamEditButton";
            this.audioParamEditButton.Size = new System.Drawing.Size(29, 24);
            this.audioParamEditButton.TabIndex = 14;
            this.audioParamEditButton.UseVisualStyleBackColor = true;
            this.audioParamEditButton.Click += new System.EventHandler(this.audioParamEditButton_Click);
            // 
            // audioParamAddButton
            // 
            this.audioParamAddButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.audioParamAddButton.Image = ((System.Drawing.Image)(resources.GetObject("audioParamAddButton.Image")));
            this.audioParamAddButton.Location = new System.Drawing.Point(592, 40);
            this.audioParamAddButton.Name = "audioParamAddButton";
            this.audioParamAddButton.Size = new System.Drawing.Size(29, 24);
            this.audioParamAddButton.TabIndex = 13;
            this.audioParamAddButton.UseVisualStyleBackColor = true;
            this.audioParamAddButton.Click += new System.EventHandler(this.audioParamAddButton_Click);
            // 
            // audioParamRemoveButton
            // 
            this.audioParamRemoveButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.audioParamRemoveButton.Image = ((System.Drawing.Image)(resources.GetObject("audioParamRemoveButton.Image")));
            this.audioParamRemoveButton.Location = new System.Drawing.Point(562, 40);
            this.audioParamRemoveButton.Name = "audioParamRemoveButton";
            this.audioParamRemoveButton.Size = new System.Drawing.Size(29, 24);
            this.audioParamRemoveButton.TabIndex = 12;
            this.audioParamRemoveButton.UseVisualStyleBackColor = true;
            this.audioParamRemoveButton.Click += new System.EventHandler(this.audioParamRemoveButton_Click);
            // 
            // audioNativeCheck
            // 
            this.audioNativeCheck.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.audioNativeCheck.Location = new System.Drawing.Point(172, 19);
            this.audioNativeCheck.Name = "audioNativeCheck";
            this.audioNativeCheck.Size = new System.Drawing.Size(449, 17);
            this.audioNativeCheck.TabIndex = 4;
            this.audioNativeCheck.Text = "Allow native file";
            this.audioNativeCheck.UseVisualStyleBackColor = true;
            // 
            // audioParamCombo
            // 
            this.audioParamCombo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.audioParamCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.audioParamCombo.FormattingEnabled = true;
            this.audioParamCombo.Location = new System.Drawing.Point(172, 42);
            this.audioParamCombo.Name = "audioParamCombo";
            this.audioParamCombo.Size = new System.Drawing.Size(353, 21);
            this.audioParamCombo.TabIndex = 9;
            // 
            // audioParamLabel
            // 
            this.audioParamLabel.Location = new System.Drawing.Point(6, 45);
            this.audioParamLabel.Name = "audioParamLabel";
            this.audioParamLabel.Size = new System.Drawing.Size(160, 13);
            this.audioParamLabel.TabIndex = 10;
            this.audioParamLabel.Text = "Encode parameters:";
            // 
            // portText
            // 
            this.portText.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.portText.Location = new System.Drawing.Point(175, 31);
            this.portText.Name = "portText";
            this.portText.Size = new System.Drawing.Size(362, 20);
            this.portText.TabIndex = 3;
            // 
            // portLabel
            // 
            this.portLabel.Location = new System.Drawing.Point(9, 34);
            this.portLabel.Name = "portLabel";
            this.portLabel.Size = new System.Drawing.Size(160, 13);
            this.portLabel.TabIndex = 2;
            this.portLabel.Text = "Port:";
            // 
            // friendlyNameText
            // 
            this.friendlyNameText.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.friendlyNameText.Location = new System.Drawing.Point(175, 5);
            this.friendlyNameText.Name = "friendlyNameText";
            this.friendlyNameText.Size = new System.Drawing.Size(362, 20);
            this.friendlyNameText.TabIndex = 1;
            // 
            // friendlyNameLabel
            // 
            this.friendlyNameLabel.Location = new System.Drawing.Point(9, 8);
            this.friendlyNameLabel.Name = "friendlyNameLabel";
            this.friendlyNameLabel.Size = new System.Drawing.Size(160, 13);
            this.friendlyNameLabel.TabIndex = 0;
            this.friendlyNameLabel.Text = "Device name:";
            // 
            // logTabPage
            // 
            this.logTabPage.Controls.Add(this.requestLabel);
            this.logTabPage.Controls.Add(this.requestCountLabel);
            this.logTabPage.Controls.Add(this.logLengthLabel);
            this.logTabPage.Controls.Add(this.logLengthUpDown);
            this.logTabPage.Controls.Add(this.logCheckBox);
            this.logTabPage.Controls.Add(this.logListBox);
            this.logTabPage.Location = new System.Drawing.Point(4, 22);
            this.logTabPage.Name = "logTabPage";
            this.logTabPage.Padding = new System.Windows.Forms.Padding(3);
            this.logTabPage.Size = new System.Drawing.Size(639, 376);
            this.logTabPage.TabIndex = 1;
            this.logTabPage.Text = "Log";
            this.logTabPage.UseVisualStyleBackColor = true;
            // 
            // requestLabel
            // 
            this.requestLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.requestLabel.AutoSize = true;
            this.requestLabel.Location = new System.Drawing.Point(460, 8);
            this.requestLabel.Name = "requestLabel";
            this.requestLabel.Size = new System.Drawing.Size(102, 13);
            this.requestLabel.TabIndex = 5;
            this.requestLabel.Text = "Number of requests:";
            this.requestLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // requestCountLabel
            // 
            this.requestCountLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.requestCountLabel.Location = new System.Drawing.Point(565, 8);
            this.requestCountLabel.Name = "requestCountLabel";
            this.requestCountLabel.Size = new System.Drawing.Size(60, 13);
            this.requestCountLabel.TabIndex = 4;
            this.requestCountLabel.Text = "0";
            this.requestCountLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // logLengthLabel
            // 
            this.logLengthLabel.Location = new System.Drawing.Point(160, 8);
            this.logLengthLabel.Name = "logLengthLabel";
            this.logLengthLabel.Size = new System.Drawing.Size(130, 13);
            this.logLengthLabel.TabIndex = 3;
            this.logLengthLabel.Text = "Log length:";
            this.logLengthLabel.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // logLengthUpDown
            // 
            this.logLengthUpDown.Location = new System.Drawing.Point(295, 5);
            this.logLengthUpDown.Maximum = new decimal(new int[] {
            5000,
            0,
            0,
            0});
            this.logLengthUpDown.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.logLengthUpDown.Name = "logLengthUpDown";
            this.logLengthUpDown.Size = new System.Drawing.Size(63, 20);
            this.logLengthUpDown.TabIndex = 2;
            this.logLengthUpDown.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.logLengthUpDown.ValueChanged += new System.EventHandler(this.logLengthUpDown_ValueChanged);
            // 
            // logCheckBox
            // 
            this.logCheckBox.AutoSize = true;
            this.logCheckBox.Location = new System.Drawing.Point(6, 7);
            this.logCheckBox.Name = "logCheckBox";
            this.logCheckBox.Size = new System.Drawing.Size(76, 17);
            this.logCheckBox.TabIndex = 1;
            this.logCheckBox.Text = "Enable log";
            this.logCheckBox.UseVisualStyleBackColor = true;
            this.logCheckBox.CheckedChanged += new System.EventHandler(this.logCheckBox_CheckedChanged);
            // 
            // logListBox
            // 
            this.logListBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.logListBox.FormattingEnabled = true;
            this.logListBox.HorizontalScrollbar = true;
            this.logListBox.Location = new System.Drawing.Point(6, 30);
            this.logListBox.Name = "logListBox";
            this.logListBox.Size = new System.Drawing.Size(624, 342);
            this.logListBox.TabIndex = 0;
            // 
            // converterTabPage
            // 
            this.converterTabPage.AutoScroll = true;
            this.converterTabPage.Controls.Add(this.paramControl);
            this.converterTabPage.Controls.Add(this.convertButton);
            this.converterTabPage.Location = new System.Drawing.Point(4, 22);
            this.converterTabPage.Name = "converterTabPage";
            this.converterTabPage.Size = new System.Drawing.Size(639, 376);
            this.converterTabPage.TabIndex = 3;
            this.converterTabPage.Text = "Converter";
            this.converterTabPage.UseVisualStyleBackColor = true;
            // 
            // convertButton
            // 
            this.convertButton.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.convertButton.Location = new System.Drawing.Point(210, 422);
            this.convertButton.Name = "convertButton";
            this.convertButton.Size = new System.Drawing.Size(120, 32);
            this.convertButton.TabIndex = 31;
            this.convertButton.Text = "Convert";
            this.convertButton.UseVisualStyleBackColor = true;
            this.convertButton.Click += new System.EventHandler(this.convertButton_Click);
            // 
            // aboutTabPage
            // 
            this.aboutTabPage.Controls.Add(this.label5);
            this.aboutTabPage.Controls.Add(this.homepageLinkLabel);
            this.aboutTabPage.Controls.Add(this.settingsLinkLabel);
            this.aboutTabPage.Controls.Add(this.aboutLabel);
            this.aboutTabPage.Location = new System.Drawing.Point(4, 22);
            this.aboutTabPage.Name = "aboutTabPage";
            this.aboutTabPage.Size = new System.Drawing.Size(639, 376);
            this.aboutTabPage.TabIndex = 4;
            this.aboutTabPage.Text = "About";
            this.aboutTabPage.UseVisualStyleBackColor = true;
            // 
            // label5
            // 
            this.label5.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label5.Location = new System.Drawing.Point(3, 144);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(633, 81);
            this.label5.TabIndex = 3;
            this.label5.Text = "----------------------------------------------\r\nHome Media Center uses external l" +
    "ibraries:\r\nFFmpeg - http://ffmpeg.zeranoe.com/\r\nWebM for DirectShow - http://cod" +
    "e.google.com/p/webm/\r\nDirectShow filters for Ogg Vorbis - http://xiph.org/dshow/";
            this.label5.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // homepageLinkLabel
            // 
            this.homepageLinkLabel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.homepageLinkLabel.Location = new System.Drawing.Point(3, 125);
            this.homepageLinkLabel.Name = "homepageLinkLabel";
            this.homepageLinkLabel.Size = new System.Drawing.Size(633, 21);
            this.homepageLinkLabel.TabIndex = 2;
            this.homepageLinkLabel.TabStop = true;
            this.homepageLinkLabel.Text = "http://hmc.codeplex.com";
            this.homepageLinkLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.homepageLinkLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.homepageLinkLabel_LinkClicked);
            // 
            // settingsLinkLabel
            // 
            this.settingsLinkLabel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.settingsLinkLabel.Location = new System.Drawing.Point(3, 108);
            this.settingsLinkLabel.Name = "settingsLinkLabel";
            this.settingsLinkLabel.Size = new System.Drawing.Size(633, 17);
            this.settingsLinkLabel.TabIndex = 1;
            this.settingsLinkLabel.TabStop = true;
            this.settingsLinkLabel.Text = "Config files";
            this.settingsLinkLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.settingsLinkLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.settingsLinkLabel_LinkClicked);
            // 
            // aboutLabel
            // 
            this.aboutLabel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.aboutLabel.Location = new System.Drawing.Point(3, 9);
            this.aboutLabel.Name = "aboutLabel";
            this.aboutLabel.Size = new System.Drawing.Size(633, 99);
            this.aboutLabel.TabIndex = 0;
            this.aboutLabel.Text = "aboutLabel";
            this.aboutLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // statusLabel
            // 
            this.statusLabel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.statusLabel.BackColor = System.Drawing.SystemColors.Control;
            this.statusLabel.Location = new System.Drawing.Point(15, 12);
            this.statusLabel.Name = "statusLabel";
            this.statusLabel.Size = new System.Drawing.Size(395, 32);
            this.statusLabel.TabIndex = 8;
            this.statusLabel.Text = "Status:";
            this.statusLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // statusTextLabel
            // 
            this.statusTextLabel.AutoSize = true;
            this.statusTextLabel.ForeColor = System.Drawing.Color.Red;
            this.statusTextLabel.Location = new System.Drawing.Point(59, 22);
            this.statusTextLabel.Name = "statusTextLabel";
            this.statusTextLabel.Size = new System.Drawing.Size(47, 13);
            this.statusTextLabel.TabIndex = 9;
            this.statusTextLabel.Text = "Stopped";
            // 
            // mainNotifyIcon
            // 
            this.mainNotifyIcon.Icon = ((System.Drawing.Icon)(resources.GetObject("mainNotifyIcon.Icon")));
            this.mainNotifyIcon.Text = "Home Media Center";
            this.mainNotifyIcon.Click += new System.EventHandler(this.mainNotifyIcon_Click);
            // 
            // paramControl
            // 
            this.paramControl.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.paramControl.Location = new System.Drawing.Point(3, 3);
            this.paramControl.MinimumSize = new System.Drawing.Size(610, 415);
            this.paramControl.Name = "paramControl";
            this.paramControl.Size = new System.Drawing.Size(633, 415);
            this.paramControl.TabIndex = 32;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(674, 461);
            this.Controls.Add(this.buildDatabaseButton);
            this.Controls.Add(this.startButton);
            this.Controls.Add(this.statusTextLabel);
            this.Controls.Add(this.statusLabel);
            this.Controls.Add(this.mainTabControl);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimumSize = new System.Drawing.Size(690, 480);
            this.Name = "MainForm";
            this.Text = "Home Media Center";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.Resize += new System.EventHandler(this.MainForm_Resize);
            this.directoriesGroup.ResumeLayout(false);
            this.mainTabControl.ResumeLayout(false);
            this.homeTabPage.ResumeLayout(false);
            this.homeTabPage.PerformLayout();
            this.codecsGroup.ResumeLayout(false);
            this.settingsTabPage.ResumeLayout(false);
            this.settingsTabPage.PerformLayout();
            this.streamGroupBox.ResumeLayout(false);
            this.videoGroupBox.ResumeLayout(false);
            this.imageGroupBox.ResumeLayout(false);
            this.audioGroupBox.ResumeLayout(false);
            this.logTabPage.ResumeLayout(false);
            this.logTabPage.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.logLengthUpDown)).EndInit();
            this.converterTabPage.ResumeLayout(false);
            this.aboutTabPage.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button startButton;
        private System.Windows.Forms.ListBox dirListBox;
        private System.Windows.Forms.Button addDirButton;
        private System.Windows.Forms.Button removeDirButton;
        private System.Windows.Forms.LinkLabel webLinkLabel;
        private System.Windows.Forms.GroupBox directoriesGroup;
        private System.Windows.Forms.Button buildDatabaseButton;
        private System.Windows.Forms.TabControl mainTabControl;
        private System.Windows.Forms.TabPage homeTabPage;
        private System.Windows.Forms.TabPage logTabPage;
        private System.Windows.Forms.NumericUpDown logLengthUpDown;
        private System.Windows.Forms.CheckBox logCheckBox;
        private System.Windows.Forms.ListBox logListBox;
        private System.Windows.Forms.Label logLengthLabel;
        private System.Windows.Forms.Label statusLabel;
        private System.Windows.Forms.Label statusTextLabel;
        private System.Windows.Forms.GroupBox codecsGroup;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label mpeg2Label;
        private System.Windows.Forms.Label ffdshowLabel;
        private System.Windows.Forms.Label wmvLabel;
        private System.Windows.Forms.Label webmLabel;
        private System.Windows.Forms.Label requestCountLabel;
        private System.Windows.Forms.Label requestLabel;
        private System.Windows.Forms.TabPage settingsTabPage;
        private System.Windows.Forms.TextBox friendlyNameText;
        private System.Windows.Forms.Label friendlyNameLabel;
        private System.Windows.Forms.TextBox portText;
        private System.Windows.Forms.Label portLabel;
        private System.Windows.Forms.CheckBox audioNativeCheck;
        private System.Windows.Forms.ComboBox audioParamCombo;
        private System.Windows.Forms.Label audioParamLabel;
        private System.Windows.Forms.CheckBox imageNativeCheck;
        private System.Windows.Forms.ComboBox imageParamCombo;
        private System.Windows.Forms.Label videoParamLabel;
        private System.Windows.Forms.ComboBox videoParamCombo;
        private System.Windows.Forms.CheckBox videoNativeCheck;
        private System.Windows.Forms.Label imageParamLabel;
        private System.Windows.Forms.ComboBox videoStreamParamCombo;
        private System.Windows.Forms.GroupBox audioGroupBox;
        private System.Windows.Forms.GroupBox videoGroupBox;
        private System.Windows.Forms.GroupBox imageGroupBox;
        private System.Windows.Forms.Button applySettingsButton;
        private System.Windows.Forms.Button audioParamRemoveButton;
        private System.Windows.Forms.Button audioParamAddButton;
        private System.Windows.Forms.Button videoStreamParamAddButton;
        private System.Windows.Forms.Button videoStreamParamRemoveButton;
        private System.Windows.Forms.Button videoParamAddButton;
        private System.Windows.Forms.Button videoParamRemoveButton;
        private System.Windows.Forms.Button imageParamAddButton;
        private System.Windows.Forms.Button imageParamRemoveButton;
        private System.Windows.Forms.TabPage converterTabPage;
        private System.Windows.Forms.Button convertButton;
        private System.Windows.Forms.TabPage aboutTabPage;
        private System.Windows.Forms.Label aboutLabel;
        private System.Windows.Forms.LinkLabel settingsLinkLabel;
        private System.Windows.Forms.LinkLabel homepageLinkLabel;
        private System.Windows.Forms.Button audioParamEditButton;
        private System.Windows.Forms.Button videoStreamParamEditButton;
        private System.Windows.Forms.Button videoParamEditButton;
        private System.Windows.Forms.Button imageParamEditButton;
        private ParametersControl paramControl;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.NotifyIcon mainNotifyIcon;
        private System.Windows.Forms.CheckBox minimizeCheckBox;
        private System.Windows.Forms.CheckBox startupCheckBox;
        private System.Windows.Forms.Label hmcLabel;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.CheckBox realTimeDatabaseRefreshCheckBox;
        private System.Windows.Forms.GroupBox streamGroupBox;
        private System.Windows.Forms.Label streamParamLabel;
        private System.Windows.Forms.CheckBox tryToForwardPortCheckBox;
    }
}

