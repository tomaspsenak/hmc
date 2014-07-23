using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Data.Linq;
using System.Data.Linq.Mapping;

namespace HomeMediaCenter
{
    public class ItemAudio : Item
    {
        public ItemAudio() : base(null, null, null) { }

        public ItemAudio(FileInfo file, string mime, ItemContainer parent) : base(file.Name, file.Name, parent)
        {
            this.Mime = mime;
            this.Date = file.LastWriteTime;
            this.Length = file.Length;

            parent.SetMediaType(MediaType.Audio);
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

        [Column(IsPrimaryKey = false, DbType = "bigint", CanBeNull = true, UpdateCheck = UpdateCheck.Never)]
        public long Length
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "bigint", CanBeNull = true, UpdateCheck = UpdateCheck.Never)]
        public long? DurationTicks
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "int", CanBeNull = true, UpdateCheck = UpdateCheck.Never)]
        public int? Bitrate
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "nvarchar(60)", CanBeNull = true, UpdateCheck = UpdateCheck.Never)]
        public string Genre
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "nvarchar(60)", CanBeNull = true, UpdateCheck = UpdateCheck.Never)]
        public string Artist
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "nvarchar(60)", CanBeNull = true, UpdateCheck = UpdateCheck.Never)]
        public string Album
        {
            get; set;
        }

        public override string GetMime()
        {
            return this.Mime;
        }

        public override string GetPath()
        {
            return System.IO.Path.Combine(this.Parent.Path, this.Path);
        }

        public override string GetThumbnailPath(ItemManager manager)
        {
            if (this.HasThumbnail.HasValue && this.HasThumbnail.Value)
                return System.IO.Path.Combine(manager.UpnpDevice.ThumbnailsPath, this.Id + ".jpg");
            return null;
        }

        public override TimeSpan? GetDuration()
        {
            return this.DurationTicks.HasValue ? TimeSpan.FromTicks(this.DurationTicks.Value) : default(TimeSpan?);
        }

        public override string GetFileFeature(MediaSettings settings)
        {
            return (this.Mime == "audio/mpeg" ? "DLNA.ORG_PN=MP3;" : string.Empty) + settings.Audio.FileFeature;
        }

        public override string GetEncodeFeature(MediaSettings settings)
        {
            return settings.Audio.EncodeFeature;
        }

        public override void RemoveMe(DataContext context, ItemManager manager)
        {
            this.Parent.CheckMediaType(MediaType.Audio);

            if (this.HasThumbnail.HasValue && this.HasThumbnail.Value)
            {
                string thumbnailPath = System.IO.Path.Combine(manager.UpnpDevice.ThumbnailsPath, this.Id + ".jpg");

                try { System.IO.File.Delete(thumbnailPath); }
                catch { }
            }
        }

        public override bool IsType(MediaType type)
        {
            return type == MediaType.Audio;
        }

        public override void RefreshMetadata(DataContext context, ItemManager manager, bool recursive)
        {
            if (manager.UpnpDevice.Stopping)
                return;

            string thumbnailPath = null;
            if (manager.UpnpDevice.GenerateThumbnails && !this.HasThumbnail.HasValue)
            {
                //Ak sa ma generovat thumbnail a HasThumbnail je null - generuj
                thumbnailPath = System.IO.Path.Combine(manager.UpnpDevice.ThumbnailsPath, this.Id + ".jpg");
            }
            else if (!manager.UpnpDevice.GenerateThumbnails && this.HasThumbnail == true)
            {
                //Ak sa nema generovat thumbnail a HasThumbnail je true - vymaz thumbnail
                string thumbnailPath2 = System.IO.Path.Combine(manager.UpnpDevice.ThumbnailsPath, this.Id + ".jpg");

                try { System.IO.File.Delete(thumbnailPath2); }
                catch { }

                this.HasThumbnail = null;
            }

            //Ak sa nepodarilo zistit metadata - skus ich zistit pri kazdom refresh (napr. ak subor este nebol cely skopirovany)
            if (!this.DurationTicks.HasValue || thumbnailPath != null)
                AssignValues(new FileInfo(GetPath()), thumbnailPath);
        }

        public override void BrowseMetadata(XmlWriter writer, MediaSettings settings, string host, string idParams, HashSet<string> filterSet)
        {
            BrowseMetadata(writer, settings, host, idParams, filterSet, this.Parent.GetParentItem(MediaType.Audio).Id);
        }

        public override void BrowseMetadata(XmlWriter writer, MediaSettings settings, string host, string idParams, HashSet<string> filterSet, int parentId)
        {
            BrowseMetadata(writer, settings, host, idParams, filterSet, parentId, null);
        }

        public override void BrowseMetadata(XmlWriter writer, MediaSettings settings, string host, string idParams, HashSet<string> filterSet, int parentId, TimeSpan? startTime)
        {
            writer.WriteStartElement("item");

            writer.WriteAttributeString("id", this.Id.ToString());
            writer.WriteAttributeString("restricted", "true");
            writer.WriteAttributeString("parentID", parentId + "_" + AudioIndex);

            //Povinne hodnoty
            writer.WriteElementString("dc", "title", null, this.Title);
            writer.WriteElementString("upnp", "class", null, "object.item.audioItem");

            //Volitelne hodnoty
            if (filterSet == null || filterSet.Contains("dc:date"))
                writer.WriteElementString("dc", "date", null, this.Date.ToString("yyyy-MM-dd"));

            if (this.Genre != null && this.Genre != string.Empty && (filterSet == null || filterSet.Contains("upnp:genre")))
                writer.WriteElementString("upnp", "genre", null, this.Genre);

            if (this.Artist != null && this.Artist != string.Empty && (filterSet == null || filterSet.Contains("upnp:artist")))
            {
                writer.WriteStartElement("upnp", "artist", null);
                writer.WriteAttributeString("role", "AlbumArtist");
                writer.WriteValue(this.Artist);
                writer.WriteEndElement();
            }

            if (this.Album != null && this.Album != string.Empty && (filterSet == null || filterSet.Contains("upnp:album")))
                writer.WriteElementString("upnp", "album", null, this.Album);

            if ((filterSet == null || filterSet.Contains("upnp:icon")) && (this.HasThumbnail.HasValue && this.HasThumbnail.Value))
                writer.WriteElementString("upnp", "icon", null, host + "/thumbnail/image.jpg?id=" + this.Id + "&codec=jpeg&width=160&height=160&keepaspect");

            if ((filterSet == null || filterSet.Contains("upnp:albumArtURI")) && (this.HasThumbnail.HasValue && this.HasThumbnail.Value))
            {
                writer.WriteStartElement("upnp", "albumArtURI", null);
                writer.WriteAttributeString("dlna", "profileID", "urn:schemas-dlna-org:metadata-1-0/", "JPEG_TN");
                writer.WriteValue(host + "/thumbnail/image.jpg?id=" + this.Id + "&codec=jpeg&width=160&height=160&keepaspect");
                writer.WriteEndElement();
            }

            if (filterSet == null || filterSet.Any(a => a.StartsWith("res")))
            {
                TimeSpan? duration = this.GetDuration();
                if (duration.HasValue && startTime.HasValue)
                    duration -= startTime;

                if (settings.Audio.NativeFile && !startTime.HasValue)
                {
                    //Povoli sa povodne audio ak sa nemeni zaciatok audia
                    writer.WriteStartElement("res");

                    if (filterSet == null || filterSet.Contains("res@size"))
                        writer.WriteAttributeString("size", this.Length.ToString());

                    if (duration.HasValue && (filterSet == null || filterSet.Contains("res@duration")))
                        writer.WriteAttributeString("duration", duration.Value.ToString("h':'mm':'ss'.'fff"));

                    if (this.Bitrate.HasValue && (filterSet == null || filterSet.Contains("res@bitrate")))
                        writer.WriteAttributeString("bitrate", this.Bitrate.Value.ToString());

                    writer.WriteAttributeString("protocolInfo", string.Format("http-get:*:{0}:{1}", this.Mime, GetFileFeature(settings)));
                    writer.WriteValue(string.Format("{0}/files/audio{1}?id={2}",host, System.IO.Path.GetExtension(this.Path), this.Id));
                    writer.WriteEndElement();
                }

                foreach (EncoderBuilder sett in settings.Audio.Encode)
                {
                    writer.WriteStartElement("res");

                    if (duration.HasValue && (filterSet == null || filterSet.Contains("res@duration")))
                        writer.WriteAttributeString("duration", duration.Value.ToString("h':'mm':'ss'.'fff"));

                    if (sett.AudBitrate != null && (filterSet == null || filterSet.Contains("res@bitrate")))
                        writer.WriteAttributeString("bitrate", sett.AudBitrate);

                    writer.WriteAttributeString("protocolInfo", string.Format("http-get:*:{0}:{1}{2}", sett.GetMime(), sett.GetDlnaType(), settings.Audio.EncodeFeature));
                    writer.WriteValue(string.Format("{0}/encode/audio?id={1}{2}{3}", host, this.Id, sett.GetParamString(),
                        startTime.HasValue ? "&starttime=" + startTime.Value.TotalSeconds : string.Empty));
                    writer.WriteEndElement();
                }
            }

            writer.WriteEndElement();
        }

        public override void BrowseWebMetadata(XmlWriter xmlWriter, MediaSettings settings, string idParams)
        {
            TimeSpan? duration = this.GetDuration();
            WriteHTML(xmlWriter, this.Id.ToString(), this.Title, duration.HasValue ? duration.Value.ToString("h':'mm':'ss") : null, this.Artist, 
                this.Album, this.Genre, this.HasThumbnail.HasValue && this.HasThumbnail.Value, System.IO.Path.GetExtension(this.Path));
        }

        public override void GetWebPlayer(XmlWriter xmlWriter, Dictionary<string, string> urlParams)
        {
            WriteHTMLPlayer(xmlWriter, this.Id.ToString(), this.GetDuration(), urlParams);
        }

        public static void WriteHTML(XmlWriter xmlWriter, string id, string title, string duration, string artist, string album, string genre, bool hasThumbanil, string noStreamExtension)
        {
            xmlWriter.WriteStartElement("tr");

            xmlWriter.WriteStartElement("td");
            xmlWriter.WriteStartElement("div");
            xmlWriter.WriteAttributeString("class", "libPlayButton");
            xmlWriter.WriteRaw(string.Format(@"<a href=""/web/player.html?id={0}"" target=""_blank"">{1}</a>", id, LanguageResource.Play));
            xmlWriter.WriteStartElement("ul");
            if (noStreamExtension != null)
                xmlWriter.WriteRaw(string.Format(@"<li><a href=""/files/audio{0}?id={1}"" target=""_blank"">{2}</a></li>", noStreamExtension, id, LanguageResource.Download));
            xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/control.html?id={0}"" target=""_blank"">{1}</a></li>", id, LanguageResource.PlayTo));
            xmlWriter.WriteEndElement();
            xmlWriter.WriteEndElement();
            xmlWriter.WriteEndElement();

            xmlWriter.WriteStartElement("td");
            if (hasThumbanil)
            {
                xmlWriter.WriteStartElement("img");
                xmlWriter.WriteAttributeString("src", string.Format("/thumbnail/image.jpg?id={0}&codec=jpeg&width=50&height=50&quality=30", id));
                xmlWriter.WriteAttributeString("alt", title);
                xmlWriter.WriteAttributeString("title", title);
                xmlWriter.WriteAttributeString("border", "0");
                xmlWriter.WriteAttributeString("width", "50");
                xmlWriter.WriteAttributeString("height", "50");
                xmlWriter.WriteAttributeString("align", "right");
                xmlWriter.WriteFullEndElement();
            }
            xmlWriter.WriteFullEndElement();

            xmlWriter.WriteElementString("td", title);
            xmlWriter.WriteElementString("td", duration == null ? string.Empty : duration);
            xmlWriter.WriteElementString("td", artist == null ? string.Empty : artist);
            xmlWriter.WriteElementString("td", album == null ? string.Empty : album);
            xmlWriter.WriteElementString("td", genre == null ? string.Empty : genre);

            xmlWriter.WriteEndElement();
        }

        public static void WriteHTMLPlayer(XmlWriter xmlWriter, string id, TimeSpan? duration, Dictionary<string, string> urlParams)
        {
            string codec = null;
            if (urlParams.ContainsKey("codec"))
                codec = urlParams["codec"].ToLower();

            string quality = null;
            if (urlParams.ContainsKey("quality"))
                quality = urlParams["quality"].ToLower();

            if (codec == "wma")
            {
                string sourceUrl = string.Format("/encode/web?id={0}&codec=wmv2&video=0&width=1&height=1&vidbitrate=1&fps=1", id);
                switch (quality)
                {
                    case "low": sourceUrl += "&audbitrate=40&quality=1"; break;
                    case "high": sourceUrl += "&audbitrate=80&quality=51"; break;
                    default /*"medium"*/:
                        quality = "medium";
                        sourceUrl += "&audbitrate=128&quality=51";
                        break;
                }

                xmlWriter.WriteRaw(@"<div>");
                xmlWriter.WriteRaw(@"<object id=""silverlightControlHost"" data=""data:application/x-silverlight-2,"" type=""application/x-silverlight-2"">");
                xmlWriter.WriteRaw(@"<param name=""source"" value=""/web/webplayer.xap"" />");
                xmlWriter.WriteRaw(@"<param name=""onError"" value=""onSilverlightError"" />");
                xmlWriter.WriteRaw(@"<param name=""background"" value=""white"" />");
                xmlWriter.WriteRaw(@"<param name=""minRuntimeVersion"" value=""4.0.60310.0"" />");
                xmlWriter.WriteRaw(@"<param name=""autoUpgrade"" value=""true"" />");
                xmlWriter.WriteRaw(@"<param name=""onLoad"" value=""silverlightControlLoaded"" />");

                xmlWriter.WriteStartElement("span");
                xmlWriter.WriteValue(LanguageResource.Install + " ");

                xmlWriter.WriteStartElement("a");
                xmlWriter.WriteAttributeString("class", "videoLink");
                xmlWriter.WriteAttributeString("href", "http://go.microsoft.com/fwlink/?LinkID=149156&v=4.0.60310.0");
                xmlWriter.WriteValue("Silverlight");
                xmlWriter.WriteEndElement();

                xmlWriter.WriteValue(string.Format(" {0} ", LanguageResource.Or));

                xmlWriter.WriteStartElement("a");
                xmlWriter.WriteAttributeString("href", sourceUrl);
                xmlWriter.WriteAttributeString("id", "videoLink");
                xmlWriter.WriteAttributeString("class", "videoLink");
                xmlWriter.WriteValue(LanguageResource.Play);
                xmlWriter.WriteEndElement();

                xmlWriter.WriteValue(" " + LanguageResource.ThroughDirectLink);
                xmlWriter.WriteEndElement();

                xmlWriter.WriteRaw(@"</object>");
                xmlWriter.WriteRaw(@"</div>");
            }
            else if (codec == "flv")
            {
                string sourceUrl = string.Format("/encode/web%3Fid={0}&codec=flv_ts&video=0", id);
                switch (quality)
                {
                    case "low": sourceUrl += "&audbitrate=64&quality=1"; break;
                    case "high": sourceUrl += "&audbitrate=256&quality=51"; break;
                    default /*"medium"*/:
                        quality = "medium";
                        sourceUrl += "&audbitrate=128&quality=51";
                        break;
                }
                sourceUrl = sourceUrl.Replace("=", "%3D").Replace("&", "%26");

                xmlWriter.WriteStartElement("a");
                xmlWriter.WriteAttributeString("id", "videoLink");
                xmlWriter.WriteAttributeString("href", sourceUrl);
                xmlWriter.WriteAttributeString("style", "display:block;margin-left:auto;margin-right:auto;width:500px;height:80px;");
                xmlWriter.WriteValue(LanguageResource.Play + " " + LanguageResource.ThroughDirectLink);
                xmlWriter.WriteEndElement();
            }
            else
            {
                codec = "webm";

                string sourceUrlQuality;
                switch (quality)
                {
                    case "low": sourceUrlQuality = "&audbitrate=64&quality=1"; break;
                    case "high": sourceUrlQuality = "&audbitrate=256&quality=51"; break;
                    default /*"medium"*/:
                        quality = "medium";
                        sourceUrlQuality = "&audbitrate=128&quality=51";
                        break;
                }
                string sourceUrl1 = string.Format("/encode/web?id={0}&codec=webm_ts&video=0{1}", id, sourceUrlQuality);
                string sourceUrl2 = string.Format("/encode/web?id={0}&codec=mp3_ts&video=0{1}", id, sourceUrlQuality);

                xmlWriter.WriteRaw(@"<video id=""streamVideo"" autoplay=""autoplay"" />");

                xmlWriter.WriteStartElement("source");
                xmlWriter.WriteAttributeString("src", sourceUrl1);
                xmlWriter.WriteAttributeString("type", "audio/webm; codecs=\"vorbis\"");
                xmlWriter.WriteEndElement();

                xmlWriter.WriteStartElement("source");
                xmlWriter.WriteAttributeString("src", sourceUrl2);
                xmlWriter.WriteAttributeString("type", "audio/mpeg");
                xmlWriter.WriteEndElement();

                xmlWriter.WriteStartElement("span");
                xmlWriter.WriteAttributeString("id", "streamVideoSpan");

                xmlWriter.WriteStartElement("a");
                xmlWriter.WriteAttributeString("href", sourceUrl1);
                xmlWriter.WriteAttributeString("id", "videoLink");
                xmlWriter.WriteAttributeString("class", "videoLink");
                xmlWriter.WriteValue(LanguageResource.Play);
                xmlWriter.WriteEndElement();

                xmlWriter.WriteValue(" " + LanguageResource.ThroughDirectLink);
                xmlWriter.WriteEndElement();

                xmlWriter.WriteRaw(@"</video>");
            }

            xmlWriter.WriteStartElement("form");
            xmlWriter.WriteAttributeString("id", "mainForm");
            xmlWriter.WriteAttributeString("action", "/Web/player.html");
            xmlWriter.WriteAttributeString("method", "get");
            xmlWriter.WriteAttributeString("enctype", "application/x-www-form-urlencoded");

            xmlWriter.WriteRaw(string.Format(@"<input type=""hidden"" name=""id"" value=""{0}"" />", id));

            xmlWriter.WriteRaw(@"<span id=""streamPos"">0:00:00</span>");
            xmlWriter.WriteRaw(@"<div id=""seekSlider""></div>");
            xmlWriter.WriteRaw(string.Format(@"<span id=""streamLength"">{0}</span>",
                duration.HasValue ? duration.Value.ToString("h':'mm':'ss") : "0:00:00"));
            xmlWriter.WriteRaw(@"<div id=""volumeSlider""></div>");
            xmlWriter.WriteRaw(@"<span id=""volumeVal"">100%</span>");

            xmlWriter.WriteRaw(@"<div id=""streamToolbar"" class=""ui-widget-header ui-corner-all"">");
            xmlWriter.WriteRaw(string.Format(@"<button type=""button"" id=""playButton"">{0}</button>", LanguageResource.Play));
            xmlWriter.WriteRaw(string.Format(@"<button type=""button"" id=""pauseButton"">{0}</button>", LanguageResource.Pause));

            xmlWriter.WriteRaw(@"<span id=""codecRadios"">");
            xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""webmRadio"" name=""codec"" value=""webm"" {0} /><label for=""webmRadio"">WebM</label>",
                codec == "webm" ? "checked=\"checked\"" : string.Empty));
            xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""wmvRadio"" name=""codec"" value=""wma"" {0} /><label for=""wmvRadio"">Wma</label>",
                codec == "wma" ? "checked=\"checked\"" : string.Empty));
            xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""flvRadio"" name=""codec"" value=""flv"" {0} /><label for=""flvRadio"">Flash</label>",
                codec == "flv" ? "checked=\"checked\"" : string.Empty));
            xmlWriter.WriteRaw(@"</span>");

            xmlWriter.WriteRaw(@"<span id=""qualityRadios"">");
            xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""lowRadio"" name=""quality"" value=""low"" {0} /><label for=""lowRadio"">{1}</label>",
                quality == "low" ? "checked=\"checked\"" : string.Empty, LanguageResource.Low));
            xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""mediumRadio"" name=""quality"" value=""medium"" {0} /><label for=""mediumRadio"">{1}</label>",
                quality == "medium" ? "checked=\"checked\"" : string.Empty, LanguageResource.Medium));
            xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""highRadio"" name=""quality"" value=""high"" {0} /><label for=""highRadio"">{1}</label>",
                quality == "high" ? "checked=\"checked\"" : string.Empty, LanguageResource.High));
            xmlWriter.WriteRaw(@"</span>");

            xmlWriter.WriteRaw(@"<button id=""submitButton"" type=""submit"">Ok</button>");
            xmlWriter.WriteRaw(@"</div>");

            //Koniec form
            xmlWriter.WriteEndElement();
        }

        private void AssignValues(FileInfo file, string thumbnailPath)
        {
            TimeSpan duration;
            int audioStreamsCount;

            int hr = DSWrapper.MediaFile.GetAudioDuration(file, out duration, out audioStreamsCount);
            if (hr == 0)
            {
                this.DurationTicks = duration.Ticks;
                if (duration.TotalSeconds == 0f)
                    this.Bitrate = 0;
                else
                    this.Bitrate = (int)(file.Length / duration.TotalSeconds);
                this.Genre = DSWrapper.MediaFile.GetAudioGenre(file).Truncate(60);
                this.Artist = DSWrapper.MediaFile.GetAudioArtist(file).Truncate(60);
                this.Album = DSWrapper.MediaFile.GetAudioAlbum(file).Truncate(60);
                if (thumbnailPath != null)
                    this.HasThumbnail = DSWrapper.MediaFile.GetAudioThumbnail(file, thumbnailPath);
            }
            else if (hr != -2147024864 && !HelperClass.IsFileLocked(file))
            {
                //Chyba -2147024864 znamena ze subor nie je dostupny - napr. pouziva ho iny proces, nie je kompletne skopirovany,...
                //V pripade inej chyby este overi ci naozaj subor nepouziva iny proces

                //Inak sa uz nepokusaj zistit metadata
                this.DurationTicks = 0;
            }
        }
    }
}
