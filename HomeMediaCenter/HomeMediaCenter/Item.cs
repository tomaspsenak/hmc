using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Data.Linq;
using System.Data.Linq.Mapping;

namespace HomeMediaCenter
{
    [Table(Name = "Items")]
    [InheritanceMapping(Code = 0, Type = typeof(ItemContainerRoot), IsDefault = true)]
    [InheritanceMapping(Code = 1, Type = typeof(ItemContainerStream))]
    [InheritanceMapping(Code = 2, Type = typeof(ItemContainerVideo))]
    [InheritanceMapping(Code = 3, Type = typeof(ItemContainer))]
    [InheritanceMapping(Code = 4, Type = typeof(ItemAudio))]
    [InheritanceMapping(Code = 5, Type = typeof(ItemImage))]
    [InheritanceMapping(Code = 6, Type = typeof(ItemVideo))]
    [InheritanceMapping(Code = 7, Type = typeof(ItemStream))]
    [InheritanceMapping(Code = 8, Type = typeof(ItemContainerStreamRoot))]
    [InheritanceMapping(Code = 9, Type = typeof(ItemContainerStreamCustom))]
    public abstract class Item
    {
        public const string AudioIndex = "1";
        public const string ImageIndex = "2";
        public const string VideoIndex = "3";

        [Column(Name = "ParentId", IsPrimaryKey = false, DbType = "int", CanBeNull = true)]
        protected int? parentId;

        private EntityRef<ItemContainer> parent = new EntityRef<ItemContainer>();

        public Item(string title, string path, ItemContainer parent)
        {
            this.Title = title;
            this.Path = path;
            if (parent != null)
            {
                Parent = parent;
                parent.Items.Add(this);
            }
        }

        [Column(IsPrimaryKey = true, DbType = "int IDENTITY(0,1)", CanBeNull = false, IsDbGenerated = true)]
        public int Id
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "nvarchar(255)", CanBeNull = false, UpdateCheck = UpdateCheck.Never)]
        public string Title
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "nvarchar(1024)", CanBeNull = true, UpdateCheck = UpdateCheck.Never)]
        public string Path
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "bit", CanBeNull = true)]
        public bool? HasThumbnail
        {
            get; set;
        }

        [Column(IsPrimaryKey = false, DbType = "tinyint", CanBeNull = false, IsDiscriminator = true)]
        public int Type
        {
            get; set;
        }

        [Association(Storage = "parent", IsForeignKey = true, ThisKey = "parentId", OtherKey = "Id")]
        public ItemContainer Parent
        {
            get { return this.parent.Entity; }
            set { this.parent.Entity = value; }
        }

        public virtual IEnumerable<Item> Children
        {
            get { return Enumerable.Empty<Item>(); }
        }

        public virtual DateTime Date
        {
            get { return new DateTime(0); }
            set { }
        }

        public virtual string GetMime() { return null; }

        public virtual string GetPath() { return this.Path; }

        public virtual TimeSpan? GetDuration() { return null; }

        public virtual string GetSubtitlesPath() { return null; }

        public virtual string GetThumbnailPath(ItemManager manager) { return null; }

        public virtual string GetFileFeature(MediaSettings settings) { return string.Empty; }

        public virtual string GetEncodeFeature(MediaSettings settings) { return string.Empty; }

        public virtual void RefreshMe(DataContext context, ItemManager manager, bool recursive) { }

        public virtual void RefreshMetadata(DataContext context, ItemManager manager, bool recursive) { }

        public virtual void RemoveMe(DataContext context, ItemManager manager) { }

        public virtual void BrowseDirectChildren(XmlWriter xmlWriter, MediaSettings settings, string host, string idParams, HashSet<string> filterSet,
            uint startingIndex, uint requestedCount, string sortCriteria, out string numberReturned, out string totalMatches, ItemContainer skippedItem = null)
        {
            numberReturned = "0";
            totalMatches = "0";
        }

        public virtual void BrowseWebDirectChildren(XmlWriter xmlWriter, MediaSettings settings, string idParams) { }

        public virtual void GetWebPlayer(XmlWriter xmlWriter, ItemManager manager, Dictionary<string, string> urlParams) { }

        public abstract bool IsType(MediaType type);

        public abstract void BrowseMetadata(XmlWriter xmlWriter, MediaSettings settings, string host, string idParams, HashSet<string> filterSet);

        public abstract void BrowseMetadata(XmlWriter xmlWriter, MediaSettings settings, string host, string idParams, HashSet<string> filterSet, int parentId);

        public virtual void BrowseMetadata(XmlWriter xmlWriter, MediaSettings settings, string host, string idParams, HashSet<string> filterSet, int parentId, TimeSpan? startTime)
        {
            BrowseMetadata(xmlWriter, settings, host, idParams, filterSet, parentId);
        }

        public abstract void BrowseWebMetadata(XmlWriter xmlWriter, MediaSettings settings, string idParams);
    }
}
