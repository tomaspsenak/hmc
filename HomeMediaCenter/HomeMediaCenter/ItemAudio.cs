using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;

namespace HomeMediaCenter
{
    [Serializable]
    public class ItemAudio : Item
    {
        private readonly string path;
        private readonly string mime;
        private readonly DateTime date;
        private readonly long length;
        private readonly TimeSpan duration;
        private readonly int bitrate;
        private readonly string genre;
        private readonly string artist;
        private readonly string album;

        public ItemAudio(ItemContainer parent, FileInfo file, string mime) : base(file.Name, parent)
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
            this.genre = FileHelper.GetAudioGenre(file);
            this.artist = FileHelper.GetAudioArtist(file);
            this.album = FileHelper.GetAudioAlbum(file);
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

        public override TimeSpan? Duration
        {
            get { return this.duration; }
        }

        public override string GetFileFeature(MediaSettings settings)
        {
            return (this.mime == "audio/mpeg" ? "DLNA.ORG_PN=MP3;" : string.Empty) + settings.AudioFileFeature;
        }

        public override string GetEncodeFeature(MediaSettings settings)
        {
            return settings.AudioEncodeFeature;
        }

        public override void WriteMe(XmlWriter writer, MediaSettings settings, string host, HashSet<string> filterSet)
        {
            writer.WriteStartElement("item");

            writer.WriteAttributeString("id", Id.ToString());
            writer.WriteAttributeString("restricted", "true");
            writer.WriteAttributeString("parentID", Parent == null ? "-1" : Parent.Id.ToString());

            //Povinne hodnoty
            writer.WriteElementString("dc", "title", null, Title);
            writer.WriteElementString("upnp", "class", null, "object.item.audioItem");

            //Volitelne hodnoty
            if (filterSet == null || filterSet.Contains("dc:date"))
                writer.WriteElementString("dc", "date", null, this.date.ToString("yyyy-MM-dd"));

            if (this.genre != string.Empty && (filterSet == null || filterSet.Contains("upnp:genre")))
                writer.WriteElementString("upnp", "genre", null, this.genre);

            if (this.artist != string.Empty && (filterSet == null || filterSet.Contains("upnp:artist")))
            {
                writer.WriteStartElement("upnp", "artist", null);
                writer.WriteAttributeString("role", "AlbumArtist");
                writer.WriteValue(this.artist);
                writer.WriteEndElement();
            }

            if (this.album != string.Empty && (filterSet == null || filterSet.Contains("upnp:album")))
                writer.WriteElementString("upnp", "album", null, this.album);

            if (filterSet == null || filterSet.Any(a => a.StartsWith("res")))
            {
                if (settings.AudioNativeFile)
                {
                    writer.WriteStartElement("res");

                    if (filterSet == null || filterSet.Contains("res@size"))
                        writer.WriteAttributeString("size", this.length.ToString());

                    if (filterSet == null || filterSet.Contains("res@duration"))
                        writer.WriteAttributeString("duration", this.duration.ToString("h':'mm':'ss'.'fff"));

                    if (filterSet == null || filterSet.Contains("res@bitrate"))
                        writer.WriteAttributeString("bitrate", this.bitrate.ToString());

                    writer.WriteAttributeString("protocolInfo", string.Format("http-get:*:{0}:{1}", this.mime, GetFileFeature(settings)));
                    writer.WriteValue(host + "/files/audio?id=" + Id);
                    writer.WriteEndElement();
                }

                foreach (MediaSettingsAudio sett in settings.AudioEncode)
                {
                    writer.WriteStartElement("res");

                    if (filterSet == null || filterSet.Contains("res@duration"))
                        writer.WriteAttributeString("duration", this.duration.ToString("h':'mm':'ss'.'fff"));

                    if (sett.AudBitrate != null && (filterSet == null || filterSet.Contains("res@bitrate")))
                        writer.WriteAttributeString("bitrate", sett.AudBitrate);

                    writer.WriteAttributeString("protocolInfo", string.Format("http-get:*:{0}:{1}", sett.Mime, settings.VideoEncodeFeature));
                    writer.WriteValue(host + "/encode/audio?id=" + Id + sett.QueryString);
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
            xmlWriter.WriteElementString("td", this.artist);
            xmlWriter.WriteElementString("td", this.album);
            xmlWriter.WriteElementString("td", this.genre);

            xmlWriter.WriteEndElement();
        }
    }
}
