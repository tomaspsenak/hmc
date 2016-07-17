using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;

namespace HomeMediaCenter
{
    public class StreamSourcesBindingList : BindingList<StreamSourcesItem>
    {
        private readonly Interfaces.IItemManager manager;
        private List<StreamSourcesItem> removedItems = new List<StreamSourcesItem>();
        private HashSet<StreamSourcesItem> updatedItems = new HashSet<StreamSourcesItem>();
        private StreamSourcesItemType actualType = StreamSourcesItemType.Stream;

        public StreamSourcesBindingList() : base() { }

        public StreamSourcesBindingList(Interfaces.IItemManager manager) 
            : base(manager.GetStreamSources())
        {
            this.manager = manager;
        }

        public StreamSourcesItemType ActualType
        {
            set { this.actualType = value; }
        }

        public void SubmitChanges()
        {
            this.manager.SetStreamSources(
                this.Items.Where(a => !a.Id.HasValue).ToArray(), 
                this.updatedItems.ToArray(), 
                this.removedItems.ToArray());
        }

        protected override void OnListChanged(ListChangedEventArgs e)
        {
            if (e.ListChangedType == ListChangedType.ItemAdded)
            {
                StreamSourcesItem item = this.Items[e.NewIndex];
                item.Type = this.actualType;
                if (this.actualType == StreamSourcesItemType.Dreambox)
                    item.Path = "http://DREAMBOXIP:80";
            }
            else if (e.ListChangedType == ListChangedType.ItemChanged)
            {
                StreamSourcesItem item = this.Items[e.NewIndex];
                if (item.Id.HasValue)
                    this.updatedItems.Add(item);
            }

            base.OnListChanged(e);
        }

        protected override void RemoveItem(int index)
        {
            this.removedItems.Add(this.Items[index]);

            base.RemoveItem(index);
        }
    }
}
