using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using HomeMediaCenter;
using HomeMediaCenter.Interfaces;
using System.Runtime.Remoting.Messaging;
using System.IO;
using System.Net;
using Microsoft.Win32;
using System.ServiceModel;

namespace HomeMediaCenterGUI
{
    public partial class MainForm : Form
    {
        private readonly SplashScreen splashScreen;
        private readonly IMediaServerDevice mediaCenter;
        private readonly ToolTip mainToolTip = new ToolTip();
        private readonly BackgroundWorker filtersWorker = new BackgroundWorker();

        private EventHandler<LogEventArgs> logEventHandler;

        private enum AfterStopped { Nothing, Close, LoadSettings };
        private AfterStopped afterStopped;

        private readonly string regRunName = "HomeMediaCenter";
        private readonly string[] supportedLanguages = new string[] { null, "en-US", "sk-SK", "da-DK" };

        public MainForm(SplashScreen splashScreen, bool serviceMode)
        {
            this.splashScreen = splashScreen;

            this.filtersWorker.DoWork += new DoWorkEventHandler(filtersWorker_DoWork);
            this.filtersWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(filtersWorker_RunWorkerCompleted);

            if (serviceMode)
            {
                this.mediaCenter = new ConfigClientService();
            }
            else
            {
                DirectoryInfo di = Directory.CreateDirectory(Path.Combine(System.Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Home Media Center"));
                this.mediaCenter = new MediaServerDevice(di.FullName);
            }

            this.logEventHandler = new EventHandler<LogEventArgs>(mediaCenter_LogEvent);
            this.mediaCenter.AsyncStartEnd += new EventHandler<ExceptionEventArgs>(mediaCenter_AsyncStartEnd);
            this.mediaCenter.AsyncStopEnd += new EventHandler<ExceptionEventArgs>(mediaCenter_AsyncStopEnd);
            this.mediaCenter.IItemManager.AsyncBuildDatabaseStart += new EventHandler<ExceptionEventArgs>(ItemManager_AsyncBuildDatabaseStart);
            this.mediaCenter.IItemManager.AsyncBuildDatabaseEnd += new EventHandler<ExceptionEventArgs>(ItemManager_AsyncBuildDatabaseEnd);

            InitializeComponent();
        }

        private void InitLanguage()
        {
            this.languageComboBox.Items.Clear();
            this.languageComboBox.Items.Add(LanguageResource.Automatic);
            foreach (string language in this.supportedLanguages.Where(a => a != null))
            {
                this.languageComboBox.Items.Add(new System.Globalization.CultureInfo(language).NativeName);
            }

            this.addDirButton.Text = LanguageResource.Add;
            this.removeDirButton.Text = LanguageResource.Remove;
            this.directoriesGroup.Text = LanguageResource.Directories;
            this.webLinkLabel.Text = LanguageResource.WebInterface;
            this.startButton.Text = LanguageResource.Start;
            this.buildDatabaseButton.Text = LanguageResource.RefreshDatabase;
            this.logTabPage.Text = LanguageResource.Log;
            this.logCheckBox.Text = LanguageResource.EnableLog;
            this.logLengthLabel.Text = LanguageResource.LogLength;
            this.copyLogMenuItem.Text = LanguageResource.Copy;
            this.statusLabel.Text = LanguageResource.Status;
            this.statusTextLabel.Text = LanguageResource.Stopped;
            this.homeTabPage.Text = LanguageResource.Home;
            this.codecsGroup.Text = LanguageResource.Codecs;
            this.requestLabel.Text = LanguageResource.RequestCount;
            this.settingsTabPage.Text = LanguageResource.Settings;
            this.audioGroupBox.Text = LanguageResource.Audio;
            this.imageGroupBox.Text = LanguageResource.Image;
            this.videoGroupBox.Text = LanguageResource.Video;
            this.streamGroupBox.Text = LanguageResource.Stream;
            this.friendlyNameLabel.Text = LanguageResource.FriendlyName;
            this.portLabel.Text = LanguageResource.Port;
            this.languageLabel.Text = LanguageResource.Language;
            this.generateThumbnailsCheckBox.Text = LanguageResource.GenerateThumbnails;
            this.showHiddenCheckBox.Text = LanguageResource.ShowHiddenFiles;
            this.audioNativeCheck.Text = LanguageResource.AllowNative;
            this.imageNativeCheck.Text = LanguageResource.AllowNative;
            this.videoNativeCheck.Text = LanguageResource.AllowNative;
            this.audioParamLabel.Text = LanguageResource.EncodeParameters;
            this.imageParamLabel.Text = LanguageResource.EncodeParameters;
            this.videoParamLabel.Text = LanguageResource.EncodeParameters;
            this.streamParamLabel.Text = LanguageResource.EncodeParameters;
            this.applySettingsButton.Text = LanguageResource.ApplyRestart;
            this.converterTabPage.Text = LanguageResource.Converter;
            this.convertButton.Text = LanguageResource.Convert;
            this.aboutTabPage.Text = LanguageResource.About;
            this.settingsLinkLabel.Text = LanguageResource.ConfigFiles;
            this.minimizeCheckBox.Text = LanguageResource.MinimizeToTray;
            this.startupCheckBox.Text = LanguageResource.RunAtStartup;
            this.realTimeDatabaseRefreshCheckBox.Text = LanguageResource.RealTimeDatabaseRefresh;
            this.tryToForwardPortCheckBox.Text = LanguageResource.EnablePortForwarding;
            this.streamSourcesButton.Text = LanguageResource.Stream;
            this.desktopStreamingCheck.Text = LanguageResource.EnableDesktopStreaming;
            this.webcamStreamingCheck.Text = LanguageResource.EnableWebcamStreaming;
            this.mainToolTip.SetToolTip(this.audioParamEditButton, LanguageResource.Edit);
            this.mainToolTip.SetToolTip(this.videoParamEditButton, LanguageResource.Edit);
            this.mainToolTip.SetToolTip(this.imageParamEditButton, LanguageResource.Edit);
            this.mainToolTip.SetToolTip(this.videoStreamParamEditButton, LanguageResource.Edit);
            this.mainToolTip.SetToolTip(this.audioParamAddButton, LanguageResource.Add);
            this.mainToolTip.SetToolTip(this.videoParamAddButton, LanguageResource.Add);
            this.mainToolTip.SetToolTip(this.imageParamAddButton, LanguageResource.Add);
            this.mainToolTip.SetToolTip(this.videoStreamParamAddButton, LanguageResource.Add);
            this.mainToolTip.SetToolTip(this.audioParamRemoveButton, LanguageResource.Remove);
            this.mainToolTip.SetToolTip(this.videoParamRemoveButton, LanguageResource.Remove);
            this.mainToolTip.SetToolTip(this.imageParamRemoveButton, LanguageResource.Remove);
            this.mainToolTip.SetToolTip(this.videoStreamParamRemoveButton, LanguageResource.Remove);
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            //Zobrazenie v tray oblasti
            if (Program.HasBackgroundArg())
            {
                this.WindowState = FormWindowState.Minimized;
                this.ShowInTaskbar = false;
                this.mainNotifyIcon.Visible = true;
            }

            //Zisti ci sa spusta pri starte
            try { this.startupCheckBox.Checked = Registry.CurrentUser.OpenSubKey("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", false).GetValue(this.regRunName) != null; }
            catch { this.startupCheckBox.Checked = false; }

            //Nacitaj nastavenia - ak neexistuju je vyhlasena chyba
            //Pri sluzbe procedura nerobi nic
            try { this.mediaCenter.LoadSettings(); }
            catch (CommunicationException) { } //Neskor sa spracuje tato vynimka
            catch { MessageBox.Show(this, LanguageResource.ConfigCorrupted, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning); }

            //InitLanguage az po LoadSettings - nastavi sa jazyk
            try { this.mediaCenter.SetCurrentThreadCulture(); }
            catch (CommunicationException) { } //Neskor sa spracuje tato vynimka

            InitLanguage();

            this.Text = "Home Media Center" + (this.mediaCenter.IsWinService ? " - " + LanguageResource.Console : string.Empty);

            string str0 = System.Environment.Is64BitProcess ? "x64" : "x86";

            string str1;
            try { str1 = System.Reflection.AssemblyName.GetAssemblyName("DSWrapper.dll").FullName; }
            catch { str1 = string.Empty; }

            string str2;
            try { str2 = System.Reflection.AssemblyName.GetAssemblyName("MFWrapper.dll").FullName; }
            catch { str2 = string.Empty; }

            string str3 = System.Reflection.Assembly.GetAssembly(typeof(HomeMediaCenter.MediaServerDevice)).FullName;
            this.aboutLabel.Text = string.Format("Home Media Center {0}\r\nTomáš Pšenák © 2016\r\ntomaspsenak@gmail.com\r\n{1}\r\n----------------------------------------------\r\n{2}\r\n{3}\r\n{4}",
                str0, LanguageResource.TranslatedBy, str1, str2, str3);

            try
            {
                this.dirListBox.DataSource = this.mediaCenter.IItemManager.GetDirectories();
                this.minimizeCheckBox.Checked = this.mediaCenter.MinimizeToTray;

                if (this.mediaCenter.IsWinService)
                    SetStatusText(this.mediaCenter.Started);
                else
                    StartMediaCenterAsync();
            }
            catch (CommunicationException)
            {
                this.splashScreen.SetStatusLabel(LanguageResource.UnableToConnectService + ". " + LanguageResource.ApplicationWillBeClosed);
                System.Threading.Thread.Sleep(5000);
                Close();
                return;
            }

            this.filtersWorker.RunWorkerAsync();

            this.splashScreen.Close(this);
        }

        private void filtersWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            e.Result = new bool[] {
                DirectShowEncoder.IsFFDSHOWInstalled(),
                DirectShowEncoder.IsWMVInstalled(),
                DirectShowEncoder.IsMPEG2Installed(),
                DirectShowEncoder.IsLAVSplitInstalled(),
                DirectShowEncoder.IsHMCInstalled()
            };
        }

        private void filtersWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            if (e.Error != null)
                return;

            bool[] results = (bool[])e.Result;
            this.ffdshowLabel.Text = results[0] ? LanguageResource.Installed : LanguageResource.NotInstalled;
            this.wmvLabel.Text = results[1] ? LanguageResource.Installed : LanguageResource.NotInstalled;
            this.mpeg2Label.Text = results[2] ? LanguageResource.Installed : LanguageResource.NotInstalled;
            this.lavLabel.Text = results[3] ? LanguageResource.Installed : LanguageResource.NotInstalled;
            this.hmcLabel.Text = results[4] ? LanguageResource.Installed : LanguageResource.NotInstalled;
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.ApplicationExitCall)
                return;

