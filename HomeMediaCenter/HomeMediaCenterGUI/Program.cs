using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using System.Security.Principal;
using System.Diagnostics;
using System.ServiceProcess;

namespace HomeMediaCenterGUI
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            SplashScreen ss = SplashScreen.ShowSplashScreen();

            bool serviceMode = false;
            if (!HasNoConsoleArg())
            {
                try
                {
                    ServiceController sc = new ServiceController("HomeMediaCenter");
                    if (sc.Status == ServiceControllerStatus.Stopped)
                    {
                        ss.SetStatusLabel(LanguageResource.StartingService + "...");

                        if (!IsAdministrator())
                        {
                            RunAsAdmin(Environment.GetCommandLineArgs()[0]);
                            return;
                        }

                        sc.Start();
                        sc.WaitForStatus(ServiceControllerStatus.Running, TimeSpan.FromSeconds(45));
                    }
                    else
                    {
                        ss.SetStatusLabel(LanguageResource.ConnectingToService + "...");
                        sc.WaitForStatus(ServiceControllerStatus.Running, TimeSpan.FromSeconds(45));
                    }

                    serviceMode = true;
                }
                catch (System.ServiceProcess.TimeoutException)
                {
                    ss.SetStatusLabel(LanguageResource.UnableToConnectService + ". " + LanguageResource.ApplicationWillBeClosed);
                    System.Threading.Thread.Sleep(5000);
                    return;
                }
                catch { }
            }

            ss.SetStatusLabel(LanguageResource.StartingApplication + "...");
            Application.Run(new MainForm(ss, serviceMode));
        }

        public static bool HasBackgroundArg()
        {
            return Environment.GetCommandLineArgs().Contains("/b", StringComparer.OrdinalIgnoreCase);
        }

        public static bool HasNoConsoleArg()
        {
            return Environment.GetCommandLineArgs().Contains("/noconsole", StringComparer.OrdinalIgnoreCase);
        }

        public static bool IsAdministrator()
        {
            WindowsIdentity identity = WindowsIdentity.GetCurrent();

            if (null != identity)
            {
                WindowsPrincipal principal = new WindowsPrincipal(identity);
                return principal.IsInRole(WindowsBuiltInRole.Administrator);
            }

            return false;
        }

        public static Process RunAsAdmin(string name)
        {
            string path = System.IO.Path.GetDirectoryName(name);

            ProcessStartInfo info = new ProcessStartInfo {
                UseShellExecute = true,
                WorkingDirectory = path,
                FileName = name
            };

            if (!IsAdministrator())
                info.Verb = "runas";

            try
            {
                return Process.Start(info);
            }
            catch
            {
                return null;
            }
        }
    }
}
