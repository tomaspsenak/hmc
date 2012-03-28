using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using Shell32;

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

        public static bool GetVideoDuration(FileInfo file, out TimeSpan duration)
        {
            Shell shell = new ShellClass();
            Folder folder = shell.NameSpace(file.DirectoryName);
            FolderItem item = folder.ParseName(file.Name);

            return TimeSpan.TryParse(folder.GetDetailsOf(item, 27), out duration);
        }

        public static string GetAudioGenre(FileInfo file)
        {
            Shell shell = new ShellClass();
            Folder folder = shell.NameSpace(file.DirectoryName);
            FolderItem item = folder.ParseName(file.Name);

            return folder.GetDetailsOf(item, 16);
        }

        public static string GetAudioArtist(FileInfo file)
        {
            Shell shell = new ShellClass();
            Folder folder = shell.NameSpace(file.DirectoryName);
            FolderItem item = folder.ParseName(file.Name);

            return folder.GetDetailsOf(item, 20);
        }

        public static string GetAudioAlbum(FileInfo file)
        {
            Shell shell = new ShellClass();
            Folder folder = shell.NameSpace(file.DirectoryName);
            FolderItem item = folder.ParseName(file.Name);

            return folder.GetDetailsOf(item, 14);
        }

        public static string GetImageResolution(FileInfo file)
        {
            Shell shell = new ShellClass();
            Folder folder = shell.NameSpace(file.DirectoryName);
            FolderItem item = folder.ParseName(file.Name);

            uint width, height;
            if (!uint.TryParse(new string(folder.GetDetailsOf(item, 162).Where(a => char.IsNumber(a)).ToArray()), out width))
                return string.Empty;
            if (!uint.TryParse(new string(folder.GetDetailsOf(item, 164).Where(a => char.IsNumber(a)).ToArray()), out height))
                return string.Empty;

            return width + "x" + height;
        }

        public static string GetVideoResolution(FileInfo file)
        {
            Shell shell = new ShellClass();
            Folder folder = shell.NameSpace(file.DirectoryName);
            FolderItem item = folder.ParseName(file.Name);

            uint width, height;
            if (!uint.TryParse(new string(folder.GetDetailsOf(item, 285).Where(a => char.IsNumber(a)).ToArray()), out width))
                return string.Empty;
            if (!uint.TryParse(new string(folder.GetDetailsOf(item, 283).Where(a => char.IsNumber(a)).ToArray()), out height))
                return string.Empty;

            return width + "x" + height;
        }

        public static void GetInfo(FileInfo file)
        {
            Shell shl = new ShellClass();
            Folder fldr = shl.NameSpace(file.DirectoryName);
            FolderItem itm = fldr.ParseName(file.Name);
            Dictionary<int, KeyValuePair<string, string>> fileProps = new Dictionary<int, KeyValuePair<string, string>>();
            for (int i = 0; i < 300; i++)
            {
                string propValue = fldr.GetDetailsOf(itm, i);
                if (propValue != "")
                {
                    fileProps.Add(i, new KeyValuePair<string, string>(fldr.GetDetailsOf(null, i), propValue));
                }
            }
        }
    }
}
