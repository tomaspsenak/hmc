using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml;
using System.Data.Linq;
using System.Data.Linq.Mapping;

namespace HomeMediaCenter
{
    public class ItemContainer : Item
    {
        protected class ObjectEqualityComparer : IEqualityComparer<object>
        {
            public new bool Equals(object x, object y) { return y.Equals(x); }

            public int GetHashCode(object obj) { return obj.GetHashCode(); }
        }

        protected class PathMime
        {
            public string Path
            {
                get; set;
            }

            public HttpMime Mime
            {
                get; set;
            }

            public override bool Equals(object obj)
            {
                if (obj is string)
                    return this.Path == (string)obj;
                return base.Equals(obj);
            }

            public override int GetHashCode()
            {
                if (this.Path == null)
                    return 0;
                return this.Path.GetHashCode();
            }
        }

        private EntitySet<Item> items = new EntitySet<Item>();

        public ItemContainer() : this(null, null, null) { }

        public ItemContainer(string title, string path, ItemContainer parent) : base(title, path, parent) { }

        [Association(Storage = "items", ThisKey = "Id", OtherKey = "parentId")]
        public EntitySet<Item> Items
        {
            get { return this.items; }
            set { this.items.Assign(value); }
        }

        [Column(IsPrimaryKey = false, DbType = "bit", CanBeNull = true)]
        public bool Audio
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "bit", CanBeNull = true)]
        public bool Image
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "bit", CanBeNull = true)]
        public bool Video
        {
            get; set;
        }

        public override IEnumerable<Item> Children
        {
            get { return this.items; }
        }

        public virtual void SetMediaType(MediaType type)
        {
            switch (type)
            {
                case MediaType.Audio: 
                    if (this.Audio)
                        return;
                    this.Audio = true;
                    break;
                case MediaType.Image:
                    if (this.Image)
                        return;
                    this.Image = true;
                    break;
                case MediaType.Video:
                    if (this.Video)
                        return;
                    this.Video = true;
                    break;
            }

            this.Parent.SetMediaType(type);
        }

        public virtual void CheckMediaType(MediaType type)
        {
            switch (type)
            {
                case MediaType.Audio:
                    if (this.Items.OfType<ItemContainer>().Any(a => a.Audio) || this.Items.OfType<ItemAudio>().Any())
                        return;
                    this.Audio = false;
                    break;
                case MediaType.Image:
                    if (this.Items.OfType<ItemContainer>().Any(a => a.Image) || this.Items.OfType<ItemImage>().Any())
                        return;
                    this.Image = false;
                    break;
                case MediaType.Video:
                    if (this.Items.OfType<ItemContainer>().Any(a => a.Video) || this.Items.OfType<ItemVideo>().Any())
                        return;
                    this.Video = false;
                    break;
            }

            this.Parent.CheckMediaType(type);
        }

        public virtual string GetParentId(MediaType type)
        {
            IEnumerable<Item> items = this.Parent.Items.Where(a => a.IsType(type));
            if (items.Count() == 1)
            {
                //Ak je jediny prvok adresar - vynecha sa a nastavi sa predchodca
                ItemContainer parent = items.OfType<ItemContainer>().FirstOrDefault();
                if (parent != null)
                    return this.Parent.GetParentId(type);
            }

            return this.Id.ToString();
        }

        public string GetItemCount(MediaType type)
        {
            IEnumerable<Item> items = this.Items.Where(a => a.IsType(type));
            int count = items.Count();

            if (count == 1)
            {
                //Ak je jediny prvok adresar - vynecha sa a spocita sa obsah nasledovnika
                ItemContainer child = items.OfType<ItemContainer>().FirstOrDefault();
                if (child != null)
                    return child.GetItemCount(type);
            }

            return count.ToString();
        }

        public override bool IsType(MediaType type)
        {
            switch (type)
            {
                case MediaType.Audio: return this.Audio;
                case MediaType.Image: return this.Image;
                case MediaType.Video: return this.Video;
                case MediaType.Other: return true;
                default: return false;
            }
        }

        public override void RefresMe(DataContext context, IEnumerable<string> directories, HttpMimeDictionary mimeTypes, MediaSettings settings, HashSet<string> subtitleExt, bool recursive)
        {
            try
            {
                //Rozdielova obnova adresarov
                directories = new DirectoryInfo(this.Path).GetDirectories().Where(a => (a.Attributes & FileAttributes.Hidden) == 0).Select(
                    a => a.FullName).ToArray();

                Item[] toRemove = this.Items.Where(a => a.GetType() == typeof(ItemContainer)).Cast<object>().Except(
                    directories, new ObjectEqualityComparer()).Cast<Item>().ToArray();
                string[] toAdd = directories.Except(this.Items.Where(a => a.GetType() == typeof(ItemContainer)).Select(a => a.Path)).ToArray();

                foreach (Item item in toRemove)
                {
                    this.Items.Remove(item);
                    item.RemoveMe(context);
                }
                context.GetTable<Item>().DeleteAllOnSubmit(toRemove);

                foreach (string path in toAdd)
                {
                    Item item = new ItemContainer(System.IO.Path.GetFileName(path), path, this);
                    //Ak nie je rekurzia - obnovia sa iba nove adresare
                    if (!recursive)
                        item.RefresMe(context, directories, mimeTypes, settings, subtitleExt, false);
                }

                //Rozdielova obnova suborov
                IEnumerable<PathMime> files = new DirectoryInfo(this.Path).GetFiles().Where(a => (a.Attributes & FileAttributes.Hidden) == 0).Select(
                    delegate(FileInfo a) {
                        HttpMime mime = mimeTypes[a.Extension];
                        if (mime == null || mime.MediaType == MediaType.Other)
                            return null;
                        return new PathMime() { Path = a.Name, Mime = mime };
                    }).Where(a => a != null).ToArray();

                toRemove = this.Items.Where(a => a.GetType() != typeof(ItemContainer)).Cast<object>().Except(
                    files.Select(a => a.Path), new ObjectEqualityComparer()).Cast<Item>().ToArray();
                PathMime[] toAddFile = files.Cast<object>().Except(this.Items.Where(a => a.GetType() != typeof(ItemContainer)).Select(
                    a => a.Path), new ObjectEqualityComparer()).Cast<PathMime>().ToArray();

                foreach (Item item in toRemove)
                {
                    this.Items.Remove(item);
                    item.RemoveMe(context);
                }
                context.GetTable<Item>().DeleteAllOnSubmit(toRemove);

                foreach (PathMime pathMime in toAddFile)
                {
                    try
                    {
                        FileInfo fi = new FileInfo(System.IO.Path.Combine(this.Path, pathMime.Path));
                        Item item = null;

                        switch (pathMime.Mime.MediaType)
                        {
                            case MediaType.Audio: item = new ItemAudio(fi, pathMime.Mime.ToString(), this); break;
                            case MediaType.Image: item = new ItemImage(fi, pathMime.Mime.ToString(), this); break;
                            case MediaType.Video: item = new ItemContainerVideo(fi, pathMime.Mime.ToString(), this); break;
                        }

                        //Ak nie je rekurzia - obnovia sa iba nove adresare a prvky
                        if (!recursive)
                            item.RefresMe(context, directories, mimeTypes, settings, subtitleExt, false);
                    }
                    catch { }
                }

                if (recursive)
                {
                    //Volanie obnovy do adresarov a prvkov
                    foreach (Item item in this.Items)
                        item.RefresMe(context, directories, mimeTypes, settings, subtitleExt, true);
                }
            }
            catch { }
        }

        public override void RemoveMe(DataContext context)
        {
            Item[] items = this.Items.ToArray();

            foreach (Item item in items)
            {
                //Najprv musi byt prvok odstraneny zo setu lebo nasledne RemoveMe overuje typy kontajnera
                this.Items.Remove(item);
                item.RemoveMe(context);
            }
            context.GetTable<Item>().DeleteAllOnSubmit(items);
        }

        public override void BrowseDirectChildren(XmlWriter xmlWriter, MediaSettings settings, string host, string idParams, HashSet<string> filterSet,
            uint startingIndex, uint requestedCount, string sortCriteria, out string numberReturned, out string totalMatches, ItemContainer skippedItem = null)
        {
            IEnumerable<Item> items;
            switch (idParams)
            {
                case AudioIndex: items = this.Items.Where(a => a.IsType(MediaType.Audio));
                    break;
                case ImageIndex: items = this.Items.Where(a => a.IsType(MediaType.Image));
                    break;
                case VideoIndex: items = this.Items.Where(a => a.IsType(MediaType.Video));
                    break;
                default: items = this.Items;
                    break;
            }

            totalMatches = items.Count().ToString();
            if (totalMatches == "1")
            {
                //Ak je jediny prvok adresar - vynecha sa a zobrazi sa obsah nasledovnika
                ItemContainer child = items.OfType<ItemContainer>().FirstOrDefault();
                if (child != null)
                {
                    child.BrowseDirectChildren(xmlWriter, settings, host, idParams, filterSet, startingIndex, requestedCount, sortCriteria,
                        out numberReturned, out totalMatches, skippedItem == null ? this : skippedItem);
                    return;
                }
            }

            foreach (string sortCrit in sortCriteria.ToLower().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries))
            {
                switch (sortCrit)
                {
                    case "+dc:title": items = (items is IOrderedEnumerable<Item>) ?
                        ((IOrderedEnumerable<Item>)items).ThenBy(a => a.Title) : items.OrderBy(a => a.Title);
                        break;
                    case "-dc:title": items = (items is IOrderedEnumerable<Item>) ?
                        ((IOrderedEnumerable<Item>)items).ThenByDescending(a => a.Title) : items.OrderByDescending(a => a.Title);
                        break;
                    case "+dc:date": items = (items is IOrderedEnumerable<Item>) ?
                        ((IOrderedEnumerable<Item>)items).ThenBy(a => a.Date.Date) : items.OrderBy(a => a.Date.Date);
                        break;
                    case "-dc:date": items = (items is IOrderedEnumerable<Item>) ?
                        ((IOrderedEnumerable<Item>)items).ThenByDescending(a => a.Date.Date) : items.OrderByDescending(a => a.Date.Date);
                        break;
                    default: break;
                }
            }

            uint count = 0;
            requestedCount = (requestedCount == 0) ? int.MaxValue : requestedCount;
            foreach (Item item in items.Skip((int)startingIndex).Take((int)requestedCount))
            {
                item.BrowseMetadata(xmlWriter, settings, host, idParams, filterSet, skippedItem == null ? this.Id.ToString() : skippedItem.Id.ToString());
                count++;
            }
            numberReturned = count.ToString();
        }

        public override void BrowseMetadata(XmlWriter xmlWriter, MediaSettings settings, string host, string idParams, HashSet<string> filterSet)
        {
            switch (idParams)
            {
                case AudioIndex: BrowseMetadata(xmlWriter, settings, host, idParams, filterSet, this.Parent.GetParentId(MediaType.Audio));
                    break;
                case ImageIndex: BrowseMetadata(xmlWriter, settings, host, idParams, filterSet, this.Parent.GetParentId(MediaType.Image));
                    break;
                case VideoIndex: BrowseMetadata(xmlWriter, settings, host, idParams, filterSet, this.Parent.GetParentId(MediaType.Video));
                    break;
                default: BrowseMetadata(xmlWriter, settings, host, idParams, filterSet, this.Parent.GetParentId(MediaType.Other));
                    break;
            }
        }

        public override void BrowseMetadata(XmlWriter xmlWriter, MediaSettings settings, string host, string idParams, HashSet<string> filterSet, string parentId)
        {
            switch (idParams)
            {
                case AudioIndex: WriteDIDL(xmlWriter, filterSet, this.Id.ToString() + "_1", parentId + "_1", GetItemCount(MediaType.Audio), 
                    this.Title, MediaType.Audio);
                    break;
                case ImageIndex: WriteDIDL(xmlWriter, filterSet, this.Id.ToString() + "_2", parentId + "_2", GetItemCount(MediaType.Image), 
                    this.Title, MediaType.Image);
                    break;
                case VideoIndex: WriteDIDL(xmlWriter, filterSet, this.Id.ToString() + "_3", parentId + "_3", GetItemCount(MediaType.Video), 
                    this.Title, MediaType.Video);
                    break;
                default: WriteDIDL(xmlWriter, filterSet, this.Id.ToString(), parentId, GetItemCount(MediaType.Other), 
                    this.Title, MediaType.Other);
                    break;
            }
        }

        public override void BrowseWebDirectChildren(XmlWriter xmlWriter, MediaSettings settings, string idParams)
        {
            IEnumerable<Item> items;
            switch (idParams)
            {
                case AudioIndex: items = this.Items.Where(a => a.IsType(MediaType.Audio));
                    break;
                case ImageIndex: items = this.Items.Where(a => a.IsType(MediaType.Image));
                    break;
                case VideoIndex: items = this.Items.Where(a => a.IsType(MediaType.Video));
                    break;
                default: items = this.Items;
                    break;
            }

            if (items.Count() == 1)
            {
                //Ak je jediny prvok adresar - vynecha sa a zobrazi sa obsah nasledovnika
                ItemContainer child = items.OfType<ItemContainer>().FirstOrDefault();
                if (child != null)
                {
                    child.BrowseWebDirectChildren(xmlWriter, settings, idParams);
                    return;
                }
            }

            xmlWriter.WriteStartElement("div");
            xmlWriter.WriteAttributeString("id", "main_content");

            switch (idParams)
            {
                case AudioIndex:

                    xmlWriter.WriteStartElement("table");

                    xmlWriter.WriteStartElement("thead");
                    xmlWriter.WriteStartElement("tr");
                    xmlWriter.WriteStartElement("th");
                    xmlWriter.WriteFullEndElement();
                    xmlWriter.WriteElementString("th", LanguageResource.Title);
                    xmlWriter.WriteElementString("th", LanguageResource.Duration);
                    xmlWriter.WriteElementString("th", LanguageResource.Artist);
                    xmlWriter.WriteElementString("th", LanguageResource.Album);
                    xmlWriter.WriteElementString("th", LanguageResource.Genre);
                    xmlWriter.WriteEndElement();
                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteStartElement("tbody");
                    foreach (Item item in items.OrderBy(a => !(a.IsType(MediaType.Other))).ThenBy(a => a.Title))
                    {
                        item.BrowseWebMetadata(xmlWriter, settings, idParams);
                    }
                    xmlWriter.WriteFullEndElement();

                    xmlWriter.WriteEndElement();
                    break;

                case ImageIndex:

                    xmlWriter.WriteStartElement("script");
                    xmlWriter.WriteAttributeString("type", "text/javascript");
                    xmlWriter.WriteValue("\r\n//");
                    xmlWriter.WriteCData(string.Format(@"
$(function () {{ $('#gallery a').lightBox({{ txtImage: '{0}', txtOf: '{1}', maxHeight: screen.height * 0.8, maxWidth: screen.width * 0.8 }}); }});
//", LanguageResource.Photo, LanguageResource.Of));
                    xmlWriter.WriteValue("\r\n");
                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteStartElement("div");
                    xmlWriter.WriteAttributeString("class", "galleryFolders");
                    foreach (Item item in items.Where(a => a.IsType(MediaType.Other)).OrderBy(a => a.Title))
                    {
                        item.BrowseWebMetadata(xmlWriter, settings, idParams);
                    }
                    xmlWriter.WriteFullEndElement();

                    xmlWriter.WriteStartElement("div");
                    xmlWriter.WriteAttributeString("class", "gallery");
                    xmlWriter.WriteAttributeString("id", "gallery");
                    foreach (Item item in items.Where(a => !(a.IsType(MediaType.Other))).OrderBy(a => a.Title))
                    {
                        item.BrowseWebMetadata(xmlWriter, settings, idParams);
                    }
                    xmlWriter.WriteFullEndElement();
                    break;

                case VideoIndex:

                    xmlWriter.WriteStartElement("table");

                    xmlWriter.WriteStartElement("thead");
                    xmlWriter.WriteStartElement("tr");
                    xmlWriter.WriteStartElement("th");
                    xmlWriter.WriteFullEndElement();
                    xmlWriter.WriteElementString("th", LanguageResource.Title);
                    xmlWriter.WriteElementString("th", LanguageResource.Duration);
                    xmlWriter.WriteElementString("th", LanguageResource.Resolution);
                    xmlWriter.WriteEndElement();
                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteStartElement("tbody");
                    foreach (Item item in items.OrderBy(a => !(a.IsType(MediaType.Other))).ThenBy(a => a.Title))
                    {
                        item.BrowseWebMetadata(xmlWriter, settings, idParams);
                    }
                    xmlWriter.WriteFullEndElement();

                    xmlWriter.WriteEndElement();
                    break;

                default: xmlWriter.WriteElementString("p", LanguageResource.UnknownID);
                    break;
            }

            xmlWriter.WriteEndElement();
        }

        public override void BrowseWebMetadata(XmlWriter xmlWriter, MediaSettings settings, string idParams)
        {
            if (idParams == AudioIndex || idParams == VideoIndex)
            {
                xmlWriter.WriteStartElement("tr");

                xmlWriter.WriteStartElement("td");
                xmlWriter.WriteRaw(@"<img src=""/web/images/folder.png"" alt="""" title="""" border=""0"" width=""18"" height=""18"" align=""right"" />");
                xmlWriter.WriteEndElement();

                xmlWriter.WriteStartElement("td");
                xmlWriter.WriteStartElement("a");
                xmlWriter.WriteAttributeString("href", "/web/page.html?id=" + this.Id + "_" + idParams);
                xmlWriter.WriteValue(this.Title);
                xmlWriter.WriteEndElement();
                xmlWriter.WriteEndElement();

                if (idParams == AudioIndex)
                {
                    xmlWriter.WriteStartElement("td");
                    xmlWriter.WriteFullEndElement();
                    xmlWriter.WriteStartElement("td");
                    xmlWriter.WriteFullEndElement();
                }
                xmlWriter.WriteStartElement("td");
                xmlWriter.WriteFullEndElement();
                xmlWriter.WriteStartElement("td");
                xmlWriter.WriteFullEndElement();

                xmlWriter.WriteEndElement();
            }
            else if (idParams == ImageIndex)
            {
                xmlWriter.WriteStartElement("a");

                xmlWriter.WriteAttributeString("href", "/web/page.html?id=" + this.Id + "_" + idParams);
                xmlWriter.WriteRaw(@"<img src=""/web/images/folder.png"" alt="""" />");
                xmlWriter.WriteStartElement("span");
                xmlWriter.WriteValue(this.Title);
                xmlWriter.WriteEndElement();

                xmlWriter.WriteEndElement();
            }
        }

        protected static void WriteDIDL(XmlWriter xmlWriter, HashSet<string> filterSet, string id, string parentID, string childCount, string title, MediaType mediaType)
        {
            xmlWriter.WriteStartElement("container");

            xmlWriter.WriteAttributeString("id", id);
            xmlWriter.WriteAttributeString("restricted", "true");
            xmlWriter.WriteAttributeString("parentID", parentID);
            xmlWriter.WriteAttributeString("childCount", childCount);
            xmlWriter.WriteAttributeString("searchable", "false");

            //Povinne hodnoty
            xmlWriter.WriteElementString("dc", "title", null, title);
            xmlWriter.WriteElementString("upnp", "class", null, "object.container");

            //Volitelne hodnoty
            if (filterSet == null || filterSet.Contains("upnp:writeStatus"))
                xmlWriter.WriteElementString("upnp", "writeStatus", null, "NOT_WRITABLE");

            switch (mediaType)
            {
                case MediaType.Audio:
                    if (filterSet == null || filterSet.Contains("upnp:searchClass"))
                    {
                        xmlWriter.WriteStartElement("upnp", "searchClass", null);
                        xmlWriter.WriteAttributeString("includeDerived", "true");
                        xmlWriter.WriteValue("object.item.audioItem");
                        xmlWriter.WriteEndElement();
                    }

                    if (filterSet == null || filterSet.Contains("av:mediaClass"))
                    {
                        xmlWriter.WriteElementString("av", "mediaClass", null, "M");
                    }

                    break;
                case MediaType.Image:
                    if (filterSet == null || filterSet.Contains("upnp:searchClass"))
                    {
                        xmlWriter.WriteStartElement("upnp", "searchClass", null);
                        xmlWriter.WriteAttributeString("includeDerived", "true");
                        xmlWriter.WriteValue("object.item.imageItem");
                        xmlWriter.WriteEndElement();
                    }

                    if (filterSet == null || filterSet.Contains("av:mediaClass"))
                    {
                        xmlWriter.WriteElementString("av", "mediaClass", null, "P");
                    }

                    break;
                case MediaType.Video:
                    if (filterSet == null || filterSet.Contains("upnp:searchClass"))
                    {
                        xmlWriter.WriteStartElement("upnp", "searchClass", null);
                        xmlWriter.WriteAttributeString("includeDerived", "true");
                        xmlWriter.WriteValue("object.item.videoItem");
                        xmlWriter.WriteEndElement();
                    }

                    if (filterSet == null || filterSet.Contains("av:mediaClass"))
                    {
                        xmlWriter.WriteElementString("av", "mediaClass", null, "V");
                    }

                    break;
                default: break;
            }

            xmlWriter.WriteEndElement();
        }
    }
}
