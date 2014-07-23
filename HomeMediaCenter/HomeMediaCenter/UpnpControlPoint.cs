using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UPNPLib;
using System.Xml;
using System.IO;
using System.Net;
using System.Net.Sockets;

namespace HomeMediaCenter
{
    public class UpnpControlPoint
    {
        private MediaServerDevice upnpDevice;
        private Dictionary<string, UPnPDevice> devices = new Dictionary<string, UPnPDevice>(StringComparer.OrdinalIgnoreCase);

        private delegate void PortForwardingDelType(bool forward, int port);
        private PortForwardingDelType portForwardingDel;

        public UpnpControlPoint(MediaServerDevice upnpDevice)
        {
            this.upnpDevice = upnpDevice;
            this.portForwardingDel = new PortForwardingDelType(PortForwarding);
        }

        public void PortForwardingAsync(bool forward, int port)
        {
            this.portForwardingDel.BeginInvoke(forward, port, new AsyncCallback(PortForwardingAsyncResult), null);
        }

        public void PortForwarding(bool forward, int port)
        {
            bool routerFound = false;

            Refresh();

            UPnPDevice[] devices;
            lock (this)
            {
                devices = this.devices.Values.Where(a => a.Type == "urn:schemas-upnp-org:device:WANConnectionDevice:1").ToArray();
            }

            foreach (UPnPDevice device in devices)
            {
                UPnPService service = GetService(device, "urn:schemas-upnp-org:service:WANIPConnection:1");
                if (service == null)
                {
                    service = GetService(device, "urn:schemas-upnp-org:service:WANPPPConnection:1");
                    if (service == null)
                        continue;
                }

                if (forward)
                {
                    //Pouzit presmerovanie
                    try
                    {
                        Uri documentUrl = new Uri(((IUPnPDeviceDocumentAccess)device).GetDocumentURL());

                        //Zistenie IP adresy tohoto PC pridelenu routrom
                        TcpClient client = new TcpClient();
                        client.Connect(documentUrl.Host, documentUrl.Port);
                        string localEndPoint = ((IPEndPoint)client.Client.LocalEndPoint).Address.ToString();
                        client.Close();

                        object[] inArgs = new object[] { string.Empty, port, "TCP", port, localEndPoint, true, "HomeMediaCenter", 0 };
                        object outArgs = null;
                        service.InvokeAction("AddPortMapping", inArgs, ref outArgs);

                        //Skontroluje ci bolo presmerovanie zapisane
                        inArgs = new object[] { string.Empty, port, "TCP" };
                        outArgs = null;
                        service.InvokeAction("GetSpecificPortMappingEntry", inArgs, ref outArgs);

                        if (((string)((object[])outArgs)[1]) != localEndPoint)
                            throw new Exception();

                        routerFound = true;
                        this.upnpDevice.OnLogEvent("Port forwarded on UPnP router " + device.FriendlyName);
                    }
                    catch
                    {
                        this.upnpDevice.OnLogEvent("Unable to forward port on UPnP router " + device.FriendlyName);
                    }
                }
                else
                {
                    //Nepouzivat presmerovanie
                    try
                    {
                        //Pri najblizsom spusteni alebo restarte odstrani presmerovany port ak existuje
                        object[] inArgs = new object[] { string.Empty, port, "TCP" };
                        object outArgs = null;
                        service.InvokeAction("DeletePortMapping", inArgs, ref outArgs);
                    }
                    catch { }
                }
            }

            if (forward && !routerFound)
                this.upnpDevice.OnLogEvent("UPnP router not found");
        }

        public void Refresh()
        {
            UPnPDeviceFinder finder = new UPnPDeviceFinder();
            UPnPDevices devs = finder.FindByType("upnp:rootdevice", 0);

            lock (this)
            {
                this.devices.Clear();
                foreach (UPnPDevice device in GetAllDevices(devs.OfType<UPnPDevice>()))
                {
                    this.devices[device.UniqueDeviceName] = device;
                }
            }
        }

