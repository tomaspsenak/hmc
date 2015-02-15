using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Reflection;

namespace HomeMediaCenter
{
    public class MediaServerDevice : UpnpDevice
    {
        private bool minimizeToTray;
        private bool tryToForwardPort;
        private bool generateThumbnails;
        private List<Guid> prefDsDemux;
        private ItemManager itemManager;
        private UpnpControlPoint controlPoint;

        private readonly string dataDirectory;
        private readonly string settingsPath;
        private readonly string databasePath;
        private readonly string thumbnailsPath;

        private readonly HlsManager hlsManager;

        public MediaServerDevice(string dataDirectory)
        {
            this.dataDirectory = dataDirectory;
            this.settingsPath = Path.Combine(dataDirectory, "settings.xml");
            this.databasePath = Path.Combine(dataDirectory, "database.db");
            this.thumbnailsPath = Path.Combine(dataDirectory, "thumbnails");

            this.prefDsDemux = new List<Guid>() { 
                new Guid(0x171252A0, 0x8820, 0x4AFE, 0x9D, 0xF8, 0x5C, 0x92, 0xB2, 0xD6, 0x6B, 0x04) //LAV Splitter
            };

            this.itemManager = new ItemManager(this);
            this.controlPoint = new UpnpControlPoint(this);
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
            this.services.Add(new MediaReceiverRegistrarService(this, this.server));

            this.server.HttpServer.AddRoute("HEAD", "/Files/*", new HttpRouteDelegate(GetLibraryFile));
            this.server.HttpServer.AddRoute("GET", "/Files/*", new HttpRouteDelegate(GetLibraryFile));
            this.server.HttpServer.AddRoute("HEAD", "/Encode/*", new HttpRouteDelegate(GetLibraryEncode));
            this.server.HttpServer.AddRoute("GET", "/Encode/*", new HttpRouteDelegate(GetLibraryEncode));
            this.server.HttpServer.AddRoute("HEAD", "/Thumbnail/*", new HttpRouteDelegate(GetThumbnailFile));
            this.server.HttpServer.AddRoute("GET", "/Thumbnail/*", new HttpRouteDelegate(GetThumbnailFile));
            this.server.HttpServer.AddRoute("HEAD", "/Subtitle/*", new HttpRouteDelegate(GetSubtitleFile));
            this.server.HttpServer.AddRoute("GET", "/Subtitle/*", new HttpRouteDelegate(GetSubtitleFile));

            this.server.HttpServer.AddRoute("GET", "/", new HttpRouteDelegate(GetWeb));
            this.server.HttpServer.AddRoute("GET", "/Web/*", new HttpRouteDelegate(GetWeb));
            this.server.HttpServer.AddRoute("GET", "/favicon.ico", new HttpRouteDelegate(GetWebIco));
            this.server.HttpServer.AddRoute("GET", "/Web/player.html", new HttpRouteDelegate(GetWebPlayer));
            this.server.HttpServer.AddRoute("HEAD", "/Web/WebPlayer.xap", new HttpRouteDelegate(GetWebSilverlight));
            this.server.HttpServer.AddRoute("GET", "/Web/WebPlayer.xap", new HttpRouteDelegate(GetWebSilverlight));
            this.server.HttpServer.AddRoute("GET", "/Web/htmlstyle.css", new HttpRouteDelegate(GetWebStyle));
            this.server.HttpServer.AddRoute("GET", "/Web/jquery.lightbox-0.5.css", new HttpRouteDelegate(GetWebStyle));
            this.server.HttpServer.AddRoute("GET", "/Web/jquery-ui-1.11.1.custom.min.css", new HttpRouteDelegate(GetWebStyle));
            this.server.HttpServer.AddRoute("GET", "/Web/control.js", new HttpRouteDelegate(GetWebJavascript));
            this.server.HttpServer.AddRoute("GET", "/Web/player.js", new HttpRouteDelegate(GetWebJavascript));
            this.server.HttpServer.AddRoute("GET", "/Web/jquery-1.11.1.min.js", new HttpRouteDelegate(GetWebJavascript));
            this.server.HttpServer.AddRoute("GET", "/Web/jquery.lightbox-0.5.min.js", new HttpRouteDelegate(GetWebJavascript));
            this.server.HttpServer.AddRoute("GET", "/Web/jquery-ui-1.11.1.custom.min.js", new HttpRouteDelegate(GetWebJavascript));
            this.server.HttpServer.AddRoute("GET", "/Web/jquery.ui.touch-punch.min.js", new HttpRouteDelegate(GetWebJavascript));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/folder.png", new HttpRouteDelegate(GetWebPng));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/folderback.png", new HttpRouteDelegate(GetWebPng));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/htmllogo.png", new HttpRouteDelegate(GetWebPng));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/ui-icons_222222_256x240.png", new HttpRouteDelegate(GetWebPng));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/ui-icons_454545_256x240.png", new HttpRouteDelegate(GetWebPng));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/ui-icons_888888_256x240.png", new HttpRouteDelegate(GetWebPng));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/htmlarrow.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/htmlorangearrow.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/htmlleftcontent.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/htmlgreenbox.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/htmlrightnav.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/lightbox-blank.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/lightbox-btn-close.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/lightbox-btn-next.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/lightbox-btn-prev.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/Images/lightbox-ico-loading.gif", new HttpRouteDelegate(GetWebGif));
            this.server.HttpServer.AddRoute("GET", "/Web/flowplayer.min.js", new HttpRouteDelegate(GetWebJavascript));
            this.server.HttpServer.AddRoute("HEAD", "/Web/flowplayer.swf", new HttpRouteDelegate(GetWebShockwave));
            this.server.HttpServer.AddRoute("GET", "/Web/flowplayer.swf", new HttpRouteDelegate(GetWebShockwave));
            this.server.HttpServer.AddRoute("HEAD", "/Web/flowplayer.controls.swf", new HttpRouteDelegate(GetWebShockwave));
            this.server.HttpServer.AddRoute("GET", "/Web/flowplayer.controls.swf", new HttpRouteDelegate(GetWebShockwave));
            this.server.HttpServer.AddRoute("POST", "/Web/devices.xml", new HttpRouteDelegate(GetWebDevices));

            this.hlsManager = new HlsManager(this);
        }

        public string DataDirectory
        {
            get { return this.dataDirectory; }
        }

        public string ThumbnailsPath
        {
            get { return this.thumbnailsPath; }
        }

        public ItemManager ItemManager
        {
            get { return this.itemManager; }
        }

        public bool MinimizeToTray
        {
            get { return this.minimizeToTray; }
            set
            {
                this.minimizeToTray = value;
                SettingsChanged();
            }
        }

        public bool TryToForwardPort
        {
            get { return this.tryToForwardPort; }
            set
            {
                CheckStopped();

                this.tryToForwardPort = value;
                SettingsChanged();
            }
        }

        public bool GenerateThumbnails
        {
            get { return this.generateThumbnails; }
            set
            {
                CheckStopped();

                this.generateThumbnails = value;
                SettingsChanged();
            }
        }

        public void LoadSettings()
        {
            CheckStopped();

            bool reThrow = true;
            try
            {
                if (!File.Exists(this.settingsPath))
                {
                    //Subor neexistuje - napr. po instalacii - nevyhadzovat chybu
                    reThrow = false;
                    throw new MediaCenterException("Settings file not found");
                }

                using (FileStream file = new FileStream(this.settingsPath, FileMode.Open, FileAccess.Read))
                {
                    XmlDocument xmlReader = new XmlDocument();
                    xmlReader.Load(file);

                    XmlNode node = xmlReader.SelectSingleNode("/HomeMediaCenter/@ModelNumber");
                    if (node == null || node.Value != this.modelNumber)
                    {
                        reThrow = false;
                        throw new MediaCenterException("Version of the configuration file is changed");
                    }

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

                    node = xmlReader.SelectSingleNode("/HomeMediaCenter/TryToForwardPort");
                    if (node != null)
                    {
                        bool.TryParse(node.InnerText, out this.tryToForwardPort);
                    }

                    node = xmlReader.SelectSingleNode("/HomeMediaCenter/GenerateThumbnails");
                    if (node != null)
                    {
                        bool.TryParse(node.InnerText, out this.generateThumbnails);
                    }

                    node = xmlReader.SelectSingleNode("/HomeMediaCenter/CultureName");
                    if (node != null && node.InnerText != string.Empty)
                    {
                        //Empty string je InvariantCulture - musi byt null
                        try { this.cultureInfo = new System.Globalization.CultureInfo(node.InnerText); }
                        catch { }
                    }

                    this.prefDsDemux.Clear();
                    foreach (XmlNode demuxGuid in xmlReader.SelectNodes("/HomeMediaCenter/PreferredDsDemux/*"))
                    {
                        try { this.prefDsDemux.Add(Guid.Parse(demuxGuid.InnerText)); }
                        catch { }
                    }

                    this.server.LoadSettings(xmlReader);

                    this.hlsManager.LoadSettings(xmlReader);

                    this.itemManager.LoadSettings(xmlReader, this.databasePath);
                }
            }
            catch
            {
                try { File.Delete(this.databasePath); }
                catch { }

                try { Directory.Delete(this.thumbnailsPath, true); }
                catch { }

                //treba nanovo zapisat settings kedze sa ho nepodarilo precitat
                this.settingsChanged = true;

                this.itemManager.LoadSettings(null, this.databasePath);

                if (reThrow)
                    throw;
            }
        }

        public void SaveSettings()
        {
            if (!this.settingsChanged)
                return;

            using (FileStream file = new FileStream(this.settingsPath, FileMode.Create, FileAccess.Write))
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
                xmlWriter.WriteElementString("TryToForwardPort", this.tryToForwardPort.ToString());
                xmlWriter.WriteElementString("GenerateThumbnails", this.generateThumbnails.ToString());
                xmlWriter.WriteElementString("CultureName", this.cultureInfo == null ? null : this.cultureInfo.Name);

                xmlWriter.WriteStartElement("PreferredDsDemux");
                foreach (Guid guid in this.prefDsDemux)
                    xmlWriter.WriteElementString("guid", guid.ToString());
                xmlWriter.WriteEndElement();

                this.server.SaveSettings(xmlWriter);

                this.hlsManager.SaveSettings(xmlWriter);

                this.itemManager.SaveSettings(xmlWriter);

                xmlWriter.WriteEndElement();
                xmlWriter.WriteEndDocument();
            }

            this.settingsChanged = false;
        }

        public string GetPrefDsDemux()
        {
            if (this.prefDsDemux.Count > 0)
            {
                //Zapisu sa hodnoty guid preferovaneho demultiplexora pre DirectShow oddelene znakom |
                string prefDsDemuxStr = string.Empty;
                foreach (Guid guid in this.prefDsDemux)
                    prefDsDemuxStr += guid + "|";
                return prefDsDemuxStr;
            }

            return null;
        }

        protected override void OnStart()
        {
            this.controlPoint.PortForwardingAsync(this.tryToForwardPort, this.server.HttpPort);

            if (this.generateThumbnails && !Directory.Exists(this.thumbnailsPath))
                Directory.CreateDirectory(this.thumbnailsPath);

            this.itemManager.Start();
            this.hlsManager.Start();

            base.OnStart();
        }

        protected override void OnStop()
        {
            base.OnStop();

            this.hlsManager.Stop();
            this.itemManager.Stop();
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

        private void GetLibraryFile(HttpRequest request)
        {
            if (!request.UrlParams.ContainsKey("id"))
                throw new HttpException(404, "Unknown source");

            string path, mime, fileFeature;
            if (!this.itemManager.GetFile(request.UrlParams["id"], out path, out mime, out fileFeature))
                throw new HttpException(404, "Bad parameter");

            GetFile(request, path, mime, fileFeature);
        }

        private void GetThumbnailFile(HttpRequest request)
        {
            if (!request.UrlParams.ContainsKey("id"))
                throw new HttpException(404, "Unknown source");

            string path, mime;
            if (!this.itemManager.GetThumbnailFile(request.UrlParams["id"], out path, out mime))
                throw new HttpException(404, "Bad parameter");

            if (request.UrlParams.ContainsKey("codec"))
                GetEncode(request, path, null, null, null);
            else
                GetFile(request, path, mime, null);
        }

        private void GetSubtitleFile(HttpRequest request)
        {
            if (!request.UrlParams.ContainsKey("id"))
                throw new HttpException(404, "Unknown source");

            string path, mime;
            if (!this.itemManager.GetSubtitleFile(request.UrlParams["id"], out path, out mime))
                throw new HttpException(404, "Bad parameter");

            GetFile(request, path, mime, null);
        }

        private void GetFile(HttpRequest request, string path, string mime, string fileFeature)
        {
            HttpResponse response = request.GetResponse();

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
                if (fileFeature != null)
                {
                    response.AddHreader("TransferMode.dlna.org", "Streaming");
                    response.AddHreader("ContentFeatures.dlna.org", fileFeature);
                }
                response.SendHeaders();

                if (request.Method == "GET")
                {
                    OnLogEvent(string.Format("Uploading file {0} as {1}", path, mime));

                    int readed;
                    byte[] buffer = new byte[4096];
                    Stream responseStream = response.GetStream();

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

        private void GetLibraryEncode(HttpRequest request)
        {
            if (!request.UrlParams.ContainsKey("id"))
                throw new HttpException(404, "Unknown source");

            TimeSpan? duration;
            string path, subtitlesPath, encodeFeature;
            if (!this.itemManager.GetEncode(request.UrlParams["id"], out path, out subtitlesPath, out encodeFeature, out duration))
                throw new HttpException(404, "Bad parameter");

            GetEncode(request, path, subtitlesPath, encodeFeature, duration);
        }

        private void GetEncode(HttpRequest request, string path, string subtitlesPath, string encodeFeature, TimeSpan? duration)
        {
            HttpResponse response = request.GetResponse();

            request.UrlParams["source"] = path;
            if (subtitlesPath != null)
                request.UrlParams["subtitles"] = subtitlesPath;
            if (encodeFeature != null)
                response.AddHreader("ContentFeatures.dlna.org", encodeFeature);

            if (duration.HasValue)
            {
                //Je mozne seekovanie pomocou casu

                double startTimeParam = 0;
                if (request.UrlParams.ContainsKey("starttime") && double.TryParse(request.UrlParams["starttime"], 
                    System.Globalization.NumberStyles.Float | System.Globalization.NumberStyles.AllowThousands, 
                    System.Globalization.CultureInfo.InvariantCulture, out startTimeParam))
                {
                    //Ak je nastaveny url parameter pozicie - treba upravit dlzku
                    duration -= TimeSpan.FromSeconds(startTimeParam);
                }

                response.AddHreader("X-AvailableSeekRange", string.Format(System.Globalization.CultureInfo.InvariantCulture,
                    "1 npt=0.0-{0:0.000}", duration.Value.TotalSeconds));

                if (request.Headers.ContainsKey("TimeSeekRange.dlna.org"))
                {
                    //Zistenie a nastavenie pozicie
                    string[] range = request.Headers["TimeSeekRange.dlna.org"].Split('=').Last().Split('-').Select(a => a.Trim()).ToArray();
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

                    request.UrlParams["starttime"] = (start + startTimeParam).ToString(System.Globalization.CultureInfo.InvariantCulture);

                    response.AddHreader("Vary", "TimeSeekRange.dlna.org");
                    response.AddHreader("TimeSeekRange.dlna.org", string.Format(System.Globalization.CultureInfo.InvariantCulture,
                        "npt={0:0.000}-{1:0.000}/{2:0.000}", start, end, duration.Value.TotalSeconds));
                }
            }

            string prefDsDemuxStr = GetPrefDsDemux();
            if (prefDsDemuxStr != null)
                request.UrlParams["prefDsDemux"] = prefDsDemuxStr;
            
            EncoderBuilder builder = EncoderBuilder.GetEncoder(request.UrlParams);

            response.AddHreader(HttpHeader.ContentType, builder.GetMime());
            response.AddHreader(HttpHeader.AcceptRanges, "none");
            response.AddHreader("TransferMode.dlna.org", "Streaming");
            response.SendHeaders();

            if (request.Method == "GET")
            {
                OnLogEvent(string.Format("Starting encode {0} as {1}", request.UrlParams["source"], builder.GetMime()));
                builder.StartEncode(response.GetStream());
            }
        }

        private void GetWeb(HttpRequest request)
        {
            HttpResponse response = request.GetResponse();

            string id;
            if (!request.UrlParams.ContainsKey("id") || (id = request.UrlParams["id"]) == string.Empty)
                id = "0";

            using (MemoryStream stream = new MemoryStream())
            using (XmlWriter xmlWriter = XmlWriter.Create(stream, new XmlWriterSettings() { Indent = true, Encoding = new UTF8Encoding(false) }))
            {
                xmlWriter.WriteStartDocument();
                xmlWriter.WriteDocType("html", "-//W3C//DTD XHTML 1.0 Transitional//EN", "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd", null);
                xmlWriter.WriteStartElement("html", "http://www.w3.org/1999/xhtml");

                xmlWriter.WriteStartElement("head");
                xmlWriter.WriteElementString("title", "Home Media Center");
                xmlWriter.WriteRaw(@"<link rel=""apple-touch-icon"" href=""/web/images/htmllogo.png?codec=png&width=114&height=114"" />");
                xmlWriter.WriteRaw(@"<link rel=""stylesheet"" type=""text/css"" href=""/web/htmlstyle.css"" />");
                xmlWriter.WriteRaw(@"<link rel=""stylesheet"" type=""text/css"" href=""/web/jquery.lightbox-0.5.css"" />");
                xmlWriter.WriteRaw(@"<link rel=""stylesheet"" type=""text/css"" href=""/web/jquery-ui-1.11.1.custom.min.css"" />");
                xmlWriter.WriteRaw(@"<script type=""text/javascript"" src=""/web/jquery-1.11.1.min.js""></script>");
                xmlWriter.WriteRaw(@"<script type=""text/javascript"" src=""/web/jquery.lightbox-0.5.min.js""></script>");
                xmlWriter.WriteRaw(@"<script type=""text/javascript"" src=""/web/jquery-ui-1.11.1.custom.min.js""></script>");
                xmlWriter.WriteRaw(@"<script type=""text/javascript"" src=""/web/jquery.ui.touch-punch.min.js""></script>");
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
                xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/page.html?id=0_2"" title="""">{0}</a></li>", LanguageResource.Photos));
                xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/page.html?id=0_1"" title="""">{0}</a></li>", LanguageResource.Music));
                xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/page.html?id=0_3"" title="""">{0}</a></li>", LanguageResource.Video));
                xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/control.html"" title="""">{0}</a></li>", LanguageResource.Control));
                xmlWriter.WriteEndElement();
                xmlWriter.WriteEndElement();

                xmlWriter.WriteEndElement();

                if (request.Url.Contains("control.html"))
                {
                    string device = request.UrlParams.ContainsKey("devices") ? request.UrlParams["devices"] : null;

                    TimeSpan startTime;
                    if (!request.UrlParams.ContainsKey("starttime") || !TimeSpan.TryParse(request.UrlParams["starttime"], out startTime))
                        startTime = TimeSpan.Zero;

                    xmlWriter.WriteStartElement("div");
                    xmlWriter.WriteAttributeString("id", "main_content");

                    xmlWriter.WriteStartElement("form");
                    xmlWriter.WriteAttributeString("action", "/web/control.html");
                    xmlWriter.WriteAttributeString("method", "get");

                    xmlWriter.WriteStartElement("div");
                    xmlWriter.WriteAttributeString("id", "left_content");

                    xmlWriter.WriteRaw(@"<script type=""text/javascript"" src=""/web/control.js""></script>");

                    xmlWriter.WriteRaw(string.Format("<h2>DLNA {0}</h2>", LanguageResource.Device));

                    xmlWriter.WriteStartElement("select");
                    xmlWriter.WriteAttributeString("id", "devices");
                    xmlWriter.WriteAttributeString("name", "devices");

                    xmlWriter.WriteStartElement("option");
                    xmlWriter.WriteAttributeString("value", "none");
                    xmlWriter.WriteValue(LanguageResource.SelectDevice);
                    xmlWriter.WriteEndElement();

                    this.controlPoint.WriteXml(xmlWriter, device);

                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteRaw(@"<div id=""devicesWorkingBar""></div>");
                    xmlWriter.WriteRaw(string.Format(@"<button id=""refreshBtn"" type=""button"">{0}</button>", LanguageResource.Refresh));

                    //Koniec left_content
                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteStartElement("div");
                    xmlWriter.WriteAttributeString("id", "right_content");

                    Item item = this.itemManager.GetItemWithParent(id);
                    if (id == "0" || item == null)
                    {
                        xmlWriter.WriteRaw(string.Format(@"<p>{0}: {1}</p>", LanguageResource.Item, LanguageResource.NotSelected));
                    }
                    else
                    {
                        xmlWriter.WriteRaw(string.Format(@"<input id=""idInput"" type=""hidden"" name=""id"" value=""{0}""></input>", id));
                        xmlWriter.WriteRaw(string.Format(@"<p>{0}: {1}</p>", LanguageResource.Item, item.Title));

                        TimeSpan? duration = item.GetDuration();
                        if (duration.HasValue)
                        {
                            xmlWriter.WriteRaw(string.Format(@"<input id=""posInput"" type=""hidden"" name=""starttime"" value=""{0}""></input>", startTime.ToString("h':'mm':'ss")));

                            xmlWriter.WriteRaw(string.Format(@"<span id=""streamPos"">{0}</span>", startTime.ToString("h':'mm':'ss")));
                            xmlWriter.WriteRaw(@"<div id=""seekSlider""></div>");
                            xmlWriter.WriteRaw(string.Format(@"<span id=""streamLength"">{0}</span>", duration.Value.ToString("h':'mm':'ss")));
                            xmlWriter.WriteStartElement("br");
                            xmlWriter.WriteEndElement();
                        }
                    }

                    xmlWriter.WriteRaw(string.Format(@"<button id=""volPBtn"" name=""action"" value=""volp""/>{0} +</button>", LanguageResource.Volume));
                    xmlWriter.WriteRaw(string.Format(@"<button id=""volMBtn"" name=""action"" value=""volm""/>{0} -</button>", LanguageResource.Volume));
                    xmlWriter.WriteRaw(string.Format(@"<button id=""playBtn"" name=""action"" value=""play""/>{0}</button>", LanguageResource.Play));
                    xmlWriter.WriteRaw(string.Format(@"<button id=""stopBtn"" name=""action"" value=""stop""/>{0}</button>", LanguageResource.Stop));

                    if (request.UrlParams.ContainsKey("action"))
                    {
                        string action = request.UrlParams["action"].ToLower();
                        try
                        {
                            if (action == "volp")
                                this.controlPoint.VolumePlus(device);
                            else if (action == "volm")
                                this.controlPoint.VolumeMinus(device);
                            else if (action == "play")
                                this.controlPoint.Play(device, this.server.HttpPort, id, this.itemManager, startTime);
                            else if (action == "stop")
                                this.controlPoint.Stop(device);
                        }
                        catch (Exception ex)
                        {
                            xmlWriter.WriteStartElement("br");
                            xmlWriter.WriteEndElement();

                            xmlWriter.WriteStartElement("span");
                            xmlWriter.WriteAttributeString("class", "error");
                            xmlWriter.WriteValue(LanguageResource.OperationFailed);
                            xmlWriter.WriteEndElement();

                            xmlWriter.WriteStartElement("span");
                            xmlWriter.WriteAttributeString("class", "ui-icon ui-icon-info");
                            xmlWriter.WriteAttributeString("title", ex.Message);
                            xmlWriter.WriteFullEndElement();
                        }
                    }

                    //Koniec right_content
                    xmlWriter.WriteEndElement();

                    //Koniec form
                    xmlWriter.WriteEndElement();

                    //Koniec main_content
                    xmlWriter.WriteEndElement();
                }
                else
                {
                    this.itemManager.GetWeb(id, xmlWriter);
                }

                xmlWriter.WriteRaw(@"<div id=""footer""><div class=""copyright"">Home Media Center " + this.modelNumber + "</div></div>");

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
                response.AddHreader(HttpHeader.CacheControl, "no-cache, no-store, must-revalidate"); //nocache hlavne pre odosielanie formulara
                response.SendHeaders();

                stream.CopyTo(response.GetStream());
            }
        }

        private void GetWebPlayer(HttpRequest request)
        {
            HttpResponse response = request.GetResponse();

            string id;
            if (!request.UrlParams.ContainsKey("id") || (id = request.UrlParams["id"]) == string.Empty)
                id = "0";

            using (MemoryStream stream = new MemoryStream())
            using (XmlWriter xmlWriter = XmlWriter.Create(stream, new XmlWriterSettings() { Indent = true, Encoding = new UTF8Encoding(false) }))
            {
                xmlWriter.WriteStartDocument();
                xmlWriter.WriteDocType("html", "-//W3C//DTD XHTML 1.0 Transitional//EN", "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd", null);
                xmlWriter.WriteStartElement("html", "http://www.w3.org/1999/xhtml");

                xmlWriter.WriteStartElement("head");
                xmlWriter.WriteElementString("title", "Home Media Center - Player");
                xmlWriter.WriteRaw(@"<link rel=""stylesheet"" type=""text/css"" href=""/web/htmlstyle.css"" />");
                xmlWriter.WriteRaw(@"<link rel=""stylesheet"" type=""text/css"" href=""/web/jquery-ui-1.11.1.custom.min.css"" />");
                xmlWriter.WriteRaw(@"<script type=""text/javascript"" src=""/web/jquery-1.11.1.min.js""></script>");
                xmlWriter.WriteRaw(@"<script type=""text/javascript"" src=""/web/jquery-ui-1.11.1.custom.min.js""></script>");
                xmlWriter.WriteRaw(@"<script type=""text/javascript"" src=""/web/jquery.ui.touch-punch.min.js""></script>");
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

                this.itemManager.GetWebPlayer(id, xmlWriter, request.UrlParams);

                //Koniec main_content
                xmlWriter.WriteFullEndElement();

                xmlWriter.WriteRaw(@"<div id=""footer""><div class=""copyright"">Home Media Center " + this.modelNumber + "</div></div>");

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

                stream.CopyTo(response.GetStream());
            }
        }

        private void GetWebStyle(HttpRequest request)
        {
            SendResource(request, "text/css");
        }

        private void GetWebIco(HttpRequest request)
        {
            SendResource(request, "image/x-icon");
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
            HttpResponse response = request.GetResponse();

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
                            memStream.CopyTo(response.GetStream());
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
                        resourceStream.CopyTo(response.GetStream());
                    }
                }
            }
        }

        private void GetWebDevices(HttpRequest request)
        {
            HttpResponse response = request.GetResponse();

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
                    this.controlPoint.Refresh();
            }

            using (MemoryStream stream = new MemoryStream())
            using (XmlWriter xmlWriter = XmlWriter.Create(stream, new XmlWriterSettings() { Indent = true }))
            {
                xmlWriter.WriteStartDocument();

                xmlWriter.WriteStartElement("devices");
                this.controlPoint.WriteXml(xmlWriter, null, type);
                xmlWriter.WriteEndElement();

                xmlWriter.Flush();
                stream.Position = 0;

                response.AddHreader(HttpHeader.ContentLength, stream.Length.ToString());
                response.AddHreader(HttpHeader.ContentType, "text/xml; charset=utf-8");
                response.SendHeaders();

                stream.CopyTo(response.GetStream());
            }
        }
    }
}
