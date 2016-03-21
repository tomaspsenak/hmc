using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;
using HomeMediaCenter.Interfaces;

namespace HomeMediaCenterService
{
    [ServiceBehavior(InstanceContextMode = InstanceContextMode.PerSession, IncludeExceptionDetailInFaults = true)]
    public class ConfigServerService : IMediaServerDevice, IUpnpServer, IItemManager, IMediaSettings
    {
        private IMediaServerDeviceCallback mediaServerCallback;
        private IItemManagerCallback itemManagerCallback;
        private bool logEventSubscribed = false;

        public ConfigServerService()
        {
            bool addClosedEvent = false;

            try
            {
                this.mediaServerCallback = OperationContext.Current.GetCallbackChannel<IMediaServerDeviceCallback>();
                addClosedEvent = true;
                MediaServerService.MediaCenter.AsyncStartEnd += new EventHandler<HomeMediaCenter.ExceptionEventArgs>(MediaCenter_AsyncStartEnd);
                MediaServerService.MediaCenter.AsyncStopEnd += new EventHandler<HomeMediaCenter.ExceptionEventArgs>(MediaCenter_AsyncStopEnd);
            }
            catch { this.mediaServerCallback = null; }

            try
            {
                this.itemManagerCallback = OperationContext.Current.GetCallbackChannel<IItemManagerCallback>();
                addClosedEvent = true;
                MediaServerService.MediaCenter.IItemManager.AsyncBuildDatabaseEnd += new EventHandler<HomeMediaCenter.ExceptionEventArgs>(IItemManager_AsyncBuildDatabaseEnd);
                MediaServerService.MediaCenter.IItemManager.AsyncBuildDatabaseStart += new EventHandler<HomeMediaCenter.ExceptionEventArgs>(IItemManager_AsyncBuildDatabaseStart);

            }
            catch { this.itemManagerCallback = null; }

            if (addClosedEvent)
                OperationContext.Current.InstanceContext.Closed += new EventHandler(InstanceContext_Closed);
        }

        private void InstanceContext_Closed(object sender, EventArgs e)
        {
            if (this.mediaServerCallback != null)
            {
                MediaServerService.MediaCenter.AsyncStartEnd -= new EventHandler<HomeMediaCenter.ExceptionEventArgs>(MediaCenter_AsyncStartEnd);
                MediaServerService.MediaCenter.AsyncStopEnd -= new EventHandler<HomeMediaCenter.ExceptionEventArgs>(MediaCenter_AsyncStopEnd);
                if (this.logEventSubscribed)
                    MediaServerService.MediaCenter.LogEvent -= new EventHandler<HomeMediaCenter.LogEventArgs>(MediaCenter_LogEvent);
            }

            if (this.itemManagerCallback != null)
            {
                MediaServerService.MediaCenter.IItemManager.AsyncBuildDatabaseEnd -= new EventHandler<HomeMediaCenter.ExceptionEventArgs>(IItemManager_AsyncBuildDatabaseEnd);
                MediaServerService.MediaCenter.IItemManager.AsyncBuildDatabaseStart -= new EventHandler<HomeMediaCenter.ExceptionEventArgs>(IItemManager_AsyncBuildDatabaseStart);
            }
        }

        private void MediaCenter_AsyncStopEnd(object sender, HomeMediaCenter.ExceptionEventArgs e)
        {
            try { this.mediaServerCallback.AsyncStopEnd(e.Message); }
            catch { }
        }

        private void MediaCenter_AsyncStartEnd(object sender, HomeMediaCenter.ExceptionEventArgs e)
        {
            try { this.mediaServerCallback.AsyncStartEnd(e.Message); }
            catch { }
        }

        private void MediaCenter_LogEvent(object sender, HomeMediaCenter.LogEventArgs e)
        {
            try
            {
                if (e.Type == HomeMediaCenter.LogEventType.Message)
                    this.mediaServerCallback.LogEventMessage(e.Message);
                else
                    this.mediaServerCallback.LogEventRequestCount(e.RequestCount);
            }
            catch { }
        }

