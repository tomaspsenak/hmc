using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using DSWrapper;

namespace HomeMediaCenterPlayer
{
    public partial class Form1 : Form
    {
        private DSPlayer player;

        public Form1()
        {
            InitializeComponent();
        }

        private void playButton_Click(object sender, EventArgs e)
        {
            string[] param = this.addressBox.Text.Split(':');
            UDPStream str = new UDPStream(System.Net.IPAddress.Any, System.Net.IPAddress.Parse(param[0]), int.Parse(param[1]));

            this.player = DSPlayer.PlayMPEG2_TS(str, this.videoBox.Checked, this.audioBox.Checked);

            this.playButton.Enabled = false;
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (this.player != null)
                this.player.Dispose();
        }
    }
}
