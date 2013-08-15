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
    public class ItemManager
    {
        private class Dir
        {
            public string Path { get; set; }
            public bool Skip { get; set; }
            public FileSystemWatcher Watcher { get; set; }
        }

        private bool realTimeDatabaseRefresh = true;
        private List<Dir> directories = new List<Dir>();
        private MediaSettings settings = new MediaSettings();
        private HttpMimeDictionary mimeTypes = HttpMime.GetDefaultMime();
        private HashSet<string> subtitleExt = new HashSet<string>(StringComparer.OrdinalIgnoreCase) {
            ".srt", ".sub", ".ssa", ".txt" };

        private readonly HashSet<string> changedPaths = new HashSet<string>();
        private Thread watcherThread;
        
        private readonly ReaderWriterLockSlim rwLock = new ReaderWriterLockSlim();
        private string dbConnectionString;

        private readonly Action asyncBuildDatabaseDel;

        public event EventHandler<ExceptionEventArgs> AsyncBuildDatabaseStart;
        public event EventHandler<ExceptionEventArgs> AsyncBuildDatabaseEnd;

        public ItemManager(MediaServerDevice upnpDevice)
        {
            this.asyncBuildDatabaseDel = new Action(BuildDatabaseSync);
        }

        public MediaSettings MediaSettings
        {
            get { return this.settings; }
        }

        public bool RealTimeDatabaseRefresh
        {
            get { return this.realTimeDatabaseRefresh; }
            set
            {
                this.realTimeDatabaseRefresh = value;

                this.rwLock.EnterWriteLock();
                try
                {
                    foreach (Dir dir in this.directories)
                    {
                        try { dir.Watcher.EnableRaisingEvents = value; }
                        catch { }
                    }
                }
                finally { this.rwLock.ExitWriteLock(); }
            }
        }

        public string[] AddDirectorySync(string directory)
        {
            this.rwLock.EnterWriteLock();
            try
            {
                AddDirectory(directory);
                return this.directories.Select(a => a.Path).ToArray();
            }
            finally { this.rwLock.ExitWriteLock(); }
        }

        public string[] RemoveDirectorySync(string directory)
        {
            this.rwLock.EnterWriteLock();
            try
            {
                RemoveDirectory(directory);
                return this.directories.Select(a => a.Path).ToArray();
            }
            finally { this.rwLock.ExitWriteLock(); }
        }

        public string[] GetDirectoriesSync()
        {
            this.rwLock.EnterReadLock();
            try
            {
                return this.directories.Select(a => a.Path).ToArray();
            }
            finally { this.rwLock.ExitReadLock(); }
        }

        public void AddSubtitleExtensionSync(string extension)
        {
            this.rwLock.EnterWriteLock();
            try
            {
                this.subtitleExt.Add(extension);
            }
            finally { this.rwLock.ExitWriteLock(); }
        }

        public void BuildDatabaseAsync()
        {
            OnAsyncBuildDatabaseStart(null);
            this.asyncBuildDatabaseDel.BeginInvoke(new AsyncCallback(AsyncBuildDatabaseResult), null);
        }

        public void BuildDatabaseSync()
        {
            this.rwLock.EnterWriteLock();
            try
            {
                using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
                using (DataContext context = new DataContext(conn))
                {
                    Item root = context.GetTable<Item>().Single(a => a.Id == 0);
                    root.RefresMe(context, GetWorkingDirectories(), this.mimeTypes, this.settings, this.subtitleExt, true);

                    context.SubmitChanges();
                }
            }
            finally { this.rwLock.ExitWriteLock(); }
        }

        public void BuildDatabaseSync(string path)
        {
            path = path.TrimEnd('\\');

            this.rwLock.EnterWriteLock();
            try
            {
                using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
                using (DataContext context = new DataContext(conn))
                {
                    Item item = context.GetTable<Item>().FirstOrDefault(a => a.Path == path);
                    if (item == null)
                    {
                        //Pokusi sa najst nadradeny adresar - kontajner
                        string subPath = Path.GetFileName(path);
                        path = Path.GetDirectoryName(path);

                        item = context.GetTable<Item>().FirstOrDefault(a => a.Path == path);
                        if (item != null)
                        {
                            Item children = item.Children.FirstOrDefault(a => a.Path == subPath);
                            if (children == null)
                            {
                                //V nadradenom adresari (kontajner) sa nasiel subor (polozka) 
                                item.RefresMe(context, GetWorkingDirectories(), this.mimeTypes, this.settings, this.subtitleExt, false);
                                context.SubmitChanges();
                            }
                            else
                            {
                                //Adresar alebo subor este neboli pridane - obnov nadradeny adresar
                                children.RefresMe(context, GetWorkingDirectories(), this.mimeTypes, this.settings, this.subtitleExt, false);
                                context.SubmitChanges();
                            }
                        }
                    }
                    else
                    {
                        //Ak sa nasla polozka s povodnou cestou - celu cestu ma iba kontajner
                        item.RefresMe(context, GetWorkingDirectories(), this.mimeTypes, this.settings, this.subtitleExt, false);
                        context.SubmitChanges();
                    }
                }
            }
            finally { this.rwLock.ExitWriteLock(); }
        }

        internal void Start()
        {
            if (this.watcherThread == null)
            {
                lock (this.changedPaths)
                {
                    this.changedPaths.Clear();
                }

                this.watcherThread = new Thread(new ThreadStart(WatcherLoop)) { IsBackground = true, Priority = ThreadPriority.BelowNormal };
                this.watcherThread.Start();
            }
        }

        internal void Stop()
        {
            if (this.watcherThread != null)
            {
                lock (this.changedPaths)
                {
                    //Ukonci vlakon - pulse pri 0 prvkoch v changedDirectories
                    this.changedPaths.Clear();
                    Monitor.Pulse(this.changedPaths);
                }

                this.watcherThread.Join();
                this.watcherThread = null;
            }
        }

        internal void SaveSettingsSync(XmlWriter xmlWriter)
        {
            this.rwLock.EnterReadLock();
            try
            {
                xmlWriter.WriteElementString("RealTimeDatabaseRefresh", this.realTimeDatabaseRefresh.ToString());

                xmlWriter.WriteStartElement("Directories");
                foreach (string dir in this.directories.Select(a => a.Path))
                    xmlWriter.WriteElementString("string", dir);
                xmlWriter.WriteEndElement();

                xmlWriter.WriteStartElement("Subtitles");
                foreach (string ext in this.subtitleExt)
                    xmlWriter.WriteElementString("string", ext);
                xmlWriter.WriteEndElement();
                
                this.settings.SaveSettings(xmlWriter);
            }
            finally { this.rwLock.ExitReadLock(); }
        }

        internal void LoadSync(XmlDocument xmlReader, string databasePath)
        {
            this.rwLock.EnterWriteLock();
            try
            {
                if (xmlReader != null)
                {
                    XmlNode node = xmlReader.SelectSingleNode("/HomeMediaCenter/RealTimeDatabaseRefresh");
                    if (node != null)
                        bool.TryParse(node.InnerText, out this.realTimeDatabaseRefresh);

                    foreach (Dir dir in this.directories)
                        dir.Watcher.Dispose();
                    this.directories.Clear();
                    foreach (XmlNode directory in xmlReader.SelectNodes("/HomeMediaCenter/Directories/*"))
                        AddDirectory(directory.InnerText);

                    this.subtitleExt.Clear();
                    foreach (XmlNode subtitle in xmlReader.SelectNodes("/HomeMediaCenter/Subtitles/*"))
                        this.subtitleExt.Add(subtitle.InnerText);

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
            finally { this.rwLock.ExitWriteLock(); }
        }

        internal bool GetFile(string objectID, out string path, out string mime, out string fileFeature)
        {
            int id;
            string[] idParams = objectID.Split(new char[] { '_' }, 2, StringSplitOptions.RemoveEmptyEntries);
            if (!int.TryParse(idParams[0], out id))
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
            string[] idParams = objectID.Split(new char[] { '_' }, 2, StringSplitOptions.RemoveEmptyEntries);
            if (!int.TryParse(idParams[0], out id))
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

        internal void Browse(Dictionary<string, string> headers, string objectID, BrowseFlag browseFlag, out string result)
        {
            string numberReturned, totalMatches;
            Browse(headers, objectID, browseFlag, "*", 0, 0, string.Empty, out result, out numberReturned, out totalMatches);
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
            string[] idParams = objectID.Split(new char[] { '_' }, 2, StringSplitOptions.RemoveEmptyEntries);
            if (!int.TryParse(idParams[0], out id))
                throw new SoapException(402, "Invalid Args");

            using (XmlWriter writer = XmlWriter.Create(sb, new XmlWriterSettings() { OmitXmlDeclaration = true }))
            {
                writer.WriteStartElement("DIDL-Lite", "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/");
                writer.WriteAttributeString("xmlns", "dc", null, "http://purl.org/dc/elements/1.1/");
                writer.WriteAttributeString("xmlns", "upnp", null, "urn:schemas-upnp-org:metadata-1-0/upnp/");
                writer.WriteAttributeString("xmlns", "av", null, "urn:schemas-sony-com:av");

                using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
                using (DataContext context = new DataContext(conn))
                {
                    Item mainObject = context.GetTable<Item>().FirstOrDefault(a => a.Id == id);
                    if (mainObject == null)
                        throw new SoapException(701, "No such object");

                    if (browseFlag == BrowseFlag.BrowseMetadata)
                    {
                        mainObject.BrowseMetadata(writer, this.settings, host, idParams.Length > 1 ? idParams[1] : string.Empty, filterSet);
                        numberReturned = "1";
                        totalMatches = "1";
                    }
                    else
                    {
                        mainObject.BrowseDirectChildren(writer, this.settings, host, idParams.Length > 1 ? idParams[1] : string.Empty, filterSet,
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
            string[] idParams = objectID.Split(new char[] { '_' }, 2, StringSplitOptions.RemoveEmptyEntries);
            if (!int.TryParse(idParams[0], out id))
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
                    item.BrowseWebDirectChildren(xmlWriter, this.settings, idParams.Length > 1 ? idParams[1] : string.Empty);
                }
            }
        }

        internal void GetWebPlayer(string objectID, XmlWriter xmlWriter, Dictionary<string, string> urlParams)
        {
            int id;
            string[] idParams = objectID.Split(new char[] { '_' }, 2, StringSplitOptions.RemoveEmptyEntries);
            if (!int.TryParse(idParams[0], out id))
                throw new HttpException(402, "GetWebPlayer - parse exception");

            using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
            using (DataContext context = new DataContext(conn))
            {
                Item item = context.GetTable<Item>().FirstOrDefault(a => a.Id == id);
                if (item != null)
                {
                    item.GetWebPlayer(xmlWriter, urlParams);
                }
            }
        }

        internal string GetTitle(string objectID)
        {
            int id;
            string[] idParams = objectID.Split(new char[] { '_' }, 2, StringSplitOptions.RemoveEmptyEntries);
            if (!int.TryParse(idParams[0], out id))
                throw new HttpException(402, "GetTitle - parse exception");

            using (SqlCeConnection conn = new SqlCeConnection(this.dbConnectionString))
            using (DataContext context = new DataContext(conn))
            {
                Item item = context.GetTable<Item>().FirstOrDefault(a => a.Id == id);
                if (item != null)
                    return item.Title;
            }

            return string.Empty;
        }

        private void AddDirectory(string directory)
        {
            directory = directory.TrimEnd('\\');

            FileSystemWatcher watcher = new FileSystemWatcher() {
                Path = directory,
                EnableRaisingEvents = false,
                IncludeSubdirectories = true,
                NotifyFilter = NotifyFilters.DirectoryName | NotifyFilters.FileName | NotifyFilters.LastWrite,
            };
            watcher.Changed += new FileSystemEventHandler(Watcher_Changed);
            watcher.Deleted += new FileSystemEventHandler(Watcher_Deleted);
            watcher.Renamed += new RenamedEventHandler(Watcher_Renamed);
            watcher.Created += new FileSystemEventHandler(Watcher_Created);

            this.directories.Add(new Dir() { Path = directory, Watcher = watcher });

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

        private void RemoveDirectory(string directory)
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

        private IEnumerable<string> GetWorkingDirectories()
        {
            return this.directories.Where(a => !a.Skip).Select(a => a.Path);
        }

        private void Watcher_Renamed(object sender, RenamedEventArgs e)
        {
            WatcherChanged(Path.GetDirectoryName(e.FullPath));
        }

        private void Watcher_Deleted(object sender, FileSystemEventArgs e)
        {
            WatcherChanged(Path.GetDirectoryName(e.FullPath));
        }

        private void Watcher_Created(object sender, FileSystemEventArgs e)
        {
            WatcherChanged(Path.GetDirectoryName(e.FullPath));
        }

        private void Watcher_Changed(object sender, FileSystemEventArgs e)
        {
            //Subor uz existuje - preto netreba aktualizovat nadradeny adresar cez GetDirectoryName
            WatcherChanged(e.FullPath);
        }

        private void WatcherChanged(string path)
        {
            lock (this.changedPaths)
            {
                this.changedPaths.Add(path);
                //Oznami vlaknu o zmene
                Monitor.Pulse(this.changedPaths);
            }
        }

        private void WatcherLoop()
        {
            while (true)
            {
                string path;

                lock (this.changedPaths)
                {
                    if (this.changedPaths.Count < 1)
                        Monitor.Wait(this.changedPaths);
                    //Ak po Pulse nema changedDirectories prvky - koniec vlakna
                    if (this.changedPaths.Count < 1)
                        return;

                    path = this.changedPaths.First();
                    this.changedPaths.Remove(path);
                }

                OnAsyncBuildDatabaseStart(null);

                //Niektore subory po udalosti changed este nie su uzavrete - pockat
                //Pomoha aj znizenie priority vlakna
                Thread.Sleep(500);

                Exception exc = null;
                try { BuildDatabaseSync(path); }
                catch (Exception ex) { exc = ex; }

                OnAsyncBuildDatabaseEnd(exc);
            }
        }

        private void AsyncBuildDatabaseResult(IAsyncResult result)
        {
            Exception exc = null;
            try { this.asyncBuildDatabaseDel.EndInvoke(result); }
            catch (Exception ex) { exc = ex; }

            OnAsyncBuildDatabaseEnd(exc);
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