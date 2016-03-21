using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading;
using System.Xml;
using System.Xml.Serialization;
using System.Data.SqlServerCe;
using System.Data.Linq;
using System.Data.Linq.Mapping;

namespace HomeMediaCenter
{
    public class ItemManager : Interfaces.IItemManager
    {
        public class Dir
        {
            public string Path { get; set; }
            public string Title { get; set; }
            public bool Skip { get; set; }
            public FileSystemWatcher Watcher { get; set; }
        }

        private bool enableWebcamStreaming = true;
        private bool enableDesktopStreaming = true;
        private bool realTimeDatabaseRefresh = true;
        private bool liteDatabaseRefresh = false;
        private bool showHiddenFiles = false;
        private HttpMimeDictionary mimeTypes = HttpMimeDictionary.GetDefaults();
        private HashSet<string> subtitleExt = new HashSet<string>(StringComparer.OrdinalIgnoreCase) {
            ".srt", ".sub", ".ssa", ".txt" };

        private readonly MediaSettings settings;
        private readonly MediaServerDevice upnpDevice;
        private readonly List<Dir> directories = new List<Dir>();
        private readonly HashSet<string> changedPaths = new HashSet<string>();
        private Thread watcherThread;
        private bool exitWatcherThread;

        private readonly object dbWriteLock = new object();
        private string dbConnectionString;

        public event EventHandler<ExceptionEventArgs> AsyncBuildDatabaseStart;
        public event EventHandler<ExceptionEventArgs> AsyncBuildDatabaseEnd;

        public ItemManager(MediaServerDevice upnpDevice)
        {
            this.upnpDevice = upnpDevice;
            this.settings = new MediaSettings(upnpDevice);
        }

        public MediaServerDevice UpnpDevice
        {
            get { return this.upnpDevice; }
        }

        internal MediaSettings MediaSettings
        {
            get { return this.settings; }
        }

        public Interfaces.IMediaSettings IMediaSettings
        {
            get { return this.settings; }
        }

        public bool RealTimeDatabaseRefresh
        {
            get { return this.realTimeDatabaseRefresh; }
            set
            {
                this.upnpDevice.CheckStopped();

                this.realTimeDatabaseRefresh = value;

                lock (this.directories)
                {
                    foreach (Dir dir in this.directories)
                    {
                        try { dir.Watcher.EnableRaisingEvents = value; }
                        catch { }
                    }
                }

                this.upnpDevice.SettingsChanged();
            }
        }

        public bool EnableDesktopStreaming
        {
            get { return this.enableDesktopStreaming; }
            set
            {
                this.upnpDevice.CheckStopped();

                this.enableDesktopStreaming = value;
                this.upnpDevice.SettingsChanged();
            }
        }

        public bool EnableWebcamStreaming
        {
            get { return this.enableWebcamStreaming; }
            set
            {
                this.upnpDevice.CheckStopped();

                this.enableWebcamStreaming = value;
                this.upnpDevice.SettingsChanged();
            }
        }

        public bool ShowHiddenFiles
        {
            get { return this.showHiddenFiles; }
            set
            {
                this.upnpDevice.CheckStopped();

                this.showHiddenFiles = value;
                this.upnpDevice.SettingsChanged();
            }
        }

        public bool CheckDirectoryPermission(string path)
        {
            try { System.IO.Directory.GetFiles(path); }
            catch { return false; }

            return true;
        }

        public string[] AddDirectory(string directory, string title)
        {
            lock (this.directories)
            {
                AddDirectoryInt(directory, title);

                WatcherChanged("*", true);

                this.upnpDevice.SettingsChanged();

                return this.directories.Select(a => a.Path).ToArray();
            }
        }

        public string[] RemoveDirectory(string directory)
        {
            lock (this.directories)
            {
                RemoveDirectoryInt(directory);

                WatcherChanged("*", true);

                this.upnpDevice.SettingsChanged();

                return this.directories.Select(a => a.Path).ToArray();
            }
        }

        public string[] GetDirectories()
        {
            lock (this.directories)
            {
                return this.directories.Select(a => a.Path).ToArray();
            }
        }

