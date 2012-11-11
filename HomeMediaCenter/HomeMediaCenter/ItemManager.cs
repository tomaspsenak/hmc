using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading;
using System.Xml;
using System.Runtime.Serialization.Formatters.Binary;
using System.Runtime.Serialization;
using System.Xml.Serialization;

namespace HomeMediaCenter
{
    [Serializable]
    public class ItemManager
    {
        private class Dir
        {
            public string Path { get; set; }
            public bool Skip { get; set; }
        }

        [NonSerialized]
        private List<Dir> directories;
        [NonSerialized]
        private HttpMimeDictionary mimeTypes;
        [NonSerialized]
        private HashSet<string> subtitleExt;

        private SortedDictionary<uint, Item> itemsId = new SortedDictionary<uint, Item>();

        private ItemContainer rootItem;
        private ItemContainer audioItem;
        private ItemContainer imageItem;
        private ItemContainer videoItem;
        private ItemContainer streamItem;

        private uint lastItemId = 0;
        private LinkedList<uint> removedItemIds = new LinkedList<uint>();

        [NonSerialized]
        private MediaSettings settings;
        [NonSerialized]
        private ReaderWriterLockSlim rwLock;

        public ItemManager()
        {
            Initialize();

            AddItem(this.rootItem = new ItemContainer("Root", null, MediaType.Other));
            AddItem(this.audioItem = new ItemContainer("Audio", this.rootItem, MediaType.Audio));
            AddItem(this.imageItem = new ItemContainer("Image", this.rootItem, MediaType.Image));
            AddItem(this.videoItem = new ItemContainer("Video", this.rootItem, MediaType.Video));
            AddItem(this.streamItem = new ItemContainerStream("Stream", this.videoItem, MediaType.Video));
        }

        public MediaSettings MediaSettings
        {
            get { return this.settings; }
        }

