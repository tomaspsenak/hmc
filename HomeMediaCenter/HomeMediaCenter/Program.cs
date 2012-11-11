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
            //using (UDPStream output = new UDPStream(System.Net.IPAddress.Parse("192.168.0.4"), System.Net.IPAddress.Parse("239.255.255.251"), 12346))
            /*using (System.IO.FileStream output = new System.IO.FileStream("D:\\aaa.mpeg", System.IO.FileMode.Create))
            {
                EncoderBuilder builder = EncoderBuilder.GetEncoder("codec=mp4&source=D:\\Videos\\Test&LOW.mp4&width=720&height=576&fps=25&audBitrate=256&vidBitrate=3000");
                builder.StartEncode(output);
            }*/
            
            /*using (UDPStream str = new UDPStream(System.Net.IPAddress.Any, System.Net.IPAddress.Parse("239.255.255.251"), 12346))
            {
                DSWrapper.DSPlayer.PlayMPEG2_TS(str, true, false);
            }*/

            /*MediaServerDevice device = new MediaServerDevice();
            System.IO.DirectoryInfo di = System.IO.Directory.CreateDirectory(
                System.IO.Path.Combine(System.Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Home Media Center"));
            device.LoadSettings(System.IO.Path.Combine(di.FullName, "settings.xml"), System.IO.Path.Combine(di.FullName, "database.db"));
            //device.ItemManager.AddDirectorySync("D:\\Videos");
            device.ItemManager.BuildDatabaseSync();
            
            device.Start();

            Console.WriteLine("Started");
            Console.ReadKey();
            device.Stop();
            device.SaveSettings("settings.xml", "library.db");

            Console.WriteLine("Stopped");*/
        }
    }
}
