using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Net.Sockets;
using System.Xml;

namespace HomeMediaCenter
{
    public enum HttpHeader { ContentLength, ContentType, Server, Date, Connection, AcceptRanges, TransferEncoding, CacheControl }

    public class HttpResponse
    {
        private readonly HttpRequest request;
        private readonly NetworkStream stream;
        private readonly Dictionary<string, string> headers = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);

        private int stateCode = 200;
        private bool responseSended;
        private ChunkedStream chunkedStream;

        public HttpResponse(HttpRequest request, NetworkStream stream)
        {
            this.request = request;
            this.stream = stream;
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
                case HttpHeader.TransferEncoding: AddHreader("Transfer-Encoding", value); return;
                case HttpHeader.CacheControl: AddHreader("Cache-Control", value); return;
            }
        }

        public void SendHeaders()
        {
            if (this.responseSended)
                return;

            AddHreader(HttpHeader.Server, string.Format("WindowsNT/{0}.{1} UPnP/1.1 HMC/1.0",
                Environment.OSVersion.Version.Major, Environment.OSVersion.Version.Minor));
            AddHreader(HttpHeader.Date, DateTime.Now.ToString("r"));
            AddHreader(HttpHeader.Connection, "close");

            byte[] data = Encoding.ASCII.GetBytes(string.Format("{0} {1} {2}\r\n", this.request.Version, this.stateCode, GetState()));
            this.stream.Write(data, 0, data.Length);

            foreach (KeyValuePair<string, string> kvp in this.headers)
            {
                data = Encoding.ASCII.GetBytes(string.Format("{0}: {1}\r\n", kvp.Key, kvp.Value));
                this.stream.Write(data, 0, data.Length);
            }

            data = Encoding.ASCII.GetBytes("\r\n");
            this.stream.Write(data, 0, data.Length);

            this.responseSended = true;

            if (this.headers.ContainsKey("Transfer-Encoding") &&
                String.Compare(this.headers["Transfer-Encoding"].Trim('"'), "chunked", true) == 0)
            {
                this.chunkedStream = new ChunkedStream(this.stream, false);
            }
        }

        public void SendSoapHeadersBody(params string[] arguments)
        {
            SendSoapHeadersBody((IEnumerable<string>) arguments);
        }

        public void SendSoapHeadersBody(IEnumerable<string> arguments)
        {
            if (this.responseSended)
                return;

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
                memStream.CopyTo(GetStream());
            }
        }

        public void SendSoapErrorHeadersBody(int errorCode, string errorDescription)
        {
            if (this.responseSended)
                return;

            MemoryStream memStream = new MemoryStream();
            using (XmlTextWriter soapWriter = new XmlTextWriter(memStream, new UTF8Encoding(false)))
            {
                soapWriter.Formatting = Formatting.Indented;
                soapWriter.WriteRaw("<?xml version=\"1.0\"?>");

                soapWriter.WriteStartElement("s", "Envelope", "http://schemas.xmlsoap.org/soap/envelope/");
                soapWriter.WriteAttributeString("s", "encodingStyle", null, "http://schemas.xmlsoap.org/soap/encoding/");

                soapWriter.WriteStartElement("s", "Body", null);

                soapWriter.WriteStartElement("s", "Fault", null);

                soapWriter.WriteElementString("faultcode", "s:Client");
                soapWriter.WriteElementString("faultstring", "UPnPError");

                soapWriter.WriteStartElement("detail");

                soapWriter.WriteStartElement("UPnPError", "urn:schemas-upnp-org:control-1-0");
                soapWriter.WriteElementString("errorCode", errorCode.ToString());
                soapWriter.WriteElementString("errorDescription", errorDescription);

                soapWriter.WriteEndElement();

                soapWriter.WriteEndElement();

                soapWriter.WriteEndElement();

                soapWriter.WriteEndElement();

                soapWriter.WriteEndElement();

                soapWriter.Flush();
                memStream.Position = 0;

                AddHreader(HttpHeader.ContentLength, memStream.Length.ToString());
                AddHreader(HttpHeader.ContentType, "text/xml; charset=\"utf-8\"");
                SetStateCode(500);

                SendHeaders();
                memStream.CopyTo(GetStream());
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
                case 403: return "Forbidden";
                case 404: return "Not Found";
                case 406: return "Not Acceptable";
                case 500: return "Internal Server Error";
            }

            return string.Empty;
        }

        public Stream GetStream()
        {
            return this.chunkedStream == null ? (Stream)this.stream : (Stream)this.chunkedStream;
        }

        public void CloseStream()
        {
            GetStream().Close();
        }
    }
}
