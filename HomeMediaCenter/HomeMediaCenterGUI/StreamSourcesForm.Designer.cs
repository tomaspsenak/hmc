namespace HomeMediaCenterGUI
{
    partial class StreamSourcesForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(StreamSourcesForm));
            this.streamsDataGrid = new System.Windows.Forms.DataGridView();
            this.titleColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.urlColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.addButton = new System.Windows.Forms.Button();
            this.removeButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.applyButton = new System.Windows.Forms.Button();
            this.mainToolTip = new System.Windows.Forms.ToolTip(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.streamsDataGrid)).BeginInit();
            this.SuspendLayout();
            // 
            // streamsDataGrid
            // 
            this.streamsDataGrid.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.streamsDataGrid.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.streamsDataGrid.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.titleColumn,
            this.urlColumn});
            this.streamsDataGrid.Location = new System.Drawing.Point(12, 12);
            this.streamsDataGrid.Name = "streamsDataGrid";
            this.streamsDataGrid.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.streamsDataGrid.Size = new System.Drawing.Size(600, 399);
            this.streamsDataGrid.TabIndex = 0;
            // 
            // titleColumn
            // 
            this.titleColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.titleColumn.DataPropertyName = "Title";
            this.titleColumn.FillWeight = 101.5228F;
            this.titleColumn.HeaderText = "Title";
            this.titleColumn.Name = "titleColumn";
            // 
            // urlColumn
            // 
            this.urlColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.urlColumn.DataPropertyName = "Path";
            this.urlColumn.FillWeight = 98.47716F;
            this.urlColumn.HeaderText = "URL Address";
            this.urlColumn.Name = "urlColumn";
            // 
            // addButton
            // 
            this.addButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.addButton.Image = ((System.Drawing.Image)(resources.GetObject("addButton.Image")));
            this.addButton.Location = new System.Drawing.Point(12, 417);
            this.addButton.Name = "addButton";
            this.addButton.Size = new System.Drawing.Size(29, 24);
            this.addButton.TabIndex = 2;
            this.addButton.UseVisualStyleBackColor = true;
            this.addButton.Click += new System.EventHandler(this.addButton_Click);
            // 
            // removeButton
            // 
            this.removeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.removeButton.Image = ((System.Drawing.Image)(resources.GetObject("removeButton.Image")));
            this.removeButton.Location = new System.Drawing.Point(47, 417);
            this.removeButton.Name = "removeButton";
            this.removeButton.Size = new System.Drawing.Size(29, 24);
            this.removeButton.TabIndex = 3;
            this.removeButton.UseVisualStyleBackColor = true;
            this.removeButton.Click += new System.EventHandler(this.removeButton_Click);
            // 
            // cancelButton
            // 
            this.cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelButton.Location = new System.Drawing.Point(537, 417);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(75, 24);
            this.cancelButton.TabIndex = 4;
            this.cancelButton.Text = "Cancel";
            this.cancelButton.UseVisualStyleBackColor = true;
            // 
            // applyButton
            // 
            this.applyButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.applyButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.applyButton.Location = new System.Drawing.Point(456, 417);
            this.applyButton.Name = "applyButton";
            this.applyButton.Size = new System.Drawing.Size(75, 24);
            this.applyButton.TabIndex = 5;
            this.applyButton.Text = "Apply";
            this.applyButton.UseVisualStyleBackColor = true;
            // 
            // StreamSourcesForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(624, 452);
            this.Controls.Add(this.applyButton);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.removeButton);
            this.Controls.Add(this.addButton);
            this.Controls.Add(this.streamsDataGrid);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimumSize = new System.Drawing.Size(640, 490);
            this.Name = "StreamSourcesForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Stream sources";
            ((System.ComponentModel.ISupportInitialize)(this.streamsDataGrid)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DataGridView streamsDataGrid;
        private System.Windows.Forms.DataGridViewTextBoxColumn titleColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn urlColumn;
        private System.Windows.Forms.Button addButton;
        private System.Windows.Forms.Button removeButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.Button applyButton;
        private System.Windows.Forms.ToolTip mainToolTip;
    }
}