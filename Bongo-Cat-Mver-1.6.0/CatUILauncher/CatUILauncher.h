#pragma once
#ifndef _CATUILAUNCHER_H
#define _CATUILAUNCHER_H

#undef _DEBUG

#include "framework.h"



using namespace System;
using namespace System::Windows;
using namespace System::Windows::Controls;
using namespace System::Windows::Media;
using namespace System::Runtime;
using namespace std;
//using namespace msclr::interop;


//消息定义
#define UIWM_CLOSE 0x0400+100                       //关闭窗口 
#define UIWM_WRITECONFIG 0x0400+2                   //写config文件并通知cat窗口读取
#define UIWM_WRITECONFIG_AND_RELOAD_FILE 0x0400+3   //写config文件并通知cat窗口重新读取素材文件



//wpf页的托管页
public ref class WPFPageHost
{
public:
    static MyWpfApp::PageHome^ homePage;
};

//创建wpf页面
HWND GetHwnd(int x, int y, int width, int height)
{

    System::Windows::Interop::HwndSourceParameters^ sourceParams = gcnew System::Windows::Interop::HwndSourceParameters("Bongo Cat Mver setting" /*any name*/);

    sourceParams->PositionX = x;
    sourceParams->PositionY = y;
    sourceParams->Height = height;
    sourceParams->Width = width;

    //sourceParams->ParentWindow = System::IntPtr::IntPtr(parent);
    sourceParams->WindowStyle = WS_VISIBLE; // style
    sourceParams->UsesPerPixelOpacity = true;//透明
    System::Windows::Interop::HwndSource^ source = gcnew System::Windows::Interop::HwndSource(*sourceParams);

    WPFPageHost::homePage = gcnew MyWpfApp::PageHome();//WPFPage();
    source->RootVisual = WPFPageHost::homePage;
    return (HWND)source->Handle.ToPointer();
}




