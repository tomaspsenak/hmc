using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.Linq;
using System.Data.Linq.Mapping;
using System.Xml;

namespace HomeMediaCenter
{
    public class ItemContainerStream : ItemContainer
    {
        protected class SubtitlesPathItemEqualityComparer : ItemEqualityComparer<string, ItemStream>
        {
            public override object GetValueX(string item) { return item; }
            public override object GetValueY(ItemStream item) { return item.SubtitlesPath; }
        }

        public ItemContainerStream() : base() { }

        public ItemContainerStream(string title, string path, ItemContainer parent) : base(title, path, parent) { }

        public override void RefreshMe(DataContext context, ItemManager manager, bool recursive)
        {
            if (manager.UpnpDevice.Stopping)
                return;

            //Rozdielova obnova parametrov poloziek v tomto kontajnery
            IEnumerable<string> paramString = manager.MediaSettings.Stream.Encode.Select(a => a.GetParamString()).ToArray();

            Item[] toRemove = this.Items.Except(paramString, new SubtitlesPathItemEqualityComparer()).Cast<Item>().ToArray();
            string[] toAdd = paramString.Except(this.Items.OfType<ItemStream>().Select(a => a.SubtitlesPath)).ToArray();

            RemoveRange(context, manager, toRemove);

            foreach (string param in toAdd)
                new ItemStream(this.Title, this, EncoderBuilder.GetEncoder(param));
        }

        public override void RefreshMetadata(DataContext context, ItemManager manager, bool recursive) { }
    }
}
