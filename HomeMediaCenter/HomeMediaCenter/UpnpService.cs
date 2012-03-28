using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Reflection;
using System.Net;

namespace HomeMediaCenter
{
    public abstract class UpnpService
    {
        protected readonly UpnpServer server;

        protected readonly string serviceType;
        protected readonly string serviceId;
        protected readonly string controlUrl;
        protected readonly string eventSubUrl;
        protected readonly string SCPDURL;

        private byte[] descArray;

        private delegate void SendEventDel(string sid, Uri uri);

        public UpnpService(UpnpServer server, string serviceType, string serviceId, string controlUrl, string eventSubUrl, string SCPDURL)
        {
            this.server = server;

            this.serviceType = serviceType;
            this.serviceId = serviceId;
            this.controlUrl = controlUrl;
            this.eventSubUrl = eventSubUrl;
            this.SCPDURL = SCPDURL;

            MemoryStream memStream = new MemoryStream();
            using (XmlTextWriter descWriter = new XmlTextWriter(memStream, new UTF8Encoding(false)))
            {
                descWriter.Formatting = Formatting.Indented;
                descWriter.WriteRaw("<?xml version=\"1.0\"?>");

                descWriter.WriteStartElement("scpd", "urn:schemas-upnp-org:service-1-0");

                descWriter.WriteStartElement("specVersion");
                descWriter.WriteElementString("major", "1");
                descWriter.WriteElementString("minor", "0");
                descWriter.WriteEndElement();

                descWriter.WriteStartElement("actionList");

                MethodInfo[] methods = GetType().GetMethods(BindingFlags.Instance | BindingFlags.NonPublic);
                foreach (MethodInfo method in methods)
                {
                    IEnumerable<UpnpServiceArgument> methAttribs = method.GetCustomAttributes(typeof(UpnpServiceArgument), true).Cast<UpnpServiceArgument>();
                    ParameterInfo[] parameters = method.GetParameters();

                    if (methAttribs.Count() > 0 || parameters.Any(a => a.GetCustomAttributes(typeof(UpnpServiceArgument), true).Length > 0))
                    {
                        descWriter.WriteStartElement("action");
                        descWriter.WriteElementString("name", method.Name);
                        descWriter.WriteStartElement("argumentList");

                        //Zapisanie vstupnych parametrov
                        foreach (ParameterInfo parameter in parameters)
                        {
                            UpnpServiceArgument paramAttrib = parameter.GetCustomAttributes(typeof(UpnpServiceArgument), true).FirstOrDefault() as UpnpServiceArgument;
                            if (paramAttrib != null)
                            {
                                descWriter.WriteStartElement("argument");
                                descWriter.WriteElementString("name", parameter.Name);
                                descWriter.WriteElementString("direction", "in");
                                descWriter.WriteElementString("relatedStateVariable", paramAttrib.RelatedStateVariable);
                                descWriter.WriteEndElement();
                            }
                        }

                        //Zapisanie vystupnych parametrov
                        foreach (UpnpServiceArgument methAttrib in methAttribs.OrderBy(a => a.Index))
                        {
                            descWriter.WriteStartElement("argument");
                            descWriter.WriteElementString("name", methAttrib.Name);
                            descWriter.WriteElementString("direction", "out");
                            descWriter.WriteElementString("relatedStateVariable", methAttrib.RelatedStateVariable);
                            descWriter.WriteEndElement();
                        }

                        descWriter.WriteEndElement();
                        descWriter.WriteEndElement();
                    }
                }

                descWriter.WriteEndElement();
                descWriter.WriteStartElement("serviceStateTable");

                //Zapisanie premennych
                foreach (UpnpServiceVariable variable in GetType().GetCustomAttributes(typeof(UpnpServiceVariable), true).Cast<UpnpServiceVariable>())
                {
                    descWriter.WriteStartElement("stateVariable");
                    descWriter.WriteAttributeString("sendEvents", variable.SendEvents ? "yes" : "no");
                    descWriter.WriteElementString("name", variable.Name);
                    descWriter.WriteElementString("dataType", variable.DataType);
                    if (variable.AllowedValue.Length > 0)
                    {
                        descWriter.WriteStartElement("allowedValueList");
                        foreach (string value in variable.AllowedValue)
                            descWriter.WriteElementString("allowedValue", value);
                        descWriter.WriteEndElement();
                    }
                    descWriter.WriteEndElement();
                }

                descWriter.WriteEndElement();

                descWriter.WriteEndElement();

                descWriter.Flush();
                this.descArray = memStream.ToArray();
            }

            server.HttpServer.AddRoute("GET", SCPDURL, new HttpRouteDelegate(GetDescription));
            server.HttpServer.AddRoute("POST", controlUrl, new HttpRouteDelegate(ProceedControl));
            server.HttpServer.AddRoute("SUBSCRIBE", eventSubUrl, new HttpRouteDelegate(ProceedEventSub));
            server.HttpServer.AddRoute("UNSUBSCRIBE", eventSubUrl, new HttpRouteDelegate(ProceedEventUnsub));
        }

