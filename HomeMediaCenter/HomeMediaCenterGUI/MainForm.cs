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

namespace HomeMediaCenterGUI
{
    public partial class MainForm : Form
    {
        private bool mediaCenterStarted;
        private MediaServerDevice mediaCenter = new MediaServerDevice();

        private EventHandler<LogEventArgs> logEventHandler;
        private Action startMediaCenterDel;
        private Action stopMediaCenterDel;
        private Action buildDatabaseDel;

        private enum AfterStopped { Nothing, Close, LoadSettings };
        private AfterStopped afterStopped;

        private readonly string directoryPath;
        private readonly string settingsPath;
        private readonly string databasePath;

        public MainForm()
        {
            this.logEventHandler = new EventHandler<LogEventArgs>(mediaCenter_LogEvent);
            this.startMediaCenterDel = new Action(delegate() { this.mediaCenter.Start(); });
            this.stopMediaCenterDel = new Action(delegate() { this.mediaCenter.Stop(); });
            this.buildDatabaseDel = new Action(delegate() { this.mediaCenter.ItemManager.BuildDatabaseSync(); });

            DirectoryInfo di = Directory.CreateDirectory(Path.Combine(System.Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Home Media Center"));
            this.directoryPath = di.FullName;
            this.settingsPath = Path.Combine(di.FullName, "settings.xml");
            this.databasePath = Path.Combine(di.FullName, "database.db");

            InitializeComponent();

            this.containerComboBox.Items.Add(new ContainerMP4());
            this.containerComboBox.Items.Add(new ContainerWMV3());
            this.containerComboBox.Items.Add(new ContainerMPEG_PS());
            this.containerComboBox.Items.Add(new ContainerMPEG_TS());
            this.containerComboBox.Items.Add(new ContainerWEBM());
            this.containerComboBox.Items.Add(new ContainerWMV2());

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
            this.friendlyNameLabel.Text = LanguageResource.FriendlyName;
            this.portLabel.Text = LanguageResource.Port;
            this.audioNativeCheck.Text = LanguageResource.AllowNative;
            this.imageNativeCheck.Text = LanguageResource.AllowNative;
            this.videoNativeCheck.Text = LanguageResource.AllowNative;
            this.audioParamLabel.Text = LanguageResource.EncodeParameters;
            this.imageParamLabel.Text = LanguageResource.EncodeParameters;
            this.videoParamLabel.Text = LanguageResource.EncodeParameters;
            this.videoStreamParamLabel.Text = LanguageResource.EncodeParametersStream;
            this.applySettingsButton.Text = LanguageResource.ApplyRestart;
            this.converterTabPage.Text = LanguageResource.Converter;
            this.containerGroupBox.Text = LanguageResource.ContainerCodec;
            this.audioCheckBox.Text = LanguageResource.Audio;
            this.videoCheckBox.Text = LanguageResource.Video;
            this.inputGroupBox.Text = LanguageResource.Input;
            this.outputGroupBox.Text = LanguageResource.Output;
            this.browseInputButton.Text = LanguageResource.Browse;
            this.browseOutputButton.Text = LanguageResource.Browse;
            this.resolutionGroupBox.Text = LanguageResource.Resolution;
            this.resolutionComboBox.Items[0] = LanguageResource.SameAsSource;
            this.keepAspectCheckBox.Text = LanguageResource.KeepAspectRatio;
            this.bitrateGroupBox.Text = LanguageResource.BitRate;
            this.bitrateVideoLabel.Text = LanguageResource.Video;
            this.bitrateAudioLabel.Text = LanguageResource.Audio;
            this.othersGroupBox.Text = LanguageResource.Others;
            this.subtitlesIntCheckBox.Text = LanguageResource.IntegrateSubtitles;
            this.startEndTimeLabel.Text = LanguageResource.StartEndTime;
            this.fpsLabel.Text = LanguageResource.FPS;
            this.fpsComboBox.Items[0] = LanguageResource.SameAsSource;
            this.convertButton.Text = LanguageResource.Convert;
            this.aboutTabPage.Text = LanguageResource.About;
            this.settingsLinkLabel.Text = LanguageResource.ConfigFiles;
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            this.ffdshowLabel.Text = DSWrapper.ContainerType.IsFFDSHOWInstalled() ? LanguageResource.Installed : LanguageResource.NotInstalled;
            this.wmvLabel.Text = DSWrapper.ContainerType.IsWMVInstalled() ? LanguageResource.Installed : LanguageResource.NotInstalled;
            this.mpeg2Label.Text = DSWrapper.ContainerType.IsMPEG2Installed() ? LanguageResource.Installed : LanguageResource.NotInstalled;
            this.webmLabel.Text = DSWrapper.ContainerType.IsWEBMInstalled() ? LanguageResource.Installed : LanguageResource.NotInstalled;

            string str0 = System.Environment.Is64BitProcess ? "x64" : "x86";
            string str1 = System.Reflection.Assembly.GetAssembly(typeof(DSWrapper.DSEncoder)).FullName;
            string str2 = System.Reflection.Assembly.GetAssembly(typeof(MFWrapper.MFEncoder)).FullName;
            string str3 = System.Reflection.Assembly.GetAssembly(typeof(HomeMediaCenter.MediaServerDevice)).FullName;
            this.aboutLabel.Text = string.Format("Home Media Center {0}\r\nTomáš Pšenák © 2012\r\ntomaspsenak@gmail.com\r\n----------------------------------------------\r\n{1}\r\n{2}\r\n{3}", str0, str1, str2, str3);

            try { this.mediaCenter.LoadSettings(this.settingsPath, this.databasePath); }
            catch { MessageBox.Show(this, LanguageResource.ConfigCorrupted, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning); }

            this.dirListBox.DataSource = this.mediaCenter.ItemManager.GetDirectoriesSync();

            StartMediaCenterAsync();
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
                BuildDatabaseAsync();
            }
        }

        private void removeDirButton_Click(object sender, EventArgs e)
        {
            string dir = this.dirListBox.SelectedItem as string;
            if (dir == null)
                MessageBox.Show(this, LanguageResource.SelectDirectory);

            this.dirListBox.DataSource = this.mediaCenter.ItemManager.RemoveDirectorySync(dir);
            BuildDatabaseAsync();
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
            BuildDatabaseAsync();
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

            this.startMediaCenterDel.BeginInvoke(new AsyncCallback(StartMediaCenterResult), null);
        }

        private void StartMediaCenterResult(IAsyncResult result)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new AsyncCallback(StartMediaCenterResult), result);
                return;
            }

