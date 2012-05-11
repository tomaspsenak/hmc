using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;

namespace HomeMediaCenter
{
    [Serializable]
    public class ItemVideo : Item
    {
        private readonly string path;
        private readonly string mime;
        private readonly DateTime date;
        private readonly long length;
        private readonly TimeSpan duration;
        private readonly int bitrate;
        private readonly string resolution;
        private readonly string subtitles;

        public ItemVideo(ItemContainer parent, FileInfo file, string mime, string subtitles) : base(file.Name + 
            (subtitles == null ? string.Empty : " Subtitles"), parent)
        {
            this.path = file.FullName;
            this.mime = mime;
            this.date = file.LastWriteTime;
            this.length = file.Length;
            FileHelper.GetVideoDuration(file, out this.duration);
            if (this.duration.TotalSeconds == 0f)
                this.bitrate = 0;
            else
                this.bitrate = (int)(file.Length / this.duration.TotalSeconds);
            this.resolution = FileHelper.GetVideoResolution(file);
            this.subtitles = subtitles;
        }

        public override DateTime Date
        {
            get { return this.date; }
        }

        public override string Path
        {
            get { return this.path; }
        }

        public override string SubtitlesPath
        {
            get { return this.subtitles; }
        }

        public override string Mime
        {
            get { return this.mime; }
        }

        public override TimeSpan? Duration
        {
            get { return this.duration; }
        }

        public override string GetFileFeature(MediaSettings settings)
        {
            return settings.VideoFileFeature;
        }

        public override string GetEncodeFeature(MediaSettings settings)
        {
            return settings.VideoEncodeFeature;
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
                if (settings.VideoNativeFile && this.subtitles == null)
                {
                    writer.WriteStartElement("res");

                    if (filterSet == null || filterSet.Contains("res@size"))
                        writer.WriteAttributeString("size", this.length.ToString());

                    if (filterSet == null || filterSet.Contains("res@duration"))
                        writer.WriteAttributeString("duration", this.duration.ToString("h':'mm':'ss'.'fff"));

                    if (filterSet == null || filterSet.Contains("res@bitrate"))
                        writer.WriteAttributeString("bitrate", this.bitrate.ToString());

                    if  (filterSet == null || filterSet.Contains("res@resolution"))
                        writer.WriteAttributeString("resolution", this.resolution);

                    writer.WriteAttributeString("protocolInfo", string.Format("http-get:*:{0}:{1}", this.mime, GetFileFeature(settings)));
                    writer.WriteValue(host + "/files/video?id=" + Id);
                    writer.WriteEndElement();
                }

                foreach (EncoderBuilder sett in settings.VideoEncode)
                {
                    writer.WriteStartElement("res");

                    if (filterSet == null || filterSet.Contains("res@duration"))
                        writer.WriteAttributeString("duration", this.duration.ToString("h':'mm':'ss'.'fff"));

                    if (sett.VidBitrate != null && (filterSet == null || filterSet.Contains("res@bitrate")))
                        writer.WriteAttributeString("bitrate", sett.VidBitrate);

                    if (sett.Resolution != null && (filterSet == null || filterSet.Contains("res@resolution")))
                        writer.WriteAttributeString("resolution", sett.Resolution);

                    writer.WriteAttributeString("protocolInfo", string.Format("http-get:*:{0}:{1}{2}", sett.GetMime(), sett.GetDlnaType(), settings.VideoEncodeFeature));
                    writer.WriteValue(host + "/encode/video?id=" + Id + sett.GetParamString());
                    writer.WriteEndElement();
                }
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
            xmlWriter.WriteElementString("td", this.duration.ToString("h':'mm':'ss"));
            xmlWriter.WriteElementString("td", this.resolution);

            xmlWriter.WriteEndElement();
        }
    }
}
