using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using HomeMediaCenter;
using System.IO;

namespace HomeMediaCenterGUI
{
    public partial class ConvertForm : Form
    {
        private Action<Stream, Action<double>> convertDel;
        private EncoderBuilder builder;

        public ConvertForm(EncoderBuilder builder, string outputPath)
        {
            this.builder = builder;
            this.convertDel = new Action<Stream, Action<double>>(builder.StartEncode);

            Stream outputStream;
            if (outputPath.StartsWith("Multicast "))
            {
                string[] param = outputPath.Substring(10).Split(':', ' ');
                outputStream = new UDPStream(System.Net.IPAddress.Parse(param[0]), System.Net.IPAddress.Parse(param[2]), int.Parse(param[3]));
            }
            else
            {
                outputStream = new FileStream(outputPath, FileMode.Create);
            }

            InitializeComponent();
            this.cancelButton.Text = LanguageResource.Cancel;
            Text = LanguageResource.Converting;

            this.convertDel.BeginInvoke(outputStream, ProgressChange, new AsyncCallback(ConvertCallback), 
                new object[] { outputStream, DateTime.Now });
        }

        private void ConvertCallback(IAsyncResult result)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new AsyncCallback(ConvertCallback), result);
                return;
            }

            Close();

            object[] parameters = (object[])result.AsyncState;
            try
            {
                ((Stream)parameters[0]).Dispose();
                convertDel.EndInvoke(result);

                string str = LanguageResource.ConversionCompleted + " " + LanguageResource.After + " " + (DateTime.Now - (DateTime)parameters[1]);
                MessageBox.Show(str, LanguageResource.ConversionCompleted, MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, LanguageResource.Error, MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void ProgressChange(double progress)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action<double>(ProgressChange), progress);
                return;
            }

            if (progress >= 0 && progress <= 1)
            {
                this.progressBar.Style = ProgressBarStyle.Blocks;
                this.progressBar.Value = (int)(progress * 100.0);
            }
            else
            {
                this.progressBar.Style = ProgressBarStyle.Marquee;
            }
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            this.builder.StopEncode();
        }

        private void ConvertForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.builder.StopEncode();
        }
    }
}
