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

        public static EncoderBuilder GetEncoder(Dictionary<string, string> parameters)
        {
            EncoderBuilder encoder;

            if (!parameters.ContainsKey("codec"))
                return null;

            if ((encoder = DirectShowEncoder.TryCreate(parameters)) != null)
                return encoder;
            else if ((encoder = MediaFoundationEncoder.TryCreate(parameters)) != null)
                return encoder;
            return GDIEncoder.TryCreate(parameters);
        }

        public void StartEncode(Stream output, Action<double> progressChangeDel)
        {
            this.progressChangeDel = progressChangeDel;
            StartEncode(output);
        }

        public virtual void StopEncode() { }

        public abstract void StartEncode(Stream output);

        public abstract string GetMime();
    }
}
