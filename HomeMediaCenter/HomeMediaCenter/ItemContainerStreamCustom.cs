using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.Linq;
using System.ComponentModel;
using System.Data.Linq.Mapping;

namespace HomeMediaCenter
{
    public class ItemContainerStreamCustom : ItemContainerStream
    {
        public ItemContainerStreamCustom() : base() { }

        public ItemContainerStreamCustom(string title, string path, ItemContainer parent) : base(title, path, parent) { }
    }
}
