using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.Linq;

namespace HomeMediaCenter
{
    public class ItemContainerStreamRoot : ItemContainerStream
    {
        protected class TitleItemEqualityComparer : ItemEqualityComparer<Item>
        {
            public override object GetValue(Item item) { return item.Title; }
        }

        public ItemContainerStreamRoot() : base() { }

        public ItemContainerStreamRoot(ItemContainer parent) : base("Stream", "Stream", parent) { }

        public override void RefresMe(DataContext context, ItemManager manager, bool recursive)
        {
            try
            {
                //Rozdielova obnova webcams
                IEnumerable<string> webcams = manager.EnableWebcamStreaming ? DSWrapper.WebcamInput.GetVideoInputNames() : Enumerable.Empty<string>();
                List<Item> toRemove = this.Items.Where(a => a.GetType() == typeof(ItemContainerStream) && a.Title != "Desktop").Cast<object>().Except(
                        webcams, new TitleItemEqualityComparer()).Cast<Item>().ToList();
                string[] toAdd = webcams.Except(this.Items.Where(a => a.GetType() == typeof(ItemContainerStream) && a.Title != "Desktop").Select(
                    a => a.Title)).ToArray();

                //Overenie ci existuje desktop container ak je zapnuty
                if (manager.EnableDesktopStreaming)
                {
                    if (!this.Items.Any(a => a.GetType() == typeof(ItemContainerStream) && a.Title == "Desktop"))
                        new ItemContainerStream("Desktop", "desktop", this);
                }
                else
                {
                    toRemove.AddRange(this.Items.Where(a => a.GetType() == typeof(ItemContainerStream) && a.Title == "Desktop"));
                }

                foreach (Item item in toRemove)
                {
                    this.Items.Remove(item);
                    item.RemoveMe(context);
                }
                context.GetTable<Item>().DeleteAllOnSubmit(toRemove);

                foreach (string webcam in toAdd)
                    new ItemContainerStream(webcam, "webcam_" + webcam, this);

                //Volanie obnovy do podadresarov - bez ohladu na premennu recursive - napr. treba obnovit aj nazvy
                foreach (Item item in this.Items)
                    item.RefresMe(context, manager, true);
            }
            catch { }
        }
    }
}
