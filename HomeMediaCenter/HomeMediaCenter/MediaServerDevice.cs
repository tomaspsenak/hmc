using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using System.Reflection;

namespace HomeMediaCenter
{
    public class MediaServerDevice : UpnpDevice
    {
        private bool minimizeToTray;
        private ItemManager itemManager = new ItemManager();
        private UpnpControlPoint controlPoint = new UpnpControlPoint();
        private enum WebContainer { WEBM, WMV, FLV };

        public MediaServerDevice()
        {
            Version ver = Assembly.GetExecutingAssembly().GetName().Version;

            this.udn = Guid.NewGuid();
            this.friendlyName = "Home Media Center";
            this.deviceType = "urn:schemas-upnp-org:device:MediaServer:1";
            this.manufacturer = "Tomáš Pšenák";
            this.modelName = "Home Media Center Server";
            this.modelNumber = ver.Major + "." + ver.Minor;
            this.serialNumber = this.udn.ToString();
            this.modelUrl = "http://hmc.codeplex.com";

            this.services.Add(new ConnectionManagerService(this, this.server));
            this.services.Add(new ContentDirectoryService(this, this.server));

            this.server.HttpServer.AddRoute("HEAD", "/Files/", new HttpRouteDelegate(GetFile));
            this.server.HttpServer.AddRoute("GET", "/Files/", new HttpRouteDelegate(GetFile));
            this.server.HttpServer.AddRoute("HEAD", "/Encode/", new HttpRouteDelegate(GetEncode));
            this.server.HttpServer.AddRoute("GET", "/Encode/", new HttpRouteDelegate(GetEncode));

            this.server.HttpServer.AddRoute("GET", "/Web/", new HttpRouteDelegate(GetWeb));
            this.server.HttpServer.AddRoute("GET", "/Web/player.html", new HttpRouteDelegate(GetWebPlayer));
            this.server.HttpServer.AddRoute("HEAD", "/Web/WebPlayer.xap", new HttpRouteDelegate(GetWebSilverlight));
            this.server.HttpServer.AddRoute("GET", "/Web/WebPlayer.xap", new HttpRouteDelegate(GetWebSilverlight));
            this.server.HttpServer.AddRoute("GET", "/Web/htmlstyle.css", new HttpRouteDelegate(GetWebStyle));
            this.server.HttpServer.AddRoute("GET", "/Web/jquery.lightbox-0.5.css", new HttpRouteDelegate(GetWebStyle));
            this.server.HttpServer.AddRoute("GET", "/Web/jquery-ui-1.8.18.custom.css", new HttpRouteDelegate(GetWebStyle));
            this.server.HttpServer.AddRoute("GET", "/Web/control.js", new HttpRouteDelegate(GetWebJavascript));
            this.server.HttpServer.AddRoute("GET", "/Web/player.js", new HttpRouteDelegate(GetWebJavascript));
            this.server.HttpServer.AddRoute("GET", "/Web/jquery-1.7.1.min.js", new HttpRouteDelegate(GetWebJavascript));
            this.server.HttpServer.AddRoute("GET", "/Web/jquery.lightbox-0.5.min.js", new HttpRouteDelegate(GetWebJavascript));
            this.server.HttpServer.AddRoute("GET", "/Web/jquery-ui-1.8.18.custom.min.js", new HttpRouteDelegate(GetWebJavascript));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/htmllogo.png", new HttpRouteDelegate(GetWebPng));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/htmlarrow.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/htmlorangearrow.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/htmlleftcontent.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/htmlgreenbox.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/htmlrightnav.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/htmlrightnava.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/lightbox-blank.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/lightbox-btn-close.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/lightbox-btn-next.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/lightbox-btn-prev.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/lightbox-ico-loading.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/loading.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/flowplayer.min.js", new HttpRouteDelegate(GetWebJavascript));
            this.server.HttpServer.AddRoute("HEAD", "/Web/flowplayer.swf", new HttpRouteDelegate(GetWebShockwave));
            this.server.HttpServer.AddRoute("GET", "/Web/flowplayer.swf", new HttpRouteDelegate(GetWebShockwave));
            this.server.HttpServer.AddRoute("HEAD", "/Web/flowplayer.controls.swf", new HttpRouteDelegate(GetWebShockwave));
            this.server.HttpServer.AddRoute("GET", "/Web/flowplayer.controls.swf", new HttpRouteDelegate(GetWebShockwave));
            this.server.HttpServer.AddRoute("POST", "/Web/devices.xml", new HttpRouteDelegate(GetWebDevices));
            this.server.HttpServer.AddRoute("POST", "/Web/control", new HttpRouteDelegate(PostWebControl));
        }

        public ItemManager ItemManager
        {
            get { return this.itemManager; }
        }

        public bool MinimizeToTray
        {
            get { return this.minimizeToTray; }
            set { this.minimizeToTray = value; }
        }

