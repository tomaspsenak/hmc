using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.Serialization;
using System.ComponentModel;

namespace HomeMediaCenter
{
    [DataContract]
    public enum StreamSourcesItemType
    {
        [EnumMember]
        Stream,
        [EnumMember]
        Dreambox
    }

    [DataContract]
    public class StreamSourcesItem : INotifyPropertyChanged
    {
        private string title;
        private string path;
        private StreamSourcesItemType type;

        public StreamSourcesItem() { }

        public StreamSourcesItem(int id, string title, string path, StreamSourcesItemType type)
        {
            this.Id = id;
            this.title = title;
            this.path = path;
            this.type = type;
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

        [DataMember]
        public StreamSourcesItemType Type
        {
            get { return this.type; }
            set
            {
                if (this.type != value)
                {
                    this.type = value;
                    NotifyPropertyChanged("Type");
                }
            }
        }

        public string TypeStr
        {
            get
            {
                switch (this.type)
                {
                    case StreamSourcesItemType.Stream: return LanguageResource.Stream;
                    case StreamSourcesItemType.Dreambox: return LanguageResource.Dreambox;
                    default: return string.Empty;
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
