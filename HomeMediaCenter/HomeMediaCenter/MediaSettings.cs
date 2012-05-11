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
        private List<EncoderBuilder> audioEncode = new List<EncoderBuilder>() {
            EncoderBuilder.GetEncoder("&codec=mpeg2_ps&audbitrate=128&video=0") };

        private bool imageNativeFile = true;
        private string imageFileFeature = "DLNA.ORG_OP=01;DLNA.ORG_FLAGS=00d00000000000000000000000000000";
        private string imageEncodeFeature = "DLNA.ORG_OP=01;DLNA.ORG_FLAGS=00d00000000000000000000000000000";
        private List<EncoderBuilder> imageEncode = new List<EncoderBuilder>() {
            EncoderBuilder.GetEncoder("&codec=jpeg&width=1280&height=720&keepaspect"),
            EncoderBuilder.GetEncoder("&codec=jpeg&width=160&height=160&keepaspect") };

        private bool videoNativeFile = true;
        private string videoFileFeature = "DLNA.ORG_OP=01;DLNA.ORG_FLAGS=01500000000000000000000000000000";
        private string videoEncodeFeature = "DLNA.ORG_OP=10;DLNA.ORG_CI=1;DLNA.ORG_FLAGS=01500000000000000000000000000000";
        private string videoStreamFeature = "DLNA.ORG_OP=00;DLNA.ORG_CI=1;DLNA.ORG_FLAGS=01500000000000000000000000000000";
        private List<EncoderBuilder> videoEncode = new List<EncoderBuilder>() {
            EncoderBuilder.GetEncoder("&codec=mpeg2_ps&vidbitrate=3000&audbitrate=128&width=720&height=576") };
        private List<EncoderBuilder> videoStreamEncode = new List<EncoderBuilder>() { 
            EncoderBuilder.GetEncoder("&codec=mpeg2_ps&vidbitrate=3000&audbitrate=128&width=720&height=576"), 
            EncoderBuilder.GetEncoder("&codec=mpeg2_ps&vidbitrate=5000&audbitrate=128&width=1280&height=768"),
            EncoderBuilder.GetEncoder("&codec=mpeg2_ps&vidbitrate=9000&audbitrate=128&width=1920&height=1080&audio=0")};

        public void SaveSettings(XmlWriter xmlWriter)
        {
            //Zapisanie audio
            xmlWriter.WriteStartElement("AudioSettings");

            xmlWriter.WriteElementString("NativeFile", this.audioNativeFile.ToString());
            xmlWriter.WriteElementString("FileFeature", this.audioFileFeature);
            xmlWriter.WriteElementString("EncodeFeature", this.audioEncodeFeature);

            xmlWriter.WriteStartElement("Parameters");
            foreach (string par in this.audioEncode.Select(a => a.GetParamString()))
                xmlWriter.WriteElementString("string", par);
            xmlWriter.WriteEndElement();

            xmlWriter.WriteEndElement();

            //Zapisanie image
            xmlWriter.WriteStartElement("ImageSettings");

            xmlWriter.WriteElementString("NativeFile", this.imageNativeFile.ToString());
            xmlWriter.WriteElementString("FileFeature", this.imageFileFeature);
            xmlWriter.WriteElementString("EncodeFeature", this.imageEncodeFeature);

            xmlWriter.WriteStartElement("Parameters");
            foreach (string par in this.imageEncode.Select(a => a.GetParamString()))
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
            foreach (string par in this.videoEncode.Select(a => a.GetParamString()))
                xmlWriter.WriteElementString("string", par);
            xmlWriter.WriteEndElement();

            xmlWriter.WriteStartElement("StreamParameters");
            foreach (string par in this.videoStreamEncode.Select(a => a.GetParamString()))
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
                this.audioEncode.Add(EncoderBuilder.GetEncoder(param.InnerText));

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
                this.imageEncode.Add(EncoderBuilder.GetEncoder(param.InnerText));

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
                this.videoEncode.Add(EncoderBuilder.GetEncoder(param.InnerText));

            this.videoStreamEncode.Clear();
            foreach (XmlNode param in xmlReader.SelectNodes("/HomeMediaCenter/VideoSettings/StreamParameters/*"))
                this.videoStreamEncode.Add(EncoderBuilder.GetEncoder(param.InnerText));
        }

        public bool AudioNativeFile
        {
            get { return this.audioNativeFile; }
            set { this.audioNativeFile = value; }
        }

        public List<EncoderBuilder> AudioEncode
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

        public List<EncoderBuilder> ImageEncode
        {
            get { return this.imageEncode; }
            set { this.imageEncode = value; }
        }

        public bool VideoNativeFile
        {
            get { return this.videoNativeFile; }
            set { this.videoNativeFile = value; }
        }

        public List<EncoderBuilder> VideoStreamEncode
        {
            get { return this.videoStreamEncode; }
            set { this.videoStreamEncode = value; }
        }

        public List<EncoderBuilder> VideoEncode
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
    }
}
