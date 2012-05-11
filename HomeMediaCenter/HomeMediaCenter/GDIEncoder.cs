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
        private GDICodec codec;

        protected GDIEncoder() { }

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
                default: return string.Empty;
            }
        }

        public override string GetDlnaType()
        {
            switch (this.codec)
            {
                case GDICodec.JPEG:
                    if (this.width == null || this.height == null)
                        return "DLNA.ORG_PN=JPEG_MED;";
                    else if (this.width <= 48 && this.height <= 48)
                        return "DLNA.ORG_PN=JPEG_SM_ICO;";
                    else if (this.width <= 120 && this.height <= 120)
                        return "DLNA.ORG_PN=JPEG_LRG_ICO;";
                    else if (this.width <= 160 && this.height <= 160)
                        return "DLNA.ORG_PN=JPEG_TN;";
                    else if (this.width <= 640 && this.height <= 480)
                        return "DLNA.ORG_PN=JPEG_SM;";
                    else if (this.width <= 1024 && this.height <= 768)
                        return "DLNA.ORG_PN=JPEG_MED;";
                    else
                        return "DLNA.ORG_PN=JPEG_LRG;";
                case GDICodec.PNG: return "DLNA.ORG_PN=PNG_LRG;";
                default: return string.Empty;
            }
        }

        public override void StartEncode(Stream input, Stream output)
        {
            using (Image origImage = Image.FromStream(input))
            {
                StartEncode(origImage, output);
            }
        }

        public override void StartEncode(Stream output)
        {
            using (Image origImage = Image.FromFile(parameters["source"]))
            {
                StartEncode(origImage, output);
            }
        }

        private void StartEncode(Image origImage, Stream output)
        {
            //Zistenie sirky a vysky, povodna hodnota ak nezadane
            int width = this.width.HasValue ? (int)this.width.Value : origImage.Width;
            int height = this.height.HasValue ? (int)this.height.Value : origImage.Height;

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
