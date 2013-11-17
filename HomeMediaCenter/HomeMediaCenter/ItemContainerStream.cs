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
        protected class SubtitlesPathItemEqualityComparer : ItemEqualityComparer<ItemStream>
        {
            public override object GetValue(ItemStream item) { return item.SubtitlesPath; }
        }

        public ItemContainerStream() : base() { }

        public ItemContainerStream(string title, string path, ItemContainer parent) : base(title, path, parent) { }

        public override void RefresMe(DataContext context, ItemManager manager, bool recursive)
        {
            //Rozdielova obnova parametrov poloziek v tomto kontajnery
            IEnumerable<string> paramString = manager.MediaSettings.Stream.Encode.Select(a => a.GetParamString()).ToArray();

            Item[] toRemove = this.Items.Cast<object>().Except(paramString, new SubtitlesPathItemEqualityComparer()).Cast<Item>().ToArray();
            string[] toAdd = paramString.Except(this.Items.OfType<ItemStream>().Select(a => a.SubtitlesPath)).ToArray();

            foreach (Item item in toRemove)
            {
                this.Items.Remove(item);
                item.RemoveMe(context);
            }
            context.GetTable<Item>().DeleteAllOnSubmit(toRemove);

            foreach (string param in toAdd)
                new ItemStream(this.Title, this, EncoderBuilder.GetEncoder(param));
        }
    }
}
