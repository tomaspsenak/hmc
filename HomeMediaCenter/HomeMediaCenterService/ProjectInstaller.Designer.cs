namespace HomeMediaCenterService
{
    partial class ProjectInstaller
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

        #region Component Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.mainServiceProcessInstaller = new System.ServiceProcess.ServiceProcessInstaller();
            this.mediaServerServiceInstaller = new System.ServiceProcess.ServiceInstaller();
            // 
            // mainServiceProcessInstaller
            // 
            this.mainServiceProcessInstaller.Account = System.ServiceProcess.ServiceAccount.NetworkService;
            this.mainServiceProcessInstaller.Password = null;
            this.mainServiceProcessInstaller.Username = null;
            // 
            // mediaServerServiceInstaller
            // 
            this.mediaServerServiceInstaller.Description = "Home Media Center is a server application for UPnP / DLNA compatible devices. It " +
    "supports streaming and transcoding media files, Windows desktop and video from w" +
    "ebcams.";
            this.mediaServerServiceInstaller.DisplayName = "HomeMediaCenter";
            this.mediaServerServiceInstaller.ServiceName = "HomeMediaCenter";
            this.mediaServerServiceInstaller.StartType = System.ServiceProcess.ServiceStartMode.Automatic;
            // 
            // ProjectInstaller
            // 
            this.Installers.AddRange(new System.Configuration.Install.Installer[] {
            this.mainServiceProcessInstaller,
            this.mediaServerServiceInstaller});

        }

        #endregion

        private System.ServiceProcess.ServiceProcessInstaller mainServiceProcessInstaller;
        private System.ServiceProcess.ServiceInstaller mediaServerServiceInstaller;
    }
}