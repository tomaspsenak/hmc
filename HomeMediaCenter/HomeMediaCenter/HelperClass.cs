using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

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
    }
}
