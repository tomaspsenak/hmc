using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HomeMediaCenterGUI
{
    public class ContainerItem
    {
        private string name;
        private string extension;
        private string paramName;
        private bool isDirectShow;
        private bool isTransportStream;
        private bool isAudio;
        private bool isVideo;
        private bool isImage;
        private bool isStreamable;

        private static readonly List<ContainerItem> containers;

        public ContainerItem(string name, string extension, string paramName, bool isDirectShow, bool isTransportStream, bool isAudio,
            bool isVideo, bool isImage, bool isStreamable)
        {
            this.name = name;
            this.extension = extension;
            this.paramName = paramName;
            this.isDirectShow = isDirectShow;
            this.isTransportStream = isTransportStream;
            this.isAudio = isAudio;
            this.isVideo = isVideo;
            this.isImage = isImage;
            this.isStreamable = isStreamable;
        }

        static ContainerItem()
        {
            containers = new List<ContainerItem> { 
                new ContainerItem("MPEG2_PS  (DirectShow)", ".mpeg", "mpeg2_ps", true, false, true, true, false, true),
                new ContainerItem("MPEG2_TS  (DirectShow)", ".ts", "mpeg2_ts", true, true, true, true, false, true),
                new ContainerItem("MPEG2_TS_H264  (DirectShow)", ".ts", "mpeg2_ts_h264", true, true, true, true, false, true),
                new ContainerItem("WebM - VP8, Vorbis  (DirectShow)", ".webm", "webm", true, false, true, true, false, false),
                new ContainerItem("WebM - VP8, Vorbis for streaming  (DirectShow)", ".webm", "webm_ts", true, false, true, true, false, true),
                new ContainerItem("WMV - WMV2, VMA8  (DirectShow)", ".wmv", "wmv2", true, false, true, true, false, true),
                new ContainerItem("WMV - WMV3, VMA9  (DirectShow)", ".wmv", "wmv3", true, false, true, true, false, true),
                new ContainerItem("MP3  (DirectShow)", ".mp3", "mp3", true, false, true, false, false, false),
                new ContainerItem("MP3 for streaming  (DirectShow)", ".mp3", "mp3_ts", true, false, true, false, false, true),
                new ContainerItem("AVI - MPEG-4, MP3  (DirectShow)", ".avi", "avi", true, false, true, true, false, false),
                new ContainerItem("MP4 - H264, AAC  (DirectShow)", ".mp4", "mp4", true, false, true, true, false, false),
                new ContainerItem("FLV - Sorenson Spark, MP3  (DirectShow)", ".flv", "flv", true, false, true, true, false, false),
                new ContainerItem("FLV - Sorenson Spark, MP3 for streaming  (DirectShow)", ".flv", "flv_ts", true, false, true, true, false, true),
                new ContainerItem("BMP", ".bmp", "bmp", true, false, false, false, true, true),
                new ContainerItem("JPEG", ".jpeg", "jpeg", true, false, false, false, true, true),
                new ContainerItem("PNG", ".png", "png", true, false, false, false, true, true)
            };

            if (Environment.OSVersion.Version.Major > 5)
            {
                containers.Add(new ContainerItem("MP4 - H264, AAC  (Media Foundation)", ".mp4", "mp4_mf", false, false, true, true, false, false));
                containers.Add(new ContainerItem("WMV - WMV3, VMA9  (Media Foundation)", ".wmv", "wmv3_mf", false, false, true, true, false, false));
            }
        }

        public static List<ContainerItem> GetContainers()
        {
            return containers;
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

        public bool IsAudio
        {
            get { return this.isAudio; }
        }

        public bool IsVideo
        {
            get { return this.isVideo; }
        }

        public bool IsImage
        {
            get { return this.isImage; }
        }

        public bool IsStreamable
        {
            get { return this.isStreamable; }
        }

        public override string ToString()
        {
            return this.name;
        }
    }
}
