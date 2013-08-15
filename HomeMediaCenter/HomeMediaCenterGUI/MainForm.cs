using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using HomeMediaCenter;
using System.Runtime.Remoting.Messaging;
using System.IO;
using System.Net;
using Microsoft.Win32;

namespace HomeMediaCenterGUI
{
    public partial class MainForm : Form
    {
        private bool mediaCenterStarted;
        private MediaServerDevice mediaCenter = new MediaServerDevice();
        private ToolTip mainToolTip = new ToolTip();

        private EventHandler<LogEventArgs> logEventHandler;

        private enum AfterStopped { Nothing, Close, LoadSettings };
        private AfterStopped afterStopped;

        private readonly string directoryPath;
        private readonly string settingsPath;
        private readonly string databasePath;
        private readonly string regRunName = "HomeMediaCenter";

        public MainForm()
        {
            this.logEventHandler = new EventHandler<LogEventArgs>(mediaCenter_LogEvent);
            this.mediaCenter.AsyncStartEnd += new EventHandler<ExceptionEventArgs>(mediaCenter_AsyncStartEnd);
            this.mediaCenter.AsyncStopEnd += new EventHandler<ExceptionEventArgs>(mediaCenter_AsyncStopEnd);
            this.mediaCenter.ItemManager.AsyncBuildDatabaseStart += new EventHandler<ExceptionEventArgs>(ItemManager_AsyncBuildDatabaseStart);
            this.mediaCenter.ItemManager.AsyncBuildDatabaseEnd += new EventHandler<ExceptionEventArgs>(ItemManager_AsyncBuildDatabaseEnd);

            DirectoryInfo di = Directory.CreateDirectory(Path.Combine(System.Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Home Media Center"));
            this.directoryPath = di.FullName;
            this.settingsPath = Path.Combine(di.FullName, "settings.xml");
            this.databasePath = Path.Combine(di.FullName, "database.db");

            InitializeComponent();

            InitLanguage();
        }

        private void InitLanguage()
        {
            this.addDirButton.Text = LanguageResource.Add;
            this.removeDirButton.Text = LanguageResource.Remove;
            this.directoriesGroup.Text = LanguageResource.Directories;
            this.webLinkLabel.Text = LanguageResource.WebInterface;
            this.startButton.Text = LanguageResource.Start;
            this.buildDatabaseButton.Text = LanguageResource.RefreshDatabase;
            this.logTabPage.Text = LanguageResource.Log;
            this.logCheckBox.Text = LanguageResource.EnableLog;
            this.logLengthLabel.Text = LanguageResource.LogLength;
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
            this.ffdshowLabel.Text = DirectShowEncoder.IsFFDSHOWInstalled() ? LanguageResource.Installed : LanguageResource.NotInstalled;
            this.wmvLabel.Text = DirectShowEncoder.IsWMVInstalled() ? LanguageResource.Installed : LanguageResource.NotInstalled;
            this.mpeg2Label.Text = DirectShowEncoder.IsMPEG2Installed() ? LanguageResource.Installed : LanguageResource.NotInstalled;
            this.webmLabel.Text = DirectShowEncoder.IsWEBMInstalled() ? LanguageResource.Installed : LanguageResource.NotInstalled;
            this.hmcLabel.Text = DirectShowEncoder.IsHMCInstalled() ? LanguageResource.Installed : LanguageResource.NotInstalled;

            string str0 = System.Environment.Is64BitProcess ? "x64" : "x86";

            string str1;
            try { str1 = System.Reflection.AssemblyName.GetAssemblyName("DSWrapper.dll").FullName; }
            catch { str1 = string.Empty; }

            string str2;
            try { str2 = System.Reflection.AssemblyName.GetAssemblyName("MFWrapper.dll").FullName; }
            catch { str2 = string.Empty; }

            string str3 = System.Reflection.Assembly.GetAssembly(typeof(HomeMediaCenter.MediaServerDevice)).FullName;
            this.aboutLabel.Text = string.Format("Home Media Center {0}\r\nTomáš Pšenák © 2013\r\ntomaspsenak@gmail.com\r\n----------------------------------------------\r\n{1}\r\n{2}\r\n{3}", str0, str1, str2, str3);

            try { this.mediaCenter.LoadSettings(this.settingsPath, this.databasePath); }
            catch { MessageBox.Show(this, LanguageResource.ConfigCorrupted, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning); }

            try { this.startupCheckBox.Checked = Registry.CurrentUser.OpenSubKey("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", false).GetValue(this.regRunName) != null; }
            catch { this.startupCheckBox.Checked = false; }

            this.dirListBox.DataSource = this.mediaCenter.ItemManager.GetDirectoriesSync();
            this.minimizeCheckBox.Checked = this.mediaCenter.MinimizeToTray;

            StartMediaCenterAsync();

            if (Environment.GetCommandLineArgs().Contains("/b", StringComparer.OrdinalIgnoreCase))
            {
                this.WindowState = FormWindowState.Minimized;
                this.ShowInTaskbar = false;
                this.mainNotifyIcon.Visible = true;
            }
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.ApplicationExitCall)
                return;

            e.Cancel = true;

            if (this.startButton.Enabled)
                StopMediaCenterAsync(AfterStopped.Close);
        }

        private void addDirButton_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog dialog = new FolderBrowserDialog();
            if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                this.dirListBox.DataSource = this.mediaCenter.ItemManager.AddDirectorySync(dialog.SelectedPath);
                this.mediaCenter.ItemManager.BuildDatabaseAsync();
            }
        }