        public void LoadSettings(string settingsPath, string databasePath)
        {
            if (File.Exists(databasePath))
            {
                using (FileStream stream = new FileStream(databasePath, FileMode.Open, FileAccess.Read))
                {
                    BinaryFormatter binFormat = new BinaryFormatter();
                    this.itemManager = (ItemManager)binFormat.Deserialize(stream);
                }
            }

            if (File.Exists(settingsPath))
            {
                using (FileStream file = new FileStream(settingsPath, FileMode.Open, FileAccess.Read))
                {
                    XmlDocument xmlReader = new XmlDocument();
                    xmlReader.Load(file);

                    XmlNode node = xmlReader.SelectSingleNode("/HomeMediaCenter/@ModelNumber");
                    if (node == null || node.Value != this.modelNumber)
                        return;

                    Guid udn;
                    node = xmlReader.SelectSingleNode("/HomeMediaCenter/Udn");
                    if (node != null && Guid.TryParse(node.InnerText, out udn))
                    {
                        this.udn = udn;
                        this.serialNumber = udn.ToString();
                    }

                    node = xmlReader.SelectSingleNode("/HomeMediaCenter/FriendlyName");
                    if (node != null)
                    {
                        this.friendlyName = node.InnerText;
                    }

                    node = xmlReader.SelectSingleNode("/HomeMediaCenter/MinimizeToTray");
                    if (node != null)
                    {
                        bool.TryParse(node.InnerText, out this.minimizeToTray);
                    }

                    this.server.LoadSettings(xmlReader);

                    this.itemManager.LoadSettingsSync(xmlReader);
                }
            }
        }

        public void SaveSettings(string settingsPath, string databasePath)
        {
            using (FileStream file = new FileStream(settingsPath, FileMode.Create, FileAccess.Write))
            using (XmlWriter xmlWriter = XmlWriter.Create(file, new XmlWriterSettings() { Indent = true }))
            {
                xmlWriter.WriteStartDocument();
                xmlWriter.WriteStartElement("HomeMediaCenter");
                xmlWriter.WriteAttributeString("xmlns", "xsi", null, "http://www.w3.org/2001/XMLSchema-instance");
                xmlWriter.WriteAttributeString("xmlns", "xsd", null, "http://www.w3.org/2001/XMLSchema");
                xmlWriter.WriteAttributeString("ModelNumber", this.modelNumber);

                xmlWriter.WriteElementString("Udn", this.udn.ToString());
                xmlWriter.WriteElementString("FriendlyName", this.friendlyName);
                xmlWriter.WriteElementString("MinimizeToTray", this.minimizeToTray.ToString());

                this.server.SaveSettings(xmlWriter);

                this.itemManager.SaveSettingsSync(xmlWriter);

                xmlWriter.WriteEndElement();
                xmlWriter.WriteEndDocument();
            }

            this.itemManager.SerializeDatabaseSync(databasePath);
        }

        protected override void WriteSpecificDescription(XmlTextWriter descWriter)
        {
            descWriter.WriteElementString("presentationURL", "/web/page.html?id=0");
            descWriter.WriteElementString("dlna", "X_DLNADOC", "urn:schemas-dlna-org:device-1-0", "DMS-1.50");

            descWriter.WriteStartElement("iconList");

            foreach (string size in new[] { "120", "48", "32" })
            {
                descWriter.WriteStartElement("icon");
                descWriter.WriteElementString("mimetype", "image/jpeg");
                descWriter.WriteElementString("width", size);
                descWriter.WriteElementString("height", size);
                descWriter.WriteElementString("depth", "24");
                descWriter.WriteElementString("url", string.Format("/web/images/htmllogo.png?codec=jpeg&width={0}&height={0}", size));
                descWriter.WriteEndElement();
            }

            descWriter.WriteEndElement();
        }

