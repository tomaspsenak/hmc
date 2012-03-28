using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace HomeMediaCenter
{
    [Serializable]
    public class ItemVideoStream : Item
    {
        private readonly string path;
        private readonly string mime;
        private readonly DateTime date;
        private readonly string bitrate;
        private readonly string resolution;
        private readonly uint width;
        private readonly uint height;
        private readonly string queryString;

        public ItemVideoStream(ItemContainer parent, string path, string title, MediaSettingsVideo settings)
            : base(string.Format("{0} {1} {2}kBps", title, settings.Resolution, settings.VidBitrate), parent)
        {
            this.path = path;
            this.mime = settings.Mime;
            this.date = DateTime.Now;
            this.bitrate = settings.VidBitrate;
            this.resolution = settings.Resolution;
            this.width = settings.Width;
            this.height = settings.Height;
            this.queryString = settings.QueryString;
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

        public override string GetEncodeFeature(MediaSettings settings)
        {
            return settings.VideoStreamFeature;
        }

        public override void WriteMe(XmlWriter writer, MediaSettings settings, string host, HashSet<string> filterSet)
        {
            writer.WriteStartElement("item");

            writer.WriteAttributeString("id", Id.ToString());
            writer.WriteAttributeString("restricted", "true");
            writer.WriteAttributeString("parentID", Parent == null ? "-1" : Parent.Id.ToString());

            //Povinne hodnoty
            writer.WriteElementString("dc", "title", null, Title);
            writer.WriteElementString("upnp", "class", null, "object.item.videoItem");

            //Volitelne hodnoty
            if (filterSet == null || filterSet.Contains("dc:date"))
                writer.WriteElementString("dc", "date", null, this.date.ToString("yyyy-MM-dd"));

            if (filterSet == null || filterSet.Any(a => a.StartsWith("res")))
            {
                writer.WriteStartElement("res");

                if (filterSet == null || filterSet.Contains("res@duration"))
                    writer.WriteAttributeString("duration", "0:00:00.000");

                if (this.bitrate != null && (filterSet == null || filterSet.Contains("res@bitrate")))
                    writer.WriteAttributeString("bitrate", this.bitrate);

                if (this.resolution != null && (filterSet == null || filterSet.Contains("res@resolution")))
                    writer.WriteAttributeString("resolution", this.resolution);

                writer.WriteAttributeString("protocolInfo", string.Format("http-get:*:{0}:{1}", this.mime, settings.VideoEncodeFeature));
                writer.WriteValue(host + "/encode/video?id=" + Id + this.queryString);
                writer.WriteEndElement();
            }

            writer.WriteEndElement();
        }

        public override void WriteMe(XmlWriter xmlWriter, MediaSettings settings)
        {
            xmlWriter.WriteStartElement("tr");

            xmlWriter.WriteStartElement("td");

            xmlWriter.WriteStartElement("a");
            xmlWriter.WriteAttributeString("href", "/web/player.html?id=" + Id);
            xmlWriter.WriteAttributeString("target", "_blank");

            xmlWriter.WriteStartElement("img");
            xmlWriter.WriteAttributeString("src", "/web/images/htmlorangearrow.gif");
            xmlWriter.WriteAttributeString("alt", string.Empty);
            xmlWriter.WriteEndElement();

            xmlWriter.WriteStartElement("span");
            xmlWriter.WriteValue(LanguageResource.Play);
            xmlWriter.WriteEndElement();

            xmlWriter.WriteEndElement();

            xmlWriter.WriteStartElement("a");
            xmlWriter.WriteAttributeString("href", "/web/control.html?id=" + Id);
            xmlWriter.WriteValue(LanguageResource.Other);
            xmlWriter.WriteEndElement();

            xmlWriter.WriteEndElement();

            xmlWriter.WriteElementString("td", Title);
            xmlWriter.WriteElementString("td", "0:00:00");
            xmlWriter.WriteElementString("td", this.resolution);

            xmlWriter.WriteEndElement();
        }
    }
}
