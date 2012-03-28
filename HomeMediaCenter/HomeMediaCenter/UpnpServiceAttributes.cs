using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HomeMediaCenter
{
    [AttributeUsage(AttributeTargets.Class, AllowMultiple = true)]
    public class UpnpServiceVariable : Attribute
    {
        private string name;
        private string dataType;
        private bool sendEvents;
        private string[] allowedValue;

        public UpnpServiceVariable(string name, string dataType, bool sendEvents, params string[] allowedValue)
        {
            this.name = name;
            this.dataType = dataType;
            this.sendEvents = sendEvents;
            this.allowedValue = allowedValue;
        }

        public UpnpServiceVariable(string name, string dataType, bool sendEvents) : this(name, dataType, sendEvents, new string[0]) { }

        public string Name
        {
            get { return this.name; }
        }

        public string DataType
        {
            get { return this.dataType; }
        }

        public bool SendEvents
        {
            get { return this.sendEvents; }
        }

        public string[] AllowedValue
        {
            get { return this.allowedValue; }
        }
    }

    [AttributeUsage(AttributeTargets.Parameter | AttributeTargets.Method, AllowMultiple = true)]
    public class UpnpServiceArgument : Attribute
    {
        private int index;
        private string name;
        private string relatedStateVariable;

        public UpnpServiceArgument(int index, string name, string relatedStateVariable)
        {
            this.index = index;
            this.name = name;
            this.relatedStateVariable = relatedStateVariable;
        }

        public UpnpServiceArgument(string relatedStateVariable)
        {
            this.relatedStateVariable = relatedStateVariable;
        }

        public int Index
        {
            get { return this.index; }
        }

        public string Name
        {
            get { return this.name; }
        }

        public string RelatedStateVariable
        {
            get { return this.relatedStateVariable; }
        }
    }
}
