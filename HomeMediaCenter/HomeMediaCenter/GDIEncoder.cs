using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;

namespace HomeMediaCenter
{
    internal class GDIEncoder : EncoderBuilder
    {
        private enum GDICodec { BMP, JPEG, PNG }
        private Dictionary<string, string> parameters;
        private GDICodec codec;

        public static GDIEncoder TryCreate(Dictionary<string, string> parameters)
        {
            GDICodec codecEnum;
            if (Enum.TryParse<GDICodec>(parameters["codec"], true, out codecEnum))
            {
                GDIEncoder encoder = new GDIEncoder();
                encoder.codec = codecEnum;
                encoder.parameters = parameters;
                return encoder;
            }

            return null;
        }

        public override string GetMime()
        {
            switch (this.codec)
            {
                case GDICodec.BMP: return "image/bmp";
                case GDICodec.JPEG: return "image/jpeg";
                case GDICodec.PNG: return "image/png";
            }

            return string.Empty;
        }

        public override void StartEncode(Stream output)
        {
            using (Image origImage = Image.FromFile(parameters["source"]))
            {
                //Zistenie sirky a vysky, povodna hodnota ak nezadane
                int width = 0, height = 0;
                if (parameters.ContainsKey("width"))
                    width = (int)uint.Parse(parameters["width"]);
                else
                    width = origImage.Width;
                if (parameters.ContainsKey("height"))
                    height = (int)uint.Parse(parameters["height"]);
                else
                    height = origImage.Height;

                //Zistenie kvality obrazka
                uint quality = 50;
                if (parameters.ContainsKey("quality"))
                {
                    quality = uint.Parse(parameters["quality"]);
                    if (quality < 1 || quality > 100)
                        throw new HttpException(400, "Encode quality must be in range 1-100");
                }

                int posX, posY, newHeight, newWidth;
                //Zistenie ci zachovat pomer stran pri zmene rozlisenia
                if (parameters.ContainsKey("keepaspect"))
                {
                    double ratio = Math.Min((double)width / (double)origImage.Width, (double)height / (double)origImage.Height);

                    newHeight = (int)(origImage.Height * ratio);
                    newWidth = (int)(origImage.Width * ratio);

                    //Vypocitanie X a Y suradnice
                    posX = Convert.ToInt32((width - newWidth) / 2);
                    posY = Convert.ToInt32((height - newHeight) / 2);
                }
                else
                {
                    posX = 0;
                    posY = 0;
                    newWidth = width;
                    newHeight = height;
                }

                using (Bitmap newImage = new Bitmap(width, height))
                {
                    using (Graphics graphic = Graphics.FromImage((Image)newImage))
                    {
                        if (quality < 20)
                            graphic.InterpolationMode = InterpolationMode.NearestNeighbor;
                        else if (quality < 40)
                            graphic.InterpolationMode = InterpolationMode.Bilinear;
                        else if (quality < 60)
                            graphic.InterpolationMode = InterpolationMode.HighQualityBilinear;
                        else if (quality < 80)
                            graphic.InterpolationMode = InterpolationMode.Bicubic;
                        else
                            graphic.InterpolationMode = InterpolationMode.HighQualityBicubic;

                        graphic.DrawImage(origImage, posX, posY, newWidth, newHeight);
                    }

                    newImage.Save(output, (codec == GDICodec.BMP ? ImageFormat.Bmp : (codec == GDICodec.JPEG ? ImageFormat.Jpeg : ImageFormat.Png)));
                }
            }
        }
    }
}
