using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace HomeMediaCenter
{
    public enum MediaType { Image, Audio, Video, Other }

    [XmlType]
    public class HttpMime
    {
        private string extension;
        private string typeName;
        private MediaType mediaType;

        public HttpMime() { }

        public HttpMime(string extension)
        {
            this.extension = extension;
        }

        public HttpMime(string extension, string typeName, MediaType mediaType) : this(extension)
        {
            this.typeName = typeName;
            this.mediaType = mediaType;
        }

        [XmlAttribute("Extension")]
        public string Extension
        {
            get { return this.extension; }
            set { this.extension = value; }
        }

        [XmlAttribute("Name")]
        public string TypeName
        {
            get { return this.typeName; }
            set { this.typeName = value; }
        }

        [XmlAttribute("MediaType")]
        public MediaType MediaType
        {
            get { return this.mediaType; }
            set { this.mediaType = value; }
        }

        public override string ToString()
        {
            switch (this.mediaType)
            {
                case MediaType.Audio: return "audio/" + this.typeName;
                case MediaType.Image: return "image/" + this.typeName;
                case MediaType.Video: return "video/" + this.typeName;
            }

            return string.Empty;
        }

        public override bool Equals(object obj)
        {
            HttpMime mime = obj as HttpMime;
            if (mime == null)
                return false;

            return this.extension == mime.extension;
        }

        public override int GetHashCode()
        {
            return this.extension.GetHashCode();
        }
    }
}
