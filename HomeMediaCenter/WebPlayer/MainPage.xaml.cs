using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Browser;
using System.Windows.Threading;

namespace WebPlayer
{
    public partial class MainPage : UserControl
    {
        private DispatcherTimer positionTimer = new DispatcherTimer();

        public MainPage()
        {
            InitializeComponent();

            this.positionTimer.Tick += new EventHandler(positionTimer_Tick);
            this.positionTimer.Interval = TimeSpan.FromSeconds(1.0);

            HtmlPage.RegisterScriptableObject("Player", this);
        }

        [ScriptableMember]
        public void Play(string url)
        {
            try
            {
                this.mediaElement.Source = new Uri(url);
                this.mediaElement.Play();

                this.positionTimer.Start();
            }
            catch { }
        }

        [ScriptableMember]
        public void Pause()
        {
            this.positionTimer.Stop();

            this.mediaElement.Pause();
        }

        [ScriptableMember]
        public void SetVolume(double volume)
        {
            this.mediaElement.Volume = volume;
        }

        [ScriptableMember]
        public double GetVolume()
        {
            return this.mediaElement.Volume;
        }

        private void positionTimer_Tick(object sender, EventArgs e)
        {
            try
            {
                HtmlPage.Window.Invoke("WebPlayerPositionChange", this.mediaElement.Position.TotalSeconds);
            }
            catch { }
        }

        private void mediaElement_MediaOpened(object sender, RoutedEventArgs e)
        {
            try
            {
                HtmlPage.Window.Invoke("WebPlayerSizeChange", this.mediaElement.NaturalVideoWidth, this.mediaElement.NaturalVideoHeight);
            }
            catch { }
        }

        private void mediaElement_CurrentStateChanged(object sender, RoutedEventArgs e)
        {
            if (this.mediaElement.CurrentState == MediaElementState.Buffering)
            {
                this.Buffering.Begin();
            }
            else
            {
                this.Buffering.Stop();
            }
        }

        private void UserControl_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Application.Current.Host.Content.IsFullScreen = !Application.Current.Host.Content.IsFullScreen;
            this.LayoutRoot.Background = new SolidColorBrush(Application.Current.Host.Content.IsFullScreen ? Colors.Black : Colors.White);
        }
    }
}