            //Skryje form - na pozadi este korektne ukonci server a ulozi nastavenia
            Hide();

            try
            {
                if (!this.mediaCenter.SaveSettings())
                    MessageBox.Show(this, LanguageResource.UnableToSaveConfigFile, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Error);

                if (this.mediaCenter.IsWinService || !this.mediaCenter.Started)
                    return;
            }
            catch (CommunicationException) { return; }

            e.Cancel = true;

            StopMediaCenterAsync(AfterStopped.Close);
        }

        private void addDirButton_Click(object sender, EventArgs e)
        {
            AddDirForm adf = new AddDirForm();
            while (true)
            {
                if (adf.ShowDialog(this) == System.Windows.Forms.DialogResult.OK)
                {
                    try
                    {
                        if (this.mediaCenter.IItemManager.CheckDirectoryPermission(adf.PathText))
                        {
                            this.dirListBox.DataSource = this.mediaCenter.IItemManager.AddDirectory(adf.PathText, adf.TitleText);
                            return;
                        }

                        MessageBox.Show(this, LanguageResource.AccessIsDenied, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                    catch (CommunicationException) { ShowCommunicationExceptionMessage(); }
                }
                else
                {
                    return;
                }
            }
        }

        private void removeDirButton_Click(object sender, EventArgs e)
        {
            string dir = this.dirListBox.SelectedItem as string;
            if (dir == null)
            {
                MessageBox.Show(this, LanguageResource.SelectDirectory);
                return;
            }

            if (MessageBox.Show(this, string.Format(LanguageResource.RemoveFolderQuestion, dir), LanguageResource.Directories,
                MessageBoxButtons.YesNo) == DialogResult.Yes)
            {
                try
                {
                    this.dirListBox.DataSource = this.mediaCenter.IItemManager.RemoveDirectory(dir);
                }
                catch (CommunicationException) { ShowCommunicationExceptionMessage(); }
            }
        }

        private void webLinkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            try
            {
                string address = "localhost";
                System.Diagnostics.Process.Start(string.Format("http://{0}:{1}/", address, this.mediaCenter.IServer.HttpPort));
            }
            catch (CommunicationException) { ShowCommunicationExceptionMessage(); }
            catch { }
        }

        private void startButton_Click(object sender, EventArgs e)
        {
            try
            {
                if (this.mediaCenter.Started)
                    StopMediaCenterAsync(AfterStopped.Nothing);
                else
                    StartMediaCenterAsync();
            }
            catch (CommunicationException) { ShowCommunicationExceptionMessage(); }
        }

        private void buildDatabaseButton_Click(object sender, EventArgs e)
        {
            try
            {
                this.mediaCenter.IItemManager.BuildDatabaseAsync();
            }
            catch (CommunicationException) { ShowCommunicationExceptionMessage(); }
        }

        private void logCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            try
            {
                if (this.logCheckBox.Checked)
                    this.mediaCenter.LogEvent += new EventHandler<LogEventArgs>(mediaCenter_LogEvent);
                else
                    this.mediaCenter.LogEvent -= new EventHandler<LogEventArgs>(mediaCenter_LogEvent);
            }
            catch (CommunicationException) { ShowCommunicationExceptionMessage(); }
        }