        public void SaveSettingsSync(XmlWriter xmlWriter)
        {
            this.rwLock.EnterReadLock();
            try
            {
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

        public void LoadSettingsSync(XmlDocument xmlReader)
        {
            this.rwLock.EnterWriteLock();
            try
            {
                this.directories.Clear();
                foreach (XmlNode directory in xmlReader.SelectNodes("/HomeMediaCenter/Directories/*"))
                    AddDirectory(directory.InnerText);

                this.subtitleExt.Clear();
                foreach (XmlNode subtitle in xmlReader.SelectNodes("/HomeMediaCenter/Subtitles/*"))
                    this.subtitleExt.Add(subtitle.InnerText);

                this.settings.LoadSettings(xmlReader);
            }
            finally { this.rwLock.ExitWriteLock(); }
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

        public bool GetFileSync(uint index, out string path, out string mime, out string fileFeature)
        {
            this.rwLock.EnterReadLock();
            try
            {
                if (!this.itemsId.ContainsKey(index))
                {
                    path = null;
                    mime = null;
                    fileFeature = null;
                    return false;
                }

                if ((path = this.itemsId[index].Path) == null)
                {
                    mime = null;
                    fileFeature = null;
                    return false;
                }

                Item item = this.itemsId[index];
                if ((mime = item.Mime) == null)
                {
                    fileFeature = null;
                    return false;
                }

                fileFeature = item.GetFileFeature(this.settings);
            }
            finally { this.rwLock.ExitReadLock(); }

            return true;
        }

        public bool GetEncodeSync(uint index, out string path, out string subtitlesPath, out string encodeFeature, out TimeSpan? duration)
        {
            this.rwLock.EnterReadLock();
            try
            {
                if (!this.itemsId.ContainsKey(index))
                {
                    path = null;
                    subtitlesPath = null;
                    encodeFeature = null;
                    duration = null;
                    return false;
                }

                Item item = this.itemsId[index];
                if ((path = item.Path) == null)
                {
                    subtitlesPath = null;
                    encodeFeature = null;
                    duration = null;
                    return false;
                }

                subtitlesPath = item.SubtitlesPath;
                encodeFeature = item.GetEncodeFeature(this.settings);
                duration = item.Duration;
            }
            finally { this.rwLock.ExitReadLock(); }

            return true;
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

        public void BuildDatabaseSync()
        {
            this.rwLock.EnterWriteLock();
            try
            {
                IEnumerable<string> directories = this.directories.Where(a => !a.Skip).Select(a => a.Path);

                this.audioItem.RefresMe(this, directories, this.mimeTypes, this.settings, this.subtitleExt);
                this.imageItem.RefresMe(this, directories, this.mimeTypes, this.settings, this.subtitleExt);
                this.videoItem.RefresMe(this, directories, this.mimeTypes, this.settings, this.subtitleExt);
                this.streamItem.RefresMe(this, directories, this.mimeTypes, this.settings, this.subtitleExt);
            }
            finally { this.rwLock.ExitWriteLock(); }
        }

        public void BrowseSync(Dictionary<string, string> headers, uint objectID, BrowseFlag browseFlag, out string result)
        {
            string numberReturned, totalMatches;
            BrowseSync(headers, objectID, browseFlag, "*", 0, 0, string.Empty, out result, out numberReturned, out totalMatches);
        }

        public void BrowseSync(Dictionary<string, string> headers, uint objectID, BrowseFlag browseFlag, string filter, uint startingIndex,
            uint requestedCount, string sortCriteria, out string result, out string numberReturned, out string totalMatches)
        {
            string host = headers.ContainsKey("host") ? "http://" + headers["host"] : string.Empty;
            StringBuilder sb = new StringBuilder();

            HashSet<string> filterSet = null;
            if (filter != "*")
                filterSet = new HashSet<string>(filter.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries), StringComparer.OrdinalIgnoreCase);

            using (XmlWriter writer = XmlWriter.Create(sb, new XmlWriterSettings() { OmitXmlDeclaration = true }))
            {
                writer.WriteStartElement("DIDL-Lite", "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/");
                writer.WriteAttributeString("xmlns", "dc", null, "http://purl.org/dc/elements/1.1/");
                writer.WriteAttributeString("xmlns", "upnp", null, "urn:schemas-upnp-org:metadata-1-0/upnp/");
                writer.WriteAttributeString("xmlns", "av", null, "urn:schemas-sony-com:av");

                this.rwLock.EnterReadLock();
                try
                {
                    if (!this.itemsId.ContainsKey(objectID))
                        throw new HttpException(500, "Browse out of range");
                    Item mainObject = this.itemsId[objectID];

                    if (browseFlag == BrowseFlag.BrowseMetadata)
                    {
                        mainObject.WriteMe(writer, this.settings, host, filterSet);
                        numberReturned = "1";
                        totalMatches = "1";
                    }
                    else
                    {
                        IEnumerable<Item> items = mainObject.GetChilds();
                        foreach (string sortCrit in sortCriteria.ToLower().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries))
                            items = GetSortedItems(items, sortCrit);

                        uint count = 0;
                        requestedCount = (requestedCount == 0) ? int.MaxValue : requestedCount; 
                        foreach (Item item in items.Skip((int)startingIndex).Take((int)requestedCount))
                        {
                            item.WriteMe(writer, this.settings, host, filterSet);
                            count++;
                        }

                        numberReturned = count.ToString();
                        totalMatches = mainObject.GetChilds().Count().ToString();
                    }
                }
                finally { this.rwLock.ExitReadLock(); }

                writer.WriteEndElement();
            }
            
            result = sb.ToString();
        }

        public void GetWebSync(uint objectID, XmlWriter xmlWriter)
        {
            this.rwLock.EnterReadLock();
            try
            {
                if (objectID == 0)
                {
                    xmlWriter.WriteStartElement("div");
                    xmlWriter.WriteAttributeString("class", "green_box");
                    xmlWriter.WriteStartElement("div");
                    xmlWriter.WriteAttributeString("id", "right_nav");
                    xmlWriter.WriteStartElement("ul");

                    xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/page.html?id=2"" title="""">{0}</a></li>", LanguageResource.Photos));
                    xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/page.html?id=1"" title="""">{0}</a></li>", LanguageResource.Music));
                    xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/page.html?id=3"" title="""">{0}</a></li>", LanguageResource.Video));
                    xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/page.html?id=4"" title="""">{0}</a></li>", LanguageResource.Stream));
                    xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/control.html"" title="""">{0}</a></li>", LanguageResource.Control));

                    xmlWriter.WriteEndElement();
                    xmlWriter.WriteEndElement();
                    xmlWriter.WriteEndElement();
                }
                else
                {
                    xmlWriter.WriteStartElement("div");
                    xmlWriter.WriteAttributeString("id", "main_content");

                    if (this.itemsId.ContainsKey(objectID))
                        this.itemsId[objectID].WriteMe(xmlWriter, this.settings);
                    else
                        xmlWriter.WriteElementString("p", LanguageResource.UnknownID);

                    xmlWriter.WriteEndElement();
                }
            }
            finally { this.rwLock.ExitReadLock(); }
        }

        public string GetTitleSync(uint objectID)
        {
            this.rwLock.EnterReadLock();
            try
            {
                if (this.itemsId.ContainsKey(objectID))
                    return this.itemsId[objectID].Title;
            }
            finally { this.rwLock.ExitReadLock(); }

            return string.Empty;
        }

        public void SerializeDatabaseSync(string path)
        {
            using (FileStream stream = new FileStream(path, FileMode.Create, FileAccess.Write))
            {
                BinaryFormatter binFormat = new BinaryFormatter();
                this.rwLock.EnterReadLock();
                try
                {
                    binFormat.Serialize(stream, this);
                }
                finally { this.rwLock.ExitReadLock(); }
            }
        }

        public bool RemoveItem(Item item)
        {
            this.removedItemIds.AddFirst(item.Id);

            return this.itemsId.Remove(item.Id);
        }

        public void AddItem(Item item)
        {
            item.Id = GetItemId();
            this.itemsId[item.Id] = item;
            if (item.Parent != null)
                item.Parent.AddItem(item);
        }

        private void AddDirectory(string directory)
        {
            if (!directory.EndsWith("\\"))
                directory += "\\";

            this.directories.Add(new Dir() { Path = directory });

            //Vynechaju sa duplicitne adresare
            foreach (Dir dir in this.directories)
                dir.Skip = this.directories.Where(a => a != dir).Any(a => dir.Path.ToLower().StartsWith(a.Path.ToLower()));
        }

        private void RemoveDirectory(string directory)
        {
            for (int i = 0; i < this.directories.Count; i++)
            {
                if (this.directories[i].Path == directory)
                {
                    this.directories.RemoveAt(i);
                    break;
                }
            }

            //Vynechaju sa duplicitne adresare
            foreach (Dir dir in this.directories)
                dir.Skip = this.directories.Where(a => a != dir).Any(a => dir.Path.ToLower().StartsWith(a.Path.ToLower()));
        }

        private uint GetItemId()
        {
            uint id;
            if (this.removedItemIds.First == null)
            {
                id = this.lastItemId++;
            }
            else
            {
                id = this.removedItemIds.First.Value;
                this.removedItemIds.RemoveFirst();
            }

            return id;
        }

        [OnDeserialized()]
        private void OnSerializingMethod(StreamingContext context)
        {
            Initialize();
        }

        private void Initialize()
        {
            this.directories = new List<Dir>();
            this.mimeTypes = HttpMime.GetDefaultMime();
            this.settings = new MediaSettings();
            this.rwLock = new ReaderWriterLockSlim();

            this.subtitleExt = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
            this.subtitleExt.Add(".srt");
            this.subtitleExt.Add(".sub");
            this.subtitleExt.Add(".ssa");
            this.subtitleExt.Add(".txt");
        }

        private static IEnumerable<Item> GetSortedItems(IEnumerable<Item> items, string sortCrit)
        {
            switch (sortCrit)
            {
                case "+dc:title": return (items is IOrderedEnumerable<Item>) ?
                    ((IOrderedEnumerable<Item>)items).ThenBy(a => a.Title) : items.OrderBy(a => a.Title);
                case "-dc:title": return (items is IOrderedEnumerable<Item>) ?
                    ((IOrderedEnumerable<Item>)items).ThenByDescending(a => a.Title) : items.OrderByDescending(a => a.Title);
                case "+dc:date": return (items is IOrderedEnumerable<Item>) ?
                    ((IOrderedEnumerable<Item>)items).ThenBy(a => a.Date.Date) : items.OrderBy(a => a.Date.Date);
                case "-dc:date": return (items is IOrderedEnumerable<Item>) ?
                    ((IOrderedEnumerable<Item>)items).ThenByDescending(a => a.Date.Date) : items.OrderByDescending(a => a.Date.Date);
                default: break;
            }
            return items;
        }
    }
}
