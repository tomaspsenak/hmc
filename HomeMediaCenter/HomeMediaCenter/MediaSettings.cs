using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Collections.ObjectModel;

namespace HomeMediaCenter
{
    public class MediaSettings
    {
        public class MediaSettingsBase
        {
            protected readonly string prefix;
            protected readonly MediaServerDevice device;

            private ReadOnlyCollection<EncoderBuilder> encode;

            public MediaSettingsBase(MediaServerDevice device, string prefix, params EncoderBuilder[] encode)
            {
                this.prefix = prefix;
                this.device = device;

                this.encode = new ReadOnlyCollection<EncoderBuilder>(encode);
            }

            public ReadOnlyCollection<EncoderBuilder> Encode
            {
                get { return this.encode; }
                set
                {
                    this.device.CheckStopped();

                    this.encode = value;
                    this.device.SettingsChanged();
                }
            }

            public void SaveSettings(XmlWriter xmlWriter)
            {
                xmlWriter.WriteStartElement(this.prefix);

                SaveSpecificSettings(xmlWriter);

                xmlWriter.WriteStartElement("Parameters");
                foreach (string par in this.encode.Select(a => a.GetParamString()))
                    xmlWriter.WriteElementString("string", par);
                xmlWriter.WriteEndElement();

                xmlWriter.WriteEndElement();
            }

            public void LoadSettings(XmlDocument xmlReader)
            {
                this.encode = xmlReader.SelectNodes("/HomeMediaCenter/" + this.prefix + "/Parameters/*").Cast<XmlNode>().Select(
                    a => EncoderBuilder.GetEncoder(a.InnerText)).ToList().AsReadOnly();

                LoadSpecificSettings(xmlReader);
            }

            protected virtual void SaveSpecificSettings(XmlWriter xmlWriter) { }
            protected virtual void LoadSpecificSettings(XmlDocument xmlReader) { }
        }

        public class MediaSettingsIMG : MediaSettingsBase
        {
            private bool nativeFile;
            private string fileFeature;
            private string encodeFeature;

            public MediaSettingsIMG(MediaServerDevice device, string prefix, bool nativeFile, string fileFeature, string encodeFeature, params EncoderBuilder[] encode) :
                base(device, prefix, encode)
            {
                this.nativeFile = nativeFile;
                this.fileFeature = fileFeature;
                this.encodeFeature = encodeFeature;
            }

            public bool NativeFile
            {
                get { return this.nativeFile; }
                set
                {
                    this.device.CheckStopped();

                    this.nativeFile = value;
                    this.device.SettingsChanged();
                }
            }

            public string FileFeature
            {
                get { return this.fileFeature; }
                set
                {
                    this.device.CheckStopped();

                    this.fileFeature = value;
                    this.device.SettingsChanged();
                }
            }

            public string EncodeFeature
            {
                get { return this.encodeFeature; }
                set
                {
                    this.device.CheckStopped();

                    this.encodeFeature = value;
                    this.device.SettingsChanged();
                }
            }

            protected override void SaveSpecificSettings(XmlWriter xmlWriter)
            {
                xmlWriter.WriteElementString("NativeFile", this.nativeFile.ToString());
                xmlWriter.WriteElementString("FileFeature", this.fileFeature);
                xmlWriter.WriteElementString("EncodeFeature", this.encodeFeature);
            }

            protected override void LoadSpecificSettings(XmlDocument xmlReader)
            {
                XmlNode node = xmlReader.SelectSingleNode("/HomeMediaCenter/" + this.prefix + "/NativeFile");
                if (node != null)
                    bool.TryParse(node.InnerText, out this.nativeFile);
                node = xmlReader.SelectSingleNode("/HomeMediaCenter/" + this.prefix + "/FileFeature");
                if (node != null)
                    this.fileFeature = node.InnerText;
                node = xmlReader.SelectSingleNode("/HomeMediaCenter/" + this.prefix + "/EncodeFeature");
                if (node != null)
                    this.encodeFeature = node.InnerText;
            }
        }

        public class MediaSettingsAV : MediaSettingsIMG
        {
            private string streamFeature;

            public MediaSettingsAV(MediaServerDevice device, string prefix, bool nativeFile, string fileFeature, string encodeFeature, string streamFeature, params EncoderBuilder[] encode)
                : base(device, prefix, nativeFile, fileFeature, encodeFeature, encode)
            {
                this.streamFeature = streamFeature;
            }

