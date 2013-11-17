using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Data.Linq;
using System.Data.Linq.Mapping;

namespace HomeMediaCenter
{
    public class ItemImage : Item
    {
        public ItemImage() : base(null, null, null) { }

        public ItemImage(FileInfo file, string mime, ItemContainer parent) : base(file.Name, file.Name, parent)
        {
            this.Mime = mime;
            this.Date = file.LastWriteTime;
            this.Length = file.Length;

            parent.SetMediaType(MediaType.Image);
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

        public override string GetMime()
        {
            return this.Mime;
        }

        public override string GetPath()
        {
            return System.IO.Path.Combine(this.Parent.Path, this.Path);
        }

        public override string GetFileFeature(MediaSettings settings)
        {
            return (this.Mime == "image/jpeg" ? "DLNA.ORG_PN=JPEG_MED;" : string.Empty) + settings.Image.FileFeature;
        }

        public override string GetEncodeFeature(MediaSettings settings)
        {
            return settings.Image.EncodeFeature;
        }

        public override void RemoveMe(DataContext context)
        {
            this.Parent.CheckMediaType(MediaType.Image);
        }

        public override bool IsType(MediaType type)
        {
            return type == MediaType.Image;
        }

        public override void RefresMe(DataContext context, ItemManager manager, bool recursive)
        {
            //Ak sa nepodarilo zistit metadata - skus ich zistit pri kazdom refresh (napr. ak subor este nebol cely skopirovany)
            if (this.Resolution == null)
                AssignValues(new FileInfo(GetPath()));
        }

        public override void BrowseMetadata(XmlWriter writer, MediaSettings settings, string host, string idParams, HashSet<string> filterSet)
        {
            BrowseMetadata(writer, settings, host, idParams, filterSet, this.Parent.GetParentItem(MediaType.Image).Id);
        }

        public override void BrowseMetadata(XmlWriter writer, MediaSettings settings, string host, string idParams, HashSet<string> filterSet, int parentId)
        {
            writer.WriteStartElement("item");

            writer.WriteAttributeString("id", this.Id.ToString());
            writer.WriteAttributeString("restricted", "true");
            writer.WriteAttributeString("parentID", parentId + "_" + ImageIndex);

            //Povinne hodnoty
            writer.WriteElementString("dc", "title", null, this.Title);
            writer.WriteElementString("upnp", "class", null, "object.item.imageItem");

            //Volitelne hodnoty
            if (filterSet == null || filterSet.Contains("dc:date"))
                writer.WriteElementString("dc", "date", null, this.Date.ToString("yyyy-MM-dd"));

            if (filterSet == null || filterSet.Contains("upnp:icon"))
                writer.WriteElementString("upnp", "icon", null, host + "/encode/image?id=" + this.Id + "&codec=jpeg&width=160&height=160&keepaspect");

            if (filterSet == null || filterSet.Contains("upnp:albumArtURI"))
            {
                writer.WriteStartElement("upnp", "albumArtURI", null);
                writer.WriteAttributeString("dlna", "profileID", "urn:schemas-dlna-org:metadata-1-0/", "JPEG_TN");
                writer.WriteValue(host + "/encode/image?id=" + this.Id + "&codec=jpeg&width=160&height=160&keepaspect");
                writer.WriteEndElement();
            }

            if (filterSet == null || filterSet.Any(a => a.StartsWith("res")))
            {
                if (settings.Image.NativeFile)
                {
                    writer.WriteStartElement("res");

                    if (filterSet == null || filterSet.Contains("res@size"))
                        writer.WriteAttributeString("size", this.Length.ToString());

                    if (this.Resolution != null && (filterSet == null || filterSet.Contains("res@resolution")))
                        writer.WriteAttributeString("resolution", this.Resolution);

                    writer.WriteAttributeString("protocolInfo", string.Format("http-get:*:{0}:{1}", this.Mime, GetFileFeature(settings)));
                    writer.WriteValue(host + "/files/image?id=" + this.Id);
                    writer.WriteEndElement();
                }

                foreach (EncoderBuilder sett in settings.Image.Encode)
                {
                    writer.WriteStartElement("res");

                    if (sett.Resolution != null && (filterSet == null || filterSet.Contains("res@resolution")))
                        writer.WriteAttributeString("resolution", sett.Resolution);

                    writer.WriteAttributeString("protocolInfo", string.Format("http-get:*:{0}:{1}{2}", sett.GetMime(), sett.GetDlnaType(), settings.Image.EncodeFeature));
                    writer.WriteValue(host + "/encode/image?id=" + this.Id + sett.GetParamString());
                    writer.WriteEndElement();
                }
            }

            writer.WriteEndElement();
        }

        public override void BrowseWebMetadata(XmlWriter xmlWriter, MediaSettings settings, string idParams)
        {
            xmlWriter.WriteStartElement("a");
            xmlWriter.WriteAttributeString("href", "/files/image?id=" + this.Id);
            xmlWriter.WriteStartElement("img");
            xmlWriter.WriteAttributeString("src", "/encode/image?id=" + this.Id + "&codec=jpeg&width=80&height=80&quality=10");
            xmlWriter.WriteAttributeString("alt", Title);
            xmlWriter.WriteEndElement();
            xmlWriter.WriteEndElement();
        }

        private void AssignValues(FileInfo file)
        {
            string resolution;
            if (DSWrapper.MediaFile.GetImageResolution(file, out resolution))
                this.Resolution = resolution;
        }
    }
}