        private void GetFile(HttpRequest request)
        {
            HttpResponse response = new HttpResponse(request);

            if (!request.UrlParams.ContainsKey("id"))
                throw new HttpException(404, "Unknown source");

            uint index;
            string path, mime, fileFeature;
            if (!uint.TryParse(request.UrlParams["id"], out index) || !this.ItemManager.GetFileSync(index, out path, out mime, out fileFeature))
                throw new HttpException(404, "Bad parameter");

            if (!File.Exists(path))
                throw new HttpException(404, "File not found");

            using (FileStream fs = new FileStream(path, FileMode.Open, FileAccess.Read))
            {
                long count;
                if (request.Headers.ContainsKey("range"))
                {
                    string[] range = request.Headers["range"].Split('=').Last().Split('-').Select(a => a.Trim()).ToArray();
                    long end, start = long.Parse(range[0]);
                    if (start < 0)
                        start += fs.Length;
                    if (range.Length < 2 || !long.TryParse(range[1], out end))
                        end = fs.Length - 1;

                    count = end - start + 1;
                    fs.Position = start;

                    response.SetStateCode(206);
                    response.AddHreader("Content-Range", string.Format("bytes {0}-{1}/{2}", start, end, fs.Length));
                }
                else
                {
                    count = fs.Length;
                }

                response.AddHreader(HttpHeader.ContentLength, count.ToString());
                response.AddHreader(HttpHeader.ContentType, mime.ToString());
                response.AddHreader(HttpHeader.AcceptRanges, "bytes");

                //Nastavenie DLNA hlaviciek - nastavenie feature aby bolo mozne seekovanie pomocu content-range
                response.AddHreader("TransferMode.DLNA.ORG", "Streaming");
                response.AddHreader("ContentFeatures.DLNA.ORG", fileFeature);
                response.SendHeaders();

                if (request.Method == "GET")
                {
                    OnLogEvent(string.Format("Uploading file {0} as {1}", path, mime));

                    int readed;
                    byte[] buffer = new byte[4096];
                    Stream responseStream = request.Socket.GetStream();

                    while ((readed = fs.Read(buffer, 0, buffer.Length)) > 0)
                    {
                        readed = (int)Math.Min(readed, count);
                        responseStream.Write(buffer, 0, readed);
                        count -= readed;

                        if (count <= 0)
                            break;
                    }
                }
            }
        }

        private void GetEncode(HttpRequest request)
        {
            HttpResponse response = new HttpResponse(request);

            if (request.UrlParams.ContainsKey("source"))
            {
                //Ak je nastaveny source - id sa ignoruje
                if (request.UrlParams["source"].Contains('\\'))
                    throw new HttpException(400, "Parameter contains invalid characters");
            }
            else if (request.UrlParams.ContainsKey("id"))
            {
                //Zisti zdroj podla id
                uint index;
                TimeSpan? duration;
                string path, subtitlesPath, encodeFeature;

                if (!uint.TryParse(request.UrlParams["id"], out index) || 
                    !this.ItemManager.GetEncodeSync(index, out path, out subtitlesPath, out encodeFeature, out duration))
                    throw new HttpException(404, "Bad parameter");

                request.UrlParams["source"] = path;
                if (subtitlesPath != null)
                    request.UrlParams["subtitles"] = subtitlesPath;
                response.AddHreader("ContentFeatures.DLNA.ORG", encodeFeature);

                if (duration.HasValue)
                {
                    //Je mozne seekovanie pomocou casu
                    response.AddHreader("X-AvailableSeekRange", string.Format(System.Globalization.CultureInfo.InvariantCulture,
                        "1 npt=0-{0:0.000}", duration.Value.TotalSeconds));

                    if (request.Headers.ContainsKey("TimeSeekRange.DLNA.ORG"))
                    {
                        //Zistenie a nastavenie pozicie
                        string[] range = request.Headers["TimeSeekRange.DLNA.ORG"].Split('=').Last().Split('-').Select(a => a.Trim()).ToArray();
                        double end, start;

                        //Cas moze byt vo formate 0.000 sekundy alebo 0:00:00.0
                        if (!double.TryParse(range[0], System.Globalization.NumberStyles.Any, System.Globalization.CultureInfo.InvariantCulture, out start))
                            start = TimeSpan.Parse(range[0]).TotalSeconds;
                        if (start < 0)
                            start += duration.Value.TotalSeconds;

                        if (range.Length < 2)
                        {
                            end = duration.Value.TotalSeconds;
                        }
                        else
                        {
                            TimeSpan endSpan;
                            if (double.TryParse(range[1], System.Globalization.NumberStyles.Any, System.Globalization.CultureInfo.InvariantCulture, out end))
                                request.UrlParams["endtime"] = end.ToString(System.Globalization.CultureInfo.InvariantCulture);
                            else if (TimeSpan.TryParse(range[1], out endSpan))
                                request.UrlParams["endtime"] = endSpan.TotalSeconds.ToString(System.Globalization.CultureInfo.InvariantCulture);
                            else
                                end = duration.Value.TotalSeconds;
                        }

                        request.UrlParams["starttime"] = start.ToString(System.Globalization.CultureInfo.InvariantCulture);

                        response.AddHreader("Vary", "TimeSeekRange.DLNA.ORG");
                        response.AddHreader("TimeSeekRange.DLNA.ORG", string.Format(System.Globalization.CultureInfo.InvariantCulture,
                            "npt={0:0.000}-{1:0.000}/{2:0.000}", start, end, duration.Value.TotalSeconds));
                    }
                }
            }
            else
                throw new HttpException(404, "Unknown source");
            
            EncoderBuilder builder = EncoderBuilder.GetEncoder(request.UrlParams);

            response.AddHreader(HttpHeader.ContentType, builder.GetMime());
            response.AddHreader(HttpHeader.AcceptRanges, "none");
            response.AddHreader("TransferMode.DLNA.ORG", "Streaming");
            response.SendHeaders();

            if (request.Method == "GET")
            {
                OnLogEvent(string.Format("Starting encode {0} as {1}", request.UrlParams["source"], builder.GetMime()));
                builder.StartEncode(request.Socket.GetStream());
            }
        }

