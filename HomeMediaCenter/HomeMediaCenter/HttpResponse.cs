using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Net.Sockets;
using System.Xml;

namespace HomeMediaCenter
{
    public enum HttpHeader { ContentLength, ContentType, Server, Date, Connection, AcceptRanges }

    public class HttpResponse
    {
        private readonly HttpRequest request;
        private readonly Dictionary<string, string> headers = new Dictionary<string, string>();
        private int stateCode = 200;

        public HttpResponse(HttpRequest request)
        {
            this.request = request;
        }

        public void SetStateCode(int stateCode)
        {
            this.stateCode = stateCode;
        }

        public void AddHreader(string key, string value)
        {
            this.headers.Add(key, value);
        }

        public void AddHreader(HttpHeader key, string value)
        {
            switch (key)
            {
                case HttpHeader.AcceptRanges: AddHreader("Accept-Ranges", value); return;
                case HttpHeader.ContentLength: AddHreader("Content-Length", value); return;
                case HttpHeader.ContentType: AddHreader("Content-Type", value); return;
                case HttpHeader.Date: AddHreader("Date", value); return;
                case HttpHeader.Server: AddHreader("Server", value); return;
                case HttpHeader.Connection: AddHreader("Connection", value); return;
            }
        }

        public void SendHeaders()
        {
            if (!this.request.ResponseSended)
            {
                AddHreader(HttpHeader.Server, string.Format("WindowsNT/{0}.{1} UPnP/1.1 HMC/1.0",
                    Environment.OSVersion.Version.Major, Environment.OSVersion.Version.Minor));
                AddHreader(HttpHeader.Date, DateTime.Now.ToString("r"));
                AddHreader(HttpHeader.Connection, "close");

                byte[] data;
                NetworkStream stream = this.request.Socket.GetStream();

                data = Encoding.ASCII.GetBytes(string.Format("{0} {1} {2}\r\n", this.request.Version, this.stateCode, GetState()));
                stream.Write(data, 0, data.Length);

                foreach (KeyValuePair<string, string> kvp in this.headers)
                {
                    data = Encoding.ASCII.GetBytes(string.Format("{0}: {1}\r\n", kvp.Key, kvp.Value));
                    stream.Write(data, 0, data.Length);
                }

                data = Encoding.ASCII.GetBytes("\r\n");
                stream.Write(data, 0, data.Length);

                this.request.ResponseSended = true;
            }
        }

        public void SendSoapHeadersBody(params string[] arguments)
        {
            SendSoapHeadersBody((IEnumerable<string>) arguments);
        }

        public void SendSoapHeadersBody(IEnumerable<string> arguments)
        {
            if (arguments.Count() != this.request.SoapOutParam.Length)
                throw new HttpException(400, "Bad number of SOAP parameters");

            MemoryStream memStream = new MemoryStream();
            using (XmlTextWriter soapWriter = new XmlTextWriter(memStream, new UTF8Encoding(false)))
            {
                soapWriter.Formatting = Formatting.Indented;
                soapWriter.WriteRaw("<?xml version=\"1.0\"?>");

                soapWriter.WriteStartElement("s", "Envelope", "http://schemas.xmlsoap.org/soap/envelope/");
                soapWriter.WriteAttributeString("s", "encodingStyle", null, "http://schemas.xmlsoap.org/soap/encoding/");

                soapWriter.WriteStartElement("s", "Body", null);

                soapWriter.WriteStartElement("u", this.request.SoapAction + "Response", this.request.SoapService);

                int i = 0;
                foreach (string argument in arguments)
                {
                    soapWriter.WriteElementString(this.request.SoapOutParam[i++], argument);
                }

                soapWriter.WriteEndElement();

                soapWriter.WriteEndElement();

                soapWriter.WriteEndElement();

                soapWriter.Flush();
                memStream.Position = 0;

                AddHreader(HttpHeader.ContentLength, memStream.Length.ToString());
                AddHreader(HttpHeader.ContentType, "text/xml; charset=\"utf-8\"");

                SendHeaders();
                memStream.CopyTo(this.request.Socket.GetStream());
            }
        }

        public string GetState()
        {
            switch (this.stateCode)
            {
                case 200: return "OK";
                case 206: return "Partial Content";
                case 400: return "Bad Request";
                case 402: return "Payment Required";
                case 404: return "Not Found";
                case 406: return "Not Acceptable";
                case 500: return "Internal Server Error";
            }

            return string.Empty;
        }
    }
}