        public Dir[] GetWorkingDirectories()
        {
            lock (this.directories)
            {
                return this.directories.Where(a => !a.Skip).Select(a => new Dir() { Title = a.Title, Path = a.Path }).ToArray();
            }
        }

        public HttpMime GetMimeType(string key)
        {
            return this.mimeTypes.GetValue(key);
        }

        public bool ContainsSubtitleExt(string ext)
        {
            return this.subtitleExt.Contains(ext);
        }

        public void BuildDatabaseAsync()
        {
            WatcherChanged("*", true);
        }

        public void BuildDatabase()
        {
            using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
            using (DataContext context = new DataContext(conn))
            {
                lock (this.dbWriteLock)
                {
                    Item root = context.GetTable<Item>().Single(a => a.Id == 0);

                    root.RefreshMe(context, this, true);
                    context.SubmitChanges();

                    if (!this.liteDatabaseRefresh)
                    {
                        root.RefreshMetadata(context, this, true);
                        context.SubmitChanges();
                    }
                }
            }
        }

        public void BuildDatabaseAsync(string path, bool recursive)
        {
            WatcherChanged(path, recursive);
        }

        public void BuildDatabaseSync(string path, bool recursive)
        {
            path = path.TrimEnd('\\');

            using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
            using (DataContext context = new DataContext(conn))
            {
                lock (this.dbWriteLock)
                {
                    Item item = context.GetTable<Item>().FirstOrDefault(a => a.Path == path);
                    if (item == null)
                    {
                        //Pokusi sa najst nadradeny adresar - kontajner
                        string fileName = Path.GetFileName(path);
                        string subPath = Path.GetDirectoryName(path);

                        item = context.GetTable<Item>().FirstOrDefault(a => a.Path == subPath);
                        if (item == null)
                            return;

                        Item children = item.Children.FirstOrDefault(a => a.Path == fileName);
                        if (children == null)
                        {
                            //Adresar alebo subor este neboli pridane - obnov nadradeny adresar
                            item.RefreshMe(context, this, false);
                            children = item.Children.FirstOrDefault(a => a.Path == path || a.Path == fileName);

                            if (children != null)
                            {
                                //V nadradenom adrasari by pozadovana polozka uz mala existovat (ak bol subor akceptovany)
                                //Obnova prebieha rekurzivne do podadrasarov
                                children.RefreshMe(context, this, true);
                                context.SubmitChanges();

                                if (!this.liteDatabaseRefresh)
                                {
                                    children.RefreshMetadata(context, this, true);
                                    context.SubmitChanges();
                                }
                            }

                            return;
                        }
                        else
                        {
                            //V nadradenom adresari (kontajner) sa nasiel subor (polozka) 
                            item = children;
                        }
                    }

                    //Ak sa nasla polozka s povodnou cestou - celu cestu ma iba kontajner
                    //alebo v nadradenom adresari sa nasla polozka (path \ subpath)
                    item.RefreshMe(context, this, recursive);
                    context.SubmitChanges();

                    if (!this.liteDatabaseRefresh)
                    {
                        item.RefreshMetadata(context, this, recursive);
                        context.SubmitChanges();
                    }
                }
            }
        }

        public List<StreamSourcesItem> GetStreamSources()
        {
            try
            {
                using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
                using (DataContext context = new DataContext(conn))
                {
                    return context.GetTable<Item>().OfType<ItemContainerStreamCustom>().Select(delegate(ItemContainerStreamCustom a) {
                        return new StreamSourcesItem(a.Id, a.Title, a.Path);
                    }).ToList();
                }
            }
            catch
            {
                return new List<StreamSourcesItem>();
            }
        }