            try { this.startMediaCenterDel.EndInvoke(result); }
            catch (Exception ex)
            {
                SetButtonEnabled(true);

                this.statusTextLabel.Text = LanguageResource.Stopped;
                this.statusTextLabel.ForeColor = Color.Red;

                MessageBox.Show(this, ex.Message, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            this.mediaCenterStarted = true;
            this.startButton.Text = LanguageResource.Stop;

            BuildDatabaseAsync();            
        }

        private void BuildDatabaseAsync()
        {
            SetButtonEnabled(false);

            this.statusTextLabel.Text = LanguageResource.RefreshingDatabase;
            this.statusTextLabel.ForeColor = Color.Orange;

            this.buildDatabaseDel.BeginInvoke(new AsyncCallback(BuildDatabaseResult), null);
        }

        private void BuildDatabaseResult(IAsyncResult result)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new AsyncCallback(BuildDatabaseResult), result);
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

            try { this.buildDatabaseDel.EndInvoke(result); }
            catch (Exception ex)
            {
                MessageBox.Show(this, ex.Message, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void StopMediaCenterAsync(AfterStopped afterStopped)
        {
            this.afterStopped = afterStopped;
            SetButtonEnabled(false);

            this.stopMediaCenterDel.BeginInvoke(new AsyncCallback(StopMediaCenterResult), null);
        }

        private void StopMediaCenterResult(IAsyncResult result)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new AsyncCallback(StopMediaCenterResult), result);
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
        }