        public string ServiceType
        {
            get { return this.serviceType; }
        }

        public virtual void WriteDescription(XmlTextWriter descWriter)
        {
            descWriter.WriteElementString("serviceType", this.serviceType);

            descWriter.WriteElementString("serviceId", this.serviceId);

            descWriter.WriteElementString("controlURL", this.controlUrl);

            descWriter.WriteElementString("eventSubURL", this.eventSubUrl);

            descWriter.WriteElementString("SCPDURL", this.SCPDURL);
        }

        private void GetDescription(HttpRequest request)
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

        private void ProceedControl(HttpRequest request)
        {
            if (!request.Headers["SOAPACTION"].Trim('"').StartsWith(this.serviceType))
                throw new HttpException(400, "Service type mismatch");

            XmlDocument xDoc = new XmlDocument();
            using (MemoryStream stream = request.GetContent())
            {
                xDoc.Load(stream);

                XmlNamespaceManager namespaceManager = new System.Xml.XmlNamespaceManager(xDoc.NameTable);
                namespaceManager.AddNamespace("soapNam", "http://schemas.xmlsoap.org/soap/envelope/");

                XmlNode bodyNode = xDoc.SelectSingleNode("/soapNam:Envelope/soapNam:Body/*[1]", namespaceManager);

                this.server.RootDevice.OnLogEvent(string.Format("ACTION: {0}, {1}", bodyNode.LocalName, this.serviceType));

                MethodInfo method = GetType().GetMethod(bodyNode.LocalName, BindingFlags.Instance | BindingFlags.NonPublic);
                string[] outParam = method.GetCustomAttributes(typeof(UpnpServiceArgument), true).Cast<UpnpServiceArgument>().OrderBy(
                    a => a.Index).Select(a => a.Name).ToArray();
                ParameterInfo[] paramDef = method.GetParameters();

                request.SetSoap(bodyNode.LocalName, this.serviceType, outParam);

                object[] paramVal = new object[paramDef.Length];
                paramVal[0] = request;
                for (int i = 1; i < paramDef.Length; i++)
                {
                    XmlNode paramNode = bodyNode.SelectSingleNode(paramDef[i].Name);
                    if (paramNode != null)
                        paramVal[i] = paramNode.InnerXml;
                }

                try 
                { 
                    method.Invoke(this, paramVal); 
                }
                catch (TargetInvocationException ex)
                {
                    if (ex.InnerException != null)
                        throw ex.InnerException;
                    throw;
                }
            }
        }

        private void ProceedEventSub(HttpRequest request)
        {
            uint timeout = uint.Parse(request.Headers["TIMEOUT"].Split(new char[] { '-' }, StringSplitOptions.RemoveEmptyEntries).Last());

            string sid;
            if (request.Headers.ContainsKey("SID"))
            {
                sid = request.Headers["SID"];
            }
            else
            {
                sid = "uuid:" + Guid.NewGuid();
                Uri uri = new Uri(request.Headers["CALLBACK"].TrimStart('<').TrimEnd('>'));

                new SendEventDel(SendEvent).BeginInvoke(sid, uri, null, null);
            }

            HttpResponse response = new HttpResponse(request);
            response.AddHreader(HttpHeader.ContentLength, "0");
            response.AddHreader("SID", sid);
            response.AddHreader("TIMEOUT", "Second-" + timeout);

            response.SendHeaders();
        }

        private void ProceedEventUnsub(HttpRequest request)
        {
            HttpResponse response = new HttpResponse(request);
            response.SendHeaders();
        }

        private void SendEvent(string sid, Uri uri)
        {
            StringBuilder content = new StringBuilder();
            using (XmlWriter writer = XmlWriter.Create(content, new XmlWriterSettings() { OmitXmlDeclaration = true }))
            {
                writer.WriteRaw(@"<?xml version=""1.0""?>");
                writer.WriteStartElement("e", "propertyset", "urn:schemas-upnp-org:event-1-0");
                WriteEventProp(writer);
                writer.WriteEndElement();
            }
            byte[] contentBytes = Encoding.UTF8.GetBytes(content.ToString());

            WebRequest request = WebRequest.Create(uri);
            request.Method = "NOTIFY";
            request.ContentType = "text/xml; charset=\"utf-8\"";
            request.ContentLength = contentBytes.Length;
            request.Headers.Add("NT", "upnp:event");
            request.Headers.Add("NTS", "upnp:propchange");
            request.Headers.Add("SID", sid);
            request.Headers.Add("SEQ", "0");

            //Proxy null zrychluje spracovanie
            request.Proxy = null;

            System.Threading.Thread.Sleep(1000);

            try
            {
                Stream dataStream = request.GetRequestStream();
                dataStream.WriteTimeout = 20000;
                dataStream.Write(contentBytes, 0, contentBytes.Length);
                dataStream.Close();
                request.GetResponse().GetResponseStream().Close();
            }
            catch { }
        }

        protected abstract void WriteEventProp(XmlWriter writer);
    }
}
