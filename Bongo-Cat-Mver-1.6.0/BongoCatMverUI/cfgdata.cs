using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Globalization;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Media;

namespace MyWpfApp
{
   
    public class configdata : INotifyPropertyChanged
    {
        static  private bool is_first_load = true;
        private int _mode;
        public int mode
        {
            get
            {
                return _mode;
            }
            set
            {
                _mode = value;
                mode2 = _mode;
                OnPropertyChanged("mode");
            }
        }
        public int mode2
        {
            get
            {
                return _mode;
            }
            set
            {
                _mode = value;
                if (is_first_load)
                {
                    OnPropertyChanged("mode2");
                    is_first_load = false;
                }

            }
        }   //用于刚加载时确认是否显示被折叠的内容
        public bool livecache;
        public int decoration_width;
        public int decoration_height;
        private bool _decoration_leftHanded;
        public bool decoration_leftHanded
        {
            get { return _decoration_leftHanded; }
            set { _decoration_leftHanded = value; OnPropertyChanged("decoration_leftHanded"); }
        }
        private int[] _decoration_rgb;
        public int[] decoration_rgb
        {
            get { return _decoration_rgb; }
            set { _decoration_rgb = value; OnPropertyChanged("decoration_rgb"); }
        }
        public int decoration_offsetXforMouse;
        public int decoration_offsetYforMouse;
        public double decoration_scalarForMouse;
        public int decoration_offsetXforPen;
        public int decoration_offsetYforPen;
        public double decoration_scalarForPen;
        private int _decoration_framerateLimit;
        public int decoration_framerateLimit
        {
            get { return _decoration_framerateLimit; }
            set { _decoration_framerateLimit = value; OnPropertyChanged("decoration_framerateLimit"); }
        }
        private bool _decoration_topWindow;
        public bool decoration_topWindow
        {
            get { return _decoration_topWindow; }
            set { _decoration_topWindow = value; OnPropertyChanged("decoration_topWindow"); }
        }
        public int[] decoration_armLineColor;
        private bool _decoration_emoticonKeep;
        public bool decoration_emoticonKeep
        {
            get 
            {
                return _decoration_emoticonKeep;
            }
            set
            {
                _decoration_emoticonKeep = value;
                OnPropertyChanged("decoration_emoticonKeep");
            }
        }
        private bool _decoration_soundKeep;
        public bool decoration_soundKeep 
        {
            get { return _decoration_soundKeep; }
            set { _decoration_soundKeep = value; OnPropertyChanged("decoration_soundKeep"); }
        }
        public int decoration_emoticonClear;
        public int decoration_correct;
        private double _decoration_l2dCorrect;
        public double decoration_l2dCorrect
        {
            get
            {
                return _decoration_l2dCorrect;
            }
            set
            {
                _decoration_l2dCorrect = value;
                OnPropertyChanged("decoration_l2dCorrect");
            }
        }
        private bool _decoration_l2dHorizontal;
        public bool decoration_l2dHorizontal
        {
            get { return _decoration_l2dHorizontal; }
            set { _decoration_l2dHorizontal = value; OnPropertyChanged("decoration_l2dHorizontal"); }
        }
        private bool _decoration_mouseForceMove;
        public bool decoration_mouseForceMove 
        { 
            get {return _decoration_mouseForceMove; }
            set { _decoration_mouseForceMove = value; OnPropertyChanged("decoration_mouseForceMove"); }
        }
        private bool _decoration_desktopPet;
        public bool decoration_desktopPet
        {
            get { return _decoration_desktopPet; }
            set { _decoration_desktopPet = value; OnPropertyChanged("decoration_desktopPet"); }
        }
        public bool workarea_workarea;
        public int[] workarea_topLeft;
        public int[] workarea_rightBottom;
        private bool _standard_mouse;
        public bool standard_mouse
        {
            get {
                return _standard_mouse;
            }
            set {
                     _standard_mouse = value;
                    OnPropertyChanged("standard_mouse");
            }
        }
        public ArrayList standard_mouseLeft;
        public ArrayList standard_mouseRight;
        public ArrayList standard_mouseSide;
        public ArrayList standard_keyboard;
        public ArrayList standard_hand;
        public ArrayList standard_face;
        public ArrayList standard_sounds;
        public bool standard_faceOn;
        public bool standard_soundsOn;
        public ArrayList keyboard_keyboard;
        public ArrayList keyboard_leftHand;
        public ArrayList keyboard_rightHand;
        public ArrayList keyboard_face;
        public ArrayList keyboard_sounds;
        public bool keyboard_faceOn;
        public bool keyboard_soundsOn;


