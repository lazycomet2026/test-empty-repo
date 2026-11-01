using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Diagnostics;
using System.IO;

namespace MyWpfApp
{
    /// <summary>
    /// Page1.xaml 的交互逻辑
    /// </summary>
    public partial class PageHome : Page,IDisposable
    {
        public bool is_window_MouseLeftButtonDown;
        public bool is_window_closed;
        //public delegate void MessageHandler(object sender, string message);
        //public event MessageHandler MessageReceived;
        static bool firstload = true;

        private bool disposed = false;
        protected virtual void Dispose(bool disposing)
        {
            if (!disposed)
            {
                // Dispose of resources held by this instance.
                // (This process is not shown here.)
                // Set the sentinel.
                disposed = true;
                // Suppress finalization of this disposed instance.
                if (disposing)
                {
                    GC.SuppressFinalize(this);
                }
            }
        }
        public void Dispose()
        {
            Dispose(true);
        }
        // Disposable types implement a finalizer.
        ~PageHome()
        {
            Dispose(false);
        }


        public PageHome()
        {
            is_window_MouseLeftButtonDown = false;
            is_window_closed = false;
            InitializeComponent();

        }

        private void button_donate_Click(object sender, RoutedEventArgs e)
        {
            Show_Page.Source = new Uri("PageSupportme.xaml", UriKind.Relative);
        }

        private void button_window_set_Click(object sender, RoutedEventArgs e)
        {
            Show_Page.Source=new Uri("MyNewPage.xaml", UriKind.Relative);
        }

        private void button_feedback_and_communicate_Click(object sender, RoutedEventArgs e)
        {
            Show_Page.Source  = new Uri("PageFeedback_and_Communicate.xaml", UriKind.Relative);
        }


        [System.Runtime.InteropServices.DllImport("user32.dll")]
        public static extern int SendMessage(IntPtr hWnd, int msg, int wParam, int lParam);
        [System.Runtime.InteropServices.DllImport("user32.dll")]
        public static extern bool ReleaseCapture();

        private void Move_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {

            ReleaseCapture();
            //发送拖动窗口的信息
            SendMessage(CommonData.HWND_UIwindow, 0x0112, 0xF010 | 2, 5);

        }

        private void Move_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            //DragMove();
            is_window_MouseLeftButtonDown = false;
        }

        private void button_tutorial_Click(object sender, RoutedEventArgs e)
        {
            if (firstload)
            {
                Show_Page.Source = new Uri("PageTutorial.xaml", UriKind.Relative);
                firstload = false;
            }
            else
            {
                Show_Page.Source = new Uri("PageTutorial.xaml", UriKind.Relative);
                CommonData.tutorialframe.Source = new Uri("tutorial/Tutorial_Homepage.xaml", UriKind.Relative);
            }
        }

        private void Page_Loaded(object sender, RoutedEventArgs e)
        {
            CommonData.readcatcfg();
        }

        private void btn_window_close_Click(object sender, RoutedEventArgs e)
        {
            //is_window_closed = true;
            //Dispose();
            CommonData.sendCloseMessage();
        }

        private void button_download_updates_Click(object sender, RoutedEventArgs e)
        {
            Show_Page.Source = new Uri("PageUpdate.xaml", UriKind.Relative);
        
            //Process proc = new System.Diagnostics.Process();
            //proc.StartInfo.FileName = "https://www.bilibili.com/read/readlist/rl191271";
            //proc.Start();
        }

        private void botton_cat_set_Click(object sender, RoutedEventArgs e)
        {
            Show_Page.Source = new Uri("setting_cat.xaml", UriKind.Relative);
        }

        private void botton_btn_MouseEnter(object sender, MouseEventArgs e)
        {
            Button button = (Button)sender;
            Panel.SetZIndex(button,1);
        }

        private void botton_btn_MouseLeave(object sender, MouseEventArgs e)
        {
            Button button = (Button)sender;
            Panel.SetZIndex(button, 0);
        }

    }







}