void sendConfigToSettingWindow()
{
    MyWpfApp::CommonData::config->mode = data::cfg["mode"].asInt();
    MyWpfApp::CommonData::config->decoration_armLineColor[0] = data::cfg["decoration"]["armLineColor"][0].asInt();
    MyWpfApp::CommonData::config->decoration_armLineColor[1] = data::cfg["decoration"]["armLineColor"][1].asInt();
    MyWpfApp::CommonData::config->decoration_armLineColor[2] = data::cfg["decoration"]["armLineColor"][2].asInt();
    MyWpfApp::CommonData::config->decoration_desktopPet = data::cfg["decoration"]["desktop_pet"].asBool();



    MyWpfApp::CommonData::config->decoration_emoticonClear = data::cfg["decoration"]["emoticonClear"][0].asInt();

    MyWpfApp::CommonData::config->decoration_emoticonKeep = data::cfg["decoration"]["emoticonKeep"].asBool();
    MyWpfApp::CommonData::config->decoration_soundKeep = data::cfg["decoration"]["soundKeep"].asBool();



    MyWpfApp::CommonData::config->decoration_framerateLimit = data::cfg["decoration"]["framerateLimit"].asInt();
    MyWpfApp::CommonData::config->decoration_height = data::cfg["decoration"]["window_size"][1].asInt();
    MyWpfApp::CommonData::config->decoration_l2dCorrect = data::cfg["decoration"]["l2d_correct"].asDouble();
    MyWpfApp::CommonData::config->decoration_l2dHorizontal = data::cfg["decoration"]["l2d_horizontal_flip"].asBool();
    MyWpfApp::CommonData::config->decoration_leftHanded = data::cfg["decoration"]["leftHanded"].asBool();
    MyWpfApp::CommonData::config->decoration_mouseForceMove = data::cfg["decoration"]["mouse_force_move"].asBool();
    MyWpfApp::CommonData::config->decoration_offsetXforMouse = data::cfg["decoration"]["offsetX"][0].asInt();
    MyWpfApp::CommonData::config->decoration_offsetXforPen = data::cfg["decoration"]["offsetX"][1].asInt();
    MyWpfApp::CommonData::config->decoration_offsetYforMouse = data::cfg["decoration"]["offsetY"][0].asInt();
    MyWpfApp::CommonData::config->decoration_offsetYforPen = data::cfg["decoration"]["offsetY"][1].asInt();
    MyWpfApp::CommonData::config->decoration_rgb[0] = data::cfg["decoration"]["rgb"][0].asInt();
    MyWpfApp::CommonData::config->decoration_rgb[1] = data::cfg["decoration"]["rgb"][1].asInt();
    MyWpfApp::CommonData::config->decoration_rgb[2] = data::cfg["decoration"]["rgb"][2].asInt();
    MyWpfApp::CommonData::config->decoration_rgb = MyWpfApp::CommonData::config->decoration_rgb;    //触发wpf数组的set事件
    MyWpfApp::CommonData::config->decoration_scalarForMouse = data::cfg["decoration"]["scalar"][0].asDouble();
    MyWpfApp::CommonData::config->decoration_scalarForPen = data::cfg["decoration"]["scalar"][1].asDouble();
    MyWpfApp::CommonData::config->decoration_topWindow = data::cfg["decoration"]["topWindow"].asBool();
    MyWpfApp::CommonData::config->decoration_width = data::cfg["decoration"]["window_size"][0].asInt();
    MyWpfApp::CommonData::config->decoration_height = data::cfg["decoration"]["window_size"][1].asInt();

    MyWpfApp::CommonData::config->workarea_rightBottom[0] = data::cfg["workarea"]["right_bottom"][0].asInt();
    MyWpfApp::CommonData::config->workarea_rightBottom[1] = data::cfg["workarea"]["right_bottom"][1].asInt();
    MyWpfApp::CommonData::config->workarea_topLeft[0] = data::cfg["workarea"]["top_left"][0].asInt();
    MyWpfApp::CommonData::config->workarea_topLeft[1] = data::cfg["workarea"]["top_left"][1].asInt();
    MyWpfApp::CommonData::config->workarea_workarea = data::cfg["workarea"]["workarea"].asBool();



    MyWpfApp::CommonData::config->standard_mouse = data::cfg["standard"]["mouse"].asBool();
    /*
    for (int i = 0; data::cfg["standard"]["face"][i].asString() != "null"; i++)
        MyWpfApp::CommonData::config->standard_face[i] = data::cfg["standard"]["face"][i].asInt();

    MyWpfApp::CommonData::config->standard_faceOn = data::cfg["standard"]["faceOn"].asBool();

    for (int i = 0; data::cfg["standard"]["hand"][i].asString() != "null"; i++)
        MyWpfApp::CommonData::config->standard_hand[i] = data::cfg["standard"]["hand"][i].asInt();

    for (int i = 0; data::cfg["standard"]["keyboard"][i].asString() != "null"; i++)
        MyWpfApp::CommonData::config->standard_keyboard[i] = data::cfg["standard"]["keyboard"][i].asInt();






    for (int i = 0; data::cfg["standard"]["mouse_left"][i].asString() != "null"; i++)
        MyWpfApp::CommonData::config->standard_mouseLeft[i] = data::cfg["standard"]["mouse_left"][i].asInt();

    for (int i = 0; data::cfg["standard"]["mouse_right"][i].asString() != "null"; i++)
        MyWpfApp::CommonData::config->standard_mouseRight[i] = data::cfg["standard"]["mouse_right"][i].asInt();

    for (int i = 0; data::cfg["standard"]["mouse_side"][i].asString() != "null"; i++)
        MyWpfApp::CommonData::config->standard_mouseSide[i] = data::cfg["standard"]["mouse_side"][i].asInt();

    for (int i = 0; data::cfg["standard"]["sounds"][i].asString() != "null"; i++)
        MyWpfApp::CommonData::config->standard_sounds[i] = data::cfg["standard"]["sounds"][i].asInt();

    MyWpfApp::CommonData::config->standard_soundsOn = data::cfg["standard"]["soundsOn"].asBool();

    for (int i = 0; data::cfg["keyboard"]["face"][i].asString() != "null"; i++)
        MyWpfApp::CommonData::config->keyboard_face[i] = data::cfg["keyboard"]["face"][i].asInt();

    MyWpfApp::CommonData::config->keyboard_faceOn = data::cfg["keyboard"]["faceOn"].asBool();


    for (int i = 0; data::cfg["keyboard"]["keyboard"][i].asString() != "null"; i++)
        MyWpfApp::CommonData::config->keyboard_keyboard[i] = data::cfg["keyboard"]["keyboard"][i].asInt();

    for (int i = 0; data::cfg["keyboard"]["lefthand"][i].asString() != "null"; i++)
        MyWpfApp::CommonData::config->keyboard_leftHand[i] = data::cfg["keyboard"]["lefthand"][i].asInt();

    for (int i = 0; data::cfg["keyboard"]["righthand"][i].asString() != "null"; i++)
        MyWpfApp::CommonData::config->keyboard_rightHand[i] = data::cfg["keyboard"]["righthand"][i].asInt();

    for (int i = 0; data::cfg["keyboard"]["sounds"][i].asString() != "null"; i++)
        MyWpfApp::CommonData::config->keyboard_sounds[i] = data::cfg["keyboard"]["sounds"][i].asInt();

    MyWpfApp::CommonData::config->standard_soundsOn = data::cfg["standard"]["soundsOn"].asBool();

    */

    MyWpfApp::CommonData::configflag = true;


}


