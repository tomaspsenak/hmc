using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;
using System.Runtime.Serialization;

namespace HomeMediaCenter.Interfaces
{
    [DataContract]
    public enum EncodeType
    {
        [EnumMember]
        Audio,
        [EnumMember]
        Image,
        [EnumMember]
        Video,
        [EnumMember]
        Stream
    }

    [ServiceContract]
    public interface IMediaSettings
    {
        [OperationContract]
        string[] GetEncodeStrings(EncodeType type);
        [OperationContract]
        void SetEncodeStrings(EncodeType type, string[] encode);
        [OperationContract]
        bool GetNativeFile(EncodeType type);
        [OperationContract]
        void SetNativeFile(EncodeType type, bool value);
    }
}
