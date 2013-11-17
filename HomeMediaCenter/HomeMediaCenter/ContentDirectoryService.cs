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
    [UpnpServiceVariable("A_ARG_TYPE_Featurelist", "string", false)]
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
            HttpResponse response = request.GetResponse();
            response.SendSoapHeadersBody("");
        }

        [UpnpServiceArgument(0, "SortCaps", "SortCapabilities")]
        private void GetSortCapabilities(HttpRequest request)
        {
            HttpResponse response = request.GetResponse();
            response.SendSoapHeadersBody("dc:title,dc:date");
        }

        [UpnpServiceArgument(0, "Id", "SystemUpdateID")]
        private void GetSystemUpdateID(HttpRequest request)
        {
            HttpResponse response = request.GetResponse();
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
            string Result, NumberReturned, TotalMatches;
            uint startingIndex, requestedCount;
            HomeMediaCenter.BrowseFlag browseFlag;

            if (!uint.TryParse(StartingIndex, out startingIndex) || !uint.TryParse(RequestedCount, out requestedCount) || 
                !Enum.TryParse(BrowseFlag, true, out browseFlag))
                throw new SoapException(402, "Invalid Args");

            SortCriteria = (SortCriteria == string.Empty) ? "+dc:title" : SortCriteria;

            this.device.ItemManager.Browse(request.Headers, ObjectID, browseFlag, Filter, startingIndex, requestedCount, SortCriteria,
                out Result, out NumberReturned, out TotalMatches);

            HttpResponse response = request.GetResponse();
            response.SendSoapHeadersBody(Result, NumberReturned, TotalMatches, "0");
        }

        [UpnpServiceArgument(0, "FeatureList", "A_ARG_TYPE_Featurelist")]
        private void X_GetFeatureList(HttpRequest request)
        {
            StringBuilder sb = new StringBuilder();

            using (XmlWriter writer = XmlWriter.Create(sb, new XmlWriterSettings() { OmitXmlDeclaration = true }))
            {
                writer.WriteStartElement("Features", "urn:schemas-upnp-org:av:avs");
                writer.WriteAttributeString("xmlns", "xsi", null, "http://www.w3.org/2001/XMLSchema-instance");
                writer.WriteAttributeString("xsi", "schemaLocation", null, "urn:schemas-upnp-org:av:avs http://www.upnp.org/schemas/av/avs.xsd");

                writer.WriteStartElement("Feature");
                writer.WriteAttributeString("name", "samsung.com_BASICVIEW");
                writer.WriteAttributeString("version", "1");

                writer.WriteStartElement("container");
                writer.WriteAttributeString("id", "0_" + Item.ImageIndex);
                writer.WriteAttributeString("type", "object.item.imageItem");
                writer.WriteEndElement();

                writer.WriteStartElement("container");
                writer.WriteAttributeString("id", "0_" + Item.AudioIndex);
                writer.WriteAttributeString("type", "object.item.audioItem");
                writer.WriteEndElement();

                writer.WriteStartElement("container");
                writer.WriteAttributeString("id", "0_" + Item.VideoIndex);
                writer.WriteAttributeString("type", "object.item.videoItem");
                writer.WriteEndElement();

                writer.WriteEndElement();

                writer.WriteEndElement();
            }
            HttpResponse response = request.GetResponse();
            response.SendSoapHeadersBody(sb.ToString());
        }
    }
}
