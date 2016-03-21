using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;
using HomeMediaCenter.Interfaces;

namespace HomeMediaCenterGUI
{
    public class ConfigClientService : MediaServerDeviceProxy
    {
        public ConfigClientService() : base(new MediaServerDeviceCallback(), 
            new NetNamedPipeBinding(NetNamedPipeSecurityMode.None), new EndpointAddress("net.pipe://localhost/HMCConfig"))  { }
    }

    [CallbackBehavior(UseSynchronizationContext = false)]
    public class MediaServerDeviceCallback : IMediaServerDeviceCallback
    {
        public MediaServerDeviceProxy Proxy
        {
            get; set;
        }

        public void LogEventMessage(string message)
        {
            Proxy.OnLogEvent(message);
        }

        public void LogEventRequestCount(int requestCount)
        {
            Proxy.OnLogEvent(requestCount);
        }

        public void AsyncStartEnd(string exception)
        {
            Proxy.OnAsyncStartEnd(exception);
        }

        public void AsyncStopEnd(string exception)
        {
            Proxy.OnAsyncStopEnd(exception);
        }
    }

    [CallbackBehavior(UseSynchronizationContext = false)]
    public class ItemManagerCallback : IItemManagerCallback
    {
        public ItemManagerProxy Proxy
        {
            get; set;
        }

        public void AsyncBuildDatabaseStart(string exception)
        {
            Proxy.OnAsyncBuildDatabaseStart(exception);
        }

        public void AsyncBuildDatabaseEnd(string exception)
        {
            Proxy.OnAsyncBuildDatabaseEnd(exception);
        }
    }

    public abstract class MediaServerDeviceProxy : DuplexClientBase<IMediaServerDevice>, IMediaServerDevice
    {
        private readonly UpnpServerProxy upnpServer;
        private readonly ItemManagerProxy itemManager;

        public MediaServerDeviceProxy(MediaServerDeviceCallback callback, NetNamedPipeBinding binding, EndpointAddress address)
            : base(callback, binding, address)
        {
            callback.Proxy = this;

            this.upnpServer = new UpnpServerProxy(binding, address);
            this.itemManager = new ItemManagerProxy(new ItemManagerCallback(), binding, address);
        }

        public void StartAsync()
        {
            Channel.StartAsync();
        }

        public void StopAsync()
        {
            Channel.StopAsync();
        }

        public void LoadSettings()
        {
            Channel.LoadSettings();
        }

        public bool SaveSettings()
        {
            return Channel.SaveSettings();
        }

        public void SetCurrentThreadCulture()
        {
            string name = Channel.CultureInfoName;
            if (name != null)
            {
                System.Globalization.CultureInfo ci = new System.Globalization.CultureInfo(name);

                System.Threading.Thread.CurrentThread.CurrentCulture = ci;
                System.Threading.Thread.CurrentThread.CurrentUICulture = ci;
            }
        }

        public string CultureInfoName
        {
            get { return Channel.CultureInfoName; }
            set { Channel.CultureInfoName = value; }
        }

        public string FriendlyName
        {
            get { return Channel.FriendlyName; }
            set { Channel.FriendlyName = value; }
        }

        public string DataDirectory
        {
            get { return Channel.DataDirectory; }
        }

        public bool TryToForwardPort
        {
            get { return Channel.TryToForwardPort; }
            set { Channel.TryToForwardPort = value; }
        }

        public bool GenerateThumbnails
        {
            get { return Channel.GenerateThumbnails; }
            set { Channel.GenerateThumbnails = value; }
        }

        public bool MinimizeToTray
        {
            get { return Channel.MinimizeToTray; }
            set { Channel.MinimizeToTray = value; }
        }

        public bool Started
        {
            get { return Channel.Started; }
        }

        public bool IsWinService
        {
            get { return true; }
        }

        public IItemManager IItemManager
        {
            get { return this.itemManager; }
        }

        public IUpnpServer IServer
        {
            get { return this.upnpServer; }
        }

        public void OnLogEvent(string message)
        {
            EventHandler<HomeMediaCenter.LogEventArgs> handler = this.logEventInner;
            if (handler != null)
            {
                handler(this, new HomeMediaCenter.LogEventArgs(message));
            }
        }

        public void OnLogEvent(int requestCount)
        {
            EventHandler<HomeMediaCenter.LogEventArgs> handler = this.logEventInner;
            if (handler != null)
            {
                handler(this, new HomeMediaCenter.LogEventArgs(requestCount));
            }
        }

        public void OnAsyncStartEnd(string exception)
        {
            OnExceptionEvent(exception, AsyncStartEnd);
        }

        public void OnAsyncStopEnd(string exception)
        {
            OnExceptionEvent(exception, AsyncStopEnd);
        }

        public void LogEventSubscribe(bool subscribe)
        {
            throw new NotImplementedException();
        }

