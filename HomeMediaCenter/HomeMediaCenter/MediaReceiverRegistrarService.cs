using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HomeMediaCenter
{
    [UpnpServiceVariable("AuthorizationDeniedUpdateID", "ui4", true)]
    [UpnpServiceVariable("A_ARG_TYPE_DeviceID", "string", false)]
    [UpnpServiceVariable("A_ARG_TYPE_RegistrationRespMsg", "bin.base64", false)]
    [UpnpServiceVariable("ValidationRevokedUpdateID", "ui4", true)]
    [UpnpServiceVariable("ValidationSucceededUpdateID", "ui4", true)]
    [UpnpServiceVariable("A_ARG_TYPE_Result", "int", false)]
    [UpnpServiceVariable("AuthorizationGrantedUpdateID", "ui4", true)]
    [UpnpServiceVariable("A_ARG_TYPE_RegistrationReqMsg", "bin.base64", false)]
    public class MediaReceiverRegistrarService : UpnpService
    {
        private MediaServerDevice device;

        public MediaReceiverRegistrarService(MediaServerDevice device, UpnpServer server) : base(server,
            "urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1", "urn:microsoft.com:serviceId:X_MS_MediaReceiverRegistrar", 
            "/X_MS_MediaReceiverRegistrar.control", "/X_MS_MediaReceiverRegistrar.event", "/X_MS_MediaReceiverRegistrar.xml")
        {
            this.device = device;
        }

        protected override void WriteEventProp(System.Xml.XmlWriter writer)
        {
            writer.WriteStartElement("e", "property", null);
            writer.WriteElementString("AuthorizationDeniedUpdateID", string.Empty);
            writer.WriteEndElement();

            writer.WriteStartElement("e", "property", null);
            writer.WriteElementString("ValidationRevokedUpdateID", string.Empty);
            writer.WriteEndElement();

            writer.WriteStartElement("e", "property", null);
            writer.WriteElementString("ValidationSucceededUpdateID", string.Empty);
            writer.WriteEndElement();

            writer.WriteStartElement("e", "property", null);
            writer.WriteElementString("AuthorizationGrantedUpdateID", string.Empty);
            writer.WriteEndElement();
        }

        [UpnpServiceArgument(0, "RegistrationRespMsg", "A_ARG_TYPE_RegistrationRespMsg")]
        private void RegisterDevice(HttpRequest request, [UpnpServiceArgument("A_ARG_TYPE_RegistrationReqMsg")] string RegistrationReqMsg)
        {
            throw new SoapException(401, "Invalid Action");
        }

        [UpnpServiceArgument(0, "Result", "A_ARG_TYPE_Result")]
        private void IsAuthorized(HttpRequest request, [UpnpServiceArgument("A_ARG_TYPE_DeviceID")] string DeviceID)
        {
            HttpResponse response = request.GetResponse();
            response.SendSoapHeadersBody("1");
        }

        [UpnpServiceArgument(0, "Result", "A_ARG_TYPE_Result")]
        private void IsValidated(HttpRequest request, [UpnpServiceArgument("A_ARG_TYPE_DeviceID")] string DeviceID)
        {
            HttpResponse response = request.GetResponse();
            response.SendSoapHeadersBody("1");
        }
    }
}
