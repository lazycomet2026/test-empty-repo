#include "header.hpp"
#include <resource.h>
#include "../mydata.hpp"
#include"../catmain.h"
#include"../catfunc.h"
#include "../../../Windows Kits/10/Include/10.0.17763.0/um/WinUser.h"
#include "../resource.h"

using namespace std;
using namespace cat_main;

//全局变量
sf::RenderWindow window;            //自身窗口实例
sf::Font catfont;                   //存储全局字体

HWND catHwnd;                       //存储自身句柄
HWND uiHwnd=NULL;                   //存储UI线程句柄
HINSTANCE g_hInst;                  
HINSTANCE hInst;
int nCmdShow_pass;

int WIDTH;                          //存储对窗口进行改变前的宽度
int HEIGHT;                         //存储对窗口进行改变前的高度

bool is_reload = false;
bool is_reload_for_wmmsg = false;
bool loadflag = false;
bool is_load_success = false;
bool is_logo_end = false;
bool is_opengl_inited = false;
bool is_UIThread_on = false;
HANDLE logoAnime;
pSoundqueue soundplayqueue;

//主DInput COM对象
LPDIRECTINPUT8 lpdi = NULL;
//键盘设备
LPDIRECTINPUTDEVICE8 lpdimouse = NULL;
//获取的鼠标数据放在此结构中
DIMOUSESTATE mouse_state_d7;
//鼠标坐标
POINT point;


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    //初始化json、读取数据
    while (!data::init());

    //【修改点1】：强制固定窗口为极小尺寸，彻底解决太大和裁切问题
    WIDTH = 350; 
    HEIGHT = 200;
    window.create(sf::VideoMode(WIDTH, HEIGHT), VERSION, sf::Style::Resize | sf::Style::Close); 
    
    //获取并存储cat窗口句柄
    catHwnd = window.getSystemHandle();

    //设置窗口图标
    HICON hIcon;
    hIcon = LoadIcon(hInstance, (LPCWSTR)IDI_ICON2);
    if (hIcon) {
        SendMessage(catHwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    }

    //初始化鼠标
    hInst=GetModuleHandle(0);
    DInput_Init_Mouse();
    //创建音频队列
    soundplayqueue = creatSoundQueue(50);

    //用线程加载各类文件,并获取线程句柄
    HANDLE handle_loadfile_thread = thread_loadfile(data::cfg["mode"].asInt());

    //【修改点2】：强制开启桌宠透明模式和窗口置顶，无视配置文件里的乱码
    data::cfg["decoration"]["desktop_pet"] = true;
    data::cfg["decoration"]["topWindow"] = true;
    
    //由config设置窗口样式。
    setWindow();

    //播放启动动画
    StartLogoAnime();

    //定时向窗口发送信息防止未响应
    static sf::Clock msgtimer;//定时器
    msgtimer.restart();//重置时钟


    //启动猫咪循环
    while (window.isOpen())
    {
#ifdef MYDEBUG
        static sf::Clock FPStimeer;
        FPStimeer.restart();
        static double fps[5] = { 0 };
        static int i = 0;
#endif // MYDEBUG

        //接受窗口消息
        MSG msg;
        if(PeekMessage(&msg, catHwnd, 0x0400 + 2, 0x0400 + 4, PM_REMOVE))
            switch (msg.message)
            {
            case UIWM_WRITECONFIG:
                //释放内存
                clearmemory();
                while (!data::init());
                //重新加载时，再次强行覆盖为小窗口透明
                data::cfg["decoration"]["desktop_pet"] = true;
                data::cfg["decoration"]["topWindow"] = true;
                setWindow();
                WIDTH = 350; HEIGHT = 200;
                window.setSize(sf::Vector2u(WIDTH, HEIGHT));
                switch (data::cfg["mode"].asInt())
                {
                case 1: standard::init(); break;
                case 2: keyboard::init(); break;
                case 98: live2d_98::init(); break;
                default: break;
                }
                is_reload_for_wmmsg = true;
                break;
            case UIWM_WRITECONFIG_AND_RELOAD_FILE:
                break;
            default:
                is_reload_for_wmmsg = false;
                break;
            }

        if (msgtimer.getElapsedTime() > sf::seconds(2))
        {
            msgtimer.restart();
            PostMessage(catHwnd, CATWM_ACTIVE, 0, 0);
        }


        //事件响应
        sf::Event event;
        while (window.pollEvent(event))
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                // 保留 Ctrl+R 重新加载
                if (event.key.code == sf::Keyboard::R && event.key.control)
                {
                    if (!is_reload) {
                        clearmemory();
                        while (!data::init());
                        data::cfg["decoration"]["desktop_pet"] = true;
                        data::cfg["decoration"]["topWindow"] = true;
                        setWindow();
                        WIDTH = 350; HEIGHT = 200;
                        window.setSize(sf::Vector2u(WIDTH, HEIGHT));
                        switch (data::cfg["mode"].asInt())
                        {
                        case 1: standard::init(); break;
                        case 2: keyboard::init(); break;
                        case 98: live2d_98::init(); break;
                        default: break;
                        }
                    }
                    is_reload = true;
                }
                //【修改点3】：彻底禁用崩溃的 Ctrl+Shift+S UI，防止程序闪现闪退
                else if (event.key.code == sf::Keyboard::S && event.key.control && event.key.shift)
                {
                    // 直接拦截，不再调用 On_Start_UI()，防止 Win7 崩溃
                    is_reload = true;
                }
                //【修改点4】：自制快捷键，按住 Ctrl + 上/下 箭头 来实时调整猫咪大小
                else if (event.key.code == sf::Keyboard::Up && event.key.control)
                {
                    WIDTH += 20; HEIGHT += 20;
                    window.setSize(sf::Vector2u(WIDTH, HEIGHT));
                }
                else if (event.key.code == sf::Keyboard::Down && event.key.control)
                {
                    if (WIDTH > 150) {
                        WIDTH -= 20; HEIGHT -= 20;
                        window.setSize(sf::Vector2u(WIDTH, HEIGHT));
                    }
                }
                break;
            case sf::Event::Resized:
                // 强制保持比例，防止被拉扁
                if (!data::cfg["decoration"]["desktop_pet"].asBool()) 
                {
                    if (event.size.width != WIDTH)
                        window.setSize(sf::Vector2u(event.size.width, event.size.width * 200 / 350));
                    else if (event.size.height != HEIGHT)
                        window.setSize(sf::Vector2u(event.size.height * 350 / 200, event.size.height));
                    WIDTH = window.getSize().x;
                    HEIGHT = window.getSize().y;
                }
                break;
            default:
                is_reload = false;
            }


        //图形绘制
        clearCatWindow();
        drawCat(data::cfg["mode"].asInt());

#ifdef MYDEBUG
        fps[i] = sf::seconds(1) / FPStimeer.getElapsedTime();
        i++;
        if (i > 4)i = 0;
        window.draw(sf::Text("FPS:"+ std::to_string(
            min(min(min(min(fps[0], fps[1]), min(fps[1], fps[2])), fps[3]), fps[4])
        ), catfont));
#endif // MYDEBUG 

        window.display();

        //循环置顶
        if (data::cfg["decoration"]["topWindow"].asBool()) 
            SetWindowPos(catHwnd, HWND_TOP, 1, 1, 1, 1, SWP_NOMOVE | SWP_NOSIZE | SWP_NOCOPYBITS| SWP_SHOWWINDOW);
    }

    return 0;
}