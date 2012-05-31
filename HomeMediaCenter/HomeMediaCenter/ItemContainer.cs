using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml;

namespace HomeMediaCenter
{
    [Serializable]
    public class ItemContainer : Item
    {
        private class PathMime : IEquatable<PathMime>
        {
            public string Path
            {
                get; set;
            }

            public string Mime
            {
                get; set;
            }

            public string Subtitles
            {
                get; set;
            }

            public bool Equals(PathMime other)
            {
                return Path == other.Path && Subtitles == other.Subtitles;
            }

            public override int GetHashCode()
            {
                if (Subtitles == null)
                    return Path.GetHashCode();
                return Path.GetHashCode() + Subtitles.GetHashCode();
            }
        }

        protected List<Item> childs = new List<Item>();
        protected MediaType mediaType;

        public ItemContainer(string title, ItemContainer parent, MediaType mediaType) : base(title, parent)
        {
            this.mediaType = mediaType;
        }

        public void AddItem(Item item)
        {
            this.childs.Add(item);
        }

        public override void RefresMe(ItemManager manager, IEnumerable<string> directories, HttpMimeDictionary mimeTypes,
            MediaSettings settings, HashSet<string> subtitleExt)
        {
            if (this.mediaType == MediaType.Other)
                return;

            IEnumerable<PathMime> files = Enumerable.Empty<PathMime>();
            foreach (string directory in directories)
            {
                files = files.Concat(FileHelper.GetFiles(directory, false).Select(delegate(string a) {
                    HttpMime mime = mimeTypes[System.IO.Path.GetExtension(a)];
                    if (mime == null || mime.MediaType != this.mediaType)
                        return null;
                    return new PathMime() { Path = a, Mime = mime.ToString() };
                }).Where(a => a != null));
            }
            
            if (this.mediaType == MediaType.Video)
            {
                LinkedList<PathMime> subtitleFiles = new LinkedList<PathMime>();

                foreach (PathMime file in files)
                {
                    DirectoryInfo fileDir = Directory.GetParent(file.Path);
                    string fileName = System.IO.Path.GetFileNameWithoutExtension(file.Path);
                    foreach (FileInfo subtitle in fileDir.GetFiles(fileName + ".*").Where(a => subtitleExt.Contains(a.Extension)))
                    {
                        subtitleFiles.AddLast(new PathMime() { Mime = file.Mime, Path = file.Path, Subtitles = subtitle.FullName });
                        break;
                    }
                }

                files = files.Concat(subtitleFiles);
            }

            PathMime[] toRemove = this.childs.Where(a => a.Path != null).Select(a => new PathMime() {
                Path = a.Path, Subtitles = a.SubtitlesPath }).Except(files).ToArray();
            PathMime[] toAdd = files.Except(this.childs.Where(a => a.Path != null).Select(a => new PathMime() { 
                Path = a.Path, Subtitles = a.SubtitlesPath })).ToArray();

            foreach (PathMime file in toRemove)
            {
                for (int i = 0; i < this.childs.Count; i++)
                {
                    if (this.childs[i].Path == file.Path && this.childs[i].SubtitlesPath == file.Subtitles)
                    {
                        Item item = this.childs[i];
                        this.childs.RemoveAt(i);
                        manager.RemoveItem(item);
                    }
                }
            }

            foreach (PathMime file in toAdd)
            {
                FileInfo fi = new FileInfo(file.Path);

                switch (this.mediaType)
                {
                    case MediaType.Audio: manager.AddItem(new ItemAudio(this, fi, file.Mime));
                        break;
                    case MediaType.Image: manager.AddItem(new ItemImage(this, fi, file.Mime));
                        break;
                    case MediaType.Video: manager.AddItem(new ItemVideo(this, fi, file.Mime, file.Subtitles));
                        break;
                }
            }
        }

        public override IEnumerable<Item> GetChilds()
        {
            return this.childs;
        }

