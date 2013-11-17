using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Win32;
using System.Xml;
using System.Xml.Serialization;

namespace HomeMediaCenter
{
    [XmlType("MimeTypes")]
    public class HttpMimeDictionary
    {
        private Dictionary<string, HttpMime> mime = new Dictionary<string, HttpMime>(StringComparer.OrdinalIgnoreCase);
        private bool useRegistry = true;

        [XmlElement("Type")]
        public HttpMime[] Items
        {
            get { return this.mime.Values.ToArray(); }
            set { this.mime = value.ToDictionary(a => a.Extension, StringComparer.OrdinalIgnoreCase); }
        }

        [XmlAttribute("UseRegistry")]
        public bool UseRegistry
        {
            get { return this.useRegistry; }
            set { this.useRegistry = value; }
        }

        public void Add(HttpMime mime)
        {
            this.mime.Add(mime.Extension, mime);
        }

        public HttpMime GetValue(string key)
        {
            if (this.mime.ContainsKey(key))
                return this.mime[key];

            if (useRegistry)
            {
                RegistryKey regKey = Registry.ClassesRoot.OpenSubKey(key);
                if (regKey != null && regKey.GetValue("Content Type") != null)
                {
                    string[] mimeType = regKey.GetValue("Content Type").ToString().Split(new char[] { '/' }, 2);
                    if (mimeType.Length == 2)
                    {
                        MediaType mediaEnum;
                        if (Enum.TryParse<MediaType>(mimeType[0], true, out mediaEnum))
                        {
                            return new HttpMime(key, mimeType[1], mediaEnum);
                        }
                    }
                }
            }

            return null;
        }

        public void SerializeMe(XmlWriter xmlWriter)
        {
            XmlSerializer serializer = new XmlSerializer(typeof(HttpMimeDictionary));
            serializer.Serialize(xmlWriter, this);
        }

        public static HttpMimeDictionary DeserializeMe(XmlDocument xmlReader)
        {
            using (XmlNodeReader node = new XmlNodeReader(xmlReader.SelectSingleNode("/HomeMediaCenter/MimeTypes")))
            {
                XmlSerializer serializer = new XmlSerializer(typeof(HttpMimeDictionary));
                return (HttpMimeDictionary)serializer.Deserialize(node);
            }
        }

        public static HttpMimeDictionary GetDefaults()
        {
            HttpMimeDictionary set = new HttpMimeDictionary();

            set.Add(new HttpMime(".asf", "x-ms-asf", MediaType.Video));
            set.Add(new HttpMime(".avi", "avi", MediaType.Video));
            set.Add(new HttpMime(".flv", "x-flv", MediaType.Video));
            set.Add(new HttpMime(".m1v", "mpeg", MediaType.Video));
            set.Add(new HttpMime(".m2v", "mpeg", MediaType.Video));
            set.Add(new HttpMime(".mkv", "x-matroska", MediaType.Video));
            set.Add(new HttpMime(".mp4", "mp4", MediaType.Video));
            set.Add(new HttpMime(".mpeg", "mpeg", MediaType.Video));
            set.Add(new HttpMime(".mpg", "mpeg", MediaType.Video));
            set.Add(new HttpMime(".webm", "webm", MediaType.Video));
            set.Add(new HttpMime(".wmv", "x-ms-wmv", MediaType.Video));

            set.Add(new HttpMime(".flac", "flac", MediaType.Audio));
            set.Add(new HttpMime(".m2a", "mpeg", MediaType.Audio));
            set.Add(new HttpMime(".mid", "midi", MediaType.Audio));
            set.Add(new HttpMime(".midi", "midi", MediaType.Audio));
            set.Add(new HttpMime(".mp2", "mpeg", MediaType.Audio));
            set.Add(new HttpMime(".mp3", "mpeg", MediaType.Audio));
            set.Add(new HttpMime(".mpa", "mpeg", MediaType.Audio));
            set.Add(new HttpMime(".mpga", "mpeg", MediaType.Audio));
            set.Add(new HttpMime(".wav", "wav", MediaType.Audio));
            set.Add(new HttpMime(".wma", "x-ms-wma", MediaType.Audio));

            set.Add(new HttpMime(".bmp", "bmp", MediaType.Image));
            set.Add(new HttpMime(".gif", "gif", MediaType.Image));
            set.Add(new HttpMime(".jpe", "jpeg", MediaType.Image));
            set.Add(new HttpMime(".jpeg", "jpeg", MediaType.Image));
            set.Add(new HttpMime(".jpg", "jpeg", MediaType.Image));
            set.Add(new HttpMime(".png", "png", MediaType.Image));
            set.Add(new HttpMime(".tif", "tiff", MediaType.Image));
            set.Add(new HttpMime(".tiff", "tiff", MediaType.Image));

            set.Add(new HttpMime(".m3u", "mpegurl", MediaType.Other));

            return set;
        }
    }
}