        public void SetStreamSources(StreamSourcesItem[] toAdd, StreamSourcesItem[] toUpdate, StreamSourcesItem[] toDelete)
        {
            if (toAdd == null) toAdd = new StreamSourcesItem[0];
            if (toUpdate == null) toUpdate = new StreamSourcesItem[0];
            if (toDelete == null) toDelete = new StreamSourcesItem[0];

            using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
            using (DataContext context = new DataContext(conn))
            {
                lock (this.dbWriteLock)
                {
                    ItemContainerStreamRoot root = context.GetTable<Item>().OfType<ItemContainerStreamRoot>().Single();

                    //Pridavanie zaznamov a odstranit nevyplnene polozky
                    context.GetTable<Item>().InsertAllOnSubmit(
                        toAdd.Where(a => !string.IsNullOrWhiteSpace(a.Title) && !string.IsNullOrWhiteSpace(a.Path)).Select(
                            a => new ItemContainerStreamCustom(a.Title, a.Path, root)));

                    //Uprava existujucich zaznamov
                    foreach (StreamSourcesItem item in toUpdate.Where(a => a.Id.HasValue))
                    {
                        ItemContainerStreamCustom updItem = root.Children.OfType<ItemContainerStreamCustom>().FirstOrDefault(a => a.Id == item.Id.Value);
                        if (updItem == null)
                            continue;

                        if (item.Path != updItem.Path)
                            updItem.Path = item.Path;

                        if (item.Title != updItem.Title)
                        {
                            //Pre aktualizaciu nazvu je potrebne potomkov odstranit - pri refresh sa pridaju so spravnym nazvom
                            updItem.RemoveMe(context, this);

                            updItem.Title = item.Title;
                        }
                    }

                    //Mazanie zaznamov
                    foreach (StreamSourcesItem item in toDelete.Where(a => a.Id.HasValue))
                    {
                        ItemContainerStreamCustom delItem = root.Children.OfType<ItemContainerStreamCustom>().FirstOrDefault(a => a.Id == item.Id.Value);
                        if (delItem == null)
                            continue;

                        context.GetTable<Item>().DeleteOnSubmit(delItem);
                        delItem.RemoveMe(context, this);
                    }

                    context.SubmitChanges();
                }
            }

            BuildDatabaseAsync("Stream", true);
        }

        internal void Start()
        {
            if (this.watcherThread == null)
            {
                lock (this.changedPaths)
                {
                    this.exitWatcherThread = false;
                    this.changedPaths.Clear();
                }

                this.watcherThread = new Thread(new ThreadStart(WatcherLoop)) { IsBackground = true, Priority = ThreadPriority.BelowNormal };
                if (this.upnpDevice.CultureInfo != null)
                    this.watcherThread.CurrentCulture = this.watcherThread.CurrentUICulture = this.upnpDevice.CultureInfo;

                this.watcherThread.Start();
            }
        }

        internal void Stop()
        {
            if (this.watcherThread != null)
            {
                lock (this.changedPaths)
                {
                    //Ukonci refresh vlakon
                    this.changedPaths.Clear();
                    this.exitWatcherThread = true;
                    Monitor.Pulse(this.changedPaths);
                }

                this.watcherThread.Join();
                this.watcherThread = null;
            }
        }

        internal void SaveSettings(XmlWriter xmlWriter)
        {
            xmlWriter.WriteElementString("RealTimeDatabaseRefresh", this.realTimeDatabaseRefresh.ToString());
            xmlWriter.WriteElementString("LiteDatabaseRefresh", this.liteDatabaseRefresh.ToString());
            xmlWriter.WriteElementString("EnableDesktopStreaming", this.enableDesktopStreaming.ToString());
            xmlWriter.WriteElementString("EnableWebcamStreaming", this.enableWebcamStreaming.ToString());
            xmlWriter.WriteElementString("ShowHiddenFiles", this.showHiddenFiles.ToString());

            xmlWriter.WriteStartElement("Directories");
            lock (this.directories)
            {
                foreach (Dir dir in this.directories)
                {
                    xmlWriter.WriteStartElement("Directory");
                    xmlWriter.WriteAttributeString("Title", dir.Title);
                    xmlWriter.WriteValue(dir.Path);
                    xmlWriter.WriteEndElement();
                }
            }
            xmlWriter.WriteEndElement();

            xmlWriter.WriteStartElement("Subtitles");
            foreach (string ext in this.subtitleExt)
                xmlWriter.WriteElementString("string", ext);
            xmlWriter.WriteEndElement();

            this.mimeTypes.SerializeMe(xmlWriter);
                
            this.settings.SaveSettings(xmlWriter);
        }

