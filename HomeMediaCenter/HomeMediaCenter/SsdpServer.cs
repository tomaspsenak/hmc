using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Net.Sockets;
using System.Net;

namespace HomeMediaCenter
{
    internal class SsdpServer
    {
        private readonly UpnpServer upnpServer;
        private Thread[] listenerThreads;
        private Socket[] listenerSockets;
        private Timer[] notifyTimers;
        private Socket[] notifySockets;

        private readonly Random rn = new Random();
        private readonly int maxAge = 1800;

        public SsdpServer(UpnpServer upnpServer)
        {
            this.upnpServer = upnpServer;
        }

        public void Start()
        {
            if (this.listenerThreads == null)
            {
                this.listenerSockets = GetSockets();
                this.listenerThreads = new Thread[this.listenerSockets.Length];
                for (int i = 0; i < this.listenerThreads.Length; i++)
                {
                    this.listenerThreads[i] = new Thread(new ParameterizedThreadStart(ListenNotify));
                    this.listenerThreads[i].Start(this.listenerSockets[i]);
                }

                this.notifySockets = GetSockets();
                this.notifyTimers = new Timer[this.notifySockets.Length];
                for (int i = 0; i < this.notifySockets.Length; i++)
                {
                    //Oznamuje v pravidelnych intervaloch pritomnost, zaciatok je oneskoreny o sekundu
                    //Sprava sa posiela o nieco skor ako maxAge, preto nie je * 1000
                    this.notifyTimers[i] = new Timer(new TimerCallback(OnNotifyTimeout), this.notifySockets[i], 1000, this.maxAge * 900);
                }
            }
        }

        public void Stop()
        {
            if (this.listenerThreads != null)
            {
                foreach (Socket socket in this.listenerSockets)
                    socket.Close();

                foreach (Timer timer in this.notifyTimers)
                    timer.Dispose();

                foreach (Socket socket in this.notifySockets)
                {
                    SendNotify(socket, ((IPEndPoint)socket.LocalEndPoint).Address.ToString(), false);
                    socket.Close();
                }

                foreach (Thread thread in this.listenerThreads)
                    thread.Join();

                this.listenerSockets = null;
                this.listenerThreads = null;
                this.notifySockets = null;
                this.notifyTimers = null;
            }
        }

        private void OnNotifyTimeout(object socketObj)
        {
            Socket socket = (Socket)socketObj;
            SendNotify(socket, ((IPEndPoint)socket.LocalEndPoint).Address.ToString(), true);
        }

