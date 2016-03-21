using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace HomeMediaCenter.Interfaces
{
    public interface IItemManagerCallback
    {
        [OperationContract(IsOneWay = true)]
        void AsyncBuildDatabaseStart(string exception);
        [OperationContract(IsOneWay = true)]
        void AsyncBuildDatabaseEnd(string exception);
    }

    [ServiceContract(CallbackContract = typeof(IItemManagerCallback))]
    public interface IItemManager
    {
        [OperationContract]
        bool CheckDirectoryPermission(string path);
        [OperationContract]
        string[] GetDirectories();
        [OperationContract]
        void BuildDatabaseAsync();
        [OperationContract]
        void BuildDatabase();
        [OperationContract]
        string[] AddDirectory(string directory, string title);
        [OperationContract]
        string[] RemoveDirectory(string directory);
        [OperationContract]
        List<StreamSourcesItem> GetStreamSources();
        [OperationContract]
        void SetStreamSources(StreamSourcesItem[] toAdd, StreamSourcesItem[] toUpdate, StreamSourcesItem[] toDelete);

        IMediaSettings IMediaSettings { get; }
        bool EnableWebcamStreaming { [OperationContract] get; [OperationContract] set; }
        bool EnableDesktopStreaming { [OperationContract] get; [OperationContract] set; }
        bool RealTimeDatabaseRefresh { [OperationContract] get; [OperationContract] set; }
        bool ShowHiddenFiles { [OperationContract] get; [OperationContract] set; }

        event EventHandler<ExceptionEventArgs> AsyncBuildDatabaseStart;
        event EventHandler<ExceptionEventArgs> AsyncBuildDatabaseEnd;
    }
}
