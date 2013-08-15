using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Data.Linq.Mapping;
using System.Data.Linq;

namespace HomeMediaCenter
{
    public class ItemStream : Item
    {
        public ItemStream() : base(null, null, null) { }

        public ItemStream(string title, string path, ItemContainer parent, EncoderBuilder encBuilder)
            : base(string.Format("{0} {1} {2}kBps {3}", title, encBuilder.Resolution, 
            encBuilder.Video ? encBuilder.VidBitrate : encBuilder.AudBitrate, encBuilder.Audio ? "+Audio" : "-Audio"), path, parent)
        {
            this.Mime = encBuilder.GetMime();
            this.Date = DateTime.Now;
            this.Resolution = encBuilder.Resolution;
            this.SubtitlesPath = encBuilder.GetParamString();
            this.Audio = encBuilder.Audio && !encBuilder.Video;
            this.Video = encBuilder.Video;

            parent.SetMediaType(this.Audio ? MediaType.Audio : MediaType.Video);
        }

        [Column(IsPrimaryKey = false, DbType = "datetime", CanBeNull = true, UpdateCheck = UpdateCheck.Never)]
        public override DateTime Date
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "nvarchar(50)", CanBeNull = true, UpdateCheck = UpdateCheck.Never)]
        public string Mime
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "nvarchar(11)", CanBeNull = true, UpdateCheck = UpdateCheck.Never)]
        public string Resolution
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "nvarchar(255)", CanBeNull = true, UpdateCheck = UpdateCheck.Never)]
        public string SubtitlesPath
        {
            //Pouziva sa ako http query string
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "bit", CanBeNull = true)]
        public bool Audio
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "bit", CanBeNull = true)]
        public bool Video
        {
            get; set;
        }

        public override bool Equals(object obj)
        {
            if (obj is string)
                return this.SubtitlesPath == (string)obj;
            return base.Equals(obj);
        }

        public override int GetHashCode()
        {
            if (this.SubtitlesPath == null)
                return 0;
            return this.SubtitlesPath.GetHashCode();
        }

        public override string GetMime()
        {
            return this.Mime;
        }

        public override string GetPath()
        {
            return this.Path;
        }

        public override TimeSpan? GetDuration()
        {
            return TimeSpan.Zero;
        }

        public override string GetEncodeFeature(MediaSettings settings)
        {
            if (this.Audio)
                return settings.AudioStreamFeature;
            else if (this.Video)
                return settings.VideoStreamFeature;
            return string.Empty;
        }

        public override void RemoveMe(DataContext context)
        {
            this.Parent.CheckMediaType(this.Audio ? MediaType.Audio : MediaType.Video);
        }

        public override bool IsType(MediaType type)
        {
            switch (type)
            {
                case MediaType.Audio: return this.Audio;
                case MediaType.Video: return this.Video;
                default: return false;
            }
        }

        public override void BrowseMetadata(XmlWriter writer, MediaSettings settings, string host, string idParams, HashSet<string> filterSet)
        {
            BrowseMetadata(writer, settings, host, idParams, filterSet, this.Parent.Id.ToString());
        }

        public override void BrowseMetadata(XmlWriter writer, MediaSettings settings, string host, string idParams, HashSet<string> filterSet, string parentId)
        {
            EncoderBuilder encBuilder = EncoderBuilder.GetEncoder(this.SubtitlesPath);

            writer.WriteStartElement("item");

            writer.WriteAttributeString("id", this.Id.ToString());
            writer.WriteAttributeString("restricted", "true");
            writer.WriteAttributeString("parentID", parentId + "_" + (this.Audio ? AudioIndex : VideoIndex));

            //Povinne hodnoty
            writer.WriteElementString("dc", "title", null, this.Title);
            writer.WriteElementString("upnp", "class", null, this.Audio ? "object.item.audioItem" : "object.item.videoItem");

            //Volitelne hodnoty
            if (filterSet == null || filterSet.Contains("dc:date"))
                writer.WriteElementString("dc", "date", null, this.Date.ToString("yyyy-MM-dd"));

            if (filterSet == null || filterSet.Any(a => a.StartsWith("res")))
            {
                writer.WriteStartElement("res");

                if (filterSet == null || filterSet.Contains("res@duration"))
                    writer.WriteAttributeString("duration", "0:00:00.000");

                if (this.Audio)
                {
                    if (encBuilder.AudBitrate != null && (filterSet == null || filterSet.Contains("res@bitrate")))
                        writer.WriteAttributeString("bitrate", encBuilder.AudBitrate);

                    writer.WriteAttributeString("protocolInfo", string.Format("http-get:*:{0}:{1}{2}", this.Mime, encBuilder.GetDlnaType(), settings.AudioStreamFeature));
                    writer.WriteValue(host + "/encode/audio?id=" + this.Id + this.SubtitlesPath);
                }
                else
                {
                    if (encBuilder.VidBitrate != null && (filterSet == null || filterSet.Contains("res@bitrate")))
                        writer.WriteAttributeString("bitrate", encBuilder.VidBitrate);

                    if (this.Resolution != null && (filterSet == null || filterSet.Contains("res@resolution")))
                        writer.WriteAttributeString("resolution", this.Resolution);

                    writer.WriteAttributeString("protocolInfo", string.Format("http-get:*:{0}:{1}{2}", this.Mime, encBuilder.GetDlnaType(), settings.VideoStreamFeature));
                    writer.WriteValue(host + "/encode/video?id=" + this.Id + this.SubtitlesPath);
                }
                writer.WriteEndElement();
            }

            writer.WriteEndElement();
        }

        public override void BrowseWebMetadata(XmlWriter xmlWriter, MediaSettings settings, string idParams)
        {
            if (this.Audio)
                ItemAudio.WriteHTML(xmlWriter, this.Id.ToString(), this.Title, "0:00:00", string.Empty, string.Empty, string.Empty);
            else
                ItemVideo.WriteHTML(xmlWriter, this.Id.ToString(), this.Title, "0:00:00", this.Resolution);
        }

        public override void GetWebPlayer(XmlWriter xmlWriter, Dictionary<string, string> urlParams)
        {
            if (this.Audio)
                ItemAudio.WriteHTMLPlayer(xmlWriter, this.Id.ToString(), GetDuration(), urlParams);
            else
                ItemVideo.WriteHTMLPlayer(xmlWriter, this.Id.ToString(), GetDuration(), urlParams, null);
        }
    }
}
