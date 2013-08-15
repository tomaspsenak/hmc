using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel.Design;
using System.Net;
using HomeMediaCenter;

namespace HomeMediaCenterGUI
{
    [Designer("System.Windows.Forms.Design.ParentControlDesigner, System.Design", typeof(IDesigner))]
    public partial class ParametersControl : UserControl
    {
        private Dictionary<string, string> parameters = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
        private bool extended;

        public ParametersControl()
        {
            InitializeComponent();

            InitLanguage();
            LoadInfoBox();
        }

        public string ParamString
        {
            get { return this.paramBox.Text; }
        }

        public string OutputText
        {
            get { return this.outputTextBox.Text; }
        }

        public EncoderBuilder GetEncoder()
        {
            if (this.editCheckBox.Checked)
                return EncoderBuilder.GetEncoder(this.paramBox.Text);
            else
                return EncoderBuilder.GetEncoder(this.parameters);
        }

        public void LoadSettings(string paramString, bool audio, bool video, bool image, bool streamable, bool extended)
        {
            this.containerComboBox.Items.Clear();
            this.containerComboBox.Items.AddRange(ContainerItem.GetContainers().Where(a => (streamable ? a.IsStreamable == streamable : true) && 
                ((audio ? a.IsAudio == true : false) || (video ? a.IsVideo == true : false) || (image ? a.IsImage == true : false))).ToArray());

            this.parameters = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
            if (paramString == null)
                paramString = string.Empty;

            this.paramBox.Text = paramString;

            foreach (string keyValue in paramString.Split(new char[] { '&' }, StringSplitOptions.RemoveEmptyEntries))
            {
                string[] keyValueArray = keyValue.Split(new char[] { '=' }, StringSplitOptions.RemoveEmptyEntries);
                this.parameters[keyValueArray[0]] = keyValueArray.Length > 1 ? keyValueArray[1] : string.Empty;
            }

            if (this.parameters.ContainsKey("codec"))
            {
                string val = this.parameters["codec"];
                this.containerComboBox.SelectedItem = this.containerComboBox.Items.Cast<ContainerItem>().Where(a => a.ParamName == val).FirstOrDefault();
            }

            if (this.parameters.ContainsKey("video") && this.parameters["video"] == "0")
                this.videoCheckBox.Checked = false;
            else
                this.videoCheckBox.Checked = true;

            if (this.parameters.ContainsKey("audio") && this.parameters["audio"] == "0")
                this.audioCheckBox.Checked = false;
            else
                this.audioCheckBox.Checked = true;

            if (this.parameters.ContainsKey("width") && this.parameters.ContainsKey("height"))
            {
                string val = this.parameters["width"] + "x" + this.parameters["height"];
                this.resolutionComboBox.SelectedItem = this.resolutionComboBox.Items.Cast<string>().Where(a => a == val).FirstOrDefault();
            }
            else
            {
                this.resolutionComboBox.SelectedIndex = 0;
            }

            if (this.parameters.ContainsKey("keepaspect"))
                this.keepAspectCheckBox.Checked = true;
            else
                this.keepAspectCheckBox.Checked = false;

            if (this.parameters.ContainsKey("fps"))
            {
                string val = this.parameters["fps"];
                this.fpsComboBox.SelectedItem = this.fpsComboBox.Items.Cast<string>().Where(a => a == val).FirstOrDefault();
            }
            else
            {
                this.fpsComboBox.SelectedIndex = 0;
            }

            if (this.parameters.ContainsKey("vidbitrate"))
            {
                string val = this.parameters["vidbitrate"] + " kbps";
                this.bitrateVideoComboBox.SelectedItem = this.bitrateVideoComboBox.Items.Cast<string>().Where(a => a == val).FirstOrDefault();
            }
            else
            {
                this.bitrateVideoComboBox.SelectedIndex = -1;
            }

            if (this.parameters.ContainsKey("scan"))
            {
                string val = this.parameters["scan"].ToLower();
                if (val == "i")
                    this.scanComboBox.SelectedIndex = 0;
                else if (val == "p")
                    this.scanComboBox.SelectedIndex = 1;
            }
            else
            {
                this.scanComboBox.SelectedIndex = -1;
            }

            if (this.parameters.ContainsKey("audbitrate"))
            {
                string val = this.parameters["audbitrate"] + " kbps";
                this.bitrateAudioComboBox.SelectedItem = this.bitrateAudioComboBox.Items.Cast<string>().Where(a => a == val).FirstOrDefault();
            }
            else
            {
                this.bitrateAudioComboBox.SelectedIndex = -1;
            }

            if (audio && !video)
            {
                this.audioCheckBox.Enabled = false;
                this.audioCheckBox.Checked = true;
                this.videoCheckBox.Enabled = false;
                this.videoCheckBox.Checked = false;
            }
            else if (image)
            {
                this.audioCheckBox.Enabled = false;
                this.audioCheckBox.Checked = false;
                this.videoCheckBox.Enabled = false;
                this.videoCheckBox.Checked = true;

                this.fpsComboBox.Enabled = false;
                this.scanComboBox.Enabled = false;
                this.bitrateVideoComboBox.Enabled = false;
            }

            this.extended = extended;
            this.othersGroupBox.Visible = extended && this.editCheckBox.Checked == false;
            this.inputGroupBox.Visible = extended && this.editCheckBox.Checked == false;
            this.outputGroupBox.Visible = extended && this.editCheckBox.Checked == false;

            this.startTimeTextBox.Text = "00:00:00";
            this.endTimeTextBox.Text = "00:00:00";
            this.subtitlesIntCheckBox.Checked = false;
            this.inputTextBox.Items[0] = "..";
            this.outputTextBox.Items[0] = "..";

            this.inputTextBox.SelectedIndex = 0;
            this.outputTextBox.SelectedIndex = 0;
            this.fpsComboBox.SelectedIndex = 5;
        }

