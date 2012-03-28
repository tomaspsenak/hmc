using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace HomeMediaCenter
{
    [Serializable]
    public abstract class Item
    {
        private uint id;
        private readonly string title;
        private readonly ItemContainer parent;

        public Item(string title, ItemContainer parent)
        {
            this.title = title;
            this.parent = parent;
        }

        public uint Id
        {
            get { return this.id; }
            set { this.id = value; }
        }

        public string Title
        {
            get { return this.title; }
        }

        public ItemContainer Parent
        {
            get { return this.parent; }
        }

        public virtual DateTime Date
        {
            get { return new DateTime(0); }
        }

        public virtual string Path
        {
            get { return null; }
        }

        public virtual string SubtitlesPath
        {
            get { return null; }
        }

        public virtual string Mime
        {
            get { return null; }
        }

        public virtual TimeSpan? Duration
        {
            get { return null; }
        }

        public virtual string GetFileFeature(MediaSettings settings)
        {
            return string.Empty;
        }

        public virtual string GetEncodeFeature(MediaSettings settings)
        {
            return string.Empty;
        }

        public virtual void RefresMe(ItemManager manager, IEnumerable<string> directories, HttpMimeDictionary mimeTypes,
            MediaSettings settings, HashSet<string> subtitleExt)
        {
        }

        public virtual IEnumerable<Item> GetChilds()
        {
            return Enumerable.Empty<Item>();
        }

        public abstract void WriteMe(XmlWriter writer, MediaSettings settings, string host, HashSet<string> filterSet);

        public abstract void WriteMe(XmlWriter xmlWriter, MediaSettings settings);
    }
}
