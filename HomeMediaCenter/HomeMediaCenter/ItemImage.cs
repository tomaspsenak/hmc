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

        public override string GetThumbnailPath(ItemManager manager)
        {
            return GetPath();
        }

        public override string GetFileFeature(MediaSettings settings)
        {
            return (this.Mime == "image/jpeg" ? "DLNA.ORG_PN=JPEG_MED;" : string.Empty) + settings.Image.FileFeature;
        }

        public override string GetEncodeFeature(MediaSettings settings)
        {
            return settings.Image.EncodeFeature;
        }

        public override void RemoveMe(DataContext context, ItemManager manager)
        {
            this.Parent.CheckMediaType(MediaType.Image);
        }

        public override bool IsType(MediaType type)
        {
            return type == MediaType.Image;
        }

        public override void RefreshMetadata(DataContext context, ItemManager manager, bool recursive)
        {
            if (manager.UpnpDevice.Stopping)
                return;

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
                writer.WriteElementString("upnp", "icon", null, host + "/thumbnail/image.jpg?id=" + this.Id + "&codec=jpeg&width=160&height=160&keepaspect");

            if (filterSet == null || filterSet.Contains("upnp:albumArtURI"))
            {
                writer.WriteStartElement("upnp", "albumArtURI", null);
                writer.WriteAttributeString("dlna", "profileID", "urn:schemas-dlna-org:metadata-1-0/", "JPEG_TN");
                writer.WriteValue(host + "/thumbnail/image.jpg?id=" + this.Id + "&codec=jpeg&width=160&height=160&keepaspect");
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
                    writer.WriteValue(string.Format("{0}/files/image{1}?id={2}", host, System.IO.Path.GetExtension(this.Path), this.Id));
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
            xmlWriter.WriteStartElement("tr");

            xmlWriter.WriteStartElement("td");
            xmlWriter.WriteStartElement("div");
            xmlWriter.WriteAttributeString("class", "libPlayButton");
            xmlWriter.WriteRaw(string.Format(@"<a href=""/files/image?id={0}"" target=""_blank"">{1}</a>", this.Id, LanguageResource.Play));
            xmlWriter.WriteStartElement("ul");
            xmlWriter.WriteRaw(string.Format(@"<li><a href=""/files/image{0}?id={1}"" target=""_blank"">{2}</a></li>", 
                System.IO.Path.GetExtension(this.Path), this.Id, LanguageResource.Download));
            xmlWriter.WriteRaw(string.Format(@"<li><a href=""/web/control.html?id={0}"" target=""_blank"">{1}</a></li>", this.Id, LanguageResource.PlayTo));
            xmlWriter.WriteEndElement();
            xmlWriter.WriteEndElement();
            xmlWriter.WriteEndElement();

            xmlWriter.WriteStartElement("td");
            xmlWriter.WriteStartElement("img");
            xmlWriter.WriteAttributeString("src", string.Format("/thumbnail/image.jpg?id={0}&codec=jpeg&width=50&height=50&quality=30", this.Id));
            xmlWriter.WriteAttributeString("alt", this.Title);
            xmlWriter.WriteAttributeString("title", this.Title);
            xmlWriter.WriteAttributeString("border", "0");
            xmlWriter.WriteAttributeString("width", "50");
            xmlWriter.WriteAttributeString("height", "50");
            xmlWriter.WriteAttributeString("align", "right");
            xmlWriter.WriteFullEndElement();
            xmlWriter.WriteFullEndElement();

            xmlWriter.WriteElementString("td", this.Title);
            xmlWriter.WriteElementString("td", this.Resolution == null ? string.Empty : this.Resolution);

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
