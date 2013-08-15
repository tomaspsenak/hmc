using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HomeMediaCenter
{
    public enum MediaType { Image, Audio, Video, Other }

    public class HttpMime
    {
        private string extension;
        private string typeName;
        private MediaType mediaType;

        public HttpMime(string extension, string typeName, MediaType mediaType)
        {
            this.extension = extension;
            this.typeName = typeName;
            this.mediaType = mediaType;
        }

        public string Extension
        {
            get { return this.extension; }
        }

        public string TypeName
        {
            get { return this.typeName; }
        }

        public MediaType MediaType
        {
            get { return this.mediaType; }
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

        public static HttpMimeDictionary GetDefaultMime()
        {
            HttpMimeDictionary set = new HttpMimeDictionary();

            set.Add(new HttpMime(".asf", "x-ms-asf", MediaType.Video));
            set.Add(new HttpMime(".avi", "avi", MediaType.Video));
            set.Add(new HttpMime(".m1v", "mpeg", MediaType.Video));
            set.Add(new HttpMime(".m2v", "mpeg", MediaType.Video));
            set.Add(new HttpMime(".mkv", "x-matroska", MediaType.Video));
            set.Add(new HttpMime(".mp4", "mp4", MediaType.Video));
            set.Add(new HttpMime(".mpeg", "mpeg", MediaType.Video));
            set.Add(new HttpMime(".mpg", "mpeg", MediaType.Video));
            set.Add(new HttpMime(".webm", "webm", MediaType.Video));
            set.Add(new HttpMime(".wmv", "x-ms-wmv", MediaType.Video));

            set.Add(new HttpMime(".m2a", "mpeg", MediaType.Audio));
            set.Add(new HttpMime(".mid", "midi", MediaType.Audio));
            set.Add(new HttpMime(".midi", "midi", MediaType.Audio));
            set.Add(new HttpMime(".mp2", "mpeg", MediaType.Audio));
            set.Add(new HttpMime(".mp3", "mpeg", MediaType.Audio));
            set.Add(new HttpMime(".mpa", "mpeg", MediaType.Audio));
            set.Add(new HttpMime(".mpga", "mpeg", MediaType.Audio));
            set.Add(new HttpMime(".wav", "wav", MediaType.Audio));
            set.Add(new HttpMime(".wma", "x-ms-wma", MediaType.Audio));
            set.Add(new HttpMime(".flac", "flac", MediaType.Audio));

            set.Add(new HttpMime(".bmp", "bmp", MediaType.Image));
            set.Add(new HttpMime(".gif", "gif", MediaType.Image));
            set.Add(new HttpMime(".jpe", "jpeg", MediaType.Image));
            set.Add(new HttpMime(".jpeg", "jpeg", MediaType.Image));
            set.Add(new HttpMime(".jpg", "jpeg", MediaType.Image));
            set.Add(new HttpMime(".png", "png", MediaType.Image));
            set.Add(new HttpMime(".tif", "tiff", MediaType.Image));
            set.Add(new HttpMime(".tiff", "tiff", MediaType.Image));

            return set;
        }
    }
}
