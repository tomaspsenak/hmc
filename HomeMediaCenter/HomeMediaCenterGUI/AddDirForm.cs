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
    public partial class AddDirForm : Form
    {
        public AddDirForm()
        {
            InitializeComponent();

            this.addButton.Text = LanguageResource.Add;
            this.cancelButton.Text = LanguageResource.Cancel;
            this.titleLabel.Text = LanguageResource.Title + ":";
            this.pathLabel.Text = LanguageResource.Path + ":";
            this.browseButton.Text = LanguageResource.Browse;
            this.Text = LanguageResource.AddDirectory;
        }

        public string PathText
        {
            get { return this.pathTextBox.Text; }
        }

        public string TitleText
        {
            get { return this.titleTextBox.Text; }
        }

        private void browserButton_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog dialog = new FolderBrowserDialog();
            if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                this.pathTextBox.Text = dialog.SelectedPath;
                this.titleTextBox.Text = dialog.SelectedPath.Split(new char[] { '\\' }, StringSplitOptions.RemoveEmptyEntries).Last();
            }
        }

        private void addButton_Click(object sender, EventArgs e)
        {
            if (this.pathTextBox.Text == null || this.pathTextBox.Text == string.Empty ||
                this.titleTextBox.Text == null || this.titleTextBox.Text == string.Empty)
                return;

            this.DialogResult = System.Windows.Forms.DialogResult.OK;
            Close();
        }
    }
}