        private void IItemManager_AsyncBuildDatabaseStart(object sender, HomeMediaCenter.ExceptionEventArgs e)
        {
            try { this.itemManagerCallback.AsyncBuildDatabaseStart(e.Message); }
            catch { }
        }

        private void IItemManager_AsyncBuildDatabaseEnd(object sender, HomeMediaCenter.ExceptionEventArgs e)
        {
            try { this.itemManagerCallback.AsyncBuildDatabaseEnd(e.Message); }
            catch { }
        }

        #region IMediaServerDevice interface implementation

        void IMediaServerDevice.StartAsync()
        {
            MediaServerService.MediaCenter.StartAsync();
        }

        void IMediaServerDevice.StopAsync()
        {
            MediaServerService.MediaCenter.StopAsync();
        }

        void IMediaServerDevice.LoadSettings()
        {
            // Vola service pri starte
        }

        bool IMediaServerDevice.SaveSettings()
        {
            return MediaServerService.MediaCenter.SaveSettings();
        }

        void IMediaServerDevice.SetCurrentThreadCulture()
        {
            // Nema zmysel cez WCF, klient je na inom vlakne
        }

        string IMediaServerDevice.CultureInfoName
        {
            get { return MediaServerService.MediaCenter.CultureInfoName; }
            set { MediaServerService.MediaCenter.CultureInfoName = value; }
        }

        string IMediaServerDevice.FriendlyName
        {
            get { return MediaServerService.MediaCenter.FriendlyName; }
            set { MediaServerService.MediaCenter.FriendlyName = value; }
        }

        string IMediaServerDevice.DataDirectory
        {
            get { return MediaServerService.MediaCenter.DataDirectory; }
        }

        bool IMediaServerDevice.TryToForwardPort
        {
            get { return MediaServerService.MediaCenter.TryToForwardPort; }
            set { MediaServerService.MediaCenter.TryToForwardPort = value; }
        }

        bool IMediaServerDevice.GenerateThumbnails
        {
            get { return MediaServerService.MediaCenter.GenerateThumbnails; }
            set { MediaServerService.MediaCenter.GenerateThumbnails = value; }
        }

        bool IMediaServerDevice.MinimizeToTray
        {
            get { return MediaServerService.MediaCenter.MinimizeToTray; }
            set { MediaServerService.MediaCenter.MinimizeToTray = value; }
        }

        bool IMediaServerDevice.Started
        {
            get { return MediaServerService.MediaCenter.Started; }
        }

        bool IMediaServerDevice.IsWinService
        {
            get { return true; }
        }

        IItemManager IMediaServerDevice.IItemManager
        {
            get { throw new NotImplementedException(); }
        }

        IUpnpServer IMediaServerDevice.IServer
        {
            get { throw new NotImplementedException(); }
        }

        void IMediaServerDevice.LogEventSubscribe(bool subscribe)
        {
            if (this.mediaServerCallback == null)
                return;

            if (subscribe && !this.logEventSubscribed)
            {
                MediaServerService.MediaCenter.LogEvent += new EventHandler<HomeMediaCenter.LogEventArgs>(MediaCenter_LogEvent);
                this.logEventSubscribed = true;
            }
            else if (!subscribe && this.logEventSubscribed)
            {
                MediaServerService.MediaCenter.LogEvent -= new EventHandler<HomeMediaCenter.LogEventArgs>(MediaCenter_LogEvent);
                this.logEventSubscribed = false;
            }
        }

        #pragma warning disable 0067
        public event EventHandler<HomeMediaCenter.LogEventArgs> LogEvent;

        public event EventHandler<HomeMediaCenter.ExceptionEventArgs> AsyncStartEnd;

        public event EventHandler<HomeMediaCenter.ExceptionEventArgs> AsyncStopEnd;
        #pragma warning restore 0067

        #endregion
        #region IUpnpServer interface implementation