void writeConfigFromUI()
{

    data::cfg["standard"]["mouse"] = MyWpfApp::CommonData::config->standard_mouse;
    data::cfg["decoration"]["emoticonKeep"]= MyWpfApp::CommonData::config->decoration_emoticonKeep;
    data::cfg["decoration"]["desktop_pet"] = MyWpfApp::CommonData::config->decoration_desktopPet;
    data::cfg["decoration"]["l2d_correct"] = MyWpfApp::CommonData::config->decoration_l2dCorrect;
    data::cfg["mode"] = MyWpfApp::CommonData::config->mode;




    data::cfg["mode"] = MyWpfApp::CommonData::config->mode;
    data::cfg["decoration"]["armLineColor"][0] = MyWpfApp::CommonData::config->decoration_armLineColor[0];
    data::cfg["decoration"]["armLineColor"][1] = MyWpfApp::CommonData::config->decoration_armLineColor[1];
    data::cfg["decoration"]["armLineColor"][2] = MyWpfApp::CommonData::config->decoration_armLineColor[2];

    data::cfg["decoration"]["desktop_pet"] = MyWpfApp::CommonData::config->decoration_desktopPet;



    data::cfg["decoration"]["emoticonClear"][0]=MyWpfApp::CommonData::config->decoration_emoticonClear;

    data::cfg["decoration"]["emoticonKeep"] = MyWpfApp::CommonData::config->decoration_emoticonKeep;
    data::cfg["decoration"]["soundKeep"] = MyWpfApp::CommonData::config->decoration_soundKeep;



    data::cfg["decoration"]["framerateLimit"] = MyWpfApp::CommonData::config->decoration_framerateLimit;
    data::cfg["decoration"]["window_size"][1] = MyWpfApp::CommonData::config->decoration_height;
    data::cfg["decoration"]["l2d_correct"] = MyWpfApp::CommonData::config->decoration_l2dCorrect;
    data::cfg["decoration"]["l2d_horizontal_flip"] = MyWpfApp::CommonData::config->decoration_l2dHorizontal;
    data::cfg["decoration"]["leftHanded"] = MyWpfApp::CommonData::config->decoration_leftHanded;
    data::cfg["decoration"]["mouse_force_move"] = MyWpfApp::CommonData::config->decoration_mouseForceMove;
    data::cfg["decoration"]["offsetX"][0] = MyWpfApp::CommonData::config->decoration_offsetXforMouse;
    data::cfg["decoration"]["offsetX"][1] = MyWpfApp::CommonData::config->decoration_offsetXforPen;
    data::cfg["decoration"]["offsetY"][0] = MyWpfApp::CommonData::config->decoration_offsetYforMouse ;
    data::cfg["decoration"]["offsetY"][1] = MyWpfApp::CommonData::config->decoration_offsetYforPen;
    data::cfg["decoration"]["rgb"][0] = MyWpfApp::CommonData::config->decoration_rgb[0];
    data::cfg["decoration"]["rgb"][1] = MyWpfApp::CommonData::config->decoration_rgb[1];
    data::cfg["decoration"]["rgb"][2]= MyWpfApp::CommonData::config->decoration_rgb[2] ;
    data::cfg["decoration"]["scalar"][0]=MyWpfApp::CommonData::config->decoration_scalarForMouse;
    data::cfg["decoration"]["scalar"][1] = MyWpfApp::CommonData::config->decoration_scalarForPen;
    data::cfg["decoration"]["topWindow"] = MyWpfApp::CommonData::config->decoration_topWindow ;
    data::cfg["decoration"]["window_size"][0]= MyWpfApp::CommonData::config->decoration_width ;
    data::cfg["decoration"]["window_size"][1]=MyWpfApp::CommonData::config->decoration_height;

    data::cfg["workarea"]["right_bottom"][0] = MyWpfApp::CommonData::config->workarea_rightBottom[0];
    data::cfg["workarea"]["right_bottom"][1] = MyWpfApp::CommonData::config->workarea_rightBottom[1];
    data::cfg["workarea"]["top_left"][0] = MyWpfApp::CommonData::config->workarea_topLeft[0];
    data::cfg["workarea"]["top_left"][1]=MyWpfApp::CommonData::config->workarea_topLeft[1];
    data::cfg["workarea"]["workarea"] = MyWpfApp::CommonData::config->workarea_workarea;


    //Json::StyledWriter sw;
    ofstream os;
    os.open("config.json", ios::out | ios::trunc);
    //os << sw.write(data::cfg);
    Json::StreamWriterBuilder builder;
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    writer->write(data::cfg, &os);
    os.close();
    //loadflag = true;
    MyWpfApp::CommonData::configChangeflag = true;

}

