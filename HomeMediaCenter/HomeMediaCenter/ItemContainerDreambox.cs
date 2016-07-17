using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.Linq;
using System.Xml;
using System.Net;

namespace HomeMediaCenter
{
    public class ItemContainerDreambox : ItemContainerStream
    {
        protected class ServiceParamItemEqualityComparer : IEqualityComparer<object>
        {
            public new bool Equals(object x, object y)
            {
                ServiceParam itemX;
                Item itemY = y as Item;

                if (itemY == null)
                {
                    itemX = y as ServiceParam;
                    itemY = x as Item;
                }
                else
                {
                    itemX = x as ServiceParam;
                }

                return (itemX.Title == itemY.Title) && (itemX.Path == itemY.Path);
            }

            public int GetHashCode(object obj)
            {
                Item itemY = obj as Item;

                if (itemY == null)
                {
                    ServiceParam itemX = obj as ServiceParam;
                    if (itemX == null)
                        throw new NullReferenceException();

                    return itemX.Title.GetHashCode() ^ itemX.Path.GetHashCode();
                }
                else
                {
                    return itemY.Title.GetHashCode() ^ itemY.Path.GetHashCode();
                }
            }
        }

        protected class ServiceParam
        {
            public string Title
            {
                get; set;
            }

            public string Path
            {
                get; set;
            }
        }

        public ItemContainerDreambox() : base() { }

        public ItemContainerDreambox(string title, string path, ItemContainer parent) : base(title, path, parent) { }

        public override void RefreshMe(DataContext context, ItemManager manager, bool recursive)
        {
            if (manager.UpnpDevice.Stopping)
                return;

            foreach (Item item in this.Items)
                item.RefreshMe(context, manager, true);
        }

        public void RefreshDreambox(DataContext context, ItemManager manager, Uri basePath, bool isBouquet, ref string pathPrefix)
        {
            Uri servicePath = new Uri(basePath, "/web/getservices" + (isBouquet ? string.Empty : ("?sRef=" + this.Path)));

            XmlDocument serviceDoc = new XmlDocument();
            serviceDoc.Load(servicePath.ToString());

            string pPrefix;
            if (isBouquet)
                pPrefix = string.Empty;
            else if (pathPrefix == null)
                pPrefix = pathPrefix = GetDreamboxStreamAddress(basePath, serviceDoc);
            else
                pPrefix = pathPrefix;


            //Rozdielova obnova parametrov poloziek v tomto kontajnery
            IEnumerable<ServiceParam> serviceParams = serviceDoc.SelectNodes("/e2servicelist/e2service").Cast<XmlNode>().Select(
                a => new ServiceParam() { Title = a.SelectSingleNode("e2servicename").InnerText, Path = pPrefix + a.SelectSingleNode("e2servicereference").InnerText }
                ).ToArray();

            Item[] toRemove = this.Items.Except(serviceParams, new ServiceParamItemEqualityComparer()).Cast<Item>().ToArray();
            ServiceParam[] toAdd = serviceParams.Except(this.Items, new ServiceParamItemEqualityComparer()).Cast<ServiceParam>().ToArray();

            RemoveRange(context, manager, toRemove);

            foreach (ServiceParam param in toAdd)
            {
                if (isBouquet)
                    new ItemContainerDreambox(param.Title, param.Path, this);
                else
                    new ItemContainerStream(param.Title, param.Path, this);
            }
        }

        private string GetDreamboxStreamAddress(Uri baseUri, XmlDocument serviceDoc)
        {
            try
            {
                //Pokusi sa ziskat stream port na zaklade prveho e2service
                string serviceRef = serviceDoc.SelectSingleNode("/e2servicelist/e2service/e2servicereference").InnerText;
                Uri playlistPath = new Uri(baseUri, "/web/stream.m3u?ref=" + serviceRef);

                using (WebClient client = new WebClient())
                using (System.IO.Stream stream = client.OpenRead(playlistPath))
                using (System.IO.StreamReader reader = new System.IO.StreamReader(stream))
                {
                    string line;
                    while ((line = reader.ReadLine()) != null)
                    {
                        if (!line.StartsWith("#"))
                            break;
                    }

                    Uri streamPath;
                    if (Uri.TryCreate(line, UriKind.Absolute, out streamPath))
                        return streamPath.GetLeftPart(UriPartial.Authority) + "/";
                }
            }
            catch { }

            //Ak nie je najdeny odkaz nastav default port
            return string.Format("http://{0}:{1}/", baseUri.Host, 8001);
        }
    }
}
