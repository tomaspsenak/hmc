using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace HomeMediaCenterGUI
{
    public partial class ParametersForm : Form
    {
        public ParametersForm(ComboBox combo)
        {
            InitializeComponent();
            this.parametersLabel.Text = LanguageResource.Parameters;
            this.addButton.Text = LanguageResource.Add;
            this.cancelButton.Text = LanguageResource.Cancel;

            if (combo.SelectedIndex >= 0)
                this.parameterBox.Text = combo.SelectedItem as string;

            this.infoTextBox.AppendText(LanguageResource.Parameters + "\r\n");
            this.infoTextBox.AppendText("&codec= \r\n");
            this.infoTextBox.AppendText("\tmpeg2_ps\t(MPEG2 Program stream, audio layer 2)\r\n");
            this.infoTextBox.AppendText("\tmpeg2layer1_ps\t(MPEG2 Program stream, audio layer 1)\r\n");
            this.infoTextBox.AppendText("\tmpeg2_ts\t\t(MPEG2 Transport stream, audio layer 2)\r\n");
            this.infoTextBox.AppendText("\tmpeg2layer1_ts\t(MPEG2 Transport stream, audio layer 1)\r\n");
            this.infoTextBox.AppendText("\twebm_ts\t\t(WebM, VP8 video, Vorbis audio)\r\n");
            this.infoTextBox.AppendText("\twmv2\t\t(Windows Media Video 8, Windows Media Audio 8)\r\n");
            this.infoTextBox.AppendText("\twmv3a2\t\t(Windows Media Video 9, Windows Media Audio 8)\r\n");
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

            this.infoTextBox.AppendText("\r\n&keepaspect\t\t(keep aspect ratio)\r\n\r\n");

            this.infoTextBox.AppendText(LanguageResource.Examples + "\r\n");
            this.infoTextBox.AppendText("&codec=mpeg2_ps&vidbitrate=9000&audbitrate=128&width=1920&height=1080&fps=25&keepaspect\r\n");
            this.infoTextBox.AppendText("&codec=mpeg2_ps&vidbitrate=5000&audbitrate=128&width=1366&height=768\r\n");
            this.infoTextBox.AppendText("&codec=mpeg2_ps&vidbitrate=3000&audbitrate=128&width=720&height=576&fps=25\r\n");
            this.infoTextBox.AppendText("&codec=wmv2&width=800&height=480&vidbitrate=400&audbitrate=40&quality=10&fps=30\r\n");
            this.infoTextBox.AppendText("&codec=mpeg2_ps&audbitrate=128&video=0\r\n");
            this.infoTextBox.AppendText("&codec=jpeg&width=1280&height=720&keepaspect\r\n");
        }

        public string QueryString
        {
            get { return this.parameterBox.Text; }
        }
    }
}