        internal void LoadSettings(XmlDocument xmlReader, string databasePath)
        {
            if (xmlReader != null)
            {
                XmlNode node = xmlReader.SelectSingleNode("/HomeMediaCenter/RealTimeDatabaseRefresh");
                if (node != null)
                    bool.TryParse(node.InnerText, out this.realTimeDatabaseRefresh);

                node = xmlReader.SelectSingleNode("/HomeMediaCenter/LiteDatabaseRefresh");
                if (node != null)
                    bool.TryParse(node.InnerText, out this.liteDatabaseRefresh);

                node = xmlReader.SelectSingleNode("/HomeMediaCenter/EnableDesktopStreaming");
                if (node != null)
                    bool.TryParse(node.InnerText, out this.enableDesktopStreaming);

                node = xmlReader.SelectSingleNode("/HomeMediaCenter/EnableWebcamStreaming");
                if (node != null)
                    bool.TryParse(node.InnerText, out this.enableWebcamStreaming);

                node = xmlReader.SelectSingleNode("/HomeMediaCenter/ShowHiddenFiles");
                if (node != null)
                    bool.TryParse(node.InnerText, out this.showHiddenFiles);

                lock (this.directories)
                {
                    foreach (Dir dir in this.directories)
                        dir.Watcher.Dispose();
                    this.directories.Clear();
                    foreach (XmlNode directory in xmlReader.SelectNodes("/HomeMediaCenter/Directories/*"))
                    {
                        XmlAttribute title = directory.Attributes["Title"];
                        AddDirectoryInt(directory.InnerText, title == null ? null : title.Value);
                    }
                }

                this.subtitleExt.Clear();
                foreach (XmlNode subtitle in xmlReader.SelectNodes("/HomeMediaCenter/Subtitles/*"))
                    this.subtitleExt.Add(subtitle.InnerText);

                this.mimeTypes = HttpMimeDictionary.DeserializeMe(xmlReader);

                this.settings.LoadSettings(xmlReader);
            }

            this.dbConnectionString = "Data Source=" + databasePath;
            if (!File.Exists(databasePath))
            {
                using (SqlCeEngine engine = new SqlCeEngine(this.dbConnectionString))
                {
                    engine.CreateDatabase();
                }

                using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
                using (DataContext context = new DataContext(conn))
                {
                    string command = CreateTable(context.Mapping.GetTable(typeof(Item)));
                    context.ExecuteCommand(command);
                    context.ExecuteCommand("CREATE INDEX pathidx ON [Items]([Path])");

                    ItemContainerRoot root = new ItemContainerRoot();
                    context.GetTable<Item>().InsertOnSubmit(root);
                    context.SubmitChanges();

                    if (root.Id != 0)
                        throw new MediaCenterException("Root container must have id with value 0");
                }
            }
            else
            {
                using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
                using (DataContext context = new DataContext(conn))
                {
                    Item root = context.GetTable<Item>().Single(a => a.GetType() == typeof(ItemContainerRoot));
                    if (root.Id != 0)
                        throw new MediaCenterException("Root container must have id with value 0");
                }
            }
        }

        internal bool GetSubtitleFile(string objectID, out string path, out string mime)
        {
            int id;
            if (!ParseIdAndParams(objectID, out id))
                throw new HttpException(402, "GetSubtitleFile - parse exception");

            using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
            using (DataContext context = new DataContext(conn))
            {
                Item item = context.GetTable<Item>().FirstOrDefault(a => a.Id == id);
                if (item == null)
                {
                    path = null;
                    mime = null;
                    return false;
                }

                if ((path = item.GetSubtitlesPath()) == null)
                {
                    mime = null;
                    return false;
                }

                mime = "text/" + Path.GetExtension(path).TrimStart('.');
            }

            return true;
        }

        internal bool GetThumbnailFile(string objectID, out string path, out string mime)
        {
            int id;
            if (!ParseIdAndParams(objectID, out id))
                throw new HttpException(402, "GetThumbnailFile - parse exception");

            using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
            using (DataContext context = new DataContext(conn))
            {
                Item item = context.GetTable<Item>().FirstOrDefault(a => a.Id == id);
                if (item == null)
                {
                    path = null;
                    mime = null;
                    return false;
                }

                if ((path = item.GetThumbnailPath(this)) == null)
                {
                    mime = null;
                    return false;
                }

                mime = "image/jpeg";
            }

            return true;
        }

