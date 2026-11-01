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
    /// setting_cat_keyboard_mode.xaml 的交互逻辑
    /// </summary>
    public partial class setting_cat_keyboard_mode : Page
    {
        public setting_cat_keyboard_mode()
        {
            InitializeComponent();
        }
        private void button_is_Using_live2d_Click(object sender, RoutedEventArgs e)
        {
            if (is_Using_live2d_check.IsChecked == false)
            {
                gird_l2d_horizontal_flip.Visibility = Visibility.Visible;
                gird_l2d_correct.Visibility = Visibility.Visible;
                Storyboard is_Using_live2d_switch = Resources["is_Using_live2d_on_animation"] as Storyboard;
                is_Using_live2d_switch.Begin();
                is_Using_live2d_check.IsChecked = true;

            }
            else
            {


                //gird_l2d_horizontal_flip.Visibility = Visibility.Collapsed;
                //gird_l2d_correct.Visibility = Visibility.Collapsed;
                Storyboard is_Using_live2d_switch = Resources["is_Using_live2d_off_animation"] as Storyboard;
                is_Using_live2d_switch.Begin();
                is_Using_live2d_check.IsChecked = false;
            }



            is_Using_live2d_check.RaiseEvent(new RoutedEventArgs(CheckBox.ClickEvent, is_Using_live2d_check));
        }

        private void TextBox_TextChanged_Num_limite(object sender, TextChangedEventArgs e)
        {
            textbox_l2d_correct.Text = LeaveOnlyNumbers(textbox_l2d_correct.Text);
        }



        private void tb_PreviewTextInput(object sender, TextCompositionEventArgs e)

        {
            if (e.Text == "." && textbox_l2d_correct.Text.Contains("."))
            {
                e.Handled = true;
                return;
            };
            string a = e.Text;
            e.Handled = !LeaveOnlyNumbers(a).Equals(e.Text);


        }


        private string LeaveOnlyNumbers(String inString)
        {
            bool occur = false;
            StringBuilder ans = new StringBuilder();
            for (int i = 0; i < inString.Length; ++i)
            {
                if (System.Text.RegularExpressions.Regex.IsMatch(inString[i].ToString(), "^[0-9\\.]*$"))
                {
                    if (inString[i] == '.')
                    {
                        if (!occur)
                        {
                            occur = true;
                            ans.Append(inString[i]);
                        }
                    }
                    else ans.Append(inString[i]);
                }
            }
            return ans.ToString();
        }



    }
}