        private void LoadInfoBox()
        {
            this.infoTextBox.AppendText(LanguageResource.Parameters + "\r\n");
            this.infoTextBox.AppendText("&codec= \r\n");
            this.infoTextBox.AppendText("\tmpeg2_ps\t(MPEG2 Program stream, audio layer 2)\r\n");
            this.infoTextBox.AppendText("\tmpeg2layer1_ps\t(MPEG2 Program stream, audio layer 1)\r\n");
            this.infoTextBox.AppendText("\tmpeg2_ts\t\t(MPEG2 Transport stream, audio layer 2)\r\n");
            this.infoTextBox.AppendText("\tmpeg2layer1_ts\t(MPEG2 Transport stream, audio layer 1)\r\n");
            this.infoTextBox.AppendText("\tmpeg2_ts_h264\t(MPEG2 Transport stream, H264 video, AAC audio layer)\r\n");
            this.infoTextBox.AppendText("\twebm_ts\t\t(WebM, VP8 video, Vorbis audio)\r\n");
            this.infoTextBox.AppendText("\twmv2\t\t(Windows Media Video 8, Windows Media Audio 8)\r\n");
            this.infoTextBox.AppendText("\twmv3\t\t(Windows Media Video 9, Windows Media Audio 10)\r\n");
            this.infoTextBox.AppendText("\tmp3_ts\t\t(MPEG audio layer 3)\r\n");
            this.infoTextBox.AppendText("\tflv_ts\t\t(Sorenson Spark, MPEG audio layer 3)\r\n");
            this.infoTextBox.AppendText("\tbmp\t\t(Bitmap image file)\r\n");
            this.infoTextBox.AppendText("\tjpeg\t\t(Joint Photographic Experts Group)\r\n");
            this.infoTextBox.AppendText("\tpng\t\t(Portable Network Graphics)\r\n");

            this.infoTextBox.AppendText("\r\n&video= \r\n");
            this.infoTextBox.AppendText("\t0\t\t(video disabled)\r\n");
            this.infoTextBox.AppendText("\t1, 2, ..\t\t(video stream index)\r\n");

            this.infoTextBox.AppendText("\r\n&audio= \r\n");
            this.infoTextBox.AppendText("\t0\t\t(audio disabled)\r\n");
            this.infoTextBox.AppendText("\t1, 2, ..\t\t(audio stream index)\r\n");

            this.infoTextBox.AppendText("\r\n&width= \r\n");
            this.infoTextBox.AppendText("\t320, 640, 720, 800, 1280, 1366, 1920\r\n");

            this.infoTextBox.AppendText("\r\n&height= \r\n");
            this.infoTextBox.AppendText("\t240, 480, 576, 720, 768, 1080\r\n");

            this.infoTextBox.AppendText("\r\n&vidbitrate= \r\n");
            this.infoTextBox.AppendText("\t1..90000\t\t(video bitrate)\r\n");

            this.infoTextBox.AppendText("\r\n&audbitrate= \r\n");
            this.infoTextBox.AppendText("\t1..90000\t\t(audio bitrate)\r\n");

            this.infoTextBox.AppendText("\r\n&quality= \r\n");
            this.infoTextBox.AppendText("\t1..100\t\t(encode quality)\r\n");

            this.infoTextBox.AppendText("\r\n&fps= \r\n");
            this.infoTextBox.AppendText("\t10..60\t\t(frames per second)\r\n");

            this.infoTextBox.AppendText("\r\n&keepaspect\t\t(keep aspect ratio)\r\n");

            this.infoTextBox.AppendText("\r\n&scan= \r\n");
            this.infoTextBox.AppendText("\ti\t\t(interlaced scan type)\r\n");
            this.infoTextBox.AppendText("\tp\t\t(progressive scan type)\r\n\r\n");

            this.infoTextBox.AppendText(LanguageResource.Examples + "\r\n");
            this.infoTextBox.AppendText("&codec=mpeg2_ps&vidbitrate=9000&audbitrate=128&width=1920&height=1080&fps=25&keepaspect\r\n");
            this.infoTextBox.AppendText("&codec=mpeg2_ps&vidbitrate=5000&audbitrate=128&width=1366&height=768\r\n");
            this.infoTextBox.AppendText("&codec=mpeg2_ps&vidbitrate=3000&audbitrate=128&width=720&height=576&fps=25\r\n");
            this.infoTextBox.AppendText("&codec=wmv2&width=800&height=480&vidbitrate=400&audbitrate=40&quality=10&fps=30\r\n");
            this.infoTextBox.AppendText("&codec=mpeg2_ps&audbitrate=128&video=0\r\n");
            this.infoTextBox.AppendText("&codec=jpeg&width=1280&height=720&keepaspect\r\n");
        }

