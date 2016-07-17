using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using HomeMediaCenter;

namespace HomeMediaCenterGUI
{
    public partial class StreamSourcesForm : Form
    {
        private StreamSourcesBindingList source;

        public StreamSourcesForm(StreamSourcesBindingList source)
        {
            this.source = source;

            InitializeComponent();

            this.Text = LanguageResource.StreamSources;
            this.cancelButton.Text = LanguageResource.Cancel;
            this.applyButton.Text = LanguageResource.Apply;
            this.streamsDataGrid.Columns[0].HeaderText = LanguageResource.Title;
            this.streamsDataGrid.Columns[1].HeaderText = LanguageResource.URLAddress;
            this.streamsDataGrid.Columns[2].HeaderText = LanguageResource.Type;
            this.mainToolTip.SetToolTip(this.addButton, LanguageResource.Add);
            this.mainToolTip.SetToolTip(this.removeButton, LanguageResource.Remove);
            this.mainToolTip.SetToolTip(this.typeComboBox, LanguageResource.Type);

            this.typeComboBox.Items.Add(LanguageResource.Stream);
            this.typeComboBox.Items.Add(LanguageResource.Dreambox);
            this.typeComboBox.SelectedIndex = 0;

            this.streamsDataGrid.AutoGenerateColumns = false;
            this.streamsDataGrid.DataSource = source;
        }

        private void addButton_Click(object sender, EventArgs e)
        {
            this.source.AddNew();
        }

        private void removeButton_Click(object sender, EventArgs e)
        {
            foreach (DataGridViewRow row in this.streamsDataGrid.SelectedRows.OfType<DataGridViewRow>())
            {
                this.source.Remove((StreamSourcesItem)row.DataBoundItem);
            }
        }

        private void typeComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            this.source.ActualType = (this.typeComboBox.SelectedIndex == 0) ? StreamSourcesItemType.Stream : StreamSourcesItemType.Dreambox;
        }
    }
}
