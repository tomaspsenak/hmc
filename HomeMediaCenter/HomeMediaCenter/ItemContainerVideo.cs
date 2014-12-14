using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Data.Linq;
using System.Xml;
using System.Data.Linq.Mapping;

namespace HomeMediaCenter
{
    public class ItemContainerVideo : ItemContainer
    {
        protected class SubtitlesPathItemEqualityComparer : ItemEqualityComparer<string, ItemVideo>
        {
            public override object GetValueX(string item) { return item; }
            public override object GetValueY(ItemVideo item) { return item.SubtitlesPath; }
        }

        public ItemContainerVideo() : base() { }

        public ItemContainerVideo(FileInfo file, string mime, ItemContainer parent) : base(file.Name, file.Name, parent)
        {
            this.Mime = mime;
            this.Date = file.LastWriteTime;
            this.Length = file.Length;

            new ItemVideo(file.Name, null, null, this);
        }

        [Column(IsPrimaryKey = false, DbType = "datetime", CanBeNull = true, UpdateCheck = UpdateCheck.Never)]
        public override DateTime Date
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "nvarchar(50)", CanBeNull = true, UpdateCheck = UpdateCheck.Never)]
        public string Mime
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "bigint", CanBeNull = true, UpdateCheck = UpdateCheck.Never)]
        public long Length
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "nvarchar(11)", CanBeNull = true, UpdateCheck = UpdateCheck.Never)]
        public string Resolution
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "bigint", CanBeNull = true, UpdateCheck = UpdateCheck.Never)]
        public long? DurationTicks
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "int", CanBeNull = true, UpdateCheck = UpdateCheck.Never)]
        public int? Bitrate
        {
            get; set;
        }

        public override ItemContainer GetParentItem(MediaType type)
        {
            if (this.Items.Count > 1)
                return base.GetParentItem(type);
            else
                return this.Parent.GetParentItem(type);
        }

        public override bool IsType(MediaType type)
        {
            //Ak obsahuje jeden prvok - nepovazuje sa za kontajner (MediaType.Other)
            if (this.Items.Count > 1 || type != MediaType.Other)
                return base.IsType(type);
            else
                return false;
        }

        public override void RefreshMe(DataContext context, ItemManager manager, bool recursive)
        {
            if (manager.UpnpDevice.Stopping)
                return;

            //Rozdielova obnova video suborov ktore maju titulky v samostatnom subore
            string[] files = new DirectoryInfo(this.Parent.Path).GetFiles(System.IO.Path.GetFileNameWithoutExtension(this.Path) + "*.*").Where(
                a => manager.ContainsSubtitleExt(a.Extension)).Select(a => a.Name).ToArray();

            Item[] toRemove = this.Items.OfType<ItemVideo>().Where(a => a.SubtitlesPath != null).Cast<object>().Except(
                files, new SubtitlesPathItemEqualityComparer()).Cast<Item>().ToArray();
            string[] toAdd = files.Except(this.Items.OfType<ItemVideo>().Where(a => a.SubtitlesPath != null).Select(
                a => a.SubtitlesPath)).ToArray();

            RemoveRange(context, manager, toRemove);

            foreach (string path in toAdd)
            {
                //name musi byt dlhsie alebo rovnake ako videoName - GetFiles obsahuje parameter "*.*"
                string videoName = System.IO.Path.GetFileNameWithoutExtension(this.Path);
                string name = System.IO.Path.GetFileNameWithoutExtension(path).Substring(videoName.Length);
                new ItemVideo(string.Format("{0} Subtitles {1}", videoName.Truncate(30), name), null, path, this);
            }
        }

        public override void RefreshMetadata(DataContext context, ItemManager manager, bool recursive)
        {
            if (manager.UpnpDevice.Stopping)
                return;

            string thumbnailPath = null;
            if (manager.UpnpDevice.GenerateThumbnails && !this.HasThumbnail.HasValue)
            {
                //Ak sa ma generovat thumbnail a HasThumbnail je null - generuj
                thumbnailPath = System.IO.Path.Combine(manager.UpnpDevice.ThumbnailsPath, this.Id + ".jpg");
            }
            else if (!manager.UpnpDevice.GenerateThumbnails && this.HasThumbnail == true)
            {
                //Ak sa nema generovat thumbnail a HasThumbnail je true - vymaz thumbnail
                string thumbnailPath2 = System.IO.Path.Combine(manager.UpnpDevice.ThumbnailsPath, this.Id + ".jpg");

                try { System.IO.File.Delete(thumbnailPath2); }
                catch { }

                this.HasThumbnail = null;
            }

            //Ak sa nepodarilo zistit metadata - skus ich zistit pri kazdom refresh (napr. ak subor este nebol cely skopirovany)
            if (!this.DurationTicks.HasValue || thumbnailPath != null)
            {
                //Vymazat stream pridane cez AssignValues
                Item[] toRemove = this.Items.OfType<ItemVideo>().Where(a => a.Path != null).ToArray();
                RemoveRange(context, manager, toRemove);

                AssignValues(manager, new FileInfo(System.IO.Path.Combine(this.Parent.Path, this.Path)), thumbnailPath);
            }
        }

        public override void RemoveMe(DataContext context, ItemManager manager)
        {
            base.RemoveMe(context, manager);

            if (this.HasThumbnail.HasValue && this.HasThumbnail.Value)
            {
                string thumbnailPath = System.IO.Path.Combine(manager.UpnpDevice.ThumbnailsPath, this.Id + ".jpg");

                try { System.IO.File.Delete(thumbnailPath); }
                catch { }
            }
        }

        public override void BrowseMetadata(XmlWriter xmlWriter, MediaSettings settings, string host, string idParams, HashSet<string> filterSet, int parentId)
        {
            if (this.Items.Count > 1)
                base.BrowseMetadata(xmlWriter, settings, host, idParams, filterSet, parentId);
            else
                this.Items.First().BrowseMetadata(xmlWriter, settings, host, idParams, filterSet, parentId);
        }

        public override void BrowseWebMetadata(XmlWriter xmlWriter, MediaSettings settings, string idParams)
        {
            if (this.Items.Count > 1)
                base.BrowseWebMetadata(xmlWriter, settings, idParams);
            else
                this.Items.First().BrowseWebMetadata(xmlWriter, settings, idParams);
        }

        private void AssignValues(ItemManager manager, FileInfo file, string thumbnailPath)
        {
            TimeSpan duration;
            string resolution;
            int audioStreamsCount;
            List<string> subtitlesStreams;

            int hr = DSWrapper.MediaFile.GetVideoInfo(file, thumbnailPath, out duration, out resolution, out audioStreamsCount, out subtitlesStreams);
            if (hr == 0)
            {
                this.Resolution = resolution;
                this.DurationTicks = duration.Ticks;
                if (duration.TotalSeconds == 0f)
                    this.Bitrate = 0;
                else
                    this.Bitrate = (int)(file.Length / duration.TotalSeconds);

                string name = System.IO.Path.GetFileNameWithoutExtension(file.Name).Truncate(30);
                if (audioStreamsCount > 1)
                {
                    for (int i = 1; i <= audioStreamsCount; i++)
                        new ItemVideo(string.Format("{0} Audio stream {1}", name, i), string.Format("&audio={0}", i), null, this);
                }

                for (int i = 0; i < subtitlesStreams.Count; i++)
                {
                    new ItemVideo(string.Format("{0} Subtitles {1}", name, subtitlesStreams[i] == null || subtitlesStreams[i] == string.Empty ? (i + 1).ToString() : subtitlesStreams[i]),
                        string.Format("&subtitles={0}", i + 1), null, this);
                }
            }
            else if (hr != -2147024864 && !HelperClass.IsFileLocked(file))
            {
                //Chyba -2147024864 znamena ze subor nie je dostupny - napr. pouziva ho iny proces, nie je kompletne skopirovany,...
                //V pripade inej chyby este overi ci naozaj subor nepouziva iny proces

                //Inak sa uz nepokusaj zistit metadata
                this.DurationTicks = 0;
            }
            else
            {
                return;
            }

            //Zisti ci existuje thumbnail subor ak je cesta k nemu zadana
            //Nezistuje pokial je hr -2147024864 alebo zamknuty subor
            if (thumbnailPath != null)
                this.HasThumbnail = File.Exists(thumbnailPath);
        }
    }
}