            public string StreamFeature
            {
                get { return this.streamFeature; }
                set
                {
                    this.device.CheckStopped();

                    this.streamFeature = value;
                    this.device.SettingsChanged();
                }
            }

            protected override void SaveSpecificSettings(XmlWriter xmlWriter)
            {
                base.SaveSpecificSettings(xmlWriter);

                xmlWriter.WriteElementString("StreamFeature", this.streamFeature);
            }

            protected override void LoadSpecificSettings(XmlDocument xmlReader)
            {
                base.LoadSpecificSettings(xmlReader);

                XmlNode node = xmlReader.SelectSingleNode("/HomeMediaCenter/" + this.prefix + "/StreamFeature");
                if (node != null)
                    this.streamFeature = node.InnerText;
            }
        }

        private readonly MediaSettingsAV audio;
        private readonly MediaSettingsIMG image;
        private readonly MediaSettingsAV video;
        private readonly MediaSettingsBase stream;

        public MediaSettings(MediaServerDevice upnpDevice)
        {
            this.audio = new MediaSettingsAV(upnpDevice, "AudioSettings",
                true,
                "DLNA.ORG_OP=01;DLNA.ORG_FLAGS=01500000000000000000000000000000",
                "DLNA.ORG_OP=10;DLNA.ORG_FLAGS=01500000000000000000000000000000",
                "DLNA.ORG_OP=00;DLNA.ORG_FLAGS=01500000000000000000000000000000",
                EncoderBuilder.GetEncoder("&codec=mp3_ts&audbitrate=128&video=0&quality=100"));

            this.image = new MediaSettingsIMG(upnpDevice, "ImageSettings",
                true,
                "DLNA.ORG_OP=01;DLNA.ORG_FLAGS=00d00000000000000000000000000000",
                "DLNA.ORG_OP=01;DLNA.ORG_FLAGS=00d00000000000000000000000000000",
                EncoderBuilder.GetEncoder("&codec=jpeg&width=160&height=160&keepaspect"));

            this.video = new MediaSettingsAV(upnpDevice, "VideoSettings",
                true,
                "DLNA.ORG_OP=01;DLNA.ORG_FLAGS=01500000000000000000000000000000",
                "DLNA.ORG_OP=10;DLNA.ORG_CI=1;DLNA.ORG_FLAGS=01500000000000000000000000000000",
                "DLNA.ORG_OP=00;DLNA.ORG_CI=1;DLNA.ORG_FLAGS=01500000000000000000000000000000",
                EncoderBuilder.GetEncoder("&codec=mpeg2_ps&vidbitrate=3000&audbitrate=128&width=720&height=576&fps=25"));

            this.stream = new MediaSettingsBase(upnpDevice, "StreamSettings",
                EncoderBuilder.GetEncoder("&codec=mpeg2_ps&vidbitrate=3000&audbitrate=128&width=720&height=576&fps=25"),
                EncoderBuilder.GetEncoder("&codec=mpeg2_ps&vidbitrate=5000&audbitrate=128&width=1280&height=768&fps=25&audio=0"),
                EncoderBuilder.GetEncoder("&codec=mpeg2_ps&vidbitrate=8000&audbitrate=128&width=1920&height=1080&fps=25&audio=0"),
                EncoderBuilder.GetEncoder("&codec=mp3_ts&audbitrate=128&video=0&quality=100"));
        }

        public MediaSettingsAV Audio
        {
            get { return this.audio; }
        }

        public MediaSettingsIMG Image
        {
            get { return this.image; }
        }

        public MediaSettingsAV Video
        {
            get { return this.video; }
        }

        public MediaSettingsBase Stream
        {
            get { return this.stream; }
        }

        public void SaveSettings(XmlWriter xmlWriter)
        {
            this.audio.SaveSettings(xmlWriter);
            this.image.SaveSettings(xmlWriter);
            this.video.SaveSettings(xmlWriter);
            this.stream.SaveSettings(xmlWriter);
        }

        public void LoadSettings(XmlDocument xmlReader)
        {
            this.audio.LoadSettings(xmlReader);
            this.image.LoadSettings(xmlReader);
            this.video.LoadSettings(xmlReader);
            this.stream.LoadSettings(xmlReader);
        }
    }
}
