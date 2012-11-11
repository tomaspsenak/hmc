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
        public ParametersForm()
        {
            InitializeComponent();

            this.addButton.Text = LanguageResource.Apply;
            this.cancelButton.Text = LanguageResource.Cancel;
        }

        public DialogResult ShowDialog(IWin32Window owner, string paramString, bool audio, bool video, bool image, bool streamable)
        {
            this.paramControl.LoadSettings(paramString, audio, video, image, streamable, false);

            return ShowDialog(owner);
        }

        public string QueryString
        {
            get { return this.paramControl.ParamString; }
        }
    }
}