        protected void OnExceptionEvent(string exception, EventHandler<HomeMediaCenter.ExceptionEventArgs> handler)
        {
            if (handler != null)
            {
                handler(this, new HomeMediaCenter.ExceptionEventArgs(exception == null ? null : new Exception(exception)));
            }
        }

        private event EventHandler<HomeMediaCenter.LogEventArgs> logEventInner;
        public event EventHandler<HomeMediaCenter.LogEventArgs> LogEvent
        {
            add
            {
                if (this.logEventInner == null)
                    Channel.LogEventSubscribe(true);
                this.logEventInner += value;
            }

            remove
            {
                this.logEventInner -= value;
                if (this.logEventInner == null)
                    Channel.LogEventSubscribe(false);
            }
        }

        public event EventHandler<HomeMediaCenter.ExceptionEventArgs> AsyncStartEnd;

        public event EventHandler<HomeMediaCenter.ExceptionEventArgs> AsyncStopEnd;
    }

    public class UpnpServerProxy : ClientBase<IUpnpServer>, IUpnpServer
    {
        public UpnpServerProxy(NetNamedPipeBinding binding, EndpointAddress address) : base(binding, address) { }

        public int HttpPort
        {
            get { return Channel.HttpPort; }
            set { Channel.HttpPort = value; }
        }
    }

    public class ItemManagerProxy : DuplexClientBase<IItemManager>, IItemManager
    {
        private readonly MediaSettingsProxy mediaSettings;

        public ItemManagerProxy(ItemManagerCallback callback, NetNamedPipeBinding binding, EndpointAddress address)
            : base(callback, binding, address)
        {
            callback.Proxy = this;

            this.mediaSettings = new MediaSettingsProxy(binding, address);
        }

        public bool CheckDirectoryPermission(string path)
        {
            return Channel.CheckDirectoryPermission(path);
        }

        public string[] GetDirectories()
        {
            return Channel.GetDirectories();
        }

        public void BuildDatabaseAsync()
        {
            Channel.BuildDatabaseAsync();
        }

        public void BuildDatabase()
        {
            Channel.BuildDatabase();
        }

        public List<HomeMediaCenter.StreamSourcesItem> GetStreamSources()
        {
            return Channel.GetStreamSources();
        }

        public void SetStreamSources(HomeMediaCenter.StreamSourcesItem[] toAdd, HomeMediaCenter.StreamSourcesItem[] toUpdate, HomeMediaCenter.StreamSourcesItem[] toDelete)
        {
            Channel.SetStreamSources(toAdd, toUpdate, toDelete);
        }

        public string[] AddDirectory(string directory, string title)
        {
            return Channel.AddDirectory(directory, title);
        }

        public string[] RemoveDirectory(string directory)
        {
            return Channel.RemoveDirectory(directory);
        }

        public IMediaSettings IMediaSettings
        {
            get { return this.mediaSettings; }
        }

        public bool EnableWebcamStreaming
        {
            get { return Channel.EnableWebcamStreaming; }
            set { Channel.EnableWebcamStreaming = true; }
        }

        public bool EnableDesktopStreaming
        {
            get { return Channel.EnableDesktopStreaming; }
            set { Channel.EnableDesktopStreaming = value; }
        }

        public bool RealTimeDatabaseRefresh
        {
            get { return Channel.RealTimeDatabaseRefresh; }
            set { Channel.RealTimeDatabaseRefresh = value; }
        }

        public bool ShowHiddenFiles
        {
            get { return Channel.ShowHiddenFiles; }
            set { Channel.ShowHiddenFiles = value; }
        }

        public void OnAsyncBuildDatabaseStart(string exception)
        {
            OnExceptionEvent(exception, AsyncBuildDatabaseStart);
        }

        public void OnAsyncBuildDatabaseEnd(string exception)
        {
            OnExceptionEvent(exception, AsyncBuildDatabaseEnd);
        }

        protected void OnExceptionEvent(string exception, EventHandler<HomeMediaCenter.ExceptionEventArgs> handler)
        {
            if (handler != null)
            {
                handler(this, new HomeMediaCenter.ExceptionEventArgs(exception == null ? null : new Exception(exception)));
            }
        }

        public event EventHandler<HomeMediaCenter.ExceptionEventArgs> AsyncBuildDatabaseStart;

        public event EventHandler<HomeMediaCenter.ExceptionEventArgs> AsyncBuildDatabaseEnd;
    }

    public class MediaSettingsProxy : ClientBase<IMediaSettings>, IMediaSettings
    {
        public MediaSettingsProxy(NetNamedPipeBinding binding, EndpointAddress address) : base(binding, address) { }

        public string[] GetEncodeStrings(EncodeType type)
        {
            return Channel.GetEncodeStrings(type);
        }

        public void SetEncodeStrings(EncodeType type, string[] encode)
        {
            Channel.SetEncodeStrings(type, encode);
        }

        public bool GetNativeFile(EncodeType type)
        {
            return Channel.GetNativeFile(type);
        }

        public void SetNativeFile(EncodeType type, bool value)
        {
            Channel.SetNativeFile(type, value);
        }
    }
}
