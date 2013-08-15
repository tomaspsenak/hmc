using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.Linq;

namespace HomeMediaCenter
{
    public class ItemContainerStream : ItemContainer
    {
        public ItemContainerStream() : this(null) { }

        public ItemContainerStream(ItemContainer parent) : this("Stream", "Stream", parent) { }

        public ItemContainerStream(string title, string path, ItemContainer parent) : base(title, path, parent) { }

        public override bool Equals(object obj)
        {
            if (obj is string)
                return this.Title == (string)obj;
            return base.Equals(obj);
        }

        public override int GetHashCode()
        {
            if (this.Title == null)
                return 0;
            return this.Title.GetHashCode();
        }

        public override void RefresMe(DataContext context, IEnumerable<string> directories, HttpMimeDictionary mimeTypes, MediaSettings settings, HashSet<string> subtitleExt, bool recursive)
        {
            //Overenie ci existuje desktop container
            if (!this.Items.Any(a => a.Title == "Desktop"))
                new ItemContainerStream("Desktop", "desktop", this);

            //Rozdielova obnova webcams
            IEnumerable<string> webcams = DSWrapper.WebcamInput.GetVideoInputNames();
            Item[] toRemove = this.Items.Where(a => a.Title != "Desktop").Cast<object>().Except(
                    webcams, new ObjectEqualityComparer()).Cast<Item>().ToArray();
            string[] toAdd = webcams.Except(this.Items.Where(a => a.Title != "Desktop").Select(a => a.Title)).ToArray();

            foreach (Item item in toRemove)
            {
                this.Items.Remove(item);
                item.RemoveMe(context);
            }
            context.GetTable<Item>().DeleteAllOnSubmit(toRemove);

            foreach (string webcam in toAdd)
                new ItemContainerStream(webcam, "webcam_" + webcam, this);

            //Rozdielova obnova parametrov poloziek v kontajneroch
            IEnumerable<string> paramString = settings.StreamEncode.Select(a => a.GetParamString()).ToArray();
            foreach (ItemContainer container in this.Items)
            {
                toRemove = container.Items.Cast<object>().Except(paramString, new ObjectEqualityComparer()).Cast<Item>().ToArray();
                toAdd = paramString.Except(container.Items.OfType<ItemStream>().Select(a => a.SubtitlesPath)).ToArray();

                foreach (Item item in toRemove)
                {
                    container.Items.Remove(item);
                    item.RemoveMe(context);
                }
                context.GetTable<Item>().DeleteAllOnSubmit(toRemove);

                foreach (string param in toAdd)
                    new ItemStream(container.Title, container.Path, container, EncoderBuilder.GetEncoder(param));
            }

            //Nevolat obnovu do podadresarov - zmazu sa
        }
    }
}
