using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace HomeMediaCenter.Interfaces
{
    public interface IMediaServerDeviceCallback
    {
        [OperationContract(IsOneWay = true)]
        void LogEventMessage(string message);
        [OperationContract(IsOneWay = true)]
        void LogEventRequestCount(int requestCount);
        [OperationContract(IsOneWay = true)]
        void AsyncStartEnd(string exception);
        [OperationContract(IsOneWay = true)]
        void AsyncStopEnd(string exception);
    }

    [ServiceContract(CallbackContract = typeof(IMediaServerDeviceCallback))]
    public interface IMediaServerDevice
    {
        [OperationContract]
        void StartAsync();
        [OperationContract]
        void StopAsync();
        [OperationContract]
        void LoadSettings();
        [OperationContract]
        bool SaveSettings();
        [OperationContract]
        void SetCurrentThreadCulture();

        string CultureInfoName { [OperationContract] get; [OperationContract] set; }
        string FriendlyName { [OperationContract] get; [OperationContract] set; }
        string DataDirectory { [OperationContract] get; }
        bool TryToForwardPort { [OperationContract] get; [OperationContract] set; }
        bool GenerateThumbnails { [OperationContract] get; [OperationContract] set; }
        bool MinimizeToTray { [OperationContract] get; [OperationContract] set; }
        bool Started { [OperationContract] get; }
        bool IsWinService { [OperationContract] get; }

        IItemManager IItemManager { get; }
        IUpnpServer IServer { get; }

        [OperationContract]
        void LogEventSubscribe(bool subscribe);

        event EventHandler<LogEventArgs> LogEvent;
        event EventHandler<ExceptionEventArgs> AsyncStartEnd;
        event EventHandler<ExceptionEventArgs> AsyncStopEnd;
    }
}