//左键拖动UI窗口
void moveUIwindow()
{
    static bool bFirstCLK = false;
    static POINT OldPos, NewPos;
    static int  xOffset, yOffset;
    static RECT subrect;
    if (WPFPageHost::homePage->is_window_MouseLeftButtonDown == true)
    {
        if (bFirstCLK == false) {
            GetCursorPos(&OldPos);
            GetWindowRect(wpfHwnd, &subrect);
            bFirstCLK = true;
        }
        else {

            GetCursorPos(&NewPos);
            xOffset = NewPos.x - OldPos.x;
            yOffset = NewPos.y - OldPos.y;
            MoveWindow(wpfHwnd, subrect.left + xOffset, subrect.top + yOffset, \
                subrect.right - subrect.left, subrect.bottom - subrect.top, TRUE);
            UpdateWindow(wpfHwnd);
        }
    }
    else
    {
        bFirstCLK = false;
    }
}

//用于转换TCHAR到STRING的函数
std::string TCHAR2STRING(TCHAR* STR)
{
    int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
    char* chRtn = new char[iLen * sizeof(char)];
    WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
    std::string str(chRtn);
    return str;
}

//定时检测cat窗口的线程函数
DWORD WINAPI checkCat(LPVOID lpParam)
{

    while (true)
    {
        WCHAR text[32] = { 0 };
        GetWindowText(hWnd, text, 32);
        //当识别到Cat窗口不存在时，退出UI窗口
        if (wcscmp(text, VERSION) != 0)
            exit(1);
        //检测定时
        Sleep(400);
    }
    return 0;
}

//创建用于定时检测cat窗口的线程
HANDLE thread_checkCat(void)
{
    HANDLE thread_handle;
    thread_handle = CreateThread(
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)checkCat,
        NULL,
        0,
        NULL
        );

    if (!thread_handle)
        data::error_msg("害，检测猫咪本体窗口的线程启动失败了", "一定是哪里出了问题");
    return thread_handle;
}






#ifdef _DEBUG
void Debugprint(string str4print)
{
    WriteConsoleA(g_hOutput, str4print.c_str(), strlen(str4print.c_str()), NULL, NULL);
}
#endif // DEBUG


#endif // !_CATUILAUNCHER_H