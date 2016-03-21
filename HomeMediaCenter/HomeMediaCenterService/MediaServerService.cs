using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.ServiceProcess;
using System.Text;
using HomeMediaCenter;
using HomeMediaCenter.Interfaces;
using System.ServiceModel;
using System.IO;

namespace HomeMediaCenterService
{
    public partial class MediaServerService : ServiceBase
    {
        private readonly ServiceHost configHost;

        public static readonly DirectoryInfo DataDir = Directory.CreateDirectory(Path.Combine(System.Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), "Home Media Center"));
        public static readonly MediaServerDevice MediaCenter = new MediaServerDevice(DataDir.FullName);

        public MediaServerService()
        {
            InitializeComponent();

            this.configHost = new ServiceHost(typeof(ConfigServerService), new Uri("net.pipe://localhost/HMCConfig"));

            var binding = new NetNamedPipeBinding(NetNamedPipeSecurityMode.None) { ReceiveTimeout = TimeSpan.MaxValue };

            this.configHost.AddServiceEndpoint(typeof(IMediaServerDevice), binding, string.Empty);
            this.configHost.AddServiceEndpoint(typeof(IUpnpServer), binding, string.Empty);
            this.configHost.AddServiceEndpoint(typeof(IItemManager), binding, string.Empty);
            this.configHost.AddServiceEndpoint(typeof(IMediaSettings), binding, string.Empty);
        }

        protected override void OnStart(string[] args)
        {
            StartMe();
        }

        internal void StartMe()
        {
            this.configHost.Open();

            //Ak este neexistuju nastavenie je vyhlasena chyba
            try
            { 
                MediaCenter.LoadSettings();
            }
            catch { }

            //Aj ked sa nepodari spustit MediaCenter, stale je zapnuty ConfigHost
            try
            {
                MediaCenter.Start();
                MediaCenter.IItemManager.BuildDatabaseAsync();
            }
            catch { }
        }

        protected override void OnStop()
        {
            MediaCenter.Stop();

            this.configHost.Close();
        }
    }
}
