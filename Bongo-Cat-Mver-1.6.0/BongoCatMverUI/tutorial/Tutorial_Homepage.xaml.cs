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

namespace MyWpfApp.tutorial
{
    /// <summary>
    /// Tutorial_Homepage.xaml 的交互逻辑
    /// </summary>
    public partial class Tutorial_Homepage : Page
    {
        public Tutorial_Homepage()
        {
            InitializeComponent();
        }

        private void Click_HowToUseThisUI(object sender, RoutedEventArgs e)
        {
            CommonData.tutorialframe.Source = new Uri("tutorial/Tutorial_HowToUseThisUI.xaml", UriKind.Relative);
        }
        private void Click_WhatYouMustKnow(object sender, RoutedEventArgs e)
        {
            CommonData.tutorialframe.Source = new Uri("tutorial/Tutorial_WhatYouMustKnow.xaml", UriKind.Relative);
        }
        private void Click_AboutBroadcast(object sender, RoutedEventArgs e)
        {
            CommonData.tutorialframe.Source = new Uri("tutorial/Tutorial_AboutBroadcast.xaml", UriKind.Relative);
        }
        private void Click_DIYCat(object sender, RoutedEventArgs e)
        {
            CommonData.tutorialframe.Source = new Uri("tutorial/Tutorial_DIYCat.xaml", UriKind.Relative);
        }
        private void Click_CatFreeze(object sender, RoutedEventArgs e)
        {
            CommonData.tutorialframe.Source = new Uri("tutorial/Question_CatFreeze.xaml", UriKind.Relative);
        }
        private void Click_Keymap(object sender, RoutedEventArgs e)
        {
            CommonData.tutorialframe.Source = new Uri("tutorial/Tutorial_Keymap.xaml", UriKind.Relative);
        }
        private void Click_Manu(object sender, RoutedEventArgs e)
        {
            CommonData.manuToggle_page.IsChecked = true;
        }
    }
}
