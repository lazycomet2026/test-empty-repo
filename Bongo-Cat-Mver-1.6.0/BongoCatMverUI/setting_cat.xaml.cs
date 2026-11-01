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
using System.Windows.Forms;
using System.Drawing;
using System.Runtime.InteropServices;


namespace MyWpfApp
{
    /// <summary>
    /// setting_cat.xaml 的交互逻辑
    /// </summary>
    public partial class setting_cat : Page
    {
        public static int[] origincolor = new int[4];


        public setting_cat()
        {
            InitializeComponent();
            DataContext = CommonData.config;
        }

        private void Button_standard_mode_Click(object sender, RoutedEventArgs e)
        {
            //Setting_Cat_Frame.Source=new Uri("setting_cat_standard_mode.xaml", UriKind.Relative);
            CommonData.config.mode = 1;
            CommonData.writecatcfg();

        }

        private void Button_keyboard_mode_Click(object sender, RoutedEventArgs e)
        {
            //Setting_Cat_Frame.Source = new Uri("setting_cat_keyboard_mode.xaml", UriKind.Relative);
            CommonData.config.mode = 2;
            CommonData.writecatcfg();
        }



        private void ToggleButton_Checked(object sender, RoutedEventArgs e)
        {

        }

        private void ToggleButton_Checked_1(object sender, RoutedEventArgs e)
        {

        }

        private void send_change_message(object sender, RoutedEventArgs e)
        {
            CommonData.writecatcfg();

        }


        private void open_config_json(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start(AppDomain.CurrentDomain.BaseDirectory+"config.json");
        }

        private void open_resources_files(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start(AppDomain.CurrentDomain.BaseDirectory);
        }

        private void Button_Click_background_color(object sender, RoutedEventArgs e)
        {
            if (pop_colorpicker.IsOpen == false)
                pop_colorpicker.IsOpen = true;
            else
                pop_colorpicker.IsOpen = false;
        }


        //pop弹窗中设置颜色的按钮
        private void button_bgcolor_white(object sender, RoutedEventArgs e)
        {
            CommonData.config.decoration_rgb[0] = 255;
            CommonData.config.decoration_rgb[1] = 255;
            CommonData.config.decoration_rgb[2] = 255;
            CommonData.config.decoration_rgb= CommonData.config.decoration_rgb; //触发事件
        }
        private void button_bgcolor_blue(object sender, RoutedEventArgs e)
        {
            CommonData.config.decoration_rgb[0] = 0;
            CommonData.config.decoration_rgb[1] = 0;
            CommonData.config.decoration_rgb[2] = 255;
            CommonData.config.decoration_rgb = CommonData.config.decoration_rgb; //触发事件
        }
        private void button_bgcolor_green(object sender, RoutedEventArgs e)
        {
            CommonData.config.decoration_rgb[0] = 0;
            CommonData.config.decoration_rgb[1] = 255;
            CommonData.config.decoration_rgb[2] = 0;
            CommonData.config.decoration_rgb = CommonData.config.decoration_rgb; //触发事件
        }
        private void button_bgcolor_red(object sender, RoutedEventArgs e)
        {
            CommonData.config.decoration_rgb[0] = 255;
            CommonData.config.decoration_rgb[1] = 0;
            CommonData.config.decoration_rgb[2] = 0;
            CommonData.config.decoration_rgb = CommonData.config.decoration_rgb; //触发事件
        }
        private void button_bgcolor_magenta(object sender, RoutedEventArgs e)
        {
            CommonData.config.decoration_rgb[0] = 255;
            CommonData.config.decoration_rgb[1] = 0;
            CommonData.config.decoration_rgb[2] = 255;
            CommonData.config.decoration_rgb = CommonData.config.decoration_rgb; //触发事件
        }

        private void popupopen(object sender, EventArgs e)
        {
            origincolor[0] = CommonData.config.decoration_rgb[0];
            origincolor[1] = CommonData.config.decoration_rgb[1];
            origincolor[2] = CommonData.config.decoration_rgb[2];
        }

        private void popyoclose(object sender, EventArgs e)
        {
            CommonData.config.decoration_rgb[0]= origincolor[0] ;
            CommonData.config.decoration_rgb[1]=origincolor[1];
            CommonData.config.decoration_rgb[2]=origincolor[2];
            CommonData.config.decoration_rgb = CommonData.config.decoration_rgb;
        }

        private void button_saveBgColor(object sender, RoutedEventArgs e)
        {
            origincolor[0] = CommonData.config.decoration_rgb[0];
            origincolor[1] = CommonData.config.decoration_rgb[1];
            origincolor[2] = CommonData.config.decoration_rgb[2];
            CommonData.writecatcfg();
        }

        private void tb_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            string a = e.Text;
            e.Handled = !LeaveOnlyNumbers(a).Equals(e.Text);
        }
        private string LeaveOnlyNumbers(String inString)
        {
            StringBuilder ans = new StringBuilder();
            for (int i = 0; i < inString.Length; ++i)
            {
                if (System.Text.RegularExpressions.Regex.IsMatch(inString[i].ToString(), "^[0-9\\.]*$"))
                {
                    if (inString[i] == '.')
                    {
                        
                    }
                    else ans.Append(inString[i]);
                }
            }
            return ans.ToString();
        }

        private void TextBox_fps_Num_limite(object sender, TextChangedEventArgs e)
        {
            textbox_fps.Text  = LeaveOnlyNumbers(textbox_fps.Text);
        }

        private void fps_return(object sender, System.Windows.Input.KeyEventArgs e)
        {
            if (e.Key == Key.Return)
                movefocus.Focus();
        }



        private void movefocus2visual(object sender, MouseButtonEventArgs e)
        {
            movefocus.Focus();
        }

        private void fps_null2zero(object sender, RoutedEventArgs e)
        {
            if(textbox_fps.Text == "")
                textbox_fps.Text ="0";

            CommonData.writecatcfg();

        }
    }
}