        internal bool GetFile(string objectID, out string path, out string mime, out string fileFeature)
        {
            int id;
            if (!ParseIdAndParams(objectID, out id))
                throw new HttpException(402, "GetFile - parse exception");

            using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
            using (DataContext context = new DataContext(conn))
            {
                Item item = context.GetTable<Item>().FirstOrDefault(a => a.Id == id);
                if (item == null)
                {
                    path = null;
                    mime = null;
                    fileFeature = null;
                    return false;
                }

                if ((path = item.GetPath()) == null)
                {
                    mime = null;
                    fileFeature = null;
                    return false;
                }

                if ((mime = item.GetMime()) == null)
                {
                    fileFeature = null;
                    return false;
                }

                fileFeature = item.GetFileFeature(this.settings);
            }

            return true;
        }

        internal bool GetEncode(string objectID, out string path, out string subtitlesPath, out string encodeFeature, out TimeSpan? duration)
        {
            int id;
            if (!ParseIdAndParams(objectID, out id))
                throw new HttpException(402, "GetEncode - parse exception");

            using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
            using (DataContext context = new DataContext(conn))
            {
                Item item = context.GetTable<Item>().FirstOrDefault(a => a.Id == id);
                if (item == null)
                {
                    path = null;
                    subtitlesPath = null;
                    encodeFeature = null;
                    duration = null;
                    return false;
                }

                if ((path = item.GetPath()) == null)
                {
                    subtitlesPath = null;
                    encodeFeature = null;
                    duration = null;
                    return false;
                }

                subtitlesPath = item.GetSubtitlesPath();
                encodeFeature = item.GetEncodeFeature(this.settings);
                duration = item.GetDuration();
            }

            return true;
        }

        internal void BrowseMetadata(string host, string objectID, TimeSpan startTime, out string result, out string firstResource)
        {
            int id;
            string parameters;
            if (!ParseIdAndParams(objectID, out id, out parameters))
                throw new SoapException(402, "Invalid Args");

            XmlDocument doc = new XmlDocument();
            using (XmlWriter writer = doc.CreateNavigator().AppendChild())
            {
                writer.WriteStartElement("DIDL-Lite", "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/");
                writer.WriteAttributeString("xmlns", "dc", null, "http://purl.org/dc/elements/1.1/");
                writer.WriteAttributeString("xmlns", "upnp", null, "urn:schemas-upnp-org:metadata-1-0/upnp/");
                writer.WriteAttributeString("xmlns", "sec", null, "http://www.sec.co.kr/dlna");

                using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
                using (DataContext context = new DataContext(conn))
                {
                    Item mainObject = context.GetTable<Item>().FirstOrDefault(a => a.Id == id);
                    if (mainObject == null)
                        throw new SoapException(701, "No such object");

                    mainObject.BrowseMetadata(writer, this.settings, host, parameters, null, 0, startTime == TimeSpan.Zero ? null : new Nullable<TimeSpan>(startTime));
                }

                writer.WriteEndElement();
            }

            XmlNamespaceManager namespaceManager = new XmlNamespaceManager(doc.NameTable);
            namespaceManager.AddNamespace("didlNam", "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/");
            XmlNode res = doc.SelectSingleNode("/didlNam:DIDL-Lite/didlNam:item/didlNam:res[1]", namespaceManager);

            result = doc.InnerXml;
            firstResource = res.InnerText;
        }

        internal void Browse(Dictionary<string, string> headers, string objectID, BrowseFlag browseFlag, string filter, uint startingIndex,
            uint requestedCount, string sortCriteria, out string result, out string numberReturned, out string totalMatches)
        {
            string host = headers.ContainsKey("host") ? "http://" + headers["host"] : string.Empty;
            StringBuilder sb = new StringBuilder();

            HashSet<string> filterSet = null;
            if (filter != "*")
                filterSet = new HashSet<string>(filter.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries), StringComparer.OrdinalIgnoreCase);

            int id;
            string parameters;
            if (!ParseIdAndParams(objectID, out id, out parameters))
                throw new SoapException(402, "Invalid Args");

