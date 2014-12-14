using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.Linq;

namespace HomeMediaCenter
{
    public class ItemContainerStreamRoot : ItemContainerStream
    {
        protected class TitleItemEqualityComparer : ItemEqualityComparer<string, Item>
        {
            public override object GetValueX(string item) { return item; }
            public override object GetValueY(Item item) { return item.Title; }
        }

        public ItemContainerStreamRoot() : base() { }

        public ItemContainerStreamRoot(ItemContainer parent) : base("Stream", "Stream", parent) { }

        public override void RefreshMe(DataContext context, ItemManager manager, bool recursive)
        {
            if (manager.UpnpDevice.Stopping)
                return;

            try
            {
                //Rozdielova obnova webcams
                IEnumerable<string> webcams = manager.EnableWebcamStreaming ? DSWrapper.WebcamInput.GetVideoInputNames() : Enumerable.Empty<string>();
                List<Item> toRemove = this.Items.Where(a => a.GetType() == typeof(ItemContainerStream) && a.Title != "Desktop").Except(
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

                RemoveRange(context, manager, toRemove.ToArray());

                foreach (string webcam in toAdd)
                    new ItemContainerStream(webcam, "webcam_" + webcam, this);

                //Volanie obnovy do podadresarov - bez ohladu na premennu recursive - napr. treba obnovit aj nazvy
                foreach (Item item in this.Items)
                    item.RefreshMe(context, manager, true);
            }
            catch { }
        }
    }
}
