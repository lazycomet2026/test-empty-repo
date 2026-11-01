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
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace MyWpfApp
{
    /// <summary>
    /// PageTutorial.xaml 的交互逻辑
    /// </summary>
    public partial class PageTutorial : Page
    {
        public PageTutorial()
        {
            InitializeComponent();
            CommonData.tutorialframe = this.frame_tutorial;
            CommonData.manuToggle_page = this.manuToggle_page;
            CommonData.manu_tutorial = this.manu_tutorial;
        }

        private void ListBoxItem_Selected(object sender, RoutedEventArgs e)
        {

        }

        private void TreeViewItem_Selected(object sender, RoutedEventArgs e)
        {

        }

        private void CloseTutorialManu(object sender, RoutedEventArgs e)
        {
            Storyboard tutorial_manu_close = Resources["is_tutorial_manu_close_animation"] as Storyboard;
            tutorial_manu_close.Begin();
            if (manuToggle_manu.IsChecked == true)
                manuToggle_manu.IsChecked = false;
        }

        private void OpenTutorialManu(object sender, RoutedEventArgs e)
        {
            Storyboard tutorial_manu_open = Resources["is_tutorial_manu_open_animation"] as Storyboard;
            tutorial_manu_open.Begin();
            //if (manuToggle_page.IsChecked == true)
            //    manuToggle_page.IsChecked = false;
        }

        private void Click_Tutorial_Homepage(object sender, RoutedEventArgs e)
        {
            frame_tutorial.Source = new Uri("tutorial/Tutorial_Homepage.xaml", UriKind.Relative);
            manuToggle_page.IsChecked=false;

        }

        private void Click_Tutorial_HowToUseThisUI(object sender, MouseButtonEventArgs e)
        {
            frame_tutorial.Source = new Uri("tutorial/Tutorial_HowToUseThisUI.xaml", UriKind.Relative);
            TreeViewItem tree =(TreeViewItem)sender;
            tree.IsSelected = true;
            manuToggle_page.IsChecked = false;
        }
        private void Click_Tutorial_WhatYouMustKnow(object sender, MouseButtonEventArgs e)
        {
            frame_tutorial.Source = new Uri("tutorial/Tutorial_WhatYouMustKnow.xaml", UriKind.Relative);
            manuToggle_page.IsChecked = false;
            TreeViewItem tree = (TreeViewItem)sender;
            tree.IsSelected = true;
        }

        private void Click_Tutorial_AboutBroadcast(object sender, MouseButtonEventArgs e)
        {
            frame_tutorial.Source = new Uri("tutorial/Tutorial_AboutBroadcast.xaml", UriKind.Relative);
            manuToggle_page.IsChecked = false;
            TreeViewItem tree = (TreeViewItem)sender;
            tree.IsSelected = true;
        }

        private void Click_Tutorial_DIYCat(object sender, MouseButtonEventArgs e)
        {
            frame_tutorial.Source = new Uri("tutorial/Tutorial_DIYCat.xaml", UriKind.Relative);
            manuToggle_page.IsChecked = false;
            TreeViewItem tree = (TreeViewItem)sender;
            tree.IsSelected = true;
        }

        private void Click_Tutorial_Keymap(object sender, MouseButtonEventArgs e)
        {
            frame_tutorial.Source = new Uri("tutorial/Tutorial_Keymap.xaml", UriKind.Relative);
            manuToggle_page.IsChecked = false;
            TreeViewItem tree = (TreeViewItem)sender;
            tree.IsSelected = true;
        }

        private void Click_Tutorial_ConfigComparisonTable(object sender, MouseButtonEventArgs e)
        {
            frame_tutorial.Source = new Uri("tutorial/Tutorial_ConfigComparisonTable.xaml", UriKind.Relative);
            manuToggle_page.IsChecked = false;
            TreeViewItem tree = (TreeViewItem)sender;
            tree.IsSelected = true;
        }

        private void Click_Tutorial_HowToAsk(object sender, MouseButtonEventArgs e)
        {
            frame_tutorial.Source = new Uri("tutorial/Tutorial_HowToAsk.xaml", UriKind.Relative);
            manuToggle_page.IsChecked = false;
            TreeViewItem tree = (TreeViewItem)sender;
            tree.IsSelected = true;
        }

        private void Click_Question_CatFreeze(object sender, MouseButtonEventArgs e)
        {
            frame_tutorial.Source = new Uri("tutorial/Question_CatFreeze.xaml", UriKind.Relative);
            manuToggle_page.IsChecked = false;
            TreeViewItem tree = (TreeViewItem)sender;
            tree.IsSelected = true;
        }

        private void Click_Question_MissingPart(object sender, MouseButtonEventArgs e)
        {
            frame_tutorial.Source = new Uri("tutorial/Question_MissingPart.xaml", UriKind.Relative);
            manuToggle_page.IsChecked = false;
            TreeViewItem tree = (TreeViewItem)sender;
            tree.IsSelected = true;
        }

        private void Click_Question_TopWindow(object sender, MouseButtonEventArgs e)
        {
            frame_tutorial.Source = new Uri("tutorial/Question_TopWindow.xaml", UriKind.Relative);
            manuToggle_page.IsChecked = false;
            TreeViewItem tree = (TreeViewItem)sender;
            tree.IsSelected = true;
        }

        private void Click_Question_BlackBackground(object sender, MouseButtonEventArgs e)
        {
            frame_tutorial.Source = new Uri("tutorial/Question_BlackBackground.xaml", UriKind.Relative);
            manuToggle_page.IsChecked = false;
            TreeViewItem tree = (TreeViewItem)sender;
            tree.IsSelected = true;
        }

        private void Click_FAQ(object sender, MouseButtonEventArgs e)
        {

            TreeViewItem treeViewItem = (TreeViewItem)sender;
            Point p=e.GetPosition(treeViewItem);
            if (p.Y < 34)
            {
                if (treeView_FAQ.IsExpanded == false)
                    treeView_FAQ.IsExpanded = true;
                else treeView_FAQ.IsExpanded = false;
                e.Handled = true;
            }
            else e.Handled = false;
        }

        private void Click_Tutorial_Homepage(object sender, MouseButtonEventArgs e)
        {
            frame_tutorial.Source = new Uri("tutorial/Tutorial_Homepage.xaml", UriKind.Relative);
            manuToggle_page.IsChecked = false;
            TreeViewItem tree = (TreeViewItem)sender;
            tree.IsSelected = true;
        }

    }
}