        public configdata() {
            decoration_rgb = new int[4];
            decoration_armLineColor = new int[4];
            workarea_topLeft = new int[2];
            workarea_rightBottom = new int[2];
            standard_mouseLeft = new ArrayList();
            standard_mouseRight = new ArrayList();
            standard_mouseSide = new ArrayList();
            standard_keyboard = new ArrayList();
            standard_hand = new ArrayList();
            standard_face = new ArrayList();
            standard_sounds = new ArrayList();
            keyboard_keyboard = new ArrayList();
            keyboard_leftHand = new ArrayList();
            keyboard_rightHand = new ArrayList();
            keyboard_face = new ArrayList();
            keyboard_sounds = new ArrayList();
        }


        public bool Standard_mouse 
        {
            get { return standard_mouse; }
            set { standard_mouse = value;
                OnPropertyChanged("Standard_mouse");
            }

        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }

    public static class CommonData {
        public static IntPtr HWND_catUIThread;  //UI线程的句柄
        public static ulong ID_catUIThread;     //UI线程ID
        public static IntPtr HWND_UIwindow;     //UI窗口句柄
        public static configdata config=new configdata();//config数据
        public static version version = new version();  //版本数据

        public static bool configflag=true;
        public static bool configChangeflag = true;

        public static Frame tutorialframe;      //存储一个框架以进行超链接导航
        public static ToggleButton manuToggle_page;//存储教程页目录的启动状态
        public static TreeView manu_tutorial;   //存放教程页菜单的引用

        public const int USER = 0x0400;//用户自定义消息的开始数值
        [System.Runtime.InteropServices.DllImport("user32.dll")]
        public static extern void PostMessage(IntPtr hWnd, int msg, int wParam, int lParam);
        [DllImport("user32", SetLastError = true)]
        public static extern bool PostThreadMesssage(ulong threadid, uint msg, int wParam, int lParam);

        public static void sendCloseMessage()
        {
            //PostThreadMesssage(ID_catUIThread, UIWM_CLOSE, 0, 0);
            PostMessage(HWND_UIwindow, UIWM_CLOSE,0,0);
        }

        public static bool readcatcfg()
        {
            configflag = false;
            return true;
        }
        public static bool writecatcfg()
        {
            PostMessage(HWND_UIwindow, UIWM_WRITECONFIG, 0, 0);
            return true;
        }
        public static bool writecatcfg_and_reloadfile()
        {
            PostMessage(HWND_UIwindow, UIWM_WRITECONFIG_AND_RELOAD_FILE, 0, 0);
            return true;
        }


        //定义消息常量
        public const int UIWM_CLOSE = USER + 100;                         //
        public const int UIWM_WRITECONFIG = USER + 2;                   //
        public const int UIWM_WRITECONFIG_AND_RELOAD_FILE = USER + 3;   //
    }