        private void InitLanguage()
        {
            this.paramGroupBox.Text = LanguageResource.Parameters;
            this.containerGroupBox.Text = LanguageResource.ContainerCodec;
            this.editCheckBox.Text = LanguageResource.Edit;
            this.audioCheckBox.Text = LanguageResource.Audio;
            this.audioGroupBox.Text = LanguageResource.Audio;
            this.videoCheckBox.Text = LanguageResource.Video;
            this.videoGroupBox.Text = LanguageResource.Video;
            this.inputGroupBox.Text = LanguageResource.Input;
            this.outputGroupBox.Text = LanguageResource.Output;
            this.browseInputButton.Text = LanguageResource.Browse;
            this.browseOutputButton.Text = LanguageResource.Browse;
            this.othersGroupBox.Text = LanguageResource.Others;
            this.startEndTimeLabel.Text = LanguageResource.StartEndTime + ":";
            this.subtitlesIntCheckBox.Text = LanguageResource.IntegrateSubtitles;
            this.bitrateVideoLabel.Text = LanguageResource.BitRate + ":";
            this.bitrateAudioLabel.Text = LanguageResource.BitRate + ":";
            this.resolutionLabel.Text = LanguageResource.Resolution + ":";
            this.keepAspectCheckBox.Text = LanguageResource.KeepAspectRatio;
            this.fpsLabel.Text = LanguageResource.FPS + ":";
            this.resolutionComboBox.Items[0] = LanguageResource.SameAsSource;
            this.fpsComboBox.Items[0] = LanguageResource.SameAsSource;
            this.scanLabel.Text = LanguageResource.ScanType + ":";
            this.scanComboBox.Items[0] = LanguageResource.Interlaced;
            this.scanComboBox.Items[1] = LanguageResource.Progressive;
        }

