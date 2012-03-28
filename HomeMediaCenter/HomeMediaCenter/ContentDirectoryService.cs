using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace HomeMediaCenter
{
    public enum BrowseFlag { BrowseMetadata, BrowseDirectChildren };

    [UpnpServiceVariable("A_ARG_TYPE_BrowseFlag", "string", false, "BrowseMetadata", "BrowseDirectChildren")]
    [UpnpServiceVariable("A_ARG_TYPE_Filter", "string", false)]
    [UpnpServiceVariable("A_ARG_TYPE_SortCriteria", "string", false)]
    [UpnpServiceVariable("A_ARG_TYPE_Index", "ui4", false)]
    [UpnpServiceVariable("A_ARG_TYPE_Count", "ui4", false)]
    [UpnpServiceVariable("A_ARG_TYPE_UpdateID", "ui4", false)]
    [UpnpServiceVariable("SearchCapabilities", "string", false)]
    [UpnpServiceVariable("SortCapabilities", "string", false)]
    [UpnpServiceVariable("SystemUpdateID", "ui4", true)]
    [UpnpServiceVariable("A_ARG_TYPE_Result", "string", false)]
    [UpnpServiceVariable("A_ARG_TYPE_ObjectID", "string", false)]
    public class ContentDirectoryService : UpnpService
    {
        private MediaServerDevice device;

        public ContentDirectoryService(MediaServerDevice device, UpnpServer server) : base(server, 
            "urn:schemas-upnp-org:service:ContentDirectory:1", "urn:upnp-org:serviceId:ContentDirectory", "/ContentDirectory.control",
            "/ContentDirectory.event", "/ContentDirectory.xml")
        {
            this.device = device;
        }

        protected override void WriteEventProp(XmlWriter writer)
        {
            writer.WriteStartElement("e", "property", null);
            writer.WriteElementString("SystemUpdateID", "0");
            writer.WriteEndElement();
        }

        [UpnpServiceArgument(0, "SearchCaps", "SearchCapabilities")]
        private void GetSearchCapabilities(HttpRequest request)
        {
            HttpResponse response = new HttpResponse(request);
            response.SendSoapHeadersBody("");
        }

        [UpnpServiceArgument(0, "SortCaps", "SortCapabilities")]
        private void GetSortCapabilities(HttpRequest request)
        {
            HttpResponse response = new HttpResponse(request);
            response.SendSoapHeadersBody("dc:title,dc:date");
        }

        [UpnpServiceArgument(0, "Id", "SystemUpdateID")]
        private void GetSystemUpdateID(HttpRequest request)
        {
            HttpResponse response = new HttpResponse(request);
            response.SendSoapHeadersBody("0");
        }

        [UpnpServiceArgument(0, "Result", "A_ARG_TYPE_Result")]
        [UpnpServiceArgument(1, "NumberReturned", "A_ARG_TYPE_Count")]
        [UpnpServiceArgument(2, "TotalMatches", "A_ARG_TYPE_Count")]
        [UpnpServiceArgument(3, "UpdateID", "A_ARG_TYPE_UpdateID")]
        private void Browse(HttpRequest request,
            [UpnpServiceArgument("A_ARG_TYPE_ObjectID")]        string ObjectID,
            [UpnpServiceArgument("A_ARG_TYPE_BrowseFlag")]      string BrowseFlag,
            [UpnpServiceArgument("A_ARG_TYPE_Filter")]          string Filter,
            [UpnpServiceArgument("A_ARG_TYPE_Index")]           string StartingIndex,
            [UpnpServiceArgument("A_ARG_TYPE_Count")]           string RequestedCount,
            [UpnpServiceArgument("A_ARG_TYPE_SortCriteria")]    string SortCriteria)
        {
            string Result, NumberReturned, TotalMatches, UpdateID;
            uint objectID, startingIndex, requestedCount;
            HomeMediaCenter.BrowseFlag browseFlag;

            if (!uint.TryParse(ObjectID, out objectID) || !uint.TryParse(StartingIndex, out startingIndex) || 
                !uint.TryParse(RequestedCount, out requestedCount) || !Enum.TryParse(BrowseFlag, true, out browseFlag))
                throw new HttpException(402, "Browse - parse exception");

            this.device.ItemManager.BrowseSync(request.Headers, objectID, browseFlag, Filter, startingIndex, requestedCount, SortCriteria,
                out Result, out NumberReturned, out TotalMatches, out UpdateID);

            HttpResponse response = new HttpResponse(request);
            response.SendSoapHeadersBody(Result, NumberReturned, TotalMatches, UpdateID);
        }
    }
}
