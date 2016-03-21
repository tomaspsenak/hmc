using System;
using System.Collections.Generic;
using System.Linq;
using System.ServiceProcess;
using System.Text;

namespace HomeMediaCenterService
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main()
        {
            /*MediaServerService service = new MediaServerService();
            service.StartMe();
            Console.ReadKey();*/

            ServiceBase[] ServicesToRun;
            ServicesToRun = new ServiceBase[] { 
				new MediaServerService() 
			};
            ServiceBase.Run(ServicesToRun);
        }
    }
}
