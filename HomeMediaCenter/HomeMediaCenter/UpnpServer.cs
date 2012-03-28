using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Net;

namespace HomeMediaCenter
{
    public class UpnpServer
    {
        private UpnpDevice rootDevice;
        private SsdpServer ssdpServer;
        private HttpServer httpServer;

        private IPAddress[] hostAddresses = new IPAddress[0];
        private int httpPort = 12346;

        private byte[] descArray;

        public UpnpServer(UpnpDevice rootDevice)
        {
            this.rootDevice = rootDevice;
            this.httpServer = new HttpServer(this);
            this.ssdpServer = new SsdpServer(this);

            this.httpServer.AddRoute("GET", "/description.xml", new HttpRouteDelegate(GetDescription));
        }

        public IPAddress[] HostAddresses
        {
            get { return this.hostAddresses; }
        }

        public UpnpDevice RootDevice
        {
            get { return this.rootDevice; }
        }

        public HttpServer HttpServer
        {
            get { return this.httpServer; }
        }

        public int HttpPort
        {
            get { return this.httpPort; }
            set { this.httpPort = value; }
        }

        public void Start()
        {
            MemoryStream memStream = new MemoryStream();
            using (XmlTextWriter descWriter = new XmlTextWriter(memStream, new UTF8Encoding(false)))
            {
                descWriter.Formatting = Formatting.Indented;
                descWriter.WriteRaw("<?xml version=\"1.0\"?>");

                descWriter.WriteStartElement("root", "urn:schemas-upnp-org:device-1-0");

                descWriter.WriteStartElement("specVersion");
                descWriter.WriteElementString("major", "1");
                descWriter.WriteElementString("minor", "0");
                descWriter.WriteEndElement();

                descWriter.WriteStartElement("device");
                this.rootDevice.WriteDescription(descWriter);
                descWriter.WriteEndElement();

                descWriter.WriteEndElement();

                descWriter.Flush();
                this.descArray = memStream.ToArray();
            }

            this.httpServer.Start();
            this.ssdpServer.Start();
        }

        public void Stop()
        {
            this.ssdpServer.Stop();
            this.httpServer.Stop();
        }

        public void GetDescription(HttpRequest request)
        {
            HttpResponse response = new HttpResponse(request);
            response.AddHreader(HttpHeader.ContentLength, this.descArray.Length.ToString());
            response.AddHreader(HttpHeader.ContentType, "text/xml; charset=\"utf-8\"");

            using (MemoryStream stream = new MemoryStream(this.descArray))
            {
                response.SendHeaders();

                stream.CopyTo(request.Socket.GetStream());
            }
        }

        public void SaveSettings(XmlWriter xmlWriter)
        {
            xmlWriter.WriteElementString("Port", this.httpPort.ToString());

            xmlWriter.WriteStartElement("HostAddresses");
            foreach (IPAddress address in this.hostAddresses)
                xmlWriter.WriteElementString("string", address.ToString());
            xmlWriter.WriteEndElement();

            this.httpServer.SaveSettings(xmlWriter);
        }

        public void LoadSettings(XmlDocument xmlReader)
        {
            int port;
            XmlNode portNode = xmlReader.SelectSingleNode("/HomeMediaCenter/Port");
            if (portNode != null && int.TryParse(portNode.InnerText, out port))
                this.httpPort = port;

            List<IPAddress> addresses = new List<IPAddress>();
            IPAddress ipAddress;
            foreach (XmlNode address in xmlReader.SelectNodes("/HomeMediaCenter/HostAddresses/*"))
                if (IPAddress.TryParse(address.InnerText, out ipAddress))
                    addresses.Add(ipAddress);
            this.hostAddresses = addresses.ToArray();

            this.httpServer.LoadSettings(xmlReader);
        }
    }
}
