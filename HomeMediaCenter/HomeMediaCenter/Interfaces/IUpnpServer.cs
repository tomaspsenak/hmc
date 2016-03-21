using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace HomeMediaCenter.Interfaces
{
    [ServiceContract]
    public interface IUpnpServer
    {
        int HttpPort { [OperationContract] get; [OperationContract] set; }
    }
}
