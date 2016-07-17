using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.Linq;

namespace HomeMediaCenter
{
    public class ItemContainerDreamboxRoot : ItemContainerDreambox
    {
        public ItemContainerDreamboxRoot() : base() { }

        public ItemContainerDreamboxRoot(string title, string path, ItemContainer parent) : base(title, path, parent) { }

        public override void RefreshMe(DataContext context, ItemManager manager, bool recursive)
        {
            if (manager.UpnpDevice.Stopping)
                return;

            try
            {
                Uri url;
                if (!Uri.TryCreate(this.Path, UriKind.Absolute, out url))
                    return;

                //Obnoveie bouquets
                string bouquetsPrefix = null;
                RefreshDreambox(context, manager, url, true, ref bouquetsPrefix);

                string servicesPrefix = null;
                foreach (ItemContainerDreambox item in this.Items.OfType<ItemContainerDreambox>())
                {
                    //Obnovenie services v bouquet
                    item.RefreshDreambox(context, manager, url, false, ref servicesPrefix);
                }

                //Volanie obnovy do podadresarov - bez ohladu na premennu recursive - napr. treba obnovit aj nazvy
                foreach (Item item in this.Items)
                    item.RefreshMe(context, manager, true);
            }
            catch { }
        }
    }
}