        private void mainTabControl_Selected(object sender, TabControlEventArgs e)
        {
            if (this.mainTabControl.SelectedTab == this.settingsTabPage)
            {
                this.friendlyNameText.Text = this.mediaCenter.FriendlyName;
                this.portText.Text = this.mediaCenter.Server.HttpPort.ToString();

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
                this.videoStreamParamCombo.Items.AddRange(settings.VideoStreamEncode.Select(a => a.GetParamString()).ToArray());
                this.videoStreamParamCombo.SelectedItem = this.videoStreamParamCombo.Items.Cast<string>().FirstOrDefault();
            }
            else if (this.mainTabControl.SelectedTab == this.converterTabPage)
            {
                this.inputTextBox.SelectedIndex = 0;
                this.containerComboBox.SelectedIndex = 0;
                this.resolutionComboBox.SelectedIndex = 0;
                this.bitrateVideoComboBox.SelectedIndex = 3;
                this.bitrateAudioComboBox.SelectedIndex = 3;
                this.fpsComboBox.SelectedIndex = 0;
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

            MediaSettings settings = this.mediaCenter.ItemManager.MediaSettings;

            settings.AudioNativeFile = this.audioNativeCheck.Checked;
            settings.AudioEncode = this.audioParamCombo.Items.Cast<string>().Select(a => EncoderBuilder.GetEncoder(a)).ToList();

            settings.ImageNativeFile = this.imageNativeCheck.Checked;
            settings.ImageEncode = this.imageParamCombo.Items.Cast<string>().Select(a => EncoderBuilder.GetEncoder(a)).ToList();

            settings.VideoNativeFile = this.videoNativeCheck.Checked;
            settings.VideoEncode = this.videoParamCombo.Items.Cast<string>().Select(a => EncoderBuilder.GetEncoder(a)).ToList();
            settings.VideoStreamEncode = this.videoStreamParamCombo.Items.Cast<string>().Select(a => EncoderBuilder.GetEncoder(a)).ToList();
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
            ParametersForm form = new ParametersForm(this.audioParamCombo) { 
                Text = LanguageResource.ParameterEditor + " - " + LanguageResource.Audio };

            if (form.ShowDialog(this) == System.Windows.Forms.DialogResult.OK)
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
            ParametersForm form = new ParametersForm(this.imageParamCombo) {
                Text = LanguageResource.ParameterEditor + " - " + LanguageResource.Image };

            if (form.ShowDialog(this) == System.Windows.Forms.DialogResult.OK)
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
            ParametersForm form = new ParametersForm(this.videoParamCombo) {
                Text = LanguageResource.ParameterEditor + " - " + LanguageResource.Video };

            if (form.ShowDialog(this) == System.Windows.Forms.DialogResult.OK)
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
            ParametersForm form = new ParametersForm(this.videoStreamParamCombo) {
                Text = LanguageResource.ParameterEditor + " - " + LanguageResource.Video };

            if (form.ShowDialog(this) == System.Windows.Forms.DialogResult.OK)
            {
                try { this.videoStreamParamCombo.Items.Add(EncoderBuilder.GetEncoder(form.QueryString).GetParamString()); }
                catch (Exception ex)
                {
                    MessageBox.Show(this, ex.Message, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
        }

        private void browseOutputButton_Click(object sender, EventArgs e)
        {
            SaveFileDialog sd = new SaveFileDialog();
            sd.DefaultExt = ((ContainerItem)this.containerComboBox.SelectedItem).Extension;
            sd.Filter = sd.DefaultExt.TrimStart('.') + "|" + sd.DefaultExt;
            if (sd.ShowDialog(this) == System.Windows.Forms.DialogResult.OK)
            {
                this.outputTextBox.Items[0] = sd.FileName;
                this.outputTextBox.SelectedIndex = 0;
            }
        }

        private void browseInputButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog od = new OpenFileDialog();
            od.Multiselect = false;
            if (od.ShowDialog(this) == System.Windows.Forms.DialogResult.OK)
            {
                this.inputTextBox.Items[0] = od.FileName;
                this.inputTextBox.SelectedIndex = 0;
            }
        }

        private void containerComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            ContainerItem item = (ContainerItem)this.containerComboBox.SelectedItem;

            if (this.outputTextBox.SelectedIndex == 0 && ((string)this.outputTextBox.SelectedItem) != "..")
            {
                int index = ((string)this.outputTextBox.SelectedItem).LastIndexOf('.');
                if (index > 0)
                    this.outputTextBox.Items[0] = ((string)this.outputTextBox.SelectedItem).Substring(0, index) + item.Extension;
            }

            this.keepAspectCheckBox.Enabled = item.IsDirectShow;
            this.subtitlesIntCheckBox.Enabled = item.IsDirectShow;

            while (this.inputTextBox.Items.Count > 1)
                this.inputTextBox.Items.RemoveAt(1);
            while (this.outputTextBox.Items.Count > 1)
                this.outputTextBox.Items.RemoveAt(1);

            this.inputTextBox.SelectedIndex = 0;
            this.outputTextBox.SelectedIndex = 0;

            if (item.IsDirectShow)
            {
                this.inputTextBox.Items.Add("Desktop");
                this.inputTextBox.Items.AddRange(DSWrapper.WebcamInput.GetVideoInputNames().Select(a => "Webcam_" + a).ToArray());
            }

            if (item.IsTransportStream)
            {
                foreach (IPAddress address in Dns.GetHostAddresses(Dns.GetHostName()).Where(a => a.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork))
                    this.outputTextBox.Items.Add(string.Format("Multicast {0} - 239.255.255.251:12346", address));
            }
        }

        private void convertButton_Click(object sender, EventArgs e)
        {
            Dictionary<string, string> parameters = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);

            try
            {
                parameters["codec"] = ((ContainerItem)this.containerComboBox.SelectedItem).ParamName;
                parameters["video"] = this.videoCheckBox.Checked ? "1" : "0";
                parameters["audio"] = this.audioCheckBox.Checked ? "1" : "0";
                parameters["source"] = (string)this.inputTextBox.SelectedItem;
                parameters["vidbitrate"] = ((string)this.bitrateVideoComboBox.SelectedItem).Split(' ').First();
                parameters["audbitrate"] = ((string)this.bitrateAudioComboBox.SelectedItem).Split(' ').First();
                parameters["starttime"] = TimeSpan.Parse(this.startTimeTextBox.Text).TotalSeconds.ToString("F0");
                parameters["endtime"] = TimeSpan.Parse(this.endTimeTextBox.Text).TotalSeconds.ToString("F0");

                if (this.resolutionComboBox.SelectedIndex > 0)
                {
                    string[] res = ((string)this.resolutionComboBox.SelectedItem).Split('x');
                    parameters["width"] = res[0];
                    parameters["height"] = res[1];
                }

                if (this.keepAspectCheckBox.Checked)
                    parameters["keepaspect"] = string.Empty;

                if (this.fpsComboBox.SelectedIndex > 0)
                    parameters["fps"] = (string)this.fpsComboBox.SelectedItem;

                if (this.subtitlesIntCheckBox.Checked)
                    parameters["subtitles"] = string.Empty;

                EncoderBuilder builder = EncoderBuilder.GetEncoder(parameters);
                if (builder == null)
                    throw new Exception("Unknown codec");

                ConvertForm cf = new ConvertForm(builder, this.outputTextBox.Text);
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
    }
}
