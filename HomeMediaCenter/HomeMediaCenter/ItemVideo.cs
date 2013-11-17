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
    public class ItemVideo : Item
    {
        public ItemVideo() : base(null, null, null) { }

        public ItemVideo(string name, string path, string subtitles, ItemContainerVideo parent) : base(name, path, parent)
        {
            this.SubtitlesPath = subtitles;

            parent.SetMediaType(MediaType.Video);
        }

        [Column(IsPrimaryKey = false, DbType = "nvarchar(255)", CanBeNull = true, UpdateCheck = UpdateCheck.Never)]
        public string SubtitlesPath
        {
            get; set;
        }

        public override DateTime Date
        {
            get { return this.Parent.Date; }
            set { }
        }

        public override string GetMime()
        {
            return ((ItemContainerVideo)this.Parent).Mime;
        }

        public override string GetPath()
        {
            //Kazdy video item ma este svoj kontajner
            return System.IO.Path.Combine(this.Parent.Parent.Path, this.Parent.Path);
        }

        public override string GetSubtitlesPath()
        {
            return this.SubtitlesPath == null ? null : System.IO.Path.Combine(this.Parent.Parent.Path, this.SubtitlesPath);
        }

        public override TimeSpan? GetDuration()
        {
            long? durationTicks = ((ItemContainerVideo)this.Parent).DurationTicks;
            return durationTicks.HasValue ? TimeSpan.FromTicks(durationTicks.Value) : default(TimeSpan?);
        }

        public override string GetFileFeature(MediaSettings settings)
        {
            return settings.Video.FileFeature;
        }

        public override string GetEncodeFeature(MediaSettings settings)
        {
            return settings.Video.EncodeFeature;
        }

        public override void RemoveMe(DataContext context)
        {
            this.Parent.CheckMediaType(MediaType.Video);
        }

        public override bool IsType(MediaType type)
        {
            return type == MediaType.Video;
        }

        public override void BrowseMetadata(XmlWriter writer, MediaSettings settings, string host, string idParams, HashSet<string> filterSet)
        {
            BrowseMetadata(writer, settings, host, idParams, filterSet, this.Parent.GetParentItem(MediaType.Video).Id);
        }

        public override void BrowseMetadata(XmlWriter writer, MediaSettings settings, string host, string idParams, HashSet<string> filterSet, int parentId)
        {
            writer.WriteStartElement("item");

            writer.WriteAttributeString("id", this.Id.ToString());
            writer.WriteAttributeString("restricted", "true");
            writer.WriteAttributeString("parentID", parentId + "_" + VideoIndex);

            //Povinne hodnoty
            writer.WriteElementString("dc", "title", null, this.Title);
            writer.WriteElementString("upnp", "class", null, "object.item.videoItem");

            //Volitelne hodnoty
            if (filterSet == null || filterSet.Contains("dc:date"))
                writer.WriteElementString("dc", "date", null, this.Date.ToString("yyyy-MM-dd"));

            if (filterSet == null || filterSet.Any(a => a.StartsWith("res")))
            {
                if (settings.Video.NativeFile && this.SubtitlesPath == null && this.Path == null)
                {
                    writer.WriteStartElement("res");

                    if (filterSet == null || filterSet.Contains("res@size"))
                        writer.WriteAttributeString("size", ((ItemContainerVideo)this.Parent).Length.ToString());

                    TimeSpan? duration = this.GetDuration();
                    if (duration.HasValue && (filterSet == null || filterSet.Contains("res@duration")))
                        writer.WriteAttributeString("duration", duration.Value.ToString("h':'mm':'ss'.'fff"));

                    int? bitrate = ((ItemContainerVideo)this.Parent).Bitrate;
                    if (bitrate.HasValue && (filterSet == null || filterSet.Contains("res@bitrate")))
                        writer.WriteAttributeString("bitrate", bitrate.Value.ToString());

                    string resolution = ((ItemContainerVideo)this.Parent).Resolution;
                    if  (resolution != null && (filterSet == null || filterSet.Contains("res@resolution")))
                        writer.WriteAttributeString("resolution", resolution);

                    writer.WriteAttributeString("protocolInfo", string.Format("http-get:*:{0}:{1}", GetMime(), GetFileFeature(settings)));
                    writer.WriteValue(host + "/files/video?id=" + this.Id);
                    writer.WriteEndElement();
                }

                foreach (EncoderBuilder sett in settings.Video.Encode)
                {
                    writer.WriteStartElement("res");

                    TimeSpan? duration = this.GetDuration();
                    if (duration.HasValue && (filterSet == null || filterSet.Contains("res@duration")))
                        writer.WriteAttributeString("duration", duration.Value.ToString("h':'mm':'ss'.'fff"));

                    if (sett.VidBitrate != null && (filterSet == null || filterSet.Contains("res@bitrate")))
                        writer.WriteAttributeString("bitrate", sett.VidBitrate);

                    if (sett.Resolution != null && (filterSet == null || filterSet.Contains("res@resolution")))
                        writer.WriteAttributeString("resolution", sett.Resolution);

                    writer.WriteAttributeString("protocolInfo", string.Format("http-get:*:{0}:{1}{2}", sett.GetMime(), sett.GetDlnaType(), settings.Video.EncodeFeature));
                    writer.WriteValue(string.Format("{0}/encode/video?id={1}{2}{3}", host, this.Id, sett.GetParamString(), this.Path));
                    writer.WriteEndElement();
                }
            }

            writer.WriteEndElement();
        }

        public override void BrowseWebMetadata(XmlWriter xmlWriter, MediaSettings settings, string idParams)
        {
            TimeSpan? duration = this.GetDuration();
            WriteHTML(xmlWriter, this.Id.ToString(), this.Title, duration.HasValue ? duration.Value.ToString("h':'mm':'ss") : null, 
                ((ItemContainerVideo)this.Parent).Resolution);
        }

        public override void GetWebPlayer(XmlWriter xmlWriter, Dictionary<string, string> urlParams)
        {
            WriteHTMLPlayer(xmlWriter, this.Id.ToString(), this.GetDuration(), urlParams, this.Path);
        }

        public static void WriteHTML(XmlWriter xmlWriter, string id, string title, string duration, string resolution)
        {
            xmlWriter.WriteStartElement("tr");

            xmlWriter.WriteStartElement("td");

            xmlWriter.WriteStartElement("a");
            xmlWriter.WriteAttributeString("href", "/web/player.html?id=" + id);
            xmlWriter.WriteAttributeString("target", "_blank");

            xmlWriter.WriteStartElement("img");
            xmlWriter.WriteAttributeString("src", "/web/images/htmlorangearrow.gif");
            xmlWriter.WriteAttributeString("alt", string.Empty);
            xmlWriter.WriteEndElement();

            xmlWriter.WriteStartElement("span");
            xmlWriter.WriteValue(LanguageResource.Play);
            xmlWriter.WriteEndElement();

            xmlWriter.WriteEndElement();

            xmlWriter.WriteStartElement("span");
            xmlWriter.WriteValue("|");
            xmlWriter.WriteEndElement();

            xmlWriter.WriteStartElement("a");
            xmlWriter.WriteAttributeString("href", "/web/control.html?id=" + id);
            xmlWriter.WriteValue(LanguageResource.PlayTo);
            xmlWriter.WriteEndElement();

            xmlWriter.WriteEndElement();

            xmlWriter.WriteElementString("td", title);
            xmlWriter.WriteElementString("td", duration == null ? string.Empty : duration);
            xmlWriter.WriteElementString("td", resolution == null ? string.Empty : resolution);

            xmlWriter.WriteEndElement();
        }

        public static void WriteHTMLPlayer(XmlWriter xmlWriter, string id, TimeSpan? duration, Dictionary<string, string> urlParams, string encodeParams)
        {
            string codec = null;
            if (urlParams.ContainsKey("codec"))
                codec = urlParams["codec"].ToLower();

            string quality = null;
            if (urlParams.ContainsKey("quality"))
                quality = urlParams["quality"].ToLower();

            string width = "800", height = "480";
            if (urlParams.ContainsKey("resolution"))
            {
                uint result;
                string[] res = urlParams["resolution"].Split(new char[] { 'x' }, StringSplitOptions.RemoveEmptyEntries);
                if (res.Length == 2 && uint.TryParse(res[0], out result) && uint.TryParse(res[1], out result))
                {
                    width = res[0];
                    height = res[1];
                }
            }
            string resolution = width + "x" + height;

            if (codec == "wmv")
            {
                string sourceUrl = string.Format("/encode/web?id={0}&codec=wmv2&width={1}&height={2}{3}", id, width, height, encodeParams);
                switch (quality)
                {
                    case "low": sourceUrl += "&vidbitrate=200&audbitrate=40&quality=1&fps=25"; break;
                    case "high": sourceUrl += "&vidbitrate=800&audbitrate=80&quality=51&fps=30"; break;
                    default /*"medium"*/: 
                        quality = "medium";
                        sourceUrl += "&vidbitrate=400&audbitrate=40&quality=1&fps=25";
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
                string sourceUrl = string.Format("/encode/web%3Fid={0}&codec=flv_ts&width={1}&height={2}{3}", id, width, height, encodeParams);
                switch (quality)
                {
                    case "low": sourceUrl += "&vidbitrate=200&audbitrate=64&quality=1&fps=25"; break;
                    case "high": sourceUrl += "&vidbitrate=1500&audbitrate=256&quality=51&fps=30"; break;
                    default /*"medium"*/: 
                        quality = "medium";
                        sourceUrl += "&vidbitrate=700&audbitrate=128&quality=1&fps=25";
                        break;
                }
                sourceUrl = sourceUrl.Replace("=", "%3D").Replace("&", "%26");

                xmlWriter.WriteStartElement("a");
                xmlWriter.WriteAttributeString("id", "videoLink");
                xmlWriter.WriteAttributeString("href", sourceUrl);
                xmlWriter.WriteAttributeString("style", string.Format("display:block;margin-left:auto;margin-right:auto;width:{0}px;height:{1}px;", width, height));
                xmlWriter.WriteValue(LanguageResource.Play + " " + LanguageResource.ThroughDirectLink);
                xmlWriter.WriteEndElement();
            }
            else
            {
                codec = "webm";

                string sourceUrl = string.Format("/encode/web?id={0}&codec=webm_ts&width={1}&height={2}{3}", id, width, height, encodeParams);
                switch (quality)
                {
                    case "low": sourceUrl += "&vidbitrate=200&audbitrate=64&quality=1"; break;
                    case "high": sourceUrl += "&vidbitrate=800&audbitrate=128&quality=51"; break;
                    default /*"medium"*/: 
                        quality = "medium";
                        sourceUrl += "&vidbitrate=400&audbitrate=64&quality=1";
                        break;
                }

                xmlWriter.WriteRaw(@"<video id=""streamVideo"" autoplay=""autoplay"" />");

                xmlWriter.WriteStartElement("source");
                xmlWriter.WriteAttributeString("src", sourceUrl);
                xmlWriter.WriteAttributeString("type", "video/webm; codecs=\"vp8.0, vorbis\"");
                xmlWriter.WriteEndElement();

                xmlWriter.WriteStartElement("span");
                xmlWriter.WriteAttributeString("id", "streamVideoSpan");

                xmlWriter.WriteStartElement("a");
                xmlWriter.WriteAttributeString("href", sourceUrl);
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
            xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""wmvRadio"" name=""codec"" value=""wmv"" {0} /><label for=""wmvRadio"">Wmv</label>",
                codec == "wmv" ? "checked=\"checked\"" : string.Empty));
            xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""flvRadio"" name=""codec"" value=""flv"" {0} /><label for=""flvRadio"">Flash</label>",
                codec == "flv" ? "checked=\"checked\"" : string.Empty));
            xmlWriter.WriteRaw(@"</span>");

            xmlWriter.WriteRaw(@"<span id=""resolutionRadios"">");
            xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""r1280x720Radio"" name=""resolution"" value=""1280x720"" {0} /><label for=""r1280x720Radio"">1280x720</label>",
                resolution == "1280x720" ? "checked=\"checked\"" : string.Empty));
            xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""r800x480Radio"" name=""resolution"" value=""800x480"" {0} /><label for=""r800x480Radio"">800x480</label>",
                resolution == "800x480" ? "checked=\"checked\"" : string.Empty));
            xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""r640x480Radio"" name=""resolution"" value=""640x480"" {0} /><label for=""r640x480Radio"">640x480</label>",
                resolution == "640x480" ? "checked=\"checked\"" : string.Empty));
            xmlWriter.WriteRaw(string.Format(@"<input type=""radio"" id=""r320x240Radio"" name=""resolution"" value=""320x240"" {0} /><label for=""r320x240Radio"">320x240</label>",
                resolution == "320x240" ? "checked=\"checked\"" : string.Empty));
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
    }
}
