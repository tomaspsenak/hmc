using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.Linq;
using System.Xml;

namespace HomeMediaCenter
{
    public class ItemContainerRoot : ItemContainer
    {
        private class PathNameItemEqualityComparer : ItemEqualityComparer<ItemManager.Dir, Item>
        {
            public override object GetValueX(ItemManager.Dir item) { return item.Path + item.Title; }
            public override object GetValueY(Item item) { return item.Path + item.Title; }
        }

        private static readonly KeyValuePair<string, string>[] rootCategories = new KeyValuePair<string, string>[] { 
            new KeyValuePair<string, string>(AudioIndex, "Audio"), new KeyValuePair<string, string>(ImageIndex, "Image"), 
            new KeyValuePair<string, string>(VideoIndex, "Video")
        };

        public ItemContainerRoot() : base("Root", null, null) { }

        public override void SetMediaType(MediaType type) { /* Zastavenie nastavovania pri korenovom prvku */ }

        public override void CheckMediaType(MediaType type) { /* Zastavenie zistovania pri korenovom prvku */ }

        public override ItemContainer GetParentItem(MediaType type) { return this; }

        public override void RefreshMe(DataContext context, ItemManager manager, bool recursive)
        {
            if (manager.UpnpDevice.Stopping)
                return;

            //Overenie ci existuje stream container
            if (!this.Items.Any(a => a.GetType() == typeof(ItemContainerStreamRoot)))
                new ItemContainerStreamRoot(this);

            //Rozdielova obnova adresarov zadanych uzivatelom
            IEnumerable<ItemManager.Dir> directories = manager.GetWorkingDirectories();
            Item[] toRemove = this.Items.Where(a => a.GetType() == typeof(ItemContainer)).Except(directories, 
                new PathNameItemEqualityComparer()).Cast<Item>().ToArray();
            ItemManager.Dir[] toAdd = directories.Except(this.Items.Where(a => a.GetType() == typeof(ItemContainer)),
                new PathNameItemEqualityComparer()).Cast<ItemManager.Dir>().ToArray();

            RemoveRange(context, manager, toRemove);

            foreach (ItemManager.Dir dir in toAdd)
            {
                //Title musi byt bez znaku '\' - problem so Samsung DLNA
                new ItemContainer(dir.Title.Replace(":\\", "_").Replace('\\', '_'), dir.Path, this);
            }

            if (recursive)
            {
                //Volanie obnovy do podadresarov
                foreach (Item item in this.Items)
                    item.RefreshMe(context, manager, true);
            }
        }

        public override void BrowseDirectChildren(XmlWriter xmlWriter, MediaSettings settings, string host, string idParams, HashSet<string> filterSet,
            uint startingIndex, uint requestedCount, string sortCriteria, out string numberReturned, out string totalMatches, ItemContainer skippedItem = null)
        {
            if (idParams == string.Empty)
            {
                IEnumerable<KeyValuePair<string, string>> items = rootCategories;

                foreach (string sortCrit in sortCriteria.ToLower().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries))
                {
                    switch (sortCrit)
                    {
                        case "+dc:title": items = (items is IOrderedEnumerable<KeyValuePair<string, string>>) ?
                            ((IOrderedEnumerable<KeyValuePair<string, string>>)items).ThenBy(a => a.Value) : items.OrderBy(a => a.Value);
                            break;
                        case "-dc:title": items = (items is IOrderedEnumerable<KeyValuePair<string, string>>) ?
                            ((IOrderedEnumerable<KeyValuePair<string, string>>)items).ThenByDescending(a => a.Value) : items.OrderByDescending(a => a.Value);
                            break;
                        default: break;
                    }
                }

                uint count = 0;
                requestedCount = (requestedCount == 0) ? int.MaxValue : requestedCount;
                foreach (KeyValuePair<string, string> item in items.Skip((int)startingIndex).Take((int)requestedCount))
                {
                    BrowseMetadata(xmlWriter, settings, host, item.Key, filterSet, 0);
                    count++;
                }

                numberReturned = count.ToString();
                totalMatches = "3";
            }
            else
            {
                base.BrowseDirectChildren(xmlWriter, settings, host, idParams, filterSet, 
                    startingIndex, requestedCount, sortCriteria, out numberReturned, out totalMatches);
            }
        }

        public override void BrowseMetadata(XmlWriter xmlWriter, MediaSettings settings, string host, string idParams, HashSet<string> filterSet)
        {
            BrowseMetadata(xmlWriter, settings, host, idParams, filterSet, 0);
        }

        public override void BrowseMetadata(XmlWriter xmlWriter, MediaSettings settings, string host, string idParams, HashSet<string> filterSet, int parentId)
        {
            //parentId nepouzivat

            switch (idParams)
            {
                case AudioIndex: WriteDIDL(xmlWriter, filterSet, this.Id.ToString() + "_" + idParams, this.Id.ToString(), 
                    GetItemCount(MediaType.Audio), "Audio", MediaType.Audio);
                    break;
                case ImageIndex: WriteDIDL(xmlWriter, filterSet, this.Id.ToString() + "_" + idParams, this.Id.ToString(),
                    GetItemCount(MediaType.Image), "Image", MediaType.Image);
                    break;
                case VideoIndex: WriteDIDL(xmlWriter, filterSet, this.Id.ToString() + "_" + idParams, this.Id.ToString(),
                    GetItemCount(MediaType.Video), "Video", MediaType.Video);
                    break;
                default : WriteDIDL(xmlWriter, filterSet, this.Id.ToString(), "-1", "3", this.Title, MediaType.Other);
                    break;
            }
        }

        public override void BrowseWebDirectChildren(XmlWriter xmlWriter, MediaSettings settings, string idParams)
        {
            if (idParams == string.Empty)
            {
                xmlWriter.WriteStartElement("div");
                xmlWriter.WriteAttributeString("class", "green_box");
                xmlWriter.WriteStartElement("div");
                xmlWriter.WriteAttributeString("id", "right_nav");
                xmlWriter.WriteStartElement("ul");

                xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/page.html?id=0_2"" title="""">{0}</a></li>", LanguageResource.Photos));
                xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/page.html?id=0_1"" title="""">{0}</a></li>", LanguageResource.Music));
                xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/page.html?id=0_3"" title="""">{0}</a></li>", LanguageResource.Video));
                xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/control.html"" title="""">{0}</a></li>", LanguageResource.Control));

                xmlWriter.WriteEndElement();
                xmlWriter.WriteEndElement();
                xmlWriter.WriteEndElement();
            }
            else
            {
                base.BrowseWebDirectChildren(xmlWriter, settings, idParams);
            }
        }
    }
}