        int IUpnpServer.HttpPort
        {
            get { return MediaServerService.MediaCenter.IServer.HttpPort; }
            set { MediaServerService.MediaCenter.IServer.HttpPort = value; }
        }

        #endregion
        #region IItemManager interface implementation

        bool IItemManager.CheckDirectoryPermission(string path)
        {
            return MediaServerService.MediaCenter.IItemManager.CheckDirectoryPermission(path);
        }

        string[] IItemManager.GetDirectories()
        {
            return MediaServerService.MediaCenter.IItemManager.GetDirectories();
        }

        void IItemManager.BuildDatabaseAsync()
        {
            MediaServerService.MediaCenter.IItemManager.BuildDatabaseAsync();
        }

        void IItemManager.BuildDatabase()
        {
            MediaServerService.MediaCenter.IItemManager.BuildDatabase();
        }

        List<HomeMediaCenter.StreamSourcesItem> IItemManager.GetStreamSources()
        {
            return MediaServerService.MediaCenter.IItemManager.GetStreamSources();
        }

        void IItemManager.SetStreamSources(HomeMediaCenter.StreamSourcesItem[] toAdd, HomeMediaCenter.StreamSourcesItem[] toUpdate, HomeMediaCenter.StreamSourcesItem[] toDelete)
        {
            MediaServerService.MediaCenter.IItemManager.SetStreamSources(toAdd, toUpdate, toDelete);
        }

        string[] IItemManager.AddDirectory(string directory, string title)
        {
            return MediaServerService.MediaCenter.IItemManager.AddDirectory(directory, title);
        }

        string[] IItemManager.RemoveDirectory(string directory)
        {
            return MediaServerService.MediaCenter.IItemManager.RemoveDirectory(directory);
        }

        public IMediaSettings IMediaSettings
        {
            get { throw new NotImplementedException(); }
        }

        bool IItemManager.EnableWebcamStreaming
        {
            get { return MediaServerService.MediaCenter.IItemManager.EnableWebcamStreaming; }
            set { MediaServerService.MediaCenter.IItemManager.EnableWebcamStreaming = value; }
        }

        bool IItemManager.EnableDesktopStreaming
        {
            get { return MediaServerService.MediaCenter.IItemManager.EnableDesktopStreaming; }
            set { MediaServerService.MediaCenter.IItemManager.EnableDesktopStreaming = value; }
        }

        bool IItemManager.RealTimeDatabaseRefresh
        {
            get { return MediaServerService.MediaCenter.IItemManager.RealTimeDatabaseRefresh; }
            set { MediaServerService.MediaCenter.IItemManager.RealTimeDatabaseRefresh = value; }
        }

        bool IItemManager.ShowHiddenFiles
        {
            get { return MediaServerService.MediaCenter.IItemManager.ShowHiddenFiles; }
            set { MediaServerService.MediaCenter.IItemManager.ShowHiddenFiles = value; }
        }

        #pragma warning disable 0067
        public event EventHandler<HomeMediaCenter.ExceptionEventArgs> AsyncBuildDatabaseStart;

        public event EventHandler<HomeMediaCenter.ExceptionEventArgs> AsyncBuildDatabaseEnd;
        #pragma warning restore 0067

        #endregion
        #region  IMediaSettings interface implementation

        string[] IMediaSettings.GetEncodeStrings(EncodeType type)
        {
            return MediaServerService.MediaCenter.IItemManager.IMediaSettings.GetEncodeStrings(type);
        }

        void IMediaSettings.SetEncodeStrings(EncodeType type, string[] encode)
        {
            MediaServerService.MediaCenter.IItemManager.IMediaSettings.SetEncodeStrings(type, encode);
        }

        bool IMediaSettings.GetNativeFile(EncodeType type)
        {
            return MediaServerService.MediaCenter.IItemManager.IMediaSettings.GetNativeFile(type);
        }

        void IMediaSettings.SetNativeFile(EncodeType type, bool value)
        {
            MediaServerService.MediaCenter.IItemManager.IMediaSettings.SetNativeFile(type, value);
        }

        #endregion
    }

}