        private void GetWeb(HttpRequest request)
        {
            HttpResponse response = new HttpResponse(request);

            uint id;
            if (!request.UrlParams.ContainsKey("id") || !uint.TryParse(request.UrlParams["id"], out id))
                id = 0;

            using (MemoryStream stream = new MemoryStream())
            using (XmlWriter xmlWriter = XmlWriter.Create(stream, new XmlWriterSettings() { Indent = true, Encoding = new UTF8Encoding(false) }))
            {
                xmlWriter.WriteStartDocument();
                xmlWriter.WriteDocType("html", "-//W3C//DTD XHTML 1.0 Transitional//EN", "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd", null);
                xmlWriter.WriteStartElement("html", "http://www.w3.org/1999/xhtml");

                xmlWriter.WriteStartElement("head");
                xmlWriter.WriteElementString("title", "Home Media Center");
                xmlWriter.WriteRaw(@"<link rel=""stylesheet"" type=""text/css"" href=""/web/htmlstyle.css"" />");
                xmlWriter.WriteRaw(@"<link rel=""stylesheet"" type=""text/css"" href=""/web/jquery.lightbox-0.5.css"" />");
                xmlWriter.WriteRaw(@"<script type=""text/javascript"" src=""/web/jquery-1.7.1.min.js""></script>");
                xmlWriter.WriteRaw(@"<script type=""text/javascript"" src=""/web/jquery.lightbox-0.5.min.js""></script>");
                xmlWriter.WriteEndElement();

                xmlWriter.WriteStartElement("body");

                xmlWriter.WriteStartElement("div");
                xmlWriter.WriteAttributeString("id", "main_container");

                xmlWriter.WriteStartElement("div");
                xmlWriter.WriteAttributeString("id", "header");

                xmlWriter.WriteRaw(@"<div id=""logo""><a href=""/web/page.html?id=0""><img src=""/web/images/htmllogo.png"" alt="""" title="""" border=""0"" width=""60"" height=""60"" /></a></div>");
                xmlWriter.WriteRaw(@"<h2>Home Media Center</h2>");

                xmlWriter.WriteStartElement("div");
                xmlWriter.WriteAttributeString("id", "menu");
                xmlWriter.WriteStartElement("ul");
                xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/page.html?id=0"" title="""">{0}</a></li>", LanguageResource.Home));
                xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/page.html?id=2"" title="""">{0}</a></li>", LanguageResource.Photos));
                xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/page.html?id=1"" title="""">{0}</a></li>", LanguageResource.Music));
                xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/page.html?id=3"" title="""">{0}</a></li>", LanguageResource.Video));
                xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/page.html?id=4"" title="""">{0}</a></li>", LanguageResource.Stream));
                xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/control.html"" title="""">{0}</a></li>", LanguageResource.Control));
                xmlWriter.WriteEndElement();
                xmlWriter.WriteEndElement();

                xmlWriter.WriteEndElement();

                if (request.Url.Contains("control.html"))
                {
                    xmlWriter.WriteStartElement("div");
                    xmlWriter.WriteAttributeString("id", "main_content");

                    xmlWriter.WriteStartElement("div");
                    xmlWriter.WriteAttributeString("id", "left_content");

                    xmlWriter.WriteRaw(@"<script type=""text/javascript"" src=""/web/control.js""></script>");

                    xmlWriter.WriteRaw(string.Format("<h2>DLNA {0}</h2>", LanguageResource.Device));

                    xmlWriter.WriteStartElement("select");
                    xmlWriter.WriteAttributeString("id", "devices");
                    xmlWriter.WriteAttributeString("name", "devices");
                    xmlWriter.WriteAttributeString("style", "margin-right:10px;");

                    xmlWriter.WriteStartElement("option");
                    xmlWriter.WriteAttributeString("value", "none");
                    xmlWriter.WriteValue(LanguageResource.SelectDevice);
                    xmlWriter.WriteEndElement();

                    this.controlPoint.WriteXml(xmlWriter);

                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteRaw(string.Format(@"<button id=""refreshBtn"" type=""button"">{0}</button>", LanguageResource.Refresh));
                    xmlWriter.WriteRaw(@"<img id=""loadingImg"" src=""/web/images/loading.gif"" alt="""" title="""" border=""0"" style=""display:none;"" />");

                    //Koniec left_content
                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteStartElement("div");
                    xmlWriter.WriteAttributeString("id", "right_content");

                    if (id == 0)
                    {
                        xmlWriter.WriteRaw(string.Format(@"<p>{0}: {1}</p>", LanguageResource.Item, LanguageResource.NotSelected));
                    }
                    else
                    {
                        xmlWriter.WriteRaw(string.Format(@"<input id=""idInput"" type=""hidden"" value=""{0}""></input>", id));
                        xmlWriter.WriteRaw(string.Format(@"<p>{0}: {1}</p>", LanguageResource.Item, this.itemManager.GetTitleSync(id)));
                    }

                    xmlWriter.WriteRaw(string.Format(@"<button id=""volPBtn"" type=""button"">{0} +</button>", LanguageResource.Volume));
                    xmlWriter.WriteRaw(string.Format(@"<button id=""volMBtn"" type=""button"">{0} -</button>", LanguageResource.Volume));
                    xmlWriter.WriteRaw(string.Format(@"<button id=""playBtn"" type=""button"">{0}</button>", LanguageResource.Play));
                    xmlWriter.WriteRaw(string.Format(@"<button id=""stopBtn"" type=""button"">{0}</button>", LanguageResource.Stop));

                    //Koniec right_content
                    xmlWriter.WriteEndElement();

                    //Koniec main_content
                    xmlWriter.WriteEndElement();
                }
                else
                {
                    this.itemManager.GetWebSync(id, xmlWriter);
                }

                xmlWriter.WriteRaw(@"<div id=""footer""><div class=""copyright"">Tomáš Pšenák</div></div>");

                //Koniec main_container
                xmlWriter.WriteEndElement();
                //Koniec body
                xmlWriter.WriteEndElement();
                //Koniec html
                xmlWriter.WriteEndElement();

                xmlWriter.Flush();
                stream.Position = 0;

                response.AddHreader(HttpHeader.ContentLength, stream.Length.ToString());
                response.AddHreader(HttpHeader.ContentType, "text/html; charset=utf-8");
                response.SendHeaders();

                stream.CopyTo(request.Socket.GetStream());
            }
        }

        private void GetWebPlayer(HttpRequest request)
        {
            HttpResponse response = new HttpResponse(request);

            uint index;
            TimeSpan? duration;
            string path, subtitlesPath, encodeFeature;

            if (!request.UrlParams.ContainsKey("id") || !uint.TryParse(request.UrlParams["id"], out index) ||
                !this.itemManager.GetEncodeSync(index, out path, out subtitlesPath, out encodeFeature, out duration))
            {
                index = 0;
                duration = null;
                path = null;
                encodeFeature = null;
            }

            string width = "800", height = "480";
            if (request.UrlParams.ContainsKey("resolution"))
            {
                string[] res = request.UrlParams["resolution"].Split(new char[] { 'x' }, StringSplitOptions.RemoveEmptyEntries);
                if (res.Length == 2)
                {
                    width = res[0];
                    height = res[1];
                }
            }

            string quality = "medium";
            if (request.UrlParams.ContainsKey("quality"))
                quality = request.UrlParams["quality"].ToLower();

            using (MemoryStream stream = new MemoryStream())
            using (XmlWriter xmlWriter = XmlWriter.Create(stream, new XmlWriterSettings() { Indent = true, Encoding = new UTF8Encoding(false) }))
            {
                xmlWriter.WriteStartDocument();
                xmlWriter.WriteDocType("html", "-//W3C//DTD XHTML 1.0 Transitional//EN", "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd", null);
                xmlWriter.WriteStartElement("html", "http://www.w3.org/1999/xhtml");

                xmlWriter.WriteStartElement("head");
                xmlWriter.WriteElementString("title", "Home Media Center - Player");
                xmlWriter.WriteRaw(@"<link rel=""stylesheet"" type=""text/css"" href=""/web/htmlstyle.css"" />");
                xmlWriter.WriteRaw(@"<link rel=""stylesheet"" type=""text/css"" href=""/web/jquery.lightbox-0.5.css"" />");
                xmlWriter.WriteRaw(@"<link rel=""stylesheet"" type=""text/css"" href=""/web/jquery-ui-1.8.18.custom.css"" />");
                xmlWriter.WriteRaw(@"<script type=""text/javascript"" src=""/web/jquery-1.7.1.min.js""></script>");
                xmlWriter.WriteRaw(@"<script type=""text/javascript"" src=""/web/jquery.lightbox-0.5.min.js""></script>");
                xmlWriter.WriteRaw(@"<script type=""text/javascript"" src=""/web/jquery-ui-1.8.18.custom.min.js""></script>");
                xmlWriter.WriteRaw(@"<script type=""text/javascript"" src=""/web/flowplayer.min.js""></script>");
                xmlWriter.WriteRaw(@"<script type=""text/javascript"" src=""/web/player.js""></script>");
                xmlWriter.WriteEndElement();

                xmlWriter.WriteStartElement("body");

                xmlWriter.WriteStartElement("div");
                xmlWriter.WriteAttributeString("id", "main_container");

                xmlWriter.WriteStartElement("div");
                xmlWriter.WriteAttributeString("id", "header");

                xmlWriter.WriteRaw(@"<div id=""logo""><a href=""/web/page.html?id=0""><img src=""/web/images/htmllogo.png"" alt="""" title="""" border=""0"" width=""60"" height=""60"" /></a></div>");
                xmlWriter.WriteRaw(@"<h2>Home Media Center</h2>");

                xmlWriter.WriteEndElement();

                xmlWriter.WriteStartElement("div");
                xmlWriter.WriteAttributeString("id", "main_content");

                WebContainer container;
                if (!request.UrlParams.ContainsKey("codec") || !Enum.TryParse<WebContainer>(request.UrlParams["codec"], true, out container))
                    container = WebContainer.WEBM;

                if (container == WebContainer.WMV)
                {
                    string sourceUrl = string.Format("/encode/web?id={0}&codec=wmv2&width={1}&height={2}", index, width, height);
                    switch (quality)
                    {
                        case "low": sourceUrl += "&vidbitrate=200&audbitrate=40&quality=1&fps=25"; break;
                        case "medium": sourceUrl += "&vidbitrate=400&audbitrate=40&quality=1&fps=25"; break;
                        case "high": sourceUrl += "&vidbitrate=800&audbitrate=80&quality=51&fps=30"; break;
                    }

                    xmlWriter.WriteRaw(@"<div>");
                    xmlWriter.WriteRaw(@"<object id=""silverlightControlHost"" data=""data:application/x-silverlight-2,"" type=""application/x-silverlight-2"">");
                    xmlWriter.WriteRaw(@"<param name=""source"" value=""/web/webplayer.xap"" />");
                    xmlWriter.WriteRaw(@"<param name=""onError"" value=""onSilverlightError"" />");
                    xmlWriter.WriteRaw(@"<param name=""background"" value=""white"" />");
                    xmlWriter.WriteRaw(@"<param name=""minRuntimeVersion"" value=""4.0.60310.0"" />");
                    xmlWriter.WriteRaw(@"<param name=""autoUpgrade"" value=""true"" />");
                    xmlWriter.WriteRaw(@"<param name=""onLoad"" value=""silverlightControlLoaded"" />");

                    xmlWriter.WriteStartElement("span");
                    xmlWriter.WriteValue(LanguageResource.Install + " ");

                    xmlWriter.WriteStartElement("a");
                    xmlWriter.WriteAttributeString("class", "videoLink");
                    xmlWriter.WriteAttributeString("href", "http://go.microsoft.com/fwlink/?LinkID=149156&v=4.0.60310.0");
                    xmlWriter.WriteValue("Silverlight");
                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteValue(string.Format(" {0} ", LanguageResource.Or));

                    xmlWriter.WriteStartElement("a");
                    xmlWriter.WriteAttributeString("href", sourceUrl);
                    xmlWriter.WriteAttributeString("id", "videoLink");
                    xmlWriter.WriteAttributeString("class", "videoLink");
                    xmlWriter.WriteValue(LanguageResource.Play);
                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteValue(" " + LanguageResource.ThroughDirectLink);
                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteRaw(@"</object>");
                    xmlWriter.WriteRaw(@"</div>");
                }
                else if (container == WebContainer.WEBM)
                {
                    string sourceUrl = string.Format("/encode/web?id={0}&codec=webm_ts&width={1}&height={2}", index, width, height);
                    switch (quality)
                    {
                        case "low": sourceUrl += "&vidbitrate=200&audbitrate=64&quality=1"; break;
                        case "medium": sourceUrl += "&vidbitrate=400&audbitrate=64&quality=1"; break;
                        case "high": sourceUrl += "&vidbitrate=800&audbitrate=128&quality=51"; break;
                    }

                    xmlWriter.WriteRaw(@"<video id=""streamVideo"" autoplay=""autoplay"" />");

                    xmlWriter.WriteStartElement("source");
                    xmlWriter.WriteAttributeString("src", sourceUrl);
                    xmlWriter.WriteAttributeString("type", "video/webm; codecs=\"vp8.0, vorbis\"");
                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteStartElement("span");
                    xmlWriter.WriteAttributeString("id", "streamVideoSpan");

                    xmlWriter.WriteStartElement("a");
                    xmlWriter.WriteAttributeString("href", sourceUrl);
                    xmlWriter.WriteAttributeString("id", "videoLink");
                    xmlWriter.WriteAttributeString("class", "videoLink");
                    xmlWriter.WriteValue(LanguageResource.Play);
                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteValue(" " + LanguageResource.ThroughDirectLink);
                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteRaw(@"</video>");
                }
                else
                {
                    string sourceUrl = string.Format("/encode/web%3Fid={0}&codec=flv_ts&width={1}&height={2}", index, width, height);
                    switch (quality)
                    {
                        case "low": sourceUrl += "&vidbitrate=200&audbitrate=64&quality=1&fps=25"; break;
                        case "medium": sourceUrl += "&vidbitrate=700&audbitrate=128&quality=1&fps=25"; break;
                        case "high": sourceUrl += "&vidbitrate=1500&audbitrate=256&quality=51&fps=30"; break;
                    }
                    sourceUrl = sourceUrl.Replace("=", "%3D").Replace("&", "%26");

                    xmlWriter.WriteStartElement("a");
                    xmlWriter.WriteAttributeString("id", "videoLink");
                    xmlWriter.WriteAttributeString("href", sourceUrl);
                    xmlWriter.WriteAttributeString("style", string.Format("display:block;margin-left:auto;margin-right:auto;width:{0}px;height:{1}px;", width, height));
                    xmlWriter.WriteValue(LanguageResource.Play + " " + LanguageResource.ThroughDirectLink);
                    xmlWriter.WriteEndElement();
                }

                xmlWriter.WriteStartElement("form");
                xmlWriter.WriteAttributeString("id", "mainForm");
                xmlWriter.WriteAttributeString("action", "/Web/player.html");
                xmlWriter.WriteAttributeString("method", "get");
                xmlWriter.WriteAttributeString("enctype", "application/x-www-form-urlencoded");

                xmlWriter.WriteRaw(string.Format(@"<input type=""hidden"" name=""id"" value=""{0}"" />", index));

                xmlWriter.WriteRaw(@"<span id=""streamPos"">0:00:00</span>");
                xmlWriter.WriteRaw(@"<div id=""seekSlider""></div>");
                xmlWriter.WriteRaw(string.Format(@"<span id=""streamLength"">{0}</span>", 
                    duration.HasValue ? duration.Value.ToString("h':'mm':'ss") : "0:00:00"));
                xmlWriter.WriteRaw(@"<div id=""volumeSlider""></div>");
                xmlWriter.WriteRaw(@"<span id=""volumeVal"">100%</span>");

                xmlWriter.WriteRaw(@"<div id=""streamToolbar"" class=""ui-widget-header ui-corner-all"">");
                xmlWriter.WriteRaw(string.Format(@"<button type=""button"" id=""playButton"">{0}</button>", LanguageResource.Play));
                xmlWriter.WriteRaw(string.Format(@"<button type=""button"" id=""pauseButton"">{0}</button>", LanguageResource.Pause));

                xmlWriter.WriteRaw(@"<span id=""codecRadios"">");
                xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""webmRadio"" name=""codec"" value=""webm"" {0} /><label for=""webmRadio"">WebM</label>",
                    container == WebContainer.WEBM ? "checked=\"checked\"" : string.Empty));
                xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""wmvRadio"" name=""codec"" value=""wmv"" {0} /><label for=""wmvRadio"">Wmv</label>",
                    container == WebContainer.WMV ? "checked=\"checked\"" : string.Empty));
                xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""flvRadio"" name=""codec"" value=""flv"" {0} /><label for=""flvRadio"">Flash</label>",
                    container == WebContainer.FLV ? "checked=\"checked\"" : string.Empty));
                xmlWriter.WriteRaw(@"</span>");

                string res = width + "x" + height;
                xmlWriter.WriteRaw(@"<span id=""resolutionRadios"">");
                xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""r1280x720Radio"" name=""resolution"" value=""1280x720"" {0} /><label for=""r1280x720Radio"">1280x720</label>",
                    res == "1280x720" ? "checked=\"checked\"" : string.Empty));
                xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""r800x480Radio"" name=""resolution"" value=""800x480"" {0} /><label for=""r800x480Radio"">800x480</label>",
                    res == "800x480" ? "checked=\"checked\"" : string.Empty));
                xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""r640x480Radio"" name=""resolution"" value=""640x480"" {0} /><label for=""r640x480Radio"">640x480</label>",
                    res == "640x480" ? "checked=\"checked\"" : string.Empty));
                xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""r320x240Radio"" name=""resolution"" value=""320x240"" {0} /><label for=""r320x240Radio"">320x240</label>",
                    res == "320x240" ? "checked=\"checked\"" : string.Empty));
                xmlWriter.WriteRaw(@"</span>");

                xmlWriter.WriteRaw(@"<span id=""qualityRadios"">");
                xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""lowRadio"" name=""quality"" value=""low"" {0} /><label for=""lowRadio"">{1}</label>",
                    quality == "low" ? "checked=\"checked\"" : string.Empty, LanguageResource.Low));
                xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""mediumRadio"" name=""quality"" value=""medium"" {0} /><label for=""mediumRadio"">{1}</label>",
                    quality == "medium" ? "checked=\"checked\"" : string.Empty, LanguageResource.Medium));
                xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""highRadio"" name=""quality"" value=""high"" {0} /><label for=""highRadio"">{1}</label>",
                    quality == "high" ? "checked=\"checked\"" : string.Empty, LanguageResource.High));
                xmlWriter.WriteRaw(@"</span>");

                xmlWriter.WriteRaw(@"<button id=""submitButton"" type=""submit"">Ok</button>");
                xmlWriter.WriteRaw(@"</div>");

                //Koniec form
                xmlWriter.WriteEndElement();

                //Koniec main_content
                xmlWriter.WriteEndElement();

                xmlWriter.WriteRaw(@"<div id=""footer""><div class=""copyright"">Tomáš Pšenák</div></div>");

                //Koniec main_container
                xmlWriter.WriteEndElement();
                //Koniec body
                xmlWriter.WriteEndElement();
                //Koniec html
                xmlWriter.WriteEndElement();

                xmlWriter.Flush();
                stream.Position = 0;

                response.AddHreader(HttpHeader.ContentLength, stream.Length.ToString());
                response.AddHreader(HttpHeader.ContentType, "text/html; charset=utf-8");
                response.SendHeaders();

                stream.CopyTo(request.Socket.GetStream());
            }
        }

        private void GetWebStyle(HttpRequest request)
        {
            SendResource(request, "text/css");
        }

        private void GetWebGif(HttpRequest request)
        {
            SendResource(request, "image/gif", request.UrlParams.ContainsKey("codec"));
        }

        private void GetWebPng(HttpRequest request)
        {
            SendResource(request, "image/png", request.UrlParams.ContainsKey("codec"));
        }

        private void GetWebJavascript(HttpRequest request)
        {
            SendResource(request, "text/javascript");
        }

        private void GetWebSilverlight(HttpRequest request)
        {
            SendResource(request, "application/x-silverlight-app");
        }

        private void GetWebShockwave(HttpRequest request)
        {
            SendResource(request, "application/x-shockwave-flash");
        }

        private void SendResource(HttpRequest request, string mime, bool encode = false)
        {
            HttpResponse response = new HttpResponse(request);

            string name = request.Url.Split(new char[] { '/' }, StringSplitOptions.RemoveEmptyEntries).Last();

            using (Stream resourceStream = Assembly.GetExecutingAssembly().GetManifestResourceStream("HomeMediaCenter.Resources." + name))
            {
                if (encode)
                {
                    EncoderBuilder builder = EncoderBuilder.GetEncoder(request.UrlParams);

                    using (MemoryStream memStream = new MemoryStream())
                    {
                        builder.StartEncode(resourceStream, memStream);

                        response.AddHreader(HttpHeader.ContentLength, memStream.Length.ToString());
                        response.AddHreader(HttpHeader.ContentType, builder.GetMime());
                        response.SendHeaders();

                        if (request.Method == "GET")
                        {
                            memStream.Position = 0;
                            memStream.CopyTo(request.Socket.GetStream());
                        }
                    }
                }
                else
                {
                    response.AddHreader(HttpHeader.ContentLength, resourceStream.Length.ToString());
                    response.AddHreader(HttpHeader.ContentType, mime);
                    response.SendHeaders();

                    if (request.Method == "GET")
                    {
                        resourceStream.CopyTo(request.Socket.GetStream());
                    }
                }
            }
        }

        private void GetWebDevices(HttpRequest request)
        {
            HttpResponse response = new HttpResponse(request);

            string type;
            if (request.UrlParams.ContainsKey("type"))
                type = request.UrlParams["type"];
            else
                type = "upnp:rootdevice";

            if (request.UrlParams.ContainsKey("refresh"))
            {
                bool refresh = false;
                bool.TryParse(request.UrlParams["refresh"], out refresh);
                if (refresh)
                    this.controlPoint.Refresh(type);
            }

            using (MemoryStream stream = new MemoryStream())
            using (XmlWriter xmlWriter = XmlWriter.Create(stream, new XmlWriterSettings() { Indent = true }))
            {
                xmlWriter.WriteStartDocument();

                xmlWriter.WriteStartElement("devices");
                this.controlPoint.WriteXml(xmlWriter);
                xmlWriter.WriteEndElement();

                xmlWriter.Flush();
                stream.Position = 0;

                response.AddHreader(HttpHeader.ContentLength, stream.Length.ToString());
                response.AddHreader(HttpHeader.ContentType, "text/xml; charset=utf-8");
                response.SendHeaders();

                stream.CopyTo(request.Socket.GetStream());
            }
        }

        private void PostWebControl(HttpRequest request)
        {
            string action = request.UrlParams["action"].ToLower();
            string device = request.UrlParams["device"];

            switch (action)
            {
                case "volp": this.controlPoint.VolumePlus(device);
                    break;
                case "volm": this.controlPoint.VolumeMinus(device);
                    break;
                case "play": this.controlPoint.Play(device, this.server.HttpPort, uint.Parse(request.UrlParams["id"]), this.itemManager);
                    break;
                case "stop": this.controlPoint.Stop(device);
                    break;
            }

            HttpResponse response = new HttpResponse(request);
            response.SendHeaders();
        }
    }
}
