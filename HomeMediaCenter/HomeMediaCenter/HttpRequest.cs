using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.Threading;
using System.Web;
using System.IO;

namespace HomeMediaCenter
{
    public class HttpRequest
    {
        private readonly TcpClient socket;
        private readonly Thread thread;

        private readonly Dictionary<string, string> headers = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
        private readonly Dictionary<string, string> urlParams = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);

        private string method = string.Empty;
        private string url = string.Empty;
        private string version = string.Empty;

        private string soapAction;
        private string soapService;
        private string[] soapOutParam;

        private bool responseSended;

        public HttpRequest(TcpClient socket, Thread thread)
        {
            this.socket = socket;
            this.thread = thread;
        }

        public Thread Thread
        {
            get { return this.thread; }
        }

        public TcpClient Socket
        {
            get { return this.socket; }
        }

        public string Method
        {
            get { return this.method; }
        }

        public string Url
        {
            get { return this.url; }
        }

        public Dictionary<string, string> UrlParams
        {
            get { return this.urlParams; }
        }

        public string Version
        {
            get { return this.version; }
        }

        public Dictionary<string, string> Headers
        {
            get { return this.headers; }
        }

        public string SoapAction
        {
            get { return this.soapAction; }
        }

        public string SoapService
        {
            get { return this.soapService; }
        }

        public string[] SoapOutParam
        {
            get { return this.soapOutParam; }
        }

        public void SetSoap(string soapAction, string soapService, string[] soapOutParam)
        {
            this.soapAction = soapAction;
            this.soapService = soapService;
            this.soapOutParam = soapOutParam;
        }

        public bool ResponseSended
        {
            get { return this.responseSended; }
            set { this.responseSended = value; }
        }

        public void ParseHeaders()
        {
            NetworkStream stream = this.socket.GetStream();
            StringBuilder sb = new StringBuilder(128);
            bool isLastR = false, isFirstLine = true;
            int iByte;

            while ((iByte = stream.ReadByte()) >= 0)
            {
                if (iByte == '\n' && isLastR)
                {
                    string line = sb.ToString(0, sb.Length - 1);
                    if (line == string.Empty)
                        break;
                    else
                        sb.Clear();

                    if (isFirstLine)
                    {
                        string[] values = line.Split(new char[] { ' ' }, 2, StringSplitOptions.RemoveEmptyEntries);
                        this.method = values[0].ToUpper();

                        int index = values[1].LastIndexOf(' ');
                        this.version = values[1].Substring(index + 1);

                        values = HttpUtility.UrlDecode(values[1].Substring(0, index)).Split(new char[] { '?' }, 2, StringSplitOptions.RemoveEmptyEntries);
                        this.url = values[0].ToLower();
                        if (values.Length == 2)
                        {
                            foreach (string parameter in values[1].Split(new char[] { '&' }, StringSplitOptions.RemoveEmptyEntries))
                            {
                                string[] keyValue = parameter.Split(new char[] { '=' }, 2, StringSplitOptions.RemoveEmptyEntries);
                                this.urlParams[keyValue[0]] = (keyValue.Length == 2) ? keyValue[1] : string.Empty;
                            }
                        }

                        isFirstLine = false;
                    }
                    else
                    {
                        string[] keyValue = line.Split(new char[] { ':' }, 2, StringSplitOptions.RemoveEmptyEntries);
                        this.headers[keyValue[0].Trim()] = keyValue.Length > 1 ? keyValue[1].Trim() : string.Empty;
                    }

                    isLastR = false;
                }
                else if (iByte == '\r')
                {
                    sb.Append((char)iByte);
                    isLastR = true;
                }
                else
                {
                    sb.Append((char)iByte);
                    isLastR = false;
                }
            }
        }

        public int GetLength()
        {
            return int.Parse(this.headers["Content-Length"]);
        }

        public MemoryStream GetContent()
        {
            NetworkStream stream = this.socket.GetStream();
            byte[] buffer = new byte[GetLength()];
            int readed, offset = 0;

            while ((readed = stream.Read(buffer, offset, buffer.Length - offset)) > 0)
            {
                offset += readed;
                if (offset >= buffer.Length)
                    break;
            }

            return new MemoryStream(buffer, 0, buffer.Length);
        }
    }
}
