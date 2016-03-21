using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Runtime.Remoting.Messaging;
using System.Globalization;

namespace HomeMediaCenter
{
    public abstract class UpnpDevice
    {
        protected Guid udn;
        protected string friendlyName;
        protected string deviceType;
        protected string manufacturer;
        protected string manufacturerUrl;
        protected string modelName;
        protected string modelNumber;
        protected string modelUrl;
        protected string serialNumber;

        protected readonly List<UpnpService> services = new List<UpnpService>();
        protected readonly UpnpServer server;
        protected bool settingsChanged;

        protected CultureInfo cultureInfo;

        private bool started;
        private bool starting;
        private bool stopping;        

        private readonly Action asyncStartDel;
        private readonly Action asyncStopDel;

        public event EventHandler<LogEventArgs> LogEvent;
        public event EventHandler<ExceptionEventArgs> AsyncStartEnd;
        public event EventHandler<ExceptionEventArgs> AsyncStopEnd;

        public UpnpDevice()
        {
            this.server = new UpnpServer(this);

            this.asyncStartDel = new Action(OnStart);
            this.asyncStopDel = new Action(OnStop);
        }

        public IEnumerable<UpnpService> Services
        {
            get { return this.services; }
        }

        public Guid Udn
        {
            get { return this.udn; }
        }

        public string DeviceType
        {
            get { return this.deviceType; }
        }

        public string ModelNumber
        {
            get { return this.modelNumber; }
        }

        public string FriendlyName
        {
            get { return this.friendlyName; }
            set
            {
                CheckStopped();

                this.friendlyName = value;
                SettingsChanged();
            }
        }

        internal UpnpServer Server
        {
            get { return this.server; }
        }

        public Interfaces.IUpnpServer IServer
        {
            get { return this.server; }
        }

        public bool Started
        {
            get { return this.started; }
        }

        public bool Stopping
        {
            get { return this.stopping; }
        }

        internal CultureInfo CultureInfo
        {
            get { return this.cultureInfo; }
            set
            {
                CheckStopped();

                this.cultureInfo = value;
                SettingsChanged();
            }
        }

        public string CultureInfoName
        {
            get
            {
                CultureInfo ci = this.cultureInfo;
                return (ci == null) ? null : ci.Name;
            }
            set
            {
                CultureInfo = (value == null) ? null : new System.Globalization.CultureInfo(value);
            }
        }

        public virtual void WriteDescription(XmlTextWriter descWriter)
        {
            descWriter.WriteElementString("UDN", "uuid:" + this.udn);

            descWriter.WriteElementString("friendlyName", this.friendlyName);

            descWriter.WriteElementString("deviceType", this.deviceType);

            descWriter.WriteElementString("manufacturer", this.manufacturer);

            if (this.manufacturerUrl != null && this.manufacturerUrl != string.Empty)
                descWriter.WriteElementString("manufacturerURL", this.manufacturerUrl);

            descWriter.WriteElementString("modelName", this.modelName);

            if (this.modelNumber != null && this.modelNumber != string.Empty)
                descWriter.WriteElementString("modelNumber", this.modelNumber);

            if (this.modelUrl != null && this.modelUrl != string.Empty)
                descWriter.WriteElementString("modelURL", this.modelUrl);

            if (this.serialNumber != null && this.serialNumber != string.Empty)
                descWriter.WriteElementString("serialNumber", this.serialNumber);

            WriteSpecificDescription(descWriter);

            descWriter.WriteStartElement("serviceList");
            foreach (UpnpService service in this.services)
            {
                descWriter.WriteStartElement("service");
                service.WriteDescription(descWriter);
                descWriter.WriteEndElement();
            }
            descWriter.WriteEndElement();
        }

        public void Start(bool async = false)
        {
            if (this.started == true || this.starting == true || this.stopping == true)
                return;

            this.started = true;
            this.starting = true;

            if (async)
            {
                this.asyncStartDel.BeginInvoke(new AsyncCallback(AsyncStartResult), null);
            }
            else
            {
                try
                {
                    OnStart();
                }
                catch
                {
                    this.started = false;
                    throw;
                }
                finally { this.starting = false; }
            }
        }

        public void StartAsync()
        {
            Start(true);
        }

        public void Stop(bool async = false)
        {
            if (this.started == false || this.starting == true || this.stopping == true)
                return;
            
            this.stopping = true;

            if (async)
            {
                this.asyncStopDel.BeginInvoke(new AsyncCallback(AsyncStopResult), null);
            }
            else
            {
                try
                {
                    OnStop();
                    this.started = false;
                }
                finally { this.stopping = false; }
            }
        }

        public void StopAsync()
        {
            Stop(true);
        }

        public void OnLogEvent(string message)
        {
            EventHandler<LogEventArgs> handler = LogEvent;
            if (handler != null)
            {
                handler(this, new LogEventArgs(message));
            }
        }

        public void OnLogEvent(int requestCount)
        {
            EventHandler<LogEventArgs> handler = LogEvent;
            if (handler != null)
            {
                handler(this, new LogEventArgs(requestCount));
            }
        }

        public void SetCurrentThreadCulture()
        {
            if (this.cultureInfo != null)
            {
                System.Threading.Thread.CurrentThread.CurrentCulture = this.cultureInfo;
                System.Threading.Thread.CurrentThread.CurrentUICulture = this.cultureInfo;
            }
        }

        protected virtual void WriteSpecificDescription(XmlTextWriter descWriter) { }

        protected void OnExceptionEvent(Exception ex, EventHandler<ExceptionEventArgs> handler)
        {
            if (handler != null)
            {
                handler(this, new ExceptionEventArgs(ex));
            }
        }

        protected virtual void OnStart()
        {
            SetCurrentThreadCulture();

            try
            {
                this.server.Start();
            }
            catch
            {
                try { OnStop(); }
                catch { }

                throw;
            }
        }

        protected virtual void OnStop()
        {
            SetCurrentThreadCulture();

            this.server.Stop();
        }

        private void AsyncStartResult(IAsyncResult result)
        {
            Exception exc = null;
            try { this.asyncStartDel.EndInvoke(result); }
            catch (Exception ex)
            {
                exc = ex;
                this.started = false;
            }

            OnExceptionEvent(exc, AsyncStartEnd);

            this.starting = false;
        }

        private void AsyncStopResult(IAsyncResult result)
        {
            Exception exc = null;
            try
            {
                this.asyncStopDel.EndInvoke(result);
                this.started = false;
            }
            catch (Exception ex) { exc = ex; }

            //Pri restarte (start servera v udalosti stop) sa nemusi server spustit
            //Preto je stopping nastavene pred vyvolanim udalosti stop
            this.stopping = false;

            OnExceptionEvent(exc, AsyncStopEnd);
        }

        internal void CheckStopped()
        {
            if (this.started)
                throw new MediaCenterException("Server must be stopped to perform this operation");
        }

        internal void SettingsChanged()
        {
            this.settingsChanged = true;
        }
    }
}
