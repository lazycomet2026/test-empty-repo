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
using System.Windows.Media.Animation;
using System.Text.RegularExpressions;

namespace MyWpfApp
{
    /// <summary>
    /// setting_cat_standard_mode.xaml 的交互逻辑
    /// </summary>
    public partial class setting_cat_standard_mode : Page
    {
        public setting_cat_standard_mode()
        {
            InitializeComponent();


            DataContext = CommonData.config;
            
        }

        private void Button_is_pen_Click(object sender, RoutedEventArgs e)
        {
            if (CommonData.config.standard_mouse == false)
                CommonData.config.standard_mouse = true;
            else
                CommonData.config.standard_mouse = false;
            CommonData.writecatcfg();
            is_pen_check.RaiseEvent(new RoutedEventArgs(CheckBox.ClickEvent, is_pen_check));
        }

        private void button_is_Using_live2d_Click(object sender, RoutedEventArgs e)
        {
            if (is_Using_live2d_check.IsChecked == false)
            {
                gird_l2d_horizontal_flip.Visibility = Visibility.Visible;
                gird_l2d_correct.Visibility = Visibility.Visible;
                Storyboard is_Using_live2d_switch = Resources["is_Using_live2d_on_animation"] as Storyboard;
                is_Using_live2d_switch.Begin();
                CommonData.config.mode = 98;
            }
            else
            {


                //gird_l2d_horizontal_flip.Visibility = Visibility.Collapsed;
                //gird_l2d_correct.Visibility = Visibility.Collapsed;
                Storyboard is_Using_live2d_switch = Resources["is_Using_live2d_off_animation"] as Storyboard;
                is_Using_live2d_switch.Begin();
                CommonData.config.mode = 1;
            }
            CommonData.writecatcfg();
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

        private void Button_is_lefthand_Click(object sender, RoutedEventArgs e)
        {
            if (CommonData.config.decoration_leftHanded == false)
                CommonData.config.decoration_leftHanded = true;
            else
                CommonData.config.decoration_leftHanded = false;
            CommonData.writecatcfg();
            is_leftHand_check.RaiseEvent(new RoutedEventArgs(CheckBox.ClickEvent, is_pen_check));
        }

        private void button_is_mouse_force_move_Click(object sender, RoutedEventArgs e)
        {
            if (CommonData.config.decoration_mouseForceMove == false)
                CommonData.config.decoration_mouseForceMove = true;
            else
                CommonData.config.decoration_mouseForceMove = false;
            CommonData.writecatcfg();
            is_mouseForceMove_check.RaiseEvent(new RoutedEventArgs(CheckBox.ClickEvent, is_pen_check));
        }

        private void button_way_to_cancel_emoticon_Click(object sender, RoutedEventArgs e)
        {
            if (CommonData.config.decoration_emoticonKeep== false)
                CommonData.config.decoration_emoticonKeep = true;
            else
                CommonData.config.decoration_emoticonKeep = false;
            CommonData.writecatcfg();
        }

        private void button_is_live2d_horizontal_flip(object sender, RoutedEventArgs e)
        {
            if (CommonData.config.decoration_l2dHorizontal == false)
                CommonData.config.decoration_l2dHorizontal = true;
            else
                CommonData.config.decoration_l2dHorizontal = false;
            CommonData.writecatcfg();
            is_live2d_horizontal_flip_check.RaiseEvent(new RoutedEventArgs(CheckBox.ClickEvent, is_pen_check));
        }

        private void button_way_to_play_sound_Click(object sender, RoutedEventArgs e)
        {
            if (CommonData.config.decoration_soundKeep == false)
                CommonData.config.decoration_soundKeep = true;
            else
                CommonData.config.decoration_soundKeep = false;
            CommonData.writecatcfg();
        }

        private void scale_null2init(object sender, RoutedEventArgs e)
        {
            if (textbox_l2d_correct.Text == "")
                textbox_l2d_correct.Text = "0.1";
            CommonData.writecatcfg();
        }

        private void live2dcorrect_return(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
                movefocus.Focus();
        }
    }

}

