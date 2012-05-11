using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HomeMediaCenter
{
    class Program
    {
        static void Main(string[] args)
        {
            /*using (DSWrapper.DSEncoder enc = new DSWrapper.DSEncoder())
            using (UDPStream output = new UDPStream(System.Net.IPAddress.Parse("192.168.0.4"), System.Net.IPAddress.Parse("239.255.255.251"), 12346))
            {
                //enc.SetInput(DSWrapper.InputType.Webcam(DSWrapper.WebcamInput.GetWebcamNames().First()));
                enc.SetInput(DSWrapper.InputType.Desktop(10));

                bool isVideo = false, isAudio = true;
                foreach (DSWrapper.PinInfoItem item in enc.SourcePins)
                {
                    if (item.MediaType == DSWrapper.PinMediaType.Video && isVideo == false)
                    {
                        item.IsSelected = true;
                        isVideo = true;
                    }
                    else if (item.MediaType == DSWrapper.PinMediaType.Audio && isAudio == false)
                    {
                        item.IsSelected = true;
                        isAudio = true;
                    }
                    else
                    {
                        item.IsSelected = false;
                    }
                    Console.WriteLine(item);
                }

                enc.SetOutput(output, DSWrapper.ContainerType.MPEG2_TS(640, 480, DSWrapper.BitrateMode.CBR, 500000, 25, 0, DSWrapper.ScanType.Interlaced, false, null, false
                            , DSWrapper.MpaLayer.Layer2, 128000));
                enc.StartEncode();
            }*/

            /*using (UDPStream str = new UDPStream(System.Net.IPAddress.Any, System.Net.IPAddress.Parse("239.255.255.251"), 12346))
            {
                DSWrapper.DSPlayer.PlayMPEG2_TS(str, true, false);
            }*/

            /*MediaServerDevice device = new MediaServerDevice();
            device.LoadSettings("settings.xml", "library.db");
            //device.ItemManager.AddDirectorySync("D:\\Videos");
            device.ItemManager.BuildDatabaseSync();
            
            device.Start();

            Console.WriteLine("Spustene");
            Console.ReadKey();
            device.Stop();
            device.SaveSettings("settings.xml", "library.db");

            Console.WriteLine("Koniec");*/
        }
    }
}
