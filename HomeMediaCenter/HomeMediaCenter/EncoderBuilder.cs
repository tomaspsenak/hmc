using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace HomeMediaCenter
{
    public abstract class EncoderBuilder
    {
        protected Action<double> progressChangeDel;
        protected Dictionary<string, string> parameters;
        protected string paramString;

        protected uint? video;
        protected uint? audio;
        protected uint? width;
        protected uint? height;
        protected uint? vidBitrate;
        protected uint? audBitrate;

        protected EncoderBuilder() { }

        public static EncoderBuilder GetEncoder(Dictionary<string, string> parameters)
        {
            EncoderBuilder encoder;

            if ((encoder = DirectShowEncoder.TryCreate(parameters)) != null ||
               (encoder = MediaFoundationEncoder.TryCreate(parameters)) != null ||
               (encoder = GDIEncoder.TryCreate(parameters)) != null)
            {
                if (parameters.ContainsKey("video"))
                    encoder.video = uint.Parse(parameters["video"]);
                if (parameters.ContainsKey("audio"))
                    encoder.audio = uint.Parse(parameters["audio"]);
                if (parameters.ContainsKey("width"))
                    encoder.width = uint.Parse(parameters["width"]);
                if (parameters.ContainsKey("height"))
                    encoder.height = uint.Parse(parameters["height"]);
                if (parameters.ContainsKey("vidbitrate"))
                    encoder.vidBitrate = uint.Parse(parameters["vidbitrate"]);
                if (parameters.ContainsKey("audbitrate"))
                    encoder.audBitrate = uint.Parse(parameters["audbitrate"]);

                return encoder;
            }

            throw new MediaCenterException("Unknown codec");
        }

        public static EncoderBuilder GetEncoder(string paramString)
        {
            Dictionary<string, string> parameters = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
            foreach (string parameter in paramString.Split(new char[] { '&' }, StringSplitOptions.RemoveEmptyEntries))
            {
                string[] keyValue = parameter.Split(new char[] { '=' }, 2, StringSplitOptions.RemoveEmptyEntries);
                parameters[keyValue[0]] = (keyValue.Length == 2) ? keyValue[1] : string.Empty;
            }

            EncoderBuilder encoder = GetEncoder(parameters);
            encoder.paramString = paramString;

            return encoder;
        }

        public string AudBitrate
        {
            get 
            {
                if (this.audBitrate.HasValue)
                    return ((this.audBitrate * 1000) / 8).ToString();
                return null;
            }
        }

        public string VidBitrate
        {
            get
            {
                if (this.vidBitrate.HasValue)
                    return ((this.vidBitrate * 1000) / 8).ToString();
                return null;
            }
        }

        public string Resolution
        {
            get 
            {
                if (this.width.HasValue && this.height.HasValue)
                    return this.width + "x" + this.height;
                return null;
            }
        }

        public bool Audio
        {
            get
            {
                if (this.audio.HasValue && this.audio.Value == 0)
                    return false;
                return true;
            }
        }

        public bool Video
        {
            get
            {
                if (this.video.HasValue && this.video.Value == 0)
                    return false;
                return true;
            }
        }

        public void StartEncode(Stream output, Action<double> progressChangeDel)
        {
            this.progressChangeDel = progressChangeDel;
            StartEncode(output);
        }

        public virtual void StopEncode() { }

        public virtual void StartEncode(Stream input, Stream output)
        {
            throw new NotImplementedException("Encoding with input stream is not supported");
        }

        public abstract void StartEncode(Stream output);

        public abstract string GetMime();

        public abstract string GetDlnaType();

        public string GetParamString()
        {
            if (this.paramString == null)
            {
                StringBuilder sb = new StringBuilder();
                foreach (KeyValuePair<string, string> kvp in this.parameters)
                    sb.AppendFormat("&{0}={1}", kvp.Key, kvp.Value);
                this.paramString = sb.ToString();
            }

            return this.paramString.StartsWith("&") ? this.paramString : "&" + this.paramString;
        }
    }
}
