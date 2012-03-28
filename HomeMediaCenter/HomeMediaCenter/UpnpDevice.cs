using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

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

        public event EventHandler<LogEventArgs> LogEvent;

        public UpnpDevice()
        {
            this.server = new UpnpServer(this);
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

        public string FriendlyName
        {
            get { return this.friendlyName; }
            set { this.friendlyName = value; }
        }

        public UpnpServer Server
        {
            get { return this.server; }
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

        public void Start()
        {
            this.server.Start();
        }

        public void Stop()
        {
            this.server.Stop();
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

        protected virtual void WriteSpecificDescription(XmlTextWriter descWriter) { }
    }
}
