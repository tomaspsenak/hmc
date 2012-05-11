using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HomeMediaCenterGUI
{
    public abstract class ContainerItem
    {
        private string name;
        private string extension;
        private string paramName;
        private bool isDirectShow;
        private bool isTransportStream;

        public ContainerItem(string name, string extension, string paramName, bool isDirectShow, bool isTransportStream)
        {
            this.name = name;
            this.extension = extension;
            this.paramName = paramName;
            this.isDirectShow = isDirectShow;
            this.isTransportStream = isTransportStream;
        }

        public string Extension
        {
            get { return this.extension; }
        }

        public string ParamName
        {
            get { return this.paramName; }
        }

        public bool IsTransportStream
        {
            get { return this.isTransportStream; }
        }

        public bool IsDirectShow
        {
            get { return this.isDirectShow; }
        }

        public override string ToString()
        {
            return this.name;
        }
    }

    public class ContainerMP4 : ContainerItem
    {
        public ContainerMP4() : base("MP4 - H264, AAC", ".mp4", "MPEG4", false, false) { }
    }

    public class ContainerWMV3: ContainerItem
    {
        public ContainerWMV3() : base("WMV - WMV3, VMA9", ".wmv", "WMV3", false, false) { }
    }

    public class ContainerMPEG_PS : ContainerItem
    {
        public ContainerMPEG_PS() : base("MPEG2_PS", ".mpeg", "MPEG2_PS", true, false) { }
    }

    public class ContainerMPEG_TS : ContainerItem
    {
        public ContainerMPEG_TS() : base("MPEG2_TS", ".mpeg", "MPEG2_TS", true, true) { }
    }

    public class ContainerWEBM : ContainerItem
    {
        public ContainerWEBM() : base("WebM - VP8, Vorbis", ".webm", "WEBM", true, false) { }
    }

    public class ContainerWMV2 : ContainerItem
    {
        public ContainerWMV2() : base("WMV - WMV2, VMA8", ".wmv", "WMV2", true, false) { }
    }
}
