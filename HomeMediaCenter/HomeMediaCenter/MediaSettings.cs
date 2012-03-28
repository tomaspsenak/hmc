using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace HomeMediaCenter
{
    public class MediaSettings
    {
        private bool audioNativeFile = true;
        private string audioFileFeature = "DLNA.ORG_OP=01;DLNA.ORG_FLAGS=01500000000000000000000000000000";
        private string audioEncodeFeature = "DLNA.ORG_OP=10;DLNA.ORG_FLAGS=01500000000000000000000000000000";
        private List<MediaSettingsAudio> audioEncode = new List<MediaSettingsAudio>() {
            MediaSettingsAudio.Create("codec=mpeg2_ps&audbitrate=128&video=0") };

        private bool imageNativeFile = true;
        private string imageFileFeature = "DLNA.ORG_OP=01;DLNA.ORG_FLAGS=00d00000000000000000000000000000";
        private string imageEncodeFeature = "DLNA.ORG_PN=JPEG_MED;DLNA.ORG_OP=01;DLNA.ORG_FLAGS=00d00000000000000000000000000000";
        private List<MediaSettingsImage> imageEncode = new List<MediaSettingsImage>() {
            MediaSettingsImage.Create("codec=jpeg&width=1280&height=720&keepaspect") };

        private bool videoNativeFile = true;
        private string videoFileFeature = "DLNA.ORG_OP=01;DLNA.ORG_FLAGS=01500000000000000000000000000000";
        private string videoEncodeFeature = "DLNA.ORG_PN=MPEG_PS_PAL;DLNA.ORG_OP=10;DLNA.ORG_CI=1;DLNA.ORG_FLAGS=01500000000000000000000000000000";
        private string videoStreamFeature = "DLNA.ORG_PN=MPEG_PS_PAL;DLNA.ORG_OP=00;DLNA.ORG_CI=1;DLNA.ORG_FLAGS=01500000000000000000000000000000";
        private List<MediaSettingsVideo> videoEncode = new List<MediaSettingsVideo>() {
            MediaSettingsVideo.Create("codec=mpeg2_ps&vidbitrate=3000&audbitrate=128&width=720&height=576&fps=25") };
        private List<MediaSettingsVideo> videoStreamEncode = new List<MediaSettingsVideo>() { 
            MediaSettingsVideo.Create("codec=mpeg2_ps&vidbitrate=3000&audbitrate=128&width=720&height=576"), 
            MediaSettingsVideo.Create("codec=mpeg2_ps&vidbitrate=5000&audbitrate=128&width=1280&height=768") };

        public void SaveSettings(XmlWriter xmlWriter)
        {
            //Zapisanie audio
            xmlWriter.WriteStartElement("AudioSettings");

            xmlWriter.WriteElementString("NativeFile", this.audioNativeFile.ToString());
            xmlWriter.WriteElementString("FileFeature", this.audioFileFeature);
            xmlWriter.WriteElementString("EncodeFeature", this.audioEncodeFeature);

            xmlWriter.WriteStartElement("Parameters");
            foreach (string par in this.audioEncode.Select(a => a.QueryString))
                xmlWriter.WriteElementString("string", par);
            xmlWriter.WriteEndElement();

            xmlWriter.WriteEndElement();

            //Zapisanie image
            xmlWriter.WriteStartElement("ImageSettings");

            xmlWriter.WriteElementString("NativeFile", this.imageNativeFile.ToString());
            xmlWriter.WriteElementString("FileFeature", this.imageFileFeature);
            xmlWriter.WriteElementString("EncodeFeature", this.imageEncodeFeature);

            xmlWriter.WriteStartElement("Parameters");
            foreach (string par in this.imageEncode.Select(a => a.QueryString))
                xmlWriter.WriteElementString("string", par);
            xmlWriter.WriteEndElement();
 
            xmlWriter.WriteEndElement();

            //Zapisanie video
            xmlWriter.WriteStartElement("VideoSettings");

            xmlWriter.WriteElementString("NativeFile", this.videoNativeFile.ToString());
            xmlWriter.WriteElementString("FileFeature", this.videoFileFeature);
            xmlWriter.WriteElementString("EncodeFeature", this.videoEncodeFeature);
            xmlWriter.WriteElementString("StreamFeature", this.videoStreamFeature);

            xmlWriter.WriteStartElement("Parameters");
            foreach (string par in this.videoEncode.Select(a => a.QueryString))
                xmlWriter.WriteElementString("string", par);
            xmlWriter.WriteEndElement();

            xmlWriter.WriteStartElement("StreamParameters");
            foreach (string par in this.videoStreamEncode.Select(a => a.QueryString))
                xmlWriter.WriteElementString("string", par);
            xmlWriter.WriteEndElement();

            xmlWriter.WriteEndElement();
        }

        public void LoadSettings(XmlDocument xmlReader)
        {
            //Nacitanie audio
            XmlNode node = xmlReader.SelectSingleNode("/HomeMediaCenter/AudioSettings/NativeFile");
            if (node != null)
                bool.TryParse(node.InnerText, out this.audioNativeFile);
            node = xmlReader.SelectSingleNode("/HomeMediaCenter/AudioSettings/FileFeature");
            if (node != null)
                this.audioFileFeature = node.InnerText;
            node = xmlReader.SelectSingleNode("/HomeMediaCenter/AudioSettings/EncodeFeature");
            if (node != null)
                this.audioEncodeFeature = node.InnerText;

            this.audioEncode.Clear();
            foreach (XmlNode param in xmlReader.SelectNodes("/HomeMediaCenter/AudioSettings/Parameters/*"))
                this.audioEncode.Add(MediaSettingsAudio.Create(param.InnerText));

            //Nacitanie image
            node = xmlReader.SelectSingleNode("/HomeMediaCenter/ImageSettings/NativeFile");
            if (node != null)
                bool.TryParse(node.InnerText, out this.imageNativeFile);
            node = xmlReader.SelectSingleNode("/HomeMediaCenter/ImageSettings/FileFeature");
            if (node != null)
                this.imageFileFeature = node.InnerText;
            node = xmlReader.SelectSingleNode("/HomeMediaCenter/ImageSettings/EncodeFeature");
            if (node != null)
                this.imageEncodeFeature = node.InnerText;

            this.imageEncode.Clear();
            foreach (XmlNode param in xmlReader.SelectNodes("/HomeMediaCenter/ImageSettings/Parameters/*"))
                this.imageEncode.Add(MediaSettingsImage.Create(param.InnerText));

            //Nacitanie video
            node = xmlReader.SelectSingleNode("/HomeMediaCenter/VideoSettings/NativeFile");
            if (node != null)
                bool.TryParse(node.InnerText, out this.videoNativeFile);
            node = xmlReader.SelectSingleNode("/HomeMediaCenter/VideoSettings/FileFeature");
            if (node != null)
                this.videoFileFeature = node.InnerText;
            node = xmlReader.SelectSingleNode("/HomeMediaCenter/VideoSettings/EncodeFeature");
            if (node != null)
                this.videoEncodeFeature = node.InnerText;
            node = xmlReader.SelectSingleNode("/HomeMediaCenter/VideoSettings/StreamFeature");
            if (node != null)
                this.videoStreamFeature = node.InnerText;

            this.videoEncode.Clear();
            foreach (XmlNode param in xmlReader.SelectNodes("/HomeMediaCenter/VideoSettings/Parameters/*"))
                this.videoEncode.Add(MediaSettingsVideo.Create(param.InnerText));

            this.videoStreamEncode.Clear();
            foreach (XmlNode param in xmlReader.SelectNodes("/HomeMediaCenter/VideoSettings/StreamParameters/*"))
                this.videoStreamEncode.Add(MediaSettingsVideo.Create(param.InnerText));
        }

        public bool AudioNativeFile
        {
            get { return this.audioNativeFile; }
            set { this.audioNativeFile = value; }
        }

        public List<MediaSettingsAudio> AudioEncode
        {
            get { return this.audioEncode; }
            set { this.audioEncode = value; }
        }

        public string AudioFileFeature
        {
            get { return this.audioFileFeature; }
            set { this.audioFileFeature = value; }
        }

        public string AudioEncodeFeature
        {
            get { return this.audioEncodeFeature; }
            set { this.audioEncodeFeature = value; }
        }

        public bool ImageNativeFile
        {
            get { return this.imageNativeFile; }
            set { this.imageNativeFile = value; }
        }

        public string ImageFileFeature
        {
            get { return this.imageFileFeature; }
            set { this.imageFileFeature = value; }
        }

        public string ImageEncodeFeature
        {
            get { return this.imageEncodeFeature; }
            set { this.imageEncodeFeature = value; }
        }

        public List<MediaSettingsImage> ImageEncode
        {
            get { return this.imageEncode; }
            set { this.imageEncode = value; }
        }

        public bool VideoNativeFile
        {
            get { return this.videoNativeFile; }
            set { this.videoNativeFile = value; }
        }

        public List<MediaSettingsVideo> VideoStreamEncode
        {
            get { return this.videoStreamEncode; }
            set { this.videoStreamEncode = value; }
        }

        public List<MediaSettingsVideo> VideoEncode
        {
            get { return this.videoEncode; }
            set { this.videoEncode = value; }
        }

        public string VideoFileFeature
        {
            get { return this.videoFileFeature; }
            set { this.videoFileFeature = value; }
        }

        public string VideoEncodeFeature
        {
            get { return this.videoEncodeFeature; }
            set { this.videoEncodeFeature = value; }
        }

        public string VideoStreamFeature
        {
            get { return this.videoStreamFeature; }
            set { this.videoStreamFeature = value; }
        }

        public static Dictionary<string, string> ParseQueryString(string queryString)
        {
            Dictionary<string, string> parameters = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
            foreach (string parameter in queryString.Split(new char[] { '&' }, StringSplitOptions.RemoveEmptyEntries))
            {
                string[] keyValue = parameter.Split(new char[] { '=' }, 2, StringSplitOptions.RemoveEmptyEntries);
                parameters[keyValue[0]] = (keyValue.Length == 2) ? keyValue[1] : string.Empty;
            }
            return parameters;
        }
    }

    public class MediaSettingsAudio
    {
        private string queryString;
        private string mime;
        private string audBitrate;

        private MediaSettingsAudio() { }

        public static MediaSettingsAudio Create(string queryString)
        {
            MediaSettingsAudio audio = new MediaSettingsAudio();

            Dictionary<string, string> parameters = MediaSettings.ParseQueryString(queryString);

            EncoderBuilder enc = EncoderBuilder.GetEncoder(parameters);
            if (enc == null)
                throw new MediaCenterException("Unknown codec");

            audio.mime = enc.GetMime();

            if (parameters.ContainsKey("audBitrate"))
                audio.audBitrate = ((uint.Parse(parameters["audBitrate"]) * 1000) / 8).ToString();

            audio.queryString = queryString.StartsWith("&") ? queryString : "&" + queryString;
            return audio;
        }

        public string QueryString
        {
            get { return this.queryString; }
        }

        public string Mime
        {
            get { return this.mime; }
        }

        public string AudBitrate
        {
            get { return this.audBitrate; }
        }
    }

    public class MediaSettingsVideo
    {
        private string queryString;
        private string mime;
        private string vidBitrate;
        uint height;
        uint width;

        private MediaSettingsVideo() { }

        public static MediaSettingsVideo Create(string queryString)
        {
            MediaSettingsVideo video = new MediaSettingsVideo();

            Dictionary<string, string> parameters = MediaSettings.ParseQueryString(queryString);

            EncoderBuilder enc = EncoderBuilder.GetEncoder(parameters);
            if (enc == null)
                throw new MediaCenterException("Unknown codec");

            video.mime = enc.GetMime();

            if (parameters.ContainsKey("vidBitrate"))
                video.vidBitrate = ((uint.Parse(parameters["vidBitrate"]) * 1000) / 8).ToString();

            if (parameters.ContainsKey("width") && parameters.ContainsKey("height"))
            {
                video.width = uint.Parse(parameters["width"]);
                video.height = uint.Parse(parameters["height"]);
            }

            video.queryString = queryString.StartsWith("&") ? queryString : "&" + queryString;
            return video;
        }

        public string QueryString
        {
            get { return this.queryString; }
        }

        public string Mime
        {
            get { return this.mime; }
        }

        public string VidBitrate
        {
            get { return this.vidBitrate; }
        }

        public string Resolution
        {
            get { return this.width + "x" + this.height; }
        }

        public uint Height
        {
            get { return this.height; }
        }

        public uint Width
        {
            get { return this.width; }
        }
    }

    public class MediaSettingsImage
    {
        private string queryString;
        private string mime;
        private string resolution;

        private MediaSettingsImage() { }

        public static MediaSettingsImage Create(string queryString)
        {
            MediaSettingsImage image = new MediaSettingsImage();

            Dictionary<string, string> parameters = MediaSettings.ParseQueryString(queryString);

            EncoderBuilder enc = EncoderBuilder.GetEncoder(parameters);
            if (enc == null)
                throw new MediaCenterException("Unknown codec");

            image.mime = enc.GetMime();

            if (parameters.ContainsKey("width") && parameters.ContainsKey("height"))
                image.resolution = parameters["width"] + "x" + parameters["height"];

            image.queryString = queryString.StartsWith("&") ? queryString : "&" + queryString;
            return image;
        }

        public string QueryString
        {
            get { return this.queryString; }
        }

        public string Mime
        {
            get { return this.mime; }
        }

        public string Resolution
        {
            get { return this.resolution; }
        }
    }
}