            using (XmlWriter writer = XmlWriter.Create(sb, new XmlWriterSettings() { OmitXmlDeclaration = true }))
            {
                writer.WriteStartElement("DIDL-Lite", "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/");
                writer.WriteAttributeString("xmlns", "dc", null, "http://purl.org/dc/elements/1.1/");
                writer.WriteAttributeString("xmlns", "upnp", null, "urn:schemas-upnp-org:metadata-1-0/upnp/");
                writer.WriteAttributeString("xmlns", "sec", null, "http://www.sec.co.kr/dlna");
                writer.WriteAttributeString("xmlns", "av", null, "urn:schemas-sony-com:av");

                using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
                using (DataContext context = new DataContext(conn))
                {
                    Item mainObject = context.GetTable<Item>().FirstOrDefault(a => a.Id == id);
                    if (mainObject == null)
                        throw new SoapException(701, "No such object");

                    if (browseFlag == BrowseFlag.BrowseMetadata)
                    {
                        mainObject.BrowseMetadata(writer, this.settings, host, parameters, filterSet);
                        numberReturned = "1";
                        totalMatches = "1";
                    }
                    else
                    {
                        mainObject.BrowseDirectChildren(writer, this.settings, host, parameters, filterSet,
                            startingIndex, requestedCount, sortCriteria, out numberReturned, out totalMatches);
                    }
                }

                writer.WriteEndElement();
            }
            
            result = sb.ToString();
        }

        internal void GetWeb(string objectID, XmlWriter xmlWriter)
        {
            int id;
            string parameters;
            if (!ParseIdAndParams(objectID, out id, out parameters))
                throw new HttpException(402, "GetWeb - parse exception");

            using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
            using (DataContext context = new DataContext(conn))
            {
                Item item = context.GetTable<Item>().FirstOrDefault(a => a.Id == id);
                if (item == null)
                {
                    xmlWriter.WriteStartElement("div");
                    xmlWriter.WriteAttributeString("id", "main_content");

                    xmlWriter.WriteElementString("p", LanguageResource.UnknownID);

                    xmlWriter.WriteEndElement();
                }
                else
                {
                    item.BrowseWebDirectChildren(xmlWriter, this.settings, parameters);
                }
            }
        }

        internal void GetWebPlayer(string objectID, XmlWriter xmlWriter, Dictionary<string, string> urlParams)
        {
            int id;
            if (!ParseIdAndParams(objectID, out id))
                throw new HttpException(402, "GetWebPlayer - parse exception");

            using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
            using (DataContext context = new DataContext(conn))
            {
                Item item = context.GetTable<Item>().FirstOrDefault(a => a.Id == id);
                if (item != null)
                {
                    item.GetWebPlayer(xmlWriter, this, urlParams);
                }
            }
        }

        internal Item GetItemWithParent(string objectID)
        {
            int id;
            if (!ParseIdAndParams(objectID, out id))
                throw new HttpException(402, "GetTitle - parse exception");

            using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
            using (DataContext context = new DataContext(conn))
            {
                Item item = context.GetTable<Item>().FirstOrDefault(a => a.Id == id);
                if (item == null)
                    return null;

                //Nacita Parent mimo DataContext
                Item parent = item.Parent;

                return item;
            }
        }

        private bool ParseIdAndParams(string input, out int id)
        {
            string parameters;
            return ParseIdAndParams(input, out id, out parameters);
        }

        private bool ParseIdAndParams(string input, out int id, out string parameters)
        {
            string[] idParams = input.Split(new char[] { '_' }, 2, StringSplitOptions.RemoveEmptyEntries);
            if (!int.TryParse(idParams[0], out id))
            {
                parameters = string.Empty;
                return false;
            }

            parameters = idParams.Length > 1 ? idParams[1] : string.Empty;

            return true;
        }