        private void mediaCenter_LogEvent(object sender, LogEventArgs e)
        {
            if (InvokeRequired)
            {
                BeginInvoke(this.logEventHandler, sender, e);
                return;
            }

            if (e.Type == LogEventType.Message)
            {
                this.logListBox.Items.Add(e.Message);
                if (this.logListBox.Items.Count > this.logLengthUpDown.Value)
                    this.logListBox.Items.RemoveAt(0);

                this.logListBox.TopIndex = this.logListBox.Items.Count - 1;
            }
            else
            {
                this.requestCountLabel.Text = e.Message;
            }
        }

        private void logLengthUpDown_ValueChanged(object sender, EventArgs e)
        {
            while (this.logListBox.Items.Count > this.logLengthUpDown.Value)
                this.logListBox.Items.RemoveAt(0);
        }

        private void StartMediaCenterAsync()
        {
            SetButtonEnabled(false);

            this.mediaCenter.StartAsync();
        }

        private void mediaCenter_AsyncStartEnd(object sender, ExceptionEventArgs e)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new EventHandler<ExceptionEventArgs>(mediaCenter_AsyncStartEnd), sender, e);
                return;
            }

            SetButtonEnabled(true);

            if (e.Exception != null)
            {
                SetStatusText(false);

                MessageBox.Show(this, e.Exception.Message, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            SetStatusText(true);

            this.mediaCenter.IItemManager.BuildDatabaseAsync();
        }

        private void ItemManager_AsyncBuildDatabaseStart(object sender, ExceptionEventArgs e)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new EventHandler<ExceptionEventArgs>(ItemManager_AsyncBuildDatabaseStart), sender, e);
                return;
            }

            this.statusTextLabel.Text = LanguageResource.RefreshingDatabase;
            this.statusTextLabel.ForeColor = Color.Orange;
        }

        private void ItemManager_AsyncBuildDatabaseEnd(object sender, ExceptionEventArgs e)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new EventHandler<ExceptionEventArgs>(ItemManager_AsyncBuildDatabaseEnd), sender, e);
                return;
            }

            SetStatusText(this.mediaCenter.Started);

            if (e.Exception != null)
            {
                MessageBox.Show(this, e.Exception.Message, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void StopMediaCenterAsync(AfterStopped afterStopped)
        {
            this.afterStopped = afterStopped;
            SetButtonEnabled(false);

            this.mediaCenter.StopAsync();
        }

        private void mediaCenter_AsyncStopEnd(object sender, ExceptionEventArgs e)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new EventHandler<ExceptionEventArgs>(mediaCenter_AsyncStopEnd), sender, e);
                return;
            }

            SetStatusText(false);

            if (this.afterStopped == AfterStopped.Close)
            {
                Application.Exit();
            }
            else if (this.afterStopped == AfterStopped.LoadSettings)
            {
                ApplySettings();
                StartMediaCenterAsync();
            }
            else
            {
                SetButtonEnabled(true);
            }
        }

        private void SetStatusText(bool started)
        {
            if (started)
            {
                this.statusTextLabel.Text = LanguageResource.Started;
                this.statusTextLabel.ForeColor = Color.Green;
                this.startButton.Text = LanguageResource.Stop;
            }
            else
            {
                this.statusTextLabel.Text = LanguageResource.Stopped;
                this.statusTextLabel.ForeColor = Color.Red;
                this.startButton.Text = LanguageResource.Start;
            }
        }

        private void SetButtonEnabled(bool enabled)
        {
            this.startButton.Enabled = enabled;
            this.buildDatabaseButton.Enabled = enabled;
            this.friendlyNameText.Enabled = enabled;
            this.portText.Enabled = enabled;
            this.applySettingsButton.Enabled = enabled;
            this.audioGroupBox.Enabled = enabled;
            this.imageGroupBox.Enabled = enabled;
            this.videoGroupBox.Enabled = enabled;
            this.streamGroupBox.Enabled = enabled;
            this.startupCheckBox.Enabled = enabled;
            this.minimizeCheckBox.Enabled = enabled;
            this.realTimeDatabaseRefreshCheckBox.Enabled = enabled;
            this.tryToForwardPortCheckBox.Enabled = enabled;
            this.streamSourcesButton.Enabled = enabled;
            this.generateThumbnailsCheckBox.Enabled = enabled;
            this.languageComboBox.Enabled = enabled;
            this.showHiddenCheckBox.Enabled = enabled;
        }

        private void mainTabControl_Selected(object sender, TabControlEventArgs e)
        {
            if (this.mainTabControl.SelectedTab == this.settingsTabPage)
            {
                try
                {
                    string langName = this.mediaCenter.CultureInfoName;
                    for (int i = 0; i < this.supportedLanguages.Length; i++)
                    {
                        if (this.supportedLanguages[i] == langName)
                        {
                            this.languageComboBox.SelectedIndex = i;
                            break;
                        }
                    }

                    this.friendlyNameText.Text = this.mediaCenter.FriendlyName;
                    this.portText.Text = this.mediaCenter.IServer.HttpPort.ToString();
                    this.tryToForwardPortCheckBox.Checked = this.mediaCenter.TryToForwardPort;

                    this.webcamStreamingCheck.Checked = this.mediaCenter.IItemManager.EnableWebcamStreaming;
                    this.desktopStreamingCheck.Checked = this.mediaCenter.IItemManager.EnableDesktopStreaming;
                    this.realTimeDatabaseRefreshCheckBox.Checked = this.mediaCenter.IItemManager.RealTimeDatabaseRefresh;
                    this.showHiddenCheckBox.Checked = this.mediaCenter.IItemManager.ShowHiddenFiles;
                    this.generateThumbnailsCheckBox.Checked = this.mediaCenter.GenerateThumbnails;

                    IMediaSettings settings = this.mediaCenter.IItemManager.IMediaSettings;

                    this.audioNativeCheck.Checked = settings.GetNativeFile(EncodeType.Audio);
                    this.audioParamCombo.Items.Clear();
                    this.audioParamCombo.Items.AddRange(settings.GetEncodeStrings(EncodeType.Audio));
                    this.audioParamCombo.SelectedItem = this.audioParamCombo.Items.Cast<string>().FirstOrDefault();

                    this.imageNativeCheck.Checked = settings.GetNativeFile(EncodeType.Image);
                    this.imageParamCombo.Items.Clear();
                    this.imageParamCombo.Items.AddRange(settings.GetEncodeStrings(EncodeType.Image));
                    this.imageParamCombo.SelectedItem = this.imageParamCombo.Items.Cast<string>().FirstOrDefault();

                    this.videoNativeCheck.Checked = settings.GetNativeFile(EncodeType.Video);
                    this.videoParamCombo.Items.Clear();
                    this.videoParamCombo.Items.AddRange(settings.GetEncodeStrings(EncodeType.Video));
                    this.videoParamCombo.SelectedItem = this.videoParamCombo.Items.Cast<string>().FirstOrDefault();

                    this.videoStreamParamCombo.Items.Clear();
                    this.videoStreamParamCombo.Items.AddRange(settings.GetEncodeStrings(EncodeType.Stream));
                    this.videoStreamParamCombo.SelectedItem = this.videoStreamParamCombo.Items.Cast<string>().FirstOrDefault();
                }
                catch (CommunicationException) { }
            }
            else if (this.mainTabControl.SelectedTab == this.converterTabPage)
            {
                this.paramControl.LoadSettings(null, true, true, false, false, true);
            }
        }

        private void applySettingsButton_Click(object sender, EventArgs e)
        {
            try
            {
                if (this.mediaCenter.Started)
                {
                    StopMediaCenterAsync(AfterStopped.LoadSettings);
                }
                else
                {
                    ApplySettings();
                }
            }
            catch (CommunicationException) { ShowCommunicationExceptionMessage(); }
        }

        private void ApplySettings()
        {
            this.mediaCenter.CultureInfoName = this.supportedLanguages[this.languageComboBox.SelectedIndex];

            this.mediaCenter.FriendlyName = this.friendlyNameText.Text;
            this.mediaCenter.IServer.HttpPort = int.Parse(this.portText.Text);
            this.mediaCenter.TryToForwardPort = this.tryToForwardPortCheckBox.Checked;

            this.mediaCenter.IItemManager.EnableWebcamStreaming = this.webcamStreamingCheck.Checked;
            this.mediaCenter.IItemManager.EnableDesktopStreaming = this.desktopStreamingCheck.Checked;
            this.mediaCenter.IItemManager.RealTimeDatabaseRefresh = this.realTimeDatabaseRefreshCheckBox.Checked;
            this.mediaCenter.IItemManager.ShowHiddenFiles = this.showHiddenCheckBox.Checked;
            this.mediaCenter.GenerateThumbnails = this.generateThumbnailsCheckBox.Checked;

            IMediaSettings settings = this.mediaCenter.IItemManager.IMediaSettings;

            settings.SetNativeFile(EncodeType.Audio, this.audioNativeCheck.Checked);
            settings.SetEncodeStrings(EncodeType.Audio, this.audioParamCombo.Items.Cast<string>().Select(a => EncoderBuilder.GetEncoder(a).GetParamString()).ToArray());

            settings.SetNativeFile(EncodeType.Image, this.imageNativeCheck.Checked);
            settings.SetEncodeStrings(EncodeType.Image, this.imageParamCombo.Items.Cast<string>().Select(a => EncoderBuilder.GetEncoder(a).GetParamString()).ToArray());

            settings.SetNativeFile(EncodeType.Video, this.videoNativeCheck.Checked);
            settings.SetEncodeStrings(EncodeType.Video, this.videoParamCombo.Items.Cast<string>().Select(a => EncoderBuilder.GetEncoder(a).GetParamString()).ToArray());

            settings.SetEncodeStrings(EncodeType.Stream, this.videoStreamParamCombo.Items.Cast<string>().Select(a => EncoderBuilder.GetEncoder(a).GetParamString()).ToArray());
        }

        private void audioParamRemoveButton_Click(object sender, EventArgs e)
        {
            if (this.audioParamCombo.SelectedIndex < 0)
                return;

            this.audioParamCombo.Items.RemoveAt(this.audioParamCombo.SelectedIndex);
        }

        private void imageParamRemoveButton_Click(object sender, EventArgs e)
        {
            if (this.imageParamCombo.SelectedIndex < 0)
                return;

            this.imageParamCombo.Items.RemoveAt(this.imageParamCombo.SelectedIndex);
        }

        private void videoParamRemoveButton_Click(object sender, EventArgs e)
        {
            if (this.videoParamCombo.SelectedIndex < 0)
                return;

            this.videoParamCombo.Items.RemoveAt(this.videoParamCombo.SelectedIndex);
        }

        private void videoStreamParamRemoveButton_Click(object sender, EventArgs e)
        {
            if (this.videoStreamParamCombo.SelectedIndex < 0)
                return;

            this.videoStreamParamCombo.Items.RemoveAt(this.videoStreamParamCombo.SelectedIndex);
        }

        private void audioParamAddButton_Click(object sender, EventArgs e)
        {
            ParametersForm form = new ParametersForm() { Text = LanguageResource.ParameterEditor + " - " + LanguageResource.Audio };

            if (form.ShowDialog(this, null, true, false, false, true) == DialogResult.OK)
            {
                try { this.audioParamCombo.Items.Add(EncoderBuilder.GetEncoder(form.QueryString).GetParamString()); }
                catch (Exception ex)
                {
                    MessageBox.Show(this, ex.Message, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
        }

        private void imageParamAddButton_Click(object sender, EventArgs e)
        {
            ParametersForm form = new ParametersForm() { Text = LanguageResource.ParameterEditor + " - " + LanguageResource.Image };

            if (form.ShowDialog(this, null, false, false, true, true) == DialogResult.OK)
            {
                try { this.imageParamCombo.Items.Add(EncoderBuilder.GetEncoder(form.QueryString).GetParamString()); }
                catch (Exception ex)
                {
                    MessageBox.Show(this, ex.Message, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
        }

        private void videoParamAddButton_Click(object sender, EventArgs e)
        {
            ParametersForm form = new ParametersForm() { Text = LanguageResource.ParameterEditor + " - " + LanguageResource.Video };

            if (form.ShowDialog(this, null, false, true, false, true) == DialogResult.OK)
            {
                try { this.videoParamCombo.Items.Add(EncoderBuilder.GetEncoder(form.QueryString).GetParamString()); }
                catch (Exception ex)
                {
                    MessageBox.Show(this, ex.Message, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
        }

        private void videoStreamParamAddButton_Click(object sender, EventArgs e)
        {
            ParametersForm form = new ParametersForm() { Text = LanguageResource.ParameterEditor + " - " + LanguageResource.Video };

            if (form.ShowDialog(this, null, true, true, false, true) == DialogResult.OK)
            {
                try { this.videoStreamParamCombo.Items.Add(EncoderBuilder.GetEncoder(form.QueryString).GetParamString()); }
                catch (Exception ex)
                {
                    MessageBox.Show(this, ex.Message, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
        }

        private void audioParamEditButton_Click(object sender, EventArgs e)
        {
            if (this.audioParamCombo.SelectedIndex < 0)
                return;

            ParametersForm form = new ParametersForm() { Text = LanguageResource.ParameterEditor + " - " + LanguageResource.Audio };

            if (form.ShowDialog(this, this.audioParamCombo.SelectedItem as string, true, false, false, true) == DialogResult.OK)
            {
                try { this.audioParamCombo.Items[this.audioParamCombo.SelectedIndex] = EncoderBuilder.GetEncoder(form.QueryString).GetParamString(); }
                catch (Exception ex)
                {
                    MessageBox.Show(this, ex.Message, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
        }

        private void imageParamEditButton_Click(object sender, EventArgs e)
        {
            if (this.imageParamCombo.SelectedIndex < 0)
                return;

            ParametersForm form = new ParametersForm() { Text = LanguageResource.ParameterEditor + " - " + LanguageResource.Image };

            if (form.ShowDialog(this, this.imageParamCombo.SelectedItem as string, false, false, true, true) == DialogResult.OK)
            {
                try { this.imageParamCombo.Items[this.imageParamCombo.SelectedIndex] = EncoderBuilder.GetEncoder(form.QueryString).GetParamString(); }
                catch (Exception ex)
                {
                    MessageBox.Show(this, ex.Message, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
        }

        private void videoParamEditButton_Click(object sender, EventArgs e)
        {
            if (this.videoParamCombo.SelectedIndex < 0)
                return;

            ParametersForm form = new ParametersForm() { Text = LanguageResource.ParameterEditor + " - " + LanguageResource.Video };

            if (form.ShowDialog(this, this.videoParamCombo.SelectedItem as string, false, true, false, true) == DialogResult.OK)
            {
                try { this.videoParamCombo.Items[this.videoParamCombo.SelectedIndex] = EncoderBuilder.GetEncoder(form.QueryString).GetParamString(); }
                catch (Exception ex)
                {
                    MessageBox.Show(this, ex.Message, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
        }

        private void videoStreamParamEditButton_Click(object sender, EventArgs e)
        {
            if (this.videoStreamParamCombo.SelectedIndex < 0)
                return;

            ParametersForm form = new ParametersForm() { Text = LanguageResource.ParameterEditor + " - " + LanguageResource.Video };

            if (form.ShowDialog(this, this.videoStreamParamCombo.SelectedItem as string, true, true, false, true) == DialogResult.OK)
            {
                try { this.videoStreamParamCombo.Items[this.videoStreamParamCombo.SelectedIndex] = EncoderBuilder.GetEncoder(form.QueryString).GetParamString(); }
                catch (Exception ex)
                {
                    MessageBox.Show(this, ex.Message, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
        }

        private void convertButton_Click(object sender, EventArgs e)
        {
            try
            {
                EncoderBuilder builder = this.paramControl.GetEncoder();
                if (builder == null)
                    throw new Exception("Unknown codec");

                ConvertForm cf = new ConvertForm(builder, this.paramControl.OutputText);
                cf.Show(this);
            }
            catch (Exception ex)
            {
                MessageBox.Show(this, ex.Message, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void settingsLinkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            try
            {
                System.Diagnostics.Process.Start(this.mediaCenter.DataDirectory);
            }
            catch (CommunicationException) { ShowCommunicationExceptionMessage(); }
            catch { }
        }

        private void homepageLinkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            try
            {
                System.Diagnostics.Process.Start("http://hmc.codeplex.com");
            }
            catch { }
        }

        private void MainForm_Resize(object sender, EventArgs e)
        {
            try
            {
                if (this.WindowState == FormWindowState.Minimized && this.mediaCenter.MinimizeToTray)
                {
                    this.ShowInTaskbar = false;
                    this.mainNotifyIcon.Visible = true;
                }
            }
            catch { }
        }

        private void mainNotifyIcon_Click(object sender, EventArgs e)
        {
            this.ShowInTaskbar = true;
            this.mainNotifyIcon.Visible = false;
            this.WindowState = FormWindowState.Normal;
        }

        private void minimizeCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            try
            {
                this.mediaCenter.MinimizeToTray = this.minimizeCheckBox.Checked;
            }
            catch (CommunicationException) { ShowCommunicationExceptionMessage(); }
        }

        private void startupCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            try
            {
                RegistryKey rk = Registry.CurrentUser.OpenSubKey("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", true);
                if (this.startupCheckBox.Checked)
                {
                    rk.SetValue(this.regRunName, "\"" + Application.ExecutablePath.ToString() + "\" /b");
                    this.minimizeCheckBox.Checked = true;
                }
                else
                {
                    rk.DeleteValue(this.regRunName, false);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(this, ex.Message, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void streamSourcesButton_Click(object sender, EventArgs e)
        {
            try
            {
                StreamSourcesBindingList source = new StreamSourcesBindingList(this.mediaCenter.IItemManager);
                StreamSourcesForm ssf = new StreamSourcesForm(source);

                if (ssf.ShowDialog(this) == System.Windows.Forms.DialogResult.OK)
                {
                    source.SubmitChanges();
                }
            }
            catch (CommunicationException)
            {
                ShowCommunicationExceptionMessage();
            }
            catch (Exception ex)
            {
                MessageBox.Show(this, ex.Message, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void copyLogMenuItem_Click(object sender, EventArgs e)
        {
            if (this.logListBox.SelectedItems.Count < 1)
                return;

            StringBuilder sb = new StringBuilder();
            foreach (string line in this.logListBox.SelectedItems.OfType<string>())
                sb.AppendLine(line);

            if (sb.Length > 0)
                Clipboard.SetText(sb.ToString());
        }

        private void ShowCommunicationExceptionMessage()
        {
            MessageBox.Show(this, LanguageResource.ConnectionToServiceLost + ". " + LanguageResource.TryToRestartApp,
                LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
    }
}