        public void WriteXml(XmlWriter xmlWriter, string selectedUDN, string type = "urn:schemas-upnp-org:device:MediaRenderer:1")
        {
            lock (this)
            {
                foreach (UPnPDevice device in this.devices.Values.Where(a => a.Type == type))
                {
                    xmlWriter.WriteStartElement("option");
                    xmlWriter.WriteAttributeString("value", device.UniqueDeviceName);
                    if (string.Compare(device.UniqueDeviceName, selectedUDN, true) == 0)
                        xmlWriter.WriteAttributeString("selected", "selected");
                    xmlWriter.WriteValue(device.FriendlyName);
                    xmlWriter.WriteEndElement();
                }
            }
        }

        public void VolumePlus(string deviceId)
        {
            UPnPService service = GetServiceSync(deviceId, "urn:schemas-upnp-org:service:RenderingControl:1");

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
            UPnPService service = GetServiceSync(deviceId, "urn:schemas-upnp-org:service:RenderingControl:1");

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

        public void Play(string deviceId, int httpPort, string objectID, ItemManager manager, TimeSpan startTime)
        {
            IUPnPDeviceDocumentAccess deviceAccess;
            UPnPService service = GetServiceSync(deviceId, "urn:schemas-upnp-org:service:AVTransport:1", out deviceAccess);
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

            string result, firstResource;
            manager.BrowseMetadata("http://" + deviceIP + ":" + httpPort, objectID, startTime, out result, out firstResource);

            object[] inArgs = new object[] { 0, firstResource, result };
            object outArgs = null;
            service.InvokeAction("SetAVTransportURI", inArgs, ref outArgs);

            inArgs = new object[] { 0, 1 };
            outArgs = null;
            service.InvokeAction("Play", inArgs, ref outArgs);
        }

        public void Stop(string deviceId)
        {
            UPnPService service = GetServiceSync(deviceId, "urn:schemas-upnp-org:service:AVTransport:1");

            object[] inArgs = new object[] { 0 };
            object outArgs = null;
            service.InvokeAction("Stop", inArgs, ref outArgs);
        }

        private UPnPService GetServiceSync(string deviceId, string serviceType)
        {
            IUPnPDeviceDocumentAccess deviceAccess;
            return GetServiceSync(deviceId, serviceType, out deviceAccess);
        }

        private UPnPService GetServiceSync(string deviceId, string serviceType, out IUPnPDeviceDocumentAccess deviceAccess)
        {
            lock (this)
            {
                UPnPDevice device;
                if (this.devices.ContainsKey(deviceId))
                    device = this.devices[deviceId];
                else
                    throw new MediaCenterException("Device " + deviceId + " not found");

                return GetService(device, serviceType, out deviceAccess);
            }
        }

        private UPnPService GetService(UPnPDevice device, string serviceType)
        {
            IUPnPDeviceDocumentAccess deviceAccess;
            return GetService(device, serviceType, out deviceAccess);
        }

        private UPnPService GetService(UPnPDevice device, string serviceType, out IUPnPDeviceDocumentAccess deviceAccess)
        {
            foreach (UPnPService service in device.Services)
            {
                if (StringComparer.OrdinalIgnoreCase.Compare(service.ServiceTypeIdentifier, serviceType) == 0)
                {
                    deviceAccess = device as IUPnPDeviceDocumentAccess;
                    return service;
                }
            }

            deviceAccess = null;
            return null;
        }

        private IEnumerable<UPnPDevice> GetAllDevices(IEnumerable<UPnPDevice> rootDevices)
        {
            IEnumerable<UPnPDevice> result = rootDevices;

            foreach (UPnPDevice device in rootDevices)
            {
                string str = device.FriendlyName;
                if (device.HasChildren)
                {
                    result = result.Concat(GetAllDevices(device.Children.OfType<UPnPDevice>()));
                }
            }

            return result;
        }

        private void PortForwardingAsyncResult(IAsyncResult result)
        {
            try
            {
                this.portForwardingDel.EndInvoke(result);
            }
            catch (Exception ex)
            {
                this.upnpDevice.OnLogEvent("Port forwarding exception: " + ex.Message);
            }
        }
    }
}
