// CatUILauncher.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "CatUILauncher.h"
#include "../../../Windows Kits/10/Include/10.0.17763.0/um/shellapi.h"
#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                //当前实例
HWND wpfHwnd;                                   //传递WPF页面句柄
HWND hWnd=NULL;                                 //链接的猫咪窗口句柄




#ifdef _DEBUG
HANDLE g_hOutput;
#endif // DEBUG


[System::STAThreadAttribute]
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    //   CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CATUILAUNCHER));

    
#ifdef _DEBUG
    //创建用于调试的控制台
    AllocConsole();
    g_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    Debugprint("debug开始\n");
    Debugprint("链接CAT窗口...\n");
#endif // DEBUG


    //获取父进程的窗口句柄，如果无效则结束进程
    LPWSTR* szArglist = NULL;
    int nArgs = 0;
    int i = 0;
    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    if (NULL == szArglist)
    {
#ifdef _DEBUG
        return FALSE;
#endif
    }
    else
    {
        if (nArgs >= 1)
        {
            hWnd = (HWND)_tcstoul(szArglist[0], NULL, 16);
            TCHAR toshow[MAX_PATH] = { 0 };
            if (!GetWindowText(hWnd, toshow, MAX_PATH - 1))return FALSE;
            


#ifdef _DEBUG
            Debugprint("父进程窗口名\n");
            Debugprint(TCHAR2STRING(toshow) + "\n");
#endif // DEBUG

        }
    }
    
#ifdef _DEBUG
    Debugprint("开始创建UI窗口\n");
#endif // DEBUG

    

    float g_DPIScaleX = 1.0f;
    float g_DPIScaleY = 1.0f;



    //创建UI窗口
    wpfHwnd = GetHwnd(100/ g_DPIScaleX, 100/ g_DPIScaleY, 1000/ g_DPIScaleX, 630/ g_DPIScaleY);

    HDC hdc = GetDC(wpfHwnd);
    g_DPIScaleX = GetDeviceCaps(hdc, LOGPIXELSX) / 96.0f;
    g_DPIScaleY = GetDeviceCaps(hdc, LOGPIXELSY) / 96.0f;
    ReleaseDC(wpfHwnd, hdc);
    MoveWindow(wpfHwnd, 100 * g_DPIScaleX, 100 * g_DPIScaleY, 1000 * g_DPIScaleX, 630 * g_DPIScaleY,true);

    //给UI窗口传递其自身句柄
    MyWpfApp::CommonData::HWND_UIwindow = IntPtr(wpfHwnd);
    //设置UI窗口图标
    HICON hIcon;
    hIcon = LoadIcon(hInstance, (LPCWSTR)IDI_CATUILAUNCHER);
    if (hIcon) {
        SendMessage(wpfHwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    }




    //向父进程传递自身窗口句柄
    
#ifdef _DEBUG
    Debugprint("进行管道链接...\n");
#endif // DEBUG

    CHAR chBuf[128] = {0};
    memset(chBuf, 0, sizeof(chBuf));
    DWORD dwRead=32, dwWritten;
    HANDLE hStdin, hStdout;
    BOOL bSuccess;
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (
        (hStdout == INVALID_HANDLE_VALUE) ||
        (hStdin == INVALID_HANDLE_VALUE)
        )
        ExitProcess(1);


    printf("%p",wpfHwnd);

    
#ifdef _DEBUG
    Debugprint("向管道写入数据...\n");
#endif // DEBUG

    sprintf_s(chBuf,128, "%p", wpfHwnd);

	bSuccess = WriteFile(hStdout, chBuf, sizeof(chBuf), &dwWritten, NULL);
	if (!bSuccess)
	{
		data::error_msg(chBuf, "一定是哪里出了问题");
	}


    //从文件读取config数据
    data::init();
    //向UI发送config.json设置
    sendConfigToSettingWindow();

#ifdef _DEBUG
    //创建用于检测猫咪本体窗口的线程
    thread_checkCat();
#endif // !_DEBUG
    thread_checkCat();



    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        switch (msg.message)
        {
        case UIWM_CLOSE:
            DestroyWindow(wpfHwnd);
            return 0;
        case UIWM_WRITECONFIG:
            writeConfigFromUI();
            PostMessage(hWnd, UIWM_WRITECONFIG, 0, 0);
            break;
        case UIWM_WRITECONFIG_AND_RELOAD_FILE:
            writeConfigFromUI();
            PostMessage(hWnd, UIWM_WRITECONFIG_AND_RELOAD_FILE, 0, 0);
            break;
        default:
            break;
        }
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