        private void editCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            bool val = this.editCheckBox.Checked;
            this.paramBox.ReadOnly = !val;
            this.infoTextBox.Visible = val;
            this.containerGroupBox.Visible = !val;
            this.videoGroupBox.Visible = !val;
            this.audioGroupBox.Visible = !val;

            if (this.extended)
            {
                this.othersGroupBox.Visible = !val;
                this.inputGroupBox.Visible = !val;
                this.outputGroupBox.Visible = !val;
            }

            if (!this.editCheckBox.Checked)
            {
                this.paramBox.Text = GetParamString();
            }
        }

        private string GetParamString()
        {
            StringBuilder sb = new StringBuilder();
            foreach (KeyValuePair<string, string> kvp in this.parameters)
            {
                sb.AppendFormat("&{0}{1}", kvp.Key, kvp.Value == string.Empty ? string.Empty : "=" + kvp.Value);
            }

            return sb.ToString();
        }

        private void containerComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            ContainerItem item = this.containerComboBox.SelectedItem as ContainerItem;
            if (item == null && this.parameters.ContainsKey("codec"))
                this.parameters.Remove("codec");
            else
                this.parameters["codec"] = item.ParamName;

            string firstOutVal = (string)this.outputTextBox.Items[0];
            if (firstOutVal != "..")
            {
                int index = firstOutVal.LastIndexOf('.');
                if (index > 0)
                    this.outputTextBox.Items[0] = firstOutVal.Substring(0, index) + (item == null ? "." : item.Extension);
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
                this.inputTextBox.Items.AddRange(DirectShowEncoder.GetVideoInputNames());
            }

            if (item.IsTransportStream)
            {
                foreach (IPAddress address in Dns.GetHostAddresses(Dns.GetHostName()).Where(a => a.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork))
                    this.outputTextBox.Items.Add(string.Format("Multicast {0} - 239.255.255.251:12346", address));
            }

