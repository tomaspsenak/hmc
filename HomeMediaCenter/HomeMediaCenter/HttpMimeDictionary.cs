using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Win32;

namespace HomeMediaCenter
{
    public class HttpMimeDictionary
    {
        private Dictionary<string, HttpMime> mime = new Dictionary<string, HttpMime>(StringComparer.OrdinalIgnoreCase);
        private bool useRegistry = true;

        public void Add(HttpMime mime)
        {
            this.mime.Add(mime.Extension, mime);
        }

        public HttpMime this[string key]
        {
            get 
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
        }
    }
}
