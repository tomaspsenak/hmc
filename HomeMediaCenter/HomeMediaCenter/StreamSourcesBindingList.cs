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

        public StreamSourcesBindingList() : base() { }

        public StreamSourcesBindingList(Interfaces.IItemManager manager) 
            : base(manager.GetStreamSources())
        {
            this.manager = manager;
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
            if (e.ListChangedType == ListChangedType.ItemChanged)
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