            this.paramBox.Text = GetParamString();
        }

        private void videoCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            this.videoGroupBox.Enabled = this.videoCheckBox.Checked;

            this.parameters["video"] = this.videoCheckBox.Checked ? "1" : "0";

            this.paramBox.Text = GetParamString();
        }

        private void audioCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            this.audioGroupBox.Enabled = this.audioCheckBox.Checked;

            this.parameters["audio"] = this.audioCheckBox.Checked ? "1" : "0";

            this.paramBox.Text = GetParamString();
        }

        private void resolutionComboBox_Changed(object sender, EventArgs e)
        {
            if (this.resolutionComboBox.SelectedIndex > 0 && this.resolutionComboBox.Enabled)
            {
                string[] res = ((string)this.resolutionComboBox.SelectedItem).Split('x');
                this.parameters["width"] = res[0];
                this.parameters["height"] = res[1];
            }
            else
            {
                if (this.parameters.ContainsKey("width"))
                    this.parameters.Remove("width");
                if (this.parameters.ContainsKey("height"))
                    this.parameters.Remove("height");
            }

            this.paramBox.Text = GetParamString();
        }

        private void keepAspectCheckBox_Changed(object sender, EventArgs e)
        {
            if (this.keepAspectCheckBox.Checked && this.keepAspectCheckBox.Enabled)
                this.parameters["keepaspect"] = string.Empty;
            else if (this.parameters.ContainsKey("keepaspect"))
                this.parameters.Remove("keepaspect");

            this.paramBox.Text = GetParamString();
        }

        private void fpsComboBox_Changed(object sender, EventArgs e)
        {
            if (this.fpsComboBox.SelectedIndex > 0 && this.fpsComboBox.Enabled)
                this.parameters["fps"] = (string)this.fpsComboBox.SelectedItem;
            else if (this.parameters.ContainsKey("fps"))
                this.parameters.Remove("fps");

            this.paramBox.Text = GetParamString();
        }

        private void bitrateVideoComboBox_Changed(object sender, EventArgs e)
        {
            string str = this.bitrateVideoComboBox.SelectedItem as string;

            if (str == null || !this.bitrateVideoComboBox.Enabled)
                this.parameters.Remove("vidbitrate");
            else
                this.parameters["vidbitrate"] = str.Split(' ').First();

            this.paramBox.Text = GetParamString();
        }

        private void scanComboBox_Changed(object sender, EventArgs e)
        {
            if (!this.scanComboBox.Enabled)
                this.parameters.Remove("scan");
            else if (this.scanComboBox.SelectedIndex == 0)
                this.parameters["scan"] = "i";
            else if (this.scanComboBox.SelectedIndex == 1)
                this.parameters["scan"] = "p";

            this.paramBox.Text = GetParamString();
        }

        private void bitrateAudioComboBox_Changed(object sender, EventArgs e)
        {
            string str = this.bitrateAudioComboBox.SelectedItem as string;

            if (str == null || !this.bitrateAudioComboBox.Enabled)
                this.parameters.Remove("audbitrate");
            else
                this.parameters["audbitrate"] = str.Split(' ').First();

            this.paramBox.Text = GetParamString();
        }

        private void startTimeTextBox_TextChanged(object sender, EventArgs e)
        {
            TimeSpan time;
            if (TimeSpan.TryParse(this.startTimeTextBox.Text, out time) && time.TotalSeconds > 0)
                this.parameters["starttime"] = time.TotalSeconds.ToString("F0");
            else if (this.parameters.ContainsKey("starttime"))
                this.parameters.Remove("starttime");

            this.paramBox.Text = GetParamString();
        }

        private void endTimeTextBox_TextChanged(object sender, EventArgs e)
        {
            TimeSpan time;
            if (TimeSpan.TryParse(this.endTimeTextBox.Text, out time) && time.TotalSeconds > 0)
                this.parameters["endtime"] = time.TotalSeconds.ToString("F0");
            else if (this.parameters.ContainsKey("endtime"))
                this.parameters.Remove("endtime");

            this.paramBox.Text = GetParamString();
        }

        private void subtitlesIntCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            if (this.subtitlesIntCheckBox.Checked)
                this.parameters["subtitles"] = string.Empty;
            else if (this.parameters.ContainsKey("subtitles"))
                this.parameters.Remove("subtitles");

            this.paramBox.Text = GetParamString();
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

        private void browseOutputButton_Click(object sender, EventArgs e)
        {
            ContainerItem item = this.containerComboBox.SelectedItem as ContainerItem;

            SaveFileDialog sd = new SaveFileDialog();
            sd.DefaultExt = item == null ? "." : item.Extension;
            sd.Filter = sd.DefaultExt.TrimStart('.') + "|" + sd.DefaultExt;
            if (sd.ShowDialog(this) == System.Windows.Forms.DialogResult.OK)
            {
                this.outputTextBox.Items[0] = sd.FileName;
                this.outputTextBox.SelectedIndex = 0;
            }
        }

        private void inputTextBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!this.extended)
                return;

            string item = this.inputTextBox.SelectedItem as string;

            if (item != null)
                this.parameters["source"] = item;
            else if (this.parameters.ContainsKey("source"))
                this.parameters.Remove("source");

            this.paramBox.Text = GetParamString();
        }
    }
}
