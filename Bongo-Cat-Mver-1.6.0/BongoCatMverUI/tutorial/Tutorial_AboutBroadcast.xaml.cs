using System;
using System.Collections.Generic;
using System.Diagnostics;
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

namespace MyWpfApp.tutorial
{
    /// <summary>
    /// Tutorial_AboutBroadcast.xaml 的交互逻辑
    /// </summary>
    public partial class Tutorial_AboutBroadcast : Page
    {
        public Tutorial_AboutBroadcast()
        {
            InitializeComponent();
        }

        private void Hyperlink_Click(object sender, RoutedEventArgs e)
        {
            Process proc = new System.Diagnostics.Process();
            proc.StartInfo.FileName = "https://jingyan.baidu.com/article/22a299b562ce0cde18376a22.html";
            proc.Start();
        
        }
    }
}
