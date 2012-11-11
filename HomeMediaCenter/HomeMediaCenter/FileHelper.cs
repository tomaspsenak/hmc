using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace HomeMediaCenter
{
    public static class FileHelper
    {
        public static IEnumerable<string> GetDirectories(string path)
        {
            IEnumerable<string> directories;

            try 
            {
                directories = Directory.GetDirectories(path);
                foreach (string dir in directories)
                    directories = directories.Union(GetDirectories(dir));
            }
            catch
            {
                directories = Enumerable.Empty<string>();
            }

            return directories;
        }

        public static IEnumerable<string> GetFiles(string path, bool showHidden)
        {
            IEnumerable<string> files;

            try
            {
                if (showHidden)
                    files = Directory.GetFiles(path);
                else
                    files = Directory.GetFiles(path).Where(a => (new FileInfo(a).Attributes & FileAttributes.Hidden) != FileAttributes.Hidden);

                string[] directories = Directory.GetDirectories(path);
                foreach (string dir in directories)
                    files = files.Union(GetFiles(dir, showHidden));
            }
            catch
            {
                files = Enumerable.Empty<string>();
            }

            return files;
        }
    }
}
