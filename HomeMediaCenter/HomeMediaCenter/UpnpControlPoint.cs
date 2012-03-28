using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UPNPLib;
using System.Xml;
using System.IO;
using System.Net;

namespace HomeMediaCenter
{
    public class UpnpControlPoint
    {
        private Dictionary<string, UPnPDevice> devices = new Dictionary<string, UPnPDevice>(StringComparer.OrdinalIgnoreCase);

        public void Refresh(string type)
        {
            UPnPDeviceFinder finder = new UPnPDeviceFinder();
            UPnPDevices devs = finder.FindByType(type, 0);

            lock (this)
            {
                this.devices.Clear();
                foreach (UPnPDevice device in devs)
                {
                    this.devices[device.UniqueDeviceName] = device;
                }
            }
        }

        public void WriteXml(XmlWriter xmlWriter)
        {
            lock (this)
            {
                foreach (UPnPDevice device in this.devices.Values)
                {
                    xmlWriter.WriteStartElement("option");
                    xmlWriter.WriteAttributeString("value", device.UniqueDeviceName);
                    xmlWriter.WriteValue(device.FriendlyName);
                    xmlWriter.WriteEndElement();
                }
            }
        }

        public void VolumePlus(string deviceId)
        {
            UPnPService service = GetService(deviceId, "urn:schemas-upnp-org:service:RenderingControl:1");

            object[] inArgs = new object[] { 0, "Master" };
            object outArgs = null;
            service.InvokeAction("GetVolume", inArgs, ref outArgs);

            ushort vol = (ushort)((object[])outArgs)[0];
            if ((vol += 5) > 100)
                vol = 100;

            inArgs = new object[] { 0, "Master", vol };
            outArgs = null;
            service.InvokeAction("SetVolume", inArgs, ref outArgs);
        }

        public void VolumeMinus(string deviceId)
        {
            UPnPService service = GetService(deviceId, "urn:schemas-upnp-org:service:RenderingControl:1");

            object[] inArgs = new object[] { 0, "Master" };
            object outArgs = null;
            service.InvokeAction("GetVolume", inArgs, ref outArgs);

            ushort vol = (ushort)((object[])outArgs)[0];
            if ((vol -= 5) > 100)
                vol = 0;

            inArgs = new object[] { 0, "Master", vol };
            outArgs = null;
            service.InvokeAction("SetVolume", inArgs, ref outArgs);
        }

        public void Play(string deviceId, int httpPort, uint objectID, ItemManager manager)
        {
            IUPnPDeviceDocumentAccess deviceAccess;
            UPnPService service = GetService(deviceId, "urn:schemas-upnp-org:service:AVTransport:1", out deviceAccess);
            IPAddress deviceIP = IPAddress.Parse(new Uri(deviceAccess.GetDocumentURL()).Host);

            //Najdenie adresy servera podla adresy zariadenia kde sa ma prehravanie spustit
            IPAddress[] addresses = Dns.GetHostAddresses(Dns.GetHostName()).Where(a => a.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork).ToArray();
            int byteIndex = 0;
            while (addresses.Length > 1)
            {
                addresses = addresses.Where(a => a.GetAddressBytes()[byteIndex] == deviceIP.GetAddressBytes()[byteIndex]).ToArray();
                byteIndex++;
            }
            //Ak ostane jedna adresa - zapise sa
            if (addresses.Length == 1)
                deviceIP = addresses[0];

            string result;
            Dictionary<string, string> headers = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
            headers.Add("host", deviceIP + ":" + httpPort);

            manager.BrowseSync(headers, objectID, BrowseFlag.BrowseMetadata, out result);

            XmlDocument xmlReader = new XmlDocument();
            xmlReader.LoadXml(result);
            XmlNamespaceManager namespaceManager = new XmlNamespaceManager(xmlReader.NameTable);
            namespaceManager.AddNamespace("didlNam", "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/");
            XmlNode res = xmlReader.SelectSingleNode("/didlNam:DIDL-Lite/didlNam:item/didlNam:res[1]", namespaceManager);

            object[] inArgs = new object[] { 0, res.InnerText, result };
            object outArgs = null;
            service.InvokeAction("SetAVTransportURI", inArgs, ref outArgs);

            inArgs = new object[] { 0, 1 };
            outArgs = null;
            service.InvokeAction("Play", inArgs, ref outArgs);
        }

        public void Stop(string deviceId)
        {
            UPnPService service = GetService(deviceId, "urn:schemas-upnp-org:service:AVTransport:1");

            object[] inArgs = new object[] { 0 };
            object outArgs = null;
            service.InvokeAction("Stop", inArgs, ref outArgs);
        }

        private UPnPService GetService(string deviceId, string serviceType)
        {
            IUPnPDeviceDocumentAccess deviceAccess;
            return GetService(deviceId, serviceType, out deviceAccess);
        }

        private UPnPService GetService(string deviceId, string serviceType, out IUPnPDeviceDocumentAccess deviceAccess)
        {
            lock (this)
            {
                UPnPDevice device;
                if (this.devices.ContainsKey(deviceId))
                    device = this.devices[deviceId];
                else
                    throw new MediaCenterException("Device " + deviceId + " not found");

                foreach (UPnPService service in device.Services)
                {
                    if (StringComparer.OrdinalIgnoreCase.Compare(service.ServiceTypeIdentifier, serviceType) == 0)
                    {
                        deviceAccess = device as IUPnPDeviceDocumentAccess;
                        return service;
                    }
                }
            }

            deviceAccess = null;
            return null;
        }
    }
}
