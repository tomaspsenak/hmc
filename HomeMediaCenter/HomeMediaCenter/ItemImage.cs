using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;

namespace HomeMediaCenter
{
    [Serializable]
    public class ItemImage : Item
    {
        private readonly string path;
        private readonly string mime;
        private readonly DateTime date;
        private readonly long length;
        private readonly string resolution;

        public ItemImage(ItemContainer parent, FileInfo file, string mime) : base(file.Name, parent)
        {
            this.path = file.FullName;
            this.mime = mime;
            this.date = file.LastWriteTime;
            this.length = file.Length;
            this.resolution = FileHelper.GetImageResolution(file);
        }

        public override DateTime Date
        {
            get { return this.date; }
        }

        public override string Path
        {
            get { return this.path; }
        }

        public override string Mime
        {
            get { return this.mime; }
        }

        public override string GetFileFeature(MediaSettings settings)
        {
            return (this.mime == "image/jpeg" ? "DLNA.ORG_PN=JPEG_MED;" : string.Empty) + settings.ImageFileFeature;
        }

        public override string GetEncodeFeature(MediaSettings settings)
        {
            return settings.ImageEncodeFeature;
        }

        public override void WriteMe(XmlWriter writer, MediaSettings settings, string host, HashSet<string> filterSet)
        {
            writer.WriteStartElement("item");

            writer.WriteAttributeString("id", Id.ToString());
            writer.WriteAttributeString("restricted", "true");
            writer.WriteAttributeString("parentID", Parent == null ? "-1" : Parent.Id.ToString());

            //Povinne hodnoty
            writer.WriteElementString("dc", "title", null, Title);
            writer.WriteElementString("upnp", "class", null, "object.item.imageItem");

            //Volitelne hodnoty
            if (filterSet == null || filterSet.Contains("dc:date"))
                writer.WriteElementString("dc", "date", null, this.date.ToString("yyyy-MM-dd"));

            if (filterSet == null || filterSet.Any(a => a.StartsWith("res")))
            {
                if (settings.ImageNativeFile)
                {
                    writer.WriteStartElement("res");

                    if (filterSet == null || filterSet.Contains("res@size"))
                        writer.WriteAttributeString("size", this.length.ToString());

                    if (this.resolution != string.Empty && (filterSet == null || filterSet.Contains("res@resolution")))
                        writer.WriteAttributeString("resolution", this.resolution);

                    writer.WriteAttributeString("protocolInfo", string.Format("http-get:*:{0}:{1}", this.mime, GetFileFeature(settings)));
                    writer.WriteValue(host + "/files/image?id=" + Id);
                    writer.WriteEndElement();
                }

                foreach (MediaSettingsImage sett in settings.ImageEncode)
                {
                    writer.WriteStartElement("res");

                    if (sett.Resolution != null && (filterSet == null || filterSet.Contains("res@resolution")))
                        writer.WriteAttributeString("resolution", sett.Resolution);

                    writer.WriteAttributeString("protocolInfo", string.Format("http-get:*:{0}:{1}", sett.Mime, settings.ImageEncodeFeature));
                    writer.WriteValue(host + "/encode/image?id=" + Id + sett.QueryString);
                    writer.WriteEndElement();
                }
            }

            writer.WriteEndElement();
        }

        public override void WriteMe(XmlWriter xmlWriter, MediaSettings settings)
        {
            xmlWriter.WriteStartElement("a");
            xmlWriter.WriteAttributeString("href", "/files/image?id=" + Id);
            xmlWriter.WriteStartElement("img");
            xmlWriter.WriteAttributeString("src", "/encode/image?id=" + Id + "&codec=jpeg&width=50&height=50&quality=10");
            xmlWriter.WriteAttributeString("alt", Title);
            xmlWriter.WriteEndElement();
            xmlWriter.WriteEndElement();
        }
    }
}
