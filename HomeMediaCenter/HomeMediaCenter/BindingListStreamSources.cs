using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Data.SqlServerCe;
using System.Data.Linq;
using System.Data.Common;

namespace HomeMediaCenter
{
    public class BindingListStreamSources : BindingList<ItemContainerStreamCustom>, IDisposable
    {
        private DataContext context;
        private readonly ItemManager manager;

        public BindingListStreamSources() : base() { }

        public BindingListStreamSources(DataContext context, ItemManager manager) 
            : base(context.GetTable<Item>().OfType<ItemContainerStreamCustom>().ToList())
        {
            this.context = context;
            this.manager = manager;
        }

        public void Dispose()
        {
            if (this.context != null)
            {
                DbConnection connection = this.context.Connection;

                this.context.Dispose();

                if (connection != null)
                    connection.Dispose();
            }
        }

        public void SubmitChanges()
        {
            if (this.context == null)
                return;

            ChangeSet changeSet = this.context.GetChangeSet();

            //Odstranit nevyplnene polozky
            IEnumerable<Item> toDelete = changeSet.Inserts.OfType<ItemContainerStreamCustom>().Where(
                a => string.IsNullOrWhiteSpace(a.Title) || string.IsNullOrWhiteSpace(a.Path));
            this.context.GetTable<Item>().DeleteAllOnSubmit(toDelete);

            lock (this.manager.DbWriteLock)
            {
                //Pre aktualizaciu nazvu je potrebne potomkov odstranit - pri refresh sa pridaju so spravnym nazvom
                foreach (ItemContainerStreamCustom toUpdate in changeSet.Updates.OfType<ItemContainerStreamCustom>())
                    toUpdate.RemoveMe(this.context, this.manager);

                this.context.SubmitChanges();
            }
        }

        protected override void OnAddingNew(AddingNewEventArgs e)
        {
            if (this.context == null)
                return;

            ItemContainerStreamRoot root = this.context.GetTable<Item>().OfType<ItemContainerStreamRoot>().Single();
            ItemContainerStreamCustom item = new ItemContainerStreamCustom(null, null, root);
            this.context.GetTable<Item>().InsertOnSubmit(item);

            e.NewObject = item;

            base.OnAddingNew(e);
        }

        protected override void RemoveItem(int index)
        {
            if (this.context == null)
                return;

            ItemContainerStreamCustom item = this.Items[index];
            this.context.GetTable<Item>().DeleteOnSubmit(item);
            item.RemoveMe(this.context, this.manager);

            base.RemoveItem(index);
        }
    }
}
