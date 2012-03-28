using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using MFWrapper;

namespace HomeMediaCenter
{
    internal class MediaFoundationEncoder : EncoderBuilder
    {
        private enum MFCodec { WMV3, MPEG4 }

        private Dictionary<string, string> parameters;
        private MFEncoder encoder;
        private MFCodec codec;

        public static MediaFoundationEncoder TryCreate(Dictionary<string, string> parameters)
        {
            MFCodec codecEnum;
            if (Enum.TryParse<MFCodec>(parameters["codec"], true, out codecEnum))
            {
                MediaFoundationEncoder encoder = new MediaFoundationEncoder();
                encoder.codec = codecEnum;
                encoder.parameters = parameters;
                return encoder;
            }

            return null;
        }

        public override string GetMime()
        {
            switch (this.codec)
            {
                case MFCodec.MPEG4: return "video/mp4";
                case MFCodec.WMV3: return "video/x-ms-wmv";
            }

            return string.Empty;
        }

        public override void StopEncode()
        {
            MFEncoder enc = this.encoder;
            if (enc != null)
                enc.StopEncode();
        }

        public override void StartEncode(Stream output)
        {
            using (MFEncoder enc = new MFEncoder())
            {
                this.encoder = enc;

                enc.SetInput(parameters["source"]);

                uint setVideo = 1, setAudio = 1;
                if (parameters.ContainsKey("video"))
                    setVideo = uint.Parse(parameters["video"]);
                if (parameters.ContainsKey("audio"))
                    setAudio = uint.Parse(parameters["audio"]);

                //Nastavenie video a zvukovej stopy
                VideoStream video = null;
                AudioStream audio = null;
                uint actVideo = 1, actAudio = 1;
                foreach (StreamInfoItem item in enc.SourceStreams)
                {
                    if (item.MediaType == StreamMediaType.Video)
                    {
                        if (actVideo == setVideo)
                        {
                            item.IsSelected = true;
                            video = item.Video;
                        }
                        else
                            item.IsSelected = false;
                        actVideo++;
                    }
                    else if (item.MediaType == StreamMediaType.Audio)
                    {
                        if (actAudio == setAudio)
                        {
                            item.IsSelected = true;
                            audio = item.Audio;
                        }
                        else
                            item.IsSelected = false;
                        actAudio++;
                    }
                    else
                    {
                        item.IsSelected = false;
                    }
                }

                //Zistenie sirky a vysky, povodna hodnota ak nezadane
                uint width = 0, height = 0;
                if (parameters.ContainsKey("width"))
                    width = uint.Parse(parameters["width"]);
                else if (video != null)
                    width = video.Width;
                if (parameters.ContainsKey("height"))
                    height = uint.Parse(parameters["height"]);
                else if (video != null)
                    height = video.Height;

                //Zistenie bitrate pre audio a vide, povodna hodnota ak nezadane
                uint vidBitrate = 0, audBitrate = 0;
                if (parameters.ContainsKey("vidbitrate"))
                    vidBitrate = uint.Parse(parameters["vidbitrate"]) * 1000;
                else if (video != null)
                    vidBitrate = video.Bitrate;
                if (parameters.ContainsKey("audbitrate"))
                    audBitrate = uint.Parse(parameters["audbitrate"]) * 1000;
                else if (audio != null)
                    audBitrate = audio.Bitrate;

                //Zistenie poctu snimok za sekundu
                uint numerator = 0, denominator = 1;
                if (parameters.ContainsKey("fps"))
                    numerator = uint.Parse(parameters["fps"]);
                else if (video != null)
                {
                    numerator = video.Numerator;
                    denominator = video.Denominator;
                }

                ContainerType container = ContainerType.MF_MP4;
                switch (this.codec)
                {
                    case MFCodec.MPEG4:
                        container = ContainerType.MF_MP4;
                        video = new VideoStream(VideoFormat.H264, width, height, numerator, denominator, InterlaceMode.Progressive, vidBitrate);
                        audio = new AudioStream(AudioFormat.AAC, 2, 44100, audBitrate);
                        break;
                    case MFCodec.WMV3:
                        container = ContainerType.MF_ASF;
                        video = new VideoStream(VideoFormat.WMV3, width, height, numerator, denominator, InterlaceMode.Progressive, vidBitrate);
                        audio = new AudioStream(AudioFormat.WMAudioV9, 2, 44100, audBitrate);
                        break;
                }

                //Nastavenie casu zaciatku a konca v sekundach
                long startTime = 0, endTime = 0;
                if (parameters.ContainsKey("starttime"))
                    startTime = (long)(double.Parse(parameters["starttime"], System.Globalization.CultureInfo.InvariantCulture) * 10000000);
                if (parameters.ContainsKey("endtime"))
                    endTime = (long)(double.Parse(parameters["endtime"], System.Globalization.CultureInfo.InvariantCulture) * 10000000);

                if (this.progressChangeDel != null)
                    enc.ProgressChange += new EventHandler<ProgressChangeEventArgs>(enc_ProgressChange);

                //Nastavenie vystupu
                enc.SetOutput(output, container, video, audio, startTime, endTime);

                enc.StartEncode();
            }
        }

        private void enc_ProgressChange(object sender, ProgressChangeEventArgs e)
        {
            this.progressChangeDel(e.Progress);
        }
    }
}