        private void removeDirButton_Click(object sender, EventArgs e)
        {
            string dir = this.dirListBox.SelectedItem as string;
            if (dir == null)
                MessageBox.Show(this, LanguageResource.SelectDirectory);

            this.dirListBox.DataSource = this.mediaCenter.ItemManager.RemoveDirectorySync(dir);
            this.mediaCenter.ItemManager.BuildDatabaseAsync();
        }

        private void webLinkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            try
            {
                IPAddress address = Dns.GetHostAddresses(Dns.GetHostName()).Where(a => a.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork).First();
                System.Diagnostics.Process.Start(string.Format("http://{0}:{1}/web/", address, this.mediaCenter.Server.HttpPort));
            }
            catch { }
        }

        private void startButton_Click(object sender, EventArgs e)
        {
            if (this.mediaCenterStarted)
                StopMediaCenterAsync(AfterStopped.Nothing);
            else
                StartMediaCenterAsync();
        }

        private void buildDatabaseButton_Click(object sender, EventArgs e)
        {
            this.mediaCenter.ItemManager.BuildDatabaseAsync();
        }

        private void logCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            if (this.logCheckBox.Checked)
                this.mediaCenter.LogEvent += new EventHandler<LogEventArgs>(mediaCenter_LogEvent);
            else
                this.mediaCenter.LogEvent -= new EventHandler<LogEventArgs>(mediaCenter_LogEvent);
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

            if (e.Exception != null)
            {
                SetButtonEnabled(true);

                this.statusTextLabel.Text = LanguageResource.Stopped;
                this.statusTextLabel.ForeColor = Color.Red;

                MessageBox.Show(this, e.Exception.Message, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            this.mediaCenterStarted = true;
            this.startButton.Text = LanguageResource.Stop;

            this.mediaCenter.ItemManager.BuildDatabaseAsync();
        }

        private void ItemManager_AsyncBuildDatabaseStart(object sender, ExceptionEventArgs e)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new EventHandler<ExceptionEventArgs>(ItemManager_AsyncBuildDatabaseStart), sender, e);
                return;
            }

            SetButtonEnabled(false);

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

            SetButtonEnabled(true);

            if (this.mediaCenterStarted)
            {
                this.statusTextLabel.Text = LanguageResource.Started;
                this.statusTextLabel.ForeColor = Color.Green;
            }
            else
            {
                this.statusTextLabel.Text = LanguageResource.Stopped;
                this.statusTextLabel.ForeColor = Color.Red;
            }

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

            this.mediaCenterStarted = false;
            this.startButton.Text = LanguageResource.Start;

            this.statusTextLabel.Text = LanguageResource.Stopped;
            this.statusTextLabel.ForeColor = Color.Red;

            if (this.afterStopped == AfterStopped.Close)
            {
                this.mediaCenter.SaveSettings(this.settingsPath, this.databasePath);
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

        private void SetButtonEnabled(bool enabled)
        {
            this.startButton.Enabled = enabled;
            this.buildDatabaseButton.Enabled = enabled;
            this.addDirButton.Enabled = enabled;
            this.removeDirButton.Enabled = enabled;
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
        }

        private void mainTabControl_Selected(object sender, TabControlEventArgs e)
        {
            if (this.mainTabControl.SelectedTab == this.settingsTabPage)
            {
                this.friendlyNameText.Text = this.mediaCenter.FriendlyName;
                this.portText.Text = this.mediaCenter.Server.HttpPort.ToString();
                this.realTimeDatabaseRefreshCheckBox.Checked = this.mediaCenter.ItemManager.RealTimeDatabaseRefresh;
                this.tryToForwardPortCheckBox.Checked = this.mediaCenter.TryToForwardPort;

                MediaSettings settings = this.mediaCenter.ItemManager.MediaSettings;

                this.audioNativeCheck.Checked = settings.AudioNativeFile;
                this.audioParamCombo.Items.Clear();
                this.audioParamCombo.Items.AddRange(settings.AudioEncode.Select(a => a.GetParamString()).ToArray());
                this.audioParamCombo.SelectedItem = this.audioParamCombo.Items.Cast<string>().FirstOrDefault();

                this.imageNativeCheck.Checked = settings.ImageNativeFile;
                this.imageParamCombo.Items.Clear();
                this.imageParamCombo.Items.AddRange(settings.ImageEncode.Select(a => a.GetParamString()).ToArray());
                this.imageParamCombo.SelectedItem = this.imageParamCombo.Items.Cast<string>().FirstOrDefault();

                this.videoNativeCheck.Checked = settings.VideoNativeFile;
                this.videoParamCombo.Items.Clear();
                this.videoParamCombo.Items.AddRange(settings.VideoEncode.Select(a => a.GetParamString()).ToArray());
                this.videoParamCombo.SelectedItem = this.videoParamCombo.Items.Cast<string>().FirstOrDefault();
                this.videoStreamParamCombo.Items.Clear();
                this.videoStreamParamCombo.Items.AddRange(settings.StreamEncode.Select(a => a.GetParamString()).ToArray());
                this.videoStreamParamCombo.SelectedItem = this.videoStreamParamCombo.Items.Cast<string>().FirstOrDefault();
            }
            else if (this.mainTabControl.SelectedTab == this.converterTabPage)
            {
                this.paramControl.LoadSettings(null, true, true, false, false, true);
            }
        }

        private void applySettingsButton_Click(object sender, EventArgs e)
        {
            if (this.mediaCenterStarted)
                StopMediaCenterAsync(AfterStopped.LoadSettings);
            else
                ApplySettings();
        }

        private void ApplySettings()
        {
            this.mediaCenter.FriendlyName = this.friendlyNameText.Text;
            this.mediaCenter.Server.HttpPort = int.Parse(this.portText.Text);
            this.mediaCenter.ItemManager.RealTimeDatabaseRefresh = this.realTimeDatabaseRefreshCheckBox.Checked;
            this.mediaCenter.TryToForwardPort = this.tryToForwardPortCheckBox.Checked;

            MediaSettings settings = this.mediaCenter.ItemManager.MediaSettings;

            settings.AudioNativeFile = this.audioNativeCheck.Checked;
            settings.AudioEncode = this.audioParamCombo.Items.Cast<string>().Select(a => EncoderBuilder.GetEncoder(a)).ToList();

            settings.ImageNativeFile = this.imageNativeCheck.Checked;
            settings.ImageEncode = this.imageParamCombo.Items.Cast<string>().Select(a => EncoderBuilder.GetEncoder(a)).ToList();

            settings.VideoNativeFile = this.videoNativeCheck.Checked;
            settings.VideoEncode = this.videoParamCombo.Items.Cast<string>().Select(a => EncoderBuilder.GetEncoder(a)).ToList();
            settings.StreamEncode = this.videoStreamParamCombo.Items.Cast<string>().Select(a => EncoderBuilder.GetEncoder(a)).ToList();
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
                System.Diagnostics.Process.Start(this.directoryPath);
            }
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
            if (this.WindowState == FormWindowState.Minimized && this.mediaCenter.MinimizeToTray)
            {
                this.ShowInTaskbar = false;
                this.mainNotifyIcon.Visible = true;
            }
        }

        private void mainNotifyIcon_Click(object sender, EventArgs e)
        {
            this.ShowInTaskbar = true;
            this.mainNotifyIcon.Visible = false;
            this.WindowState = FormWindowState.Normal;
        }

        private void minimizeCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            this.mediaCenter.MinimizeToTray = this.minimizeCheckBox.Checked;
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
    }
}