    //反向bool 转换器
    public class bool_swap : IValueConverter
    {
        //当值从绑定源传播给绑定目标时，调用方法Convert
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null)
                return DependencyProperty.UnsetValue;
            bool a = (bool)value;
            if (a == true)    //98是live2d模式值
                return false;
            else return true;
        }
        //当值从绑定目标传播给绑定源时，调用此方法ConvertBack
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            bool? usinglive2d = (bool)value;
            switch (usinglive2d)
            {
                case true:
                    return false;
                case false:
                    return true;
                default:
                    return null;
            }
        }
    }
    //反向visibil 转换器
    public class de_bool_to_visibility : IValueConverter
    {
        //当值从绑定源传播给绑定目标时，调用方法Convert
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null)
                return DependencyProperty.UnsetValue;
            bool a = (bool)value;
            if (a == true)    //98是live2d模式值
                return Visibility.Collapsed;
            else return Visibility.Visible;
        }
        //当值从绑定目标传播给绑定源时，调用此方法ConvertBack
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            int? usinglive2d = (int)value;
            switch (usinglive2d)
            {
                case (int)Visibility.Visible:
                    return false;
                case (int)Visibility.Collapsed:
                    return true;
                default:
                    return null;
            }
        }
    }




    //live2d与mode 转换器
    public class ModeConver : IValueConverter
    {
        //当值从绑定源传播给绑定目标时，调用方法Convert
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null)
                return DependencyProperty.UnsetValue;
            int a = (int)value;
            if (a == 98)    //98是live2d模式值
                return true;
            else return false;
        }
        //当值从绑定目标传播给绑定源时，调用此方法ConvertBack
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            bool? usinglive2d= (bool)value;
            switch (usinglive2d)
            {
                case true:
                    return 98;
                case false:
                    return 1;
                default:
                    return null;
            }
        }
    }

    //live2dMode与visibility 转换器
    public class l2dMode2visibilityConver : IValueConverter
    {
        //当值从绑定源传播给绑定目标时，调用方法Convert
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null)
                return DependencyProperty.UnsetValue;
            int a = (int)value;
            if (a == 98)    //98是live2d模式值
                return 50;//grid高50
            else return 0;
        }
        //当值从绑定目标传播给绑定源时，调用此方法ConvertBack
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            int usinglive2d = (int)value;
            switch (usinglive2d)
            {
                case 50:
                    return 98;
                case 0:
                    return 1;
                default:
                    return null;
            }
        }
    }




    //表情持续方式与控件显示转换器
    [ValueConversion(typeof(bool), typeof(String))]
    public class EmoticonKeepConver : IValueConverter
    {
        //当值从绑定源传播给绑定目标时，调用方法Convert
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null)
                return DependencyProperty.UnsetValue;
            bool a = (bool)value;
            if (a == true)
                return "按键取消";
            else return "松手取消";
        }
        //当值从绑定目标传播给绑定源时，调用此方法ConvertBack
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            String EmoticonKeep = (String)value;
            switch (EmoticonKeep)
            {
                case "按键取消":
                    return true;
                case "松手取消":
                    return false;
                default:
                    return null;
            }
        }
    }
    //音频持续方式与控件显示转换器
    [ValueConversion(typeof(bool), typeof(String))]
    public class SoundKeepConver : IValueConverter
    {
        //当值从绑定源传播给绑定目标时，调用方法Convert
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null)
                return DependencyProperty.UnsetValue;
            bool a = (bool)value;
            if (a == true)
                return "同时播放";
            else return "重新播放";
        }
        //当值从绑定目标传播给绑定源时，调用此方法ConvertBack
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            String EmoticonKeep = (String)value;
            switch (EmoticonKeep)
            {
                case "同时播放":
                    return true;
                case "重新播放":
                    return false;
                default:
                    return null;
            }
        }
    }

    //是否最新版本与控件显示文本转换器
    [ValueConversion(typeof(bool), typeof(String))]
    public class UpdateboolConver : IValueConverter
    {
        //当值从绑定源传播给绑定目标时，调用方法Convert
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null)
                return DependencyProperty.UnsetValue;
            bool a = (bool)value;
            if (a == true)
                return "当前已经是最新版本，不需要更新啦";
            else return "发现新版本，点击下载";
        }
        //当值从绑定目标传播给绑定源时，调用此方法ConvertBack
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            String EmoticonKeep = (String)value;
            switch (EmoticonKeep)
            {
                case "当前已经是最新版本，不需要更新啦":
                    return true;
                case "发现新版本，点击下载":
                    return false;
                default:
                    return null;
            }
        }
    }

    //rgba数组与颜色笔刷的转换器 todo增加a维度
    [ValueConversion(typeof(int[]), typeof(SolidColorBrush))]
    public class rgb2SolidColorBrushConver : IValueConverter
    {
        //当值从绑定源传播给绑定目标时，调用方法Convert
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null)
                return DependencyProperty.UnsetValue;
            int[] a = (int[])value;
            return new SolidColorBrush(System.Windows.Media.Color.FromRgb((Byte)a[0], (Byte)a[1], (Byte)a[2]));
        }
        //当值从绑定目标传播给绑定源时，调用此方法ConvertBack
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            SolidColorBrush bgcolor = (SolidColorBrush)value;
            int[] a = new int[4];
            System.Windows.Media.Color color = bgcolor.Color;
            a[0] = color.R;
            a[1] = color.G;
            a[2] = color.B;
            return a;
        }
    }
    //rgba数组与颜色笔刷的反色的转换器 todo增加a维度
    [ValueConversion(typeof(int[]), typeof(SolidColorBrush))]
    public class rgb2SolidColorBrushConver2 : IValueConverter
    {
        //当值从绑定源传播给绑定目标时，调用方法Convert
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null)
                return DependencyProperty.UnsetValue;
            int[] a = (int[])value;
            double light = (a[0] * 0.30 + a[1] * 0.59 + a[2] * 0.11)/255;

            if (light > 0.5)
            {
                return new SolidColorBrush(System.Windows.Media.Color.FromRgb(81, 45, 168));
            }
            else
                return new SolidColorBrush(System.Windows.Media.Color.FromRgb(255, 255, 255));

        }
        //当值从绑定目标传播给绑定源时，调用此方法ConvertBack
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            SolidColorBrush bgcolor = (SolidColorBrush)value;
            int[] a = new int[4];
            System.Windows.Media.Color color = bgcolor.Color;
            a[0] = 255 - color.R;
            a[1] = 255 - color.G;
            a[2] = 255 - color.B;
            return a;
        }
    }
    ////rgba数组与颜色笔刷的反色的转换器 todo增加a维度
    //[ValueConversion(typeof(int[]), typeof(SolidColorBrush))]
    //public class rgb2SolidColorBrushConver2 : IValueConverter
    //{
    //    //当值从绑定源传播给绑定目标时，调用方法Convert
    //    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
    //    {
    //        if (value == null)
    //            return DependencyProperty.UnsetValue;
    //        int[] a = (int[])value;
    //        return new SolidColorBrush(System.Windows.Media.Color.FromRgb((Byte)(255 - a[0]), (Byte)(255 - a[1]), (Byte)(255 - a[2])));
    //    }
    //    //当值从绑定目标传播给绑定源时，调用此方法ConvertBack
    //    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
    //    {
    //        SolidColorBrush bgcolor = (SolidColorBrush)value;
    //        int[] a = new int[4];
    //        System.Windows.Media.Color color = bgcolor.Color;
    //        a[0] = 255 - color.R;
    //        a[1] = 255 - color.G;
    //        a[2] = 255 - color.B;
    //        return a;
    //    }
    //}
    //rgba数组与颜色的转换器 todo增加a维度
    [ValueConversion(typeof(int[]), typeof(System.Windows.Media.Color))]
    public class rgb2colorConver : IValueConverter
    {
        //当值从绑定源传播给绑定目标时，调用方法Convert
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null)
                return DependencyProperty.UnsetValue;
            int[] a = (int[])value;
            return System.Windows.Media.Color.FromRgb((Byte)a[0], (Byte)a[1], (Byte)a[2]);
        }
        //当值从绑定目标传播给绑定源时，调用此方法ConvertBack
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            System.Windows.Media.Color color = (System.Windows.Media.Color)value;
            int[] a = new int[4];
            a[0] = color.R;
            a[1] = color.G;
            a[2] = color.B;
            return a;
        }
    }

    //rgba数组与各单独r、g、b数值的转换器
    [ValueConversion(typeof(int[]), typeof(int))]
    public class rgb2rConver : IValueConverter
    {
        //当值从绑定源传播给绑定目标时，调用方法Convert
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null)
                return DependencyProperty.UnsetValue;
            int[] a = (int[])value;
            return a[0];
        }
        //当值从绑定目标传播给绑定源时，调用此方法ConvertBack
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            double colorR = (double)value;
            CommonData.config.decoration_rgb[0]= (int)colorR;
            return CommonData.config.decoration_rgb;
        }
    }
    [ValueConversion(typeof(int[]), typeof(int))]
    public class rgb2gConver : IValueConverter
    {
        //当值从绑定源传播给绑定目标时，调用方法Convert
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null)
                return DependencyProperty.UnsetValue;
            int[] a = (int[])value;
            return a[1];
        }
        //当值从绑定目标传播给绑定源时，调用此方法ConvertBack
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            double colorG = (double)value;
            CommonData.config.decoration_rgb[1] = (int)colorG;
            return CommonData.config.decoration_rgb;
        }
    }
    [ValueConversion(typeof(int[]), typeof(int))]
    public class rgb2bConver : IValueConverter
    {
        //当值从绑定源传播给绑定目标时，调用方法Convert
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null)
                return DependencyProperty.UnsetValue;
            int[] a = (int[])value;
            return a[2];
        }
        //当值从绑定目标传播给绑定源时，调用此方法ConvertBack
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            double colorB = (double)value;
            CommonData.config.decoration_rgb[2] = (int)colorB;
            return CommonData.config.decoration_rgb;
        }
    }






    ////y方式与控件文本文字转换器
    //public class EmoticonKeepConver : IValueConverter
    //{
    //    //当值从绑定源传播给绑定目标时，调用方法Convert
    //    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
    //    {
    //        if (value == null)
    //            return DependencyProperty.UnsetValue;
    //        bool a = (bool)value;
    //        if (a == true)
    //            return "按键取消";
    //        else return "松手取消";
    //    }
    //    //当值从绑定目标传播给绑定源时，调用此方法ConvertBack
    //    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
    //    {
    //        string EmoticonKeep = (string)value;
    //        switch (EmoticonKeep)
    //        {
    //            case "按键取消":
    //                return true;
    //            case "松手取消":
    //                return false;
    //            default:
    //                return null;
    //        }
    //    }
    //}


    //定义一些当前版本的信息
    public class version : INotifyPropertyChanged
    {
        public static String version1="0";
        public static String version2 = "1";
        public static String version3 = "4";
        public static String version4 = "0";
        private static bool _is_lastest;
        public bool is_lastest
        {
            get { return _is_lastest; }
            set { _is_lastest = value; OnPropertyChanged("is_lastest"); }
        }

        public static bool is_lastestversion(String v1,String v2,String v3,String v4)
        {
            if (v1.Equals(version1) && v2.Equals(version2) && v3.Equals(version3) && v4.Equals(version4))
                return true;
            else return false;
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

    }











}