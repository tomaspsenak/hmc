namespace HomeMediaCenterPlayer
{
    partial class Form1
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
            this.playButton = new System.Windows.Forms.Button();
            this.addressBox = new System.Windows.Forms.TextBox();
            this.videoBox = new System.Windows.Forms.CheckBox();
            this.audioBox = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // playButton
            // 
            this.playButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.playButton.Location = new System.Drawing.Point(167, 39);
            this.playButton.Name = "playButton";
            this.playButton.Size = new System.Drawing.Size(75, 23);
            this.playButton.TabIndex = 0;
            this.playButton.Text = "Play";
            this.playButton.UseVisualStyleBackColor = true;
            this.playButton.Click += new System.EventHandler(this.playButton_Click);
            // 
            // addressBox
            // 
            this.addressBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.addressBox.Location = new System.Drawing.Point(12, 13);
            this.addressBox.Name = "addressBox";
            this.addressBox.Size = new System.Drawing.Size(230, 20);
            this.addressBox.TabIndex = 1;
            this.addressBox.Text = "239.255.255.251:12346";
            // 
            // videoBox
            // 
            this.videoBox.AutoSize = true;
            this.videoBox.Checked = true;
            this.videoBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.videoBox.Location = new System.Drawing.Point(12, 43);
            this.videoBox.Name = "videoBox";
            this.videoBox.Size = new System.Drawing.Size(53, 17);
            this.videoBox.TabIndex = 2;
            this.videoBox.Text = "Video";
            this.videoBox.UseVisualStyleBackColor = true;
            // 
            // audioBox
            // 
            this.audioBox.AutoSize = true;
            this.audioBox.Location = new System.Drawing.Point(71, 43);
            this.audioBox.Name = "audioBox";
            this.audioBox.Size = new System.Drawing.Size(53, 17);
            this.audioBox.TabIndex = 3;
            this.audioBox.Text = "Audio";
            this.audioBox.UseVisualStyleBackColor = true;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(253, 83);
            this.Controls.Add(this.audioBox);
            this.Controls.Add(this.videoBox);
            this.Controls.Add(this.addressBox);
            this.Controls.Add(this.playButton);
            this.Name = "Form1";
            this.Text = "MPEG-2 Multicast";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button playButton;
        private System.Windows.Forms.TextBox addressBox;
        private System.Windows.Forms.CheckBox videoBox;
        private System.Windows.Forms.CheckBox audioBox;
    }
}

