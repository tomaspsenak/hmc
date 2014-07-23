using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Xml;

namespace HomeMediaCenter
{
    public delegate void HttpRouteDelegate(HttpRequest request);

    public class HttpServer
    {
        private readonly UpnpServer upnpServer;
        private TcpListener listenerSocket;
        private Thread listenerThread;

        private readonly Dictionary<string, HttpRouteDelegate> routeTable = new Dictionary<string, HttpRouteDelegate>(StringComparer.OrdinalIgnoreCase);
        private readonly LinkedList<HttpRequest> requestList = new LinkedList<HttpRequest>();

        private int receiveTimeout = 900000;
        private int sendTimeout = 900000;

        public HttpServer(UpnpServer upnpServer)
        {
            this.upnpServer = upnpServer;
        }

        public void AddRoute(string method, string path, HttpRouteDelegate routeDelegate)
        {
            this.routeTable.Add(method + "_" + path, routeDelegate);
        }

        public HttpRouteDelegate GetRoute(string method, string path)
        {
            try { return this.routeTable[method + "_" + path]; }
            catch { return null; }
        }

        public void SaveSettings(XmlWriter xmlWriter)
        {
            xmlWriter.WriteElementString("ReceiveTimeout", this.receiveTimeout.ToString());
            xmlWriter.WriteElementString("SendTimeout", this.sendTimeout.ToString());
        }

        public void LoadSettings(XmlDocument xmlReader)
        {
            int timeout;
            XmlNode timeNode = xmlReader.SelectSingleNode("/HomeMediaCenter/ReceiveTimeout");
            if (timeNode != null && int.TryParse(timeNode.InnerText, out timeout))
                this.receiveTimeout = timeout;

            timeNode = xmlReader.SelectSingleNode("/HomeMediaCenter/SendTimeout");
            if (timeNode != null && int.TryParse(timeNode.InnerText, out timeout))
                this.sendTimeout = timeout;
        }

        public void Start()
        {
            if (this.listenerSocket == null)
            {
                try
                {
                    this.listenerSocket = new TcpListener(IPAddress.Any, this.upnpServer.HttpPort);
                    this.listenerSocket.Start();
                }
                catch (Exception ex)
                {
                    this.listenerSocket = null;
                    throw new MediaCenterException("Unable to start HTTP server: " + ex.Message);
                }

                this.listenerThread = new Thread(new ThreadStart(Listen));
                if (this.upnpServer.RootDevice.CultureInfo != null)
                    this.listenerThread.CurrentCulture = this.listenerThread.CurrentUICulture = this.upnpServer.RootDevice.CultureInfo;

                this.listenerThread.Start();
            }
        }

        public void Stop()
        {
            if (this.listenerSocket != null)
            {
                //Najprv sa uzavre vytvaranie novych poziadaviek - uz sa nevytvori nova
                this.listenerSocket.Server.Close();
                this.listenerThread.Join();

                //Nasledne sa uzatvara kazda poziadvaka
                lock (this.requestList)
                {
                    foreach (HttpRequest req in this.requestList)
                        req.CloseStream();
                }

                while (true)
                {
                    LinkedListNode<HttpRequest> lln;

                    lock (this.requestList)
                    {
                        lln = this.requestList.Last;
                    }

                    if (lln == null)
                        break;
                    else
                        lln.Value.Thread.Join();
                }

                this.listenerSocket = null;
                this.listenerThread = null;
            }
        }

        private void Listen()
        {
            this.upnpServer.RootDevice.OnLogEvent(string.Format("HTTP server started on {0}", this.listenerSocket.LocalEndpoint));

            while (true)
            {
                TcpClient socket;

                try { socket = this.listenerSocket.AcceptTcpClient(); }
                catch { break; }

                socket.ReceiveTimeout = this.receiveTimeout;
                socket.SendTimeout = this.sendTimeout;

                //Pre poziadavku vytvori uzol v zretazenom zozname
                LinkedListNode<HttpRequest> lln = new LinkedListNode<HttpRequest>(new HttpRequest(socket, 
                    new Thread(new ParameterizedThreadStart(ProceedRequest))));
                if (this.upnpServer.RootDevice.CultureInfo != null)
                    lln.Value.Thread.CurrentCulture = lln.Value.Thread.CurrentUICulture = this.upnpServer.RootDevice.CultureInfo;

                //Prida uzol do lzz, vytvori nove vlakno pre poziadavku, ako parameter da uzol z lzz
                Monitor.Enter(this.requestList);
                try
                {
                    lln.Value.Thread.Start(lln);
                    this.requestList.AddLast(lln);

                    this.upnpServer.RootDevice.OnLogEvent(this.requestList.Count);
                }
                catch (OutOfMemoryException) { }
                finally { Monitor.Exit(this.requestList); }
            }

            this.upnpServer.RootDevice.OnLogEvent("HTTP server stopped");
        }

        private void ProceedRequest(object obj)
        {
            LinkedListNode<HttpRequest> lln = (LinkedListNode<HttpRequest>)obj;

            try
            {
                lln.Value.ParseHeaders();

                this.upnpServer.RootDevice.OnLogEvent(string.Format("{0} {1}", lln.Value.Method, lln.Value.Url));

                HttpRouteDelegate del = GetRoute(lln.Value.Method, lln.Value.Url);
                if (del == null)
                {
                    string[] fragments = lln.Value.Url.Split(new char[] { '/' }, StringSplitOptions.RemoveEmptyEntries);
                    for (int i = fragments.Length; i >= 0; i--)
                    {
                        string str = "/";
                        for (int j = 0; j < i; j++)
                        {
                            str += fragments[j] + "/";
                        }
                        str += "*";

                        if ((del = GetRoute(lln.Value.Method, str)) != null)
                            break;
                    }
                    if (del == null)
                        throw new HttpException(404, "Not found - " + lln.Value.Url);
                }

                del(lln.Value);
            }
            catch (HttpException ex)
            {
                this.upnpServer.RootDevice.OnLogEvent(ex.Message);

                HttpResponse response = lln.Value.GetResponse();
                response.SetStateCode(ex.Code);
                try { response.SendHeaders(); }
                catch { }
            }
            catch (SoapException ex)
            {
                this.upnpServer.RootDevice.OnLogEvent(ex.Message);

                HttpResponse response = lln.Value.GetResponse();
                try { response.SendSoapErrorHeadersBody(ex.Code, ex.Message); }
                catch { }
            }
            catch (Exception ex)
            {
                this.upnpServer.RootDevice.OnLogEvent(ex.Message);

                HttpResponse response = lln.Value.GetResponse();
                response.SetStateCode(500);
                try { response.SendHeaders(); }
                catch { }
            }

            //Odstrani uzol poziadavky zo zretazeneho zoznamu
            lock (this.requestList)
            {
                this.requestList.Remove(lln);

                this.upnpServer.RootDevice.OnLogEvent(this.requestList.Count);
            }
            
            //Pockat kratky cas kym sa uzatvori spojenie
            Thread.Sleep(10);
            //Uzatvorenie spojenia
            try { lln.Value.CloseStream(); }
            catch { }
        }
    }
}
