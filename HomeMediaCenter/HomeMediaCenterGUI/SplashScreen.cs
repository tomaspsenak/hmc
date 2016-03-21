using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace HomeMediaCenterGUI
{
    public partial class SplashScreen : Form
    {
        private readonly Thread screenThread;
        private readonly AutoResetEvent shownWaitHandle;

        private SplashScreen()
        {
            this.shownWaitHandle = new AutoResetEvent(false);
            this.screenThread = new Thread(new ThreadStart(ShowMe)) { IsBackground = true };
            this.screenThread.SetApartmentState(ApartmentState.STA);

            InitializeComponent();

            this.pictureBoxLogo.Image = Bitmap.FromHicon(new Icon(this.Icon, new Size(48, 48)).Handle);
        }

        public static SplashScreen ShowSplashScreen()
        {
            SplashScreen ss = new SplashScreen();

            if (!Program.HasBackgroundArg())
            {
                ss.screenThread.Start();
                ss.shownWaitHandle.WaitOne();
            }

            return ss;
        }

        public new void Close()
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action(Close));
                return;
            }

            base.Close();
        }

        public void Close(Form children)
        {
            if (InvokeRequired)
            {
                children.Activate();

                BeginInvoke(new Action<Form>(Close), children);
                return;
            }

            base.Close();
        }

        public void SetStatusLabel(string text)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action<string>(SetStatusLabel), text);
                return;
            }

            this.statusLabel.Text = text;
        }

        private void ShowMe()
        {
            Application.Run(this);
        }

        protected override void OnShown(EventArgs e)
        {
            this.shownWaitHandle.Set();

            base.OnShown(e);
        }
    }
}