        private void AddDirectoryInt(string path, string title)
        {
            path = path.TrimEnd('\\');

            if (title == null || title == string.Empty)
                title = path;

            FileSystemWatcher watcher = new FileSystemWatcher() {
                Path = path,
                EnableRaisingEvents = false,
                IncludeSubdirectories = true,
                NotifyFilter = NotifyFilters.DirectoryName | NotifyFilters.FileName | NotifyFilters.LastWrite,
            };
            watcher.Changed += new FileSystemEventHandler(Watcher_Changed);
            watcher.Deleted += new FileSystemEventHandler(Watcher_Deleted);
            watcher.Renamed += new RenamedEventHandler(Watcher_Renamed);
            watcher.Created += new FileSystemEventHandler(Watcher_Created);

            this.directories.Add(new Dir() { Path = path, Title = title, Watcher = watcher });

            //Vynechaju sa duplicitne adresare
            //Najkratsia cesta sa pouzije - s najmensim poctom znaku '\\', ostatne sa vynechaju
            bool isFirst = true;
            path = path.ToLower();
            foreach (Dir dir in this.directories.Where(a => a.Path.ToLower().StartsWith(path)).OrderBy(a => a.Path.Count(b => b == '\\')))
            {
                if (!isFirst)
                    dir.Skip = true;
                isFirst = false;

                if (this.realTimeDatabaseRefresh)
                {
                    try { dir.Watcher.EnableRaisingEvents = !dir.Skip; }
                    catch { }
                }
            }
        }

        private void RemoveDirectoryInt(string directory)
        {
            for (int i = 0; i < this.directories.Count; i++)
            {
                if (this.directories[i].Path == directory)
                {
                    this.directories[i].Watcher.Dispose();
                    this.directories.RemoveAt(i);
                    break;
                }
            }

            //Vynechaju sa duplicitne adresare
            foreach (Dir dir in this.directories)
            {
                dir.Skip = this.directories.Where(a => a != dir).Any(a => dir.Path.ToLower().StartsWith(a.Path.ToLower()));

                if (this.realTimeDatabaseRefresh)
                {
                    try { dir.Watcher.EnableRaisingEvents = !dir.Skip; }
                    catch { }
                }
            }
        }

        private void Watcher_Renamed(object sender, RenamedEventArgs e)
        {
            WatcherChanged(Path.GetDirectoryName(e.OldFullPath), false);
            WatcherChanged(e.FullPath, true);
        }

        private void Watcher_Deleted(object sender, FileSystemEventArgs e)
        {
            //Pri odstraneni sa obnovuje nadradeny adresar
            WatcherChanged(Path.GetDirectoryName(e.FullPath), false);
        }

        private void Watcher_Created(object sender, FileSystemEventArgs e)
        {
            //Pri vytvoreni sa obnovi nadradeny adresar ale metadata sa zistuju pre konkretny subor
            WatcherChanged(e.FullPath, true);
        }

        private void Watcher_Changed(object sender, FileSystemEventArgs e)
        {
            //Subor uz existuje - preto netreba aktualizovat nadradeny adresar cez GetDirectoryName
            WatcherChanged(e.FullPath, false);
        }

        private void WatcherChanged(string path, bool recursive)
        {
            if (this.watcherThread == null)
                return;

            //* je vzdy rekurzivna preto sa nenastavuje
            if (path == "*")
                recursive = false;

            lock (this.changedPaths)
            {
                if (recursive)
                {
                    //Ak je rekurzia - odstrani sa nerekurzivny path (ak existuje)
                    this.changedPaths.Remove(path);
                    this.changedPaths.Add('*' + path);
                }
                else if (!this.changedPaths.Contains('*' + path))
                {
                    //Ak nie je rekurzia a neexistuje ani rekurzivny path - prida sa nerekurzivny path
                    this.changedPaths.Add(path);
                }
                else
                {
                    return;
                }

                //Oznami vlaknu o zmene
                Monitor.Pulse(this.changedPaths);
            }
        }

        private void WatcherLoop()
        {
            while (true)
            {
                string path;
                Exception exc = null;

                lock (this.changedPaths)
                {
                    if (this.exitWatcherThread == false && this.changedPaths.Count < 1)
                        Monitor.Wait(this.changedPaths);

                    //Ak exitWatcherThread - koniec vlakna
                    if (this.exitWatcherThread)
                        return;

                    path = this.changedPaths.First();
                    this.changedPaths.Remove(path);
                }

                OnAsyncBuildDatabaseStart(null);

                if (path == "*")
                {
                    //Obnovenie celej databazy

                    try { BuildDatabase(); }
                    catch (Exception ex) { exc = ex; }
                }
                else
                {
                    //Niektore subory po udalosti changed este nie su uzavrete - pockat
                    //Pomoha aj znizenie priority vlakna
                    Thread.Sleep(500);

                    try
                    {
                        bool recursive;
                        if (path[0] == '*')
                        {
                            //Ak path zacina s '*' - jedna sa o rekurzivnu obnovu (ak je to mozne)
                            path = path.Substring(1);
                            recursive = true;
                        }
                        else
                        {
                            recursive = false;
                        }

                        BuildDatabaseSync(path, recursive);
                    }
                    catch (Exception ex) { exc = ex; }
                }

                OnAsyncBuildDatabaseEnd(exc);
            }
        }