        public override void WriteMe(XmlWriter writer, MediaSettings settings, string host, HashSet<string> filterSet)
        {
            writer.WriteStartElement("container");

            writer.WriteAttributeString("id", Id.ToString());
            writer.WriteAttributeString("restricted", "true");
            writer.WriteAttributeString("parentID", Parent == null ? "-1" : Parent.Id.ToString());
            writer.WriteAttributeString("childCount", this.childs.Count.ToString());
            writer.WriteAttributeString("searchable", "false");

            //Povinne hodnoty
            writer.WriteElementString("dc", "title", null, Title);
            writer.WriteElementString("upnp", "class", null, "object.container");

            //Volitelne hodnoty
            if (filterSet == null || filterSet.Contains("upnp:writeStatus"))
                writer.WriteElementString("upnp", "writeStatus", null, "NOT_WRITABLE");

            switch (this.mediaType)
            {
                case MediaType.Audio:
                    if (filterSet == null || filterSet.Contains("upnp:searchClass"))
                    {
                        writer.WriteStartElement("upnp", "searchClass", null);
                        writer.WriteAttributeString("includeDerived", "true");
                        writer.WriteValue("object.item.audioItem");
                        writer.WriteEndElement();
                    }

                    if (filterSet == null || filterSet.Contains("av:mediaClass"))
                    {
                        writer.WriteElementString("av", "mediaClass", null, "M");
                    }

                    break;
                case MediaType.Image:
                    if (filterSet == null || filterSet.Contains("upnp:searchClass"))
                    {
                        writer.WriteStartElement("upnp", "searchClass", null);
                        writer.WriteAttributeString("includeDerived", "true");
                        writer.WriteValue("object.item.imageItem");
                        writer.WriteEndElement();
                    }

                    if (filterSet == null || filterSet.Contains("av:mediaClass"))
                    {
                        writer.WriteElementString("av", "mediaClass", null, "P");
                    }

                    break;
                case MediaType.Video:
                    if (filterSet == null || filterSet.Contains("upnp:searchClass"))
                    {
                        writer.WriteStartElement("upnp", "searchClass", null);
                        writer.WriteAttributeString("includeDerived", "true");
                        writer.WriteValue("object.item.videoItem");
                        writer.WriteEndElement();
                    }

                    if (filterSet == null || filterSet.Contains("av:mediaClass"))
                    {
                        writer.WriteElementString("av", "mediaClass", null, "V");
                    }

                    break;
                default: break;
            }

            writer.WriteEndElement();
        }

        public override void WriteMe(XmlWriter xmlWriter, MediaSettings settings)
        {
            switch (this.mediaType)
            {
                case MediaType.Audio:

                    xmlWriter.WriteStartElement("table");

                    xmlWriter.WriteStartElement("thead");
                    xmlWriter.WriteStartElement("tr");
                    xmlWriter.WriteElementString("th", "");
                    xmlWriter.WriteElementString("th", LanguageResource.Title);
                    xmlWriter.WriteElementString("th", LanguageResource.Duration);
                    xmlWriter.WriteElementString("th", LanguageResource.Artist);
                    xmlWriter.WriteElementString("th", LanguageResource.Album);
                    xmlWriter.WriteElementString("th", LanguageResource.Genre);
                    xmlWriter.WriteEndElement();
                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteStartElement("tbody");
                    foreach (Item item in this.childs.OrderBy(a => a.Title))
                    {
                        item.WriteMe(xmlWriter, settings);
                    }
                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteEndElement();

                    break;
                case MediaType.Image:

                    xmlWriter.WriteStartElement("script");
                    xmlWriter.WriteAttributeString("type", "text/javascript");
                    xmlWriter.WriteValue("\r\n//");
                    xmlWriter.WriteCData(string.Format(@"
$(function () {{ $('.gallery a').lightBox({{ txtImage: '{0}', txtOf: '{1}', maxHeight: screen.height * 0.8, maxWidth: screen.width * 0.8 }}); }});
//", LanguageResource.Photo, LanguageResource.Of));
                    xmlWriter.WriteValue("\r\n");
                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteStartElement("div");
                    xmlWriter.WriteAttributeString("class", "gallery");
                    foreach (Item item in this.childs.OrderBy(a => a.Title))
                    {
                        item.WriteMe(xmlWriter, settings);
                    }
                    xmlWriter.WriteEndElement();

                    break;
                case MediaType.Video:

                    xmlWriter.WriteStartElement("table");

                    xmlWriter.WriteStartElement("thead");
                    xmlWriter.WriteStartElement("tr");
                    xmlWriter.WriteElementString("th", "");
                    xmlWriter.WriteElementString("th", LanguageResource.Title);
                    xmlWriter.WriteElementString("th", LanguageResource.Duration);
                    xmlWriter.WriteElementString("th", LanguageResource.Resolution);
                    xmlWriter.WriteEndElement();
                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteStartElement("tbody");
                    foreach (Item item in this.childs.OrderBy(a => a.Title))
                    {
                        item.WriteMe(xmlWriter, settings);
                    }
                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteEndElement();
                    
                    break;
                default: break;
            }
        }
    }
}