        private Socket[] GetSockets()
        {
            IEnumerable<IPAddress> addresses;

            if (this.upnpServer.HostAddresses.Length <= 0)
                addresses = Dns.GetHostAddresses(Dns.GetHostName()).Where(a => a.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork);
            else
                addresses = this.upnpServer.HostAddresses;

            return addresses.Select(delegate(IPAddress address) {

                    Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
                    socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, 1);
                    socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Broadcast, 1);
                    socket.SetSocketOption(SocketOptionLevel.IP, SocketOptionName.AddMembership,
                        new MulticastOption(IPAddress.Parse("239.255.255.250"), address));
                    socket.Bind(new IPEndPoint(address, 1900));

                    return socket;

                }).ToArray();
        }

        private void SendNotify(Socket socket, string host, bool isAlive)
        {
            SendNotifyMessage(socket, host, "upnp:rootdevice", this.upnpServer.RootDevice.Udn.ToString(), isAlive);
            SendNotifyMessage(socket, host, this.upnpServer.RootDevice.Udn.ToString(), this.upnpServer.RootDevice.Udn.ToString(), isAlive);
            SendNotifyMessage(socket, host, this.upnpServer.RootDevice.DeviceType, this.upnpServer.RootDevice.Udn.ToString(), isAlive);

            foreach (UpnpService service in this.upnpServer.RootDevice.Services)
                SendNotifyMessage(socket, host, service.ServiceType, this.upnpServer.RootDevice.Udn.ToString(), isAlive);
        }

        private void SendNotifyMessage(Socket socket, string host, string nt, string usn, bool isAlive)
        {
            string message = string.Format(@"NOTIFY * HTTP/1.1
HOST: 239.255.255.250:1900
CACHE-CONTROL: max-age = {0}
LOCATION: http://{1}:{2}/description.xml
NT: {3}
NTS: ssdp:{4}
SERVER: WindowsNT/{5}.{6} UPnP/1.1 HMC/1.0
USN: uuid:{7}{8}

",          this.maxAge, host, this.upnpServer.HttpPort, nt == usn ? "uuid:" + nt : nt, isAlive ? "alive" : "byebye", 
            Environment.OSVersion.Version.Major, Environment.OSVersion.Version.Minor, usn, nt == usn ? string.Empty : "::" + nt);

            byte[] data = Encoding.ASCII.GetBytes(message);

            try { socket.SendTo(data, new IPEndPoint(IPAddress.Broadcast, 1900)); }
            catch { }

            Thread.Sleep(100);
        }

        private void ListenNotify(object socketObj)
        {
            Socket socket = (Socket)socketObj;
            byte[] buffer = new byte[1024];
            int length;

            string localEndPoint = ((IPEndPoint)socket.LocalEndPoint).Address.ToString();

            this.upnpServer.RootDevice.OnLogEvent(string.Format("SSDP server started on {0}", localEndPoint));

            while (true)
            {
                EndPoint receivePoint = new IPEndPoint(IPAddress.Any, 0);

                try { length = socket.ReceiveFrom(buffer, 0, buffer.Length, SocketFlags.None, ref receivePoint); }
                catch { break; }

                string[] lines = Encoding.ASCII.GetString(buffer, 0, length).Split(new string[] { "\r\n" }, 
                    StringSplitOptions.RemoveEmptyEntries);

                if (lines.Length > 0 && lines[0] == "M-SEARCH * HTTP/1.1")
                {
                    Dictionary<string, string> dict = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);

                    for (int i = 1; i < lines.Length; i++)
                    {
                        string[] keyValue = lines[i].Split(new char[] { ':' }, 2);
                        if (keyValue.Length == 2)
                        {
                            dict[keyValue[0].Trim()] = keyValue[1].Trim();
                        }
                    }

                    if (!dict.ContainsKey("MAN") || StringComparer.OrdinalIgnoreCase.Compare(dict["MAN"], "\"ssdp:discover\"") != 0)
                        continue;

                    int mx;
                    if (!dict.ContainsKey("MX") || !int.TryParse(dict["MX"], out mx) || mx < 0)
                        continue;


                    if (!dict.ContainsKey("ST"))
                        continue;

                    string st = dict["ST"];
                    string usn = "uuid:" + this.upnpServer.RootDevice.Udn;
                    if (StringComparer.OrdinalIgnoreCase.Compare(st, "upnp:rootdevice") == 0 ||
                        StringComparer.OrdinalIgnoreCase.Compare(st, this.upnpServer.RootDevice.DeviceType) == 0)
                    {
                        //Ak je pozadovane upnp:rootdevice alebo typ hlavneho zariadenia - pokracuj dalej
                    }
                    else if (StringComparer.OrdinalIgnoreCase.Compare(st, usn) == 0)
                    {
                        //Ak je pozadovany identifikator, zjednoti st a usn koli velkym a malym pismenam - pokracuj dalej
                        st = usn;
                    }
                    else if (StringComparer.OrdinalIgnoreCase.Compare(st, "ssdp:all") == 0)
                    {
                        //Pri ssdp:all treba informovat o vsetkych zariadeniach a sluzbach
                        Thread.Sleep(rn.Next(mx * 850));

                        SendResponseMessage(socket, receivePoint, localEndPoint, "upnp:rootdevice", usn);
                        SendResponseMessage(socket, receivePoint, localEndPoint, usn, usn);
                        SendResponseMessage(socket, receivePoint, localEndPoint, this.upnpServer.RootDevice.DeviceType, usn);
                            
                        foreach (UpnpService service in this.upnpServer.RootDevice.Services)
                            SendResponseMessage(socket, receivePoint, localEndPoint, service.ServiceType, usn);

                        continue;
                    }
                    else if (this.upnpServer.RootDevice.Services.FirstOrDefault(a => 
                        StringComparer.OrdinalIgnoreCase.Compare(st, a.ServiceType) == 0) == null)
                    {
                        //Ak nie je pozadovany typ ani v services - ignoruj spravu
                        continue;
                    }

                    //mx sa nenasobi * 1000 - znizenie o cas potrebny na vykonanie funkcie
                    Thread.Sleep(rn.Next(mx * 850));

                    SendResponseMessage(socket, receivePoint, localEndPoint, st, usn);
                }
            }

            this.upnpServer.RootDevice.OnLogEvent("SSDP server stopped");
        }

        private void SendResponseMessage(Socket socket, EndPoint receivePoint, string host, string st, string usn)
        {
            string message = string.Format(@"HTTP/1.1 200 OK
CACHE-CONTROL: max-age = {0}
DATE: {1}
EXT:
LOCATION: http://{2}:{3}/description.xml
SERVER: WindowsNT/{4}.{5} UPnP/1.1 HMC/1.0
ST: {6}
USN: {7}{8}

",          this.maxAge, DateTime.Now.ToString("r"), host, this.upnpServer.HttpPort, Environment.OSVersion.Version.Major, 
            Environment.OSVersion.Version.Minor, st, usn, st == usn ? string.Empty : "::" + st);

            byte[] data = Encoding.ASCII.GetBytes(message);

            try { socket.SendTo(data, receivePoint); }
            catch { }
        }
    }
}