        private void OnAsyncBuildDatabaseStart(Exception ex)
        {
            EventHandler<ExceptionEventArgs> handler = AsyncBuildDatabaseStart;
            if (handler != null)
            {
                handler(this, new ExceptionEventArgs(ex));
            }
        }

        private void OnAsyncBuildDatabaseEnd(Exception ex)
        {
            EventHandler<ExceptionEventArgs> handler = AsyncBuildDatabaseEnd;
            if (handler != null)
            {
                handler(this, new ExceptionEventArgs(ex));
            }
        }

        private static string CreateTable(MetaTable metaTable)
        {
            Dictionary<string, MetaDataMember> members = new Dictionary<string, MetaDataMember>();
            HashSet<string> associations = new HashSet<string>();
            StringBuilder tableDef = new StringBuilder();

            tableDef.AppendFormat("CREATE TABLE [{0}](", metaTable.TableName);

            IEnumerable<MetaType> types = new MetaType[] { metaTable.RowType };
            types = types.Concat(metaTable.RowType.DerivedTypes);

            foreach (MetaType type in types)
            {
                foreach (MetaDataMember member in type.DataMembers)
                {
                    if (!member.IsPersistent || member.Association != null)
                        continue;

                    if (member.DbType == null)
                        throw new Exception(string.Format("DbType not specified in member \"{0}\" in table \"{1}\".", member.MappedName, metaTable.TableName));

                    if (members.ContainsKey(member.MappedName))
                    {
                        MetaDataMember oldMember = members[member.MappedName];
                        if (oldMember.CanBeNull != member.CanBeNull || oldMember.DbType != member.DbType || oldMember.IsPrimaryKey != member.IsPrimaryKey)
                            throw new Exception(string.Format("Member \"{0}\"  in table \"{1}\" already specified.", member.MappedName, metaTable.TableName));
                    }
                    else
                    {
                        members[member.MappedName] = member;
                        tableDef.AppendFormat("[{0}] {1} {2},\r\n", member.MappedName, member.DbType, member.CanBeNull ? "NULL" : "NOT NULL");
                    }
                }
            }

            bool isPrimaryKey = false;
            foreach (MetaDataMember member in members.Values.Where(a => a.IsPrimaryKey))
            {
                if (!isPrimaryKey)
                    tableDef.Append("PRIMARY KEY (");

                tableDef.AppendFormat("[{0}], ", member.MappedName);
                isPrimaryKey = true;
            }

            if (isPrimaryKey)
            {
                tableDef.Remove(tableDef.Length - 2, 2);
                tableDef.Append("),\r\n");
            }

            foreach (MetaType type in types)
            {
                foreach (MetaAssociation assoc in type.Associations.Where(a => a.IsForeignKey))
                {
                    StringBuilder assocStr = new StringBuilder();

                    assocStr.Append("FOREIGN KEY (");

                    foreach (MetaDataMember member in assoc.ThisKey)
                        assocStr.AppendFormat("[{0}], ", member.MappedName);

                    assocStr.Remove(assocStr.Length - 2, 2);
                    assocStr.AppendFormat(") REFERENCES [{0}](", assoc.OtherType.Table.TableName);

                    foreach (MetaDataMember member in assoc.OtherKey)
                        assocStr.AppendFormat("[{0}], ", member.MappedName);

                    assocStr.Remove(assocStr.Length - 2, 2);
                    assocStr.Append(")");

                    associations.Add(assocStr.ToString());
                }
            }

            foreach (string assocStr in associations)
                tableDef.Append(assocStr + ",\r\n");

            tableDef.Remove(tableDef.Length - 3, 3);
            tableDef.Append(")");

            return tableDef.ToString();
        }
    }
}