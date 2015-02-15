using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Drawing;

namespace HomeMediaCenter
{
    public static class HelperClass
    {
        public static string Truncate(this string value, int maxLength)
        {
            if (string.IsNullOrEmpty(value))
                return value;
            if (value.Length <= maxLength)
                return value;

            return value.Substring(0, maxLength - 3) + "...";
        }

        public static bool IsFileLocked(FileInfo fileInfo)
        {
            FileStream fs = null;
            try
            {
                fs = fileInfo.Open(FileMode.Open);
            }
            catch (IOException e)
            {
                uint hr = (uint)System.Runtime.InteropServices.Marshal.GetHRForException(e);

                return hr == 0x80070020 || hr == 0x80070021; //FileLocked || PortionOfFileLocked
            }
            finally
            {
                if (fs != null)
                    fs.Dispose();
            }

            return false;
        }

        public static Bitmap GetTextBitmap(string text, int width, int height, float fontSize)
        {
            Bitmap bitmap = new Bitmap(width, height);

            using (Graphics graphics = Graphics.FromImage(bitmap))
            {
                graphics.Clear(Color.White);

                RectangleF rectf = new RectangleF(0, 0, width, height);

                StringFormat sf = new StringFormat();
                sf.LineAlignment = StringAlignment.Center;
                sf.Alignment = StringAlignment.Center;

                graphics.DrawString(text, new Font(FontFamily.GenericSansSerif, fontSize), Brushes.Black, rectf, sf);
            }

            return bitmap;
        }
    }
}
