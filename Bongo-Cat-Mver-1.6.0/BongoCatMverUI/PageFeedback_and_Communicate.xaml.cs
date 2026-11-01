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

namespace MyWpfApp
{
    /// <summary>
    /// PageFeedback_and_Communicate.xaml 的交互逻辑
    /// </summary>
    public partial class PageFeedback_and_Communicate : Page
    {
        public PageFeedback_and_Communicate()
        {
            InitializeComponent();
        }

        private void Add_QQ_1(object sender, RoutedEventArgs e)
        {
 
            qqweb.Source = new Uri("https://space.bilibili.com/5808772");
            qqweb.Source = new Uri("tencent://groupwpa/?subcmd=all&param=7B2267726F757055696E223A3337343239353932322C2274696D655374616D70223A313538333634353633337D0A");
       
        }

        private void Add_QQ_2(object sender, RoutedEventArgs e)
        {
            qqweb.Source = new Uri("https://space.bilibili.com/5808772");
            qqweb.Source = new Uri("tencent://groupwpa/?subcmd=all&param=7B2267726F757055696E223A3638353632353234332C2274696D655374616D70223A313538333635343235317D0A");
        }
    }
}
