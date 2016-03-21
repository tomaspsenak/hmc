using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.Serialization;
using System.ComponentModel;

namespace HomeMediaCenter
{
    [DataContract]
    public class StreamSourcesItem : INotifyPropertyChanged
    {
        private string title;
        private string path;

        public StreamSourcesItem() { }

        public StreamSourcesItem(int id, string title, string path)
        {
            this.Id = id;
            this.title = title;
            this.path = path;
        }

        [DataMember]
        public int? Id
        {
            get; set;
        }

        [DataMember]
        public string Title
        {
            get { return this.title; }
            set
            {
                if (this.title != value)
                {
                    this.title = value;
                    NotifyPropertyChanged("Title");
                }
            }
        }

        [DataMember]
        public string Path
        {
            get { return this.path; }
            set
            {
                if (this.path != value)
                {
                    this.path = value;
                    NotifyPropertyChanged("Path");
                }
            }
        }

        private void NotifyPropertyChanged(string propertyName)
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
                handler(this, new PropertyChangedEventArgs(propertyName));
        }

        public event PropertyChangedEventHandler PropertyChanged;
    }
}
