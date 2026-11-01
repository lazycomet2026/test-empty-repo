#pragma once
#ifndef _CATMAIN_H
#define _CATMAIN_H


#include<header.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<CubismFramework.hpp>
#include"LAppAllocator.hpp"
#include "myUserModel.hpp"
#include"LAppPal.hpp"
#include <tchar.h>


using namespace Csm;

namespace cat_main
{
	bool config_init();//初始化json、读取config
	DWORD WINAPI loadfile(LPVOID lpParam);//读取文件的线程所用的函数
	void setWindow();//根据config改变窗口样式
	HANDLE thread_loadfile(int mode);//创建读取文件的线程
	void StartLogoAnime();//播放启动动画
	void clearCatWindow();//根据config内容来擦除窗口图像
	void drawCat(int mode);//根据模式值来绘制猫咪
	void   PrintMessage(const  Csm::csmChar* message) { printf("%s", message); return; }//用于初始化csm的函数之一
	void lockCat();//桌宠模式下锁定与解锁猫咪
	void moveCatPosition();//拖动窗口
	void moveCatSize();//改变窗口大小
	DWORD WINAPI UI_Start(LPVOID lpParam);//用于启动UI窗口并建立链接的函数


	bool is_first_load_cat = true;	//是否为这次运行的第一次加载猫咪
	static long windowlangdefault;	//默认情况下的窗口long
	static RECT rectdefault;		//默认情况下的窗口rect
	static long ret;				//存储当前窗口long值
	static bool is_UI_on = false;			//存储UI启动状态

	LAppAllocator   _cubismAllocator;

	enum MODE
	{
		STANDARD = 1,
		KEYBOARD = 2,
		LIVE2D = 98
	};

	//初始化json、读取config
	bool config_init()
	{
		return true;
	}

	//根据config改变窗口样式
	void setWindow()
	{
		//sfwindow的宽高与带边框的windows宽高的差值
		static int offsetX, offsetY;
		static bool lastmode_ispet=false;

		//MoveWindow(catHwnd, window.getPosition().x, window.getPosition().y, data::cfg["decoration"]["window_size"][0].asInt(), data::cfg["decoration"]["window_size"][1].asInt(), TRUE);
		if (is_first_load_cat) {
			windowlangdefault = GetWindowLong(catHwnd, GWL_STYLE);
			GetWindowRect(catHwnd, &rectdefault);
			is_first_load_cat = false;
			offsetX = rectdefault.right - rectdefault.left - WIDTH;
			offsetY = rectdefault.bottom - rectdefault.top - HEIGHT;
		}
		//如果使用桌宠，则设置透明窗口无边框
		if (data::cfg["decoration"]["desktop_pet"].asBool())
		{
			//设置窗口无边框

			SetWindowLong(catHwnd, GWL_STYLE, windowlangdefault & (~(WS_CAPTION | WS_THICKFRAME)));
			//根据重置前的模式来决定是否微调窗口位置
			RECT catwndrect;
			GetClientRect(catHwnd,&catwndrect);
			if(lastmode_ispet)
				MoveWindow(catHwnd, window.getPosition().x, window.getPosition().y, WIDTH, HEIGHT, TRUE);
			else
				MoveWindow(catHwnd, window.getPosition().x+offsetX/2, window.getPosition().y + offsetY-offsetX/2, WIDTH, HEIGHT, TRUE);
			lastmode_ispet = true;
			//设置窗口透明
			DWM_BLURBEHIND bb = { 0 };
			HRGN hRgn = CreateRectRgn(0, 0, -1, -1); //应用毛玻璃的矩形范围，参数(0,0,-1,-1)可以让整个窗口客户区变成透明的，而鼠标是可以捕获到透明的区域
			bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
			bb.hRgnBlur = hRgn;
			bb.fEnable = TRUE;
			DwmEnableBlurBehindWindow(catHwnd, &bb);
		}

		//如果未使用桌宠，则设置窗口有边框
		else
		{
			//设置窗口边框风格
			SetWindowLong(catHwnd, GWL_STYLE, windowlangdefault);

			//根据重置前的模式来决定是否微调窗口位置
			if(lastmode_ispet)
				MoveWindow(catHwnd, window.getPosition().x-offsetX/2, window.getPosition().y- offsetY+ offsetX / 2, \
					offsetX + WIDTH, offsetY + HEIGHT,
					TRUE);
			else
			MoveWindow(catHwnd, window.getPosition().x, window.getPosition().y, \
				offsetX + WIDTH , offsetY+HEIGHT,
				TRUE);
			lastmode_ispet = false;
			//设置窗口不透明
			DWM_BLURBEHIND bb = { 0 };
			HRGN hRgn = CreateRectRgn(0, 0, -1, -1); //应用毛玻璃的矩形范围，参数(0,0,-1,-1)可以让整个窗口客户区变成透明的，而鼠标是可以捕获到透明的区域
			bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
			bb.hRgnBlur = NULL;
			bb.fEnable = FALSE;
			DwmEnableBlurBehindWindow(catHwnd, &bb);
		}

		//设置置顶
		if (data::cfg["decoration"]["topWindow"].asBool())
			SetWindowPos(catHwnd, HWND_TOPMOST, 1, 1, 1, 1, SWP_NOMOVE | SWP_NOSIZE);
		else
			SetWindowPos(catHwnd, HWND_NOTOPMOST, 1, 1, 1, 1, SWP_NOMOVE | SWP_NOSIZE);

		//如果是模式98，则初始化openGL
		if (data::cfg["mode"].asInt() == LIVE2D) {
			if (glfwInit() == GL_FALSE)
			{
				data::error_msg("glfw初始化失败了。", "一定是哪里出了问题");
			}
			if (glewInit() != GLEW_OK) {
				glfwTerminate();
				data::error_msg("glew初始化失败了。", "一定是哪里出了问题");
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			CubismFramework::Option   _cubismOption;
			// ログ出力のレベルを設定。LogLevel_Verboseの場合は詳細ログを出力させる。
			_cubismOption.LoggingLevel = CubismFramework::Option::LogLevel_Verbose;
			_cubismOption.LogFunction = PrintMessage;

			// CubismNativeFrameworkの初期化に必要なパラメータを設定する。
			CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);

			// CubismFrameworkを初期化する。
			CubismFramework::Initialize();


			live2d_98::myModel = new myUserModel();

			// Read moc3 file.
			//这些内容也必须在主线程里做，不然猫咪的纹理会变成黑色
			std::string modelPath = "Resources/cat/";
			std::string modelJsonName = "cat.model3.json";
			live2d_98::myModel->LoadAssets(modelPath.c_str(), modelJsonName.c_str());
			LAppPal::UpdateTime();
			is_opengl_inited = true;//告知l2d初始化线程opengl的初始化情况

		}

		//保存当前long值，提供给桌宠模式窗口锁定函数
		static long ret = ::GetWindowLong(catHwnd, GWL_EXSTYLE);

		//设置刷新率fps
		window.setFramerateLimit(data::cfg["decoration"]["framerateLimit"].asInt());
	}


	//创建读取文件的线程
	HANDLE thread_loadfile(int mode)
	{
		HANDLE thread_handle;
		thread_handle = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)loadfile,
			&mode,
			0,
			NULL
		);

		if (!thread_handle)
			data::error_msg("害，载入文件的线程启动失败了", "一定是哪里出了问题");
		return thread_handle;
	}

	//读取文件的线程所用的函数
	DWORD WINAPI loadfile(LPVOID lpParam)
	{
		bool flag = false;

		while (flag == false) {
			//读取程序所需要的字体文件
			if (!catfont.loadFromFile("Resources/cat.ttf")) {
				data::error_msg("字体没加载好，是不是你把cat.ttf弄丢了？？", "加载文件出错了");
				flag = false;
				continue;
			}
			//按模式载入文件
			switch (data::cfg["mode"].asInt())
			{
			case STANDARD:
				flag = standard::init();
				if (flag == false)data::error_msg("标准模式初始化出问题了。", "一定是哪里出了问题");
				break;
			case KEYBOARD:
				flag = keyboard::init();
				if (flag == false)data::error_msg("纯键盘模式初始化出问题了。", "一定是哪里出了问题");
				break;
			case LIVE2D:
				flag = live2d_98::init();
				if (flag == false)data::error_msg("live2d模式出问题了。", "一定是哪里出了问题");
				break;
			default:
				data::error_msg("兄弟你这模式的数字没设置对头啊。", "你设置错了");
				flag = false;
			}
		}
		is_load_success = true;
		return 0;
	}

	//播放启动动画
	void StartLogoAnime()
	{
		//todo判断是否为第一次载入

		//创建并重启计时器
		sf::Clock textTimer;
		textTimer.restart();
		//加载logo图片
		sf::Sprite L2dlogo;
		L2dlogo.setTexture(data::load_texture("Resources/l2dlogo.png"), true);
		//将logo图片设置在中心
		L2dlogo.setPosition((WIDTH- L2dlogo.getTextureRect().width)/2,(HEIGHT- L2dlogo.getTextureRect().height)/2);
		//启动动画的动画循环
		while (window.isOpen() && ((textTimer.getElapsedTime() < sf::seconds(2)) || is_load_success == false))
		{
			//继续消息循环
			sf::Event event;
			while (window.pollEvent(event))
				switch (event.type)
				{
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::Resized:
					//非桌宠模式的情况下用这个方式固定窗口比例
					if (!data::cfg["decoration"]["desktop_pet"].asBool())
					{
						if (event.size.width != WIDTH)
							window.setSize(sf::Vector2u(event.size.width, event.size.width * data::cfg["decoration"]["window_size"][1].asInt() / data::cfg["decoration"]["window_size"][0].asInt()));
						else if (event.size.height != HEIGHT)
							window.setSize(sf::Vector2u(event.size.height * data::cfg["decoration"]["window_size"][0].asInt() / data::cfg["decoration"]["window_size"][1].asInt(), event.size.height));
						//存储新的宽高值
						WIDTH = window.getSize().x;
						HEIGHT = window.getSize().y;
					}
					break;
				default:
					break;
				}
			window.clear(sf::Color::White);
			window.draw(L2dlogo);
			window.display();
		}



	}

	//根据config内容来擦除窗口图像
	void clearCatWindow()
	{
		//如果启用了桌宠，则把窗口擦至全透明
		if (data::cfg["decoration"]["desktop_pet"].asBool())
			window.clear(sf::Color(0, 0, 0, 0));
		//如果未启动桌宠，则把窗口擦至rgb设置的色值
		else
			window.clear(
				sf::Color(
					data::cfg["decoration"]["rgb"][0].asInt(),
					data::cfg["decoration"]["rgb"][1].asInt(),
					data::cfg["decoration"]["rgb"][2].asInt()
				)
			);
	}


	//根据模式值来绘制猫咪
	void drawCat(int mode)
	{

		switch (mode)
		{
		case STANDARD:
			standard::draw();
			break;
		case KEYBOARD:
			keyboard::draw();
			break;
		case LIVE2D:
			live2d_98::draw();
			break;
		default:
			return;
		}
		//桌宠模式下根据情况锁定与解锁猫咪
		lockCat();

		//桌宠模式下对猫咪进行移动和大小的改变
		if (data::cfg["decoration"]["desktop_pet"].asBool())
		{
			moveCatPosition();
			moveCatSize();
		}

	};

	//桌宠模式下锁定与解锁猫咪
	void lockCat()
	{
		static bool T_keystate = false;			//T键状态值和鼠标穿透状态值
		static bool is_mouse_through = false;		//鼠标穿透状态值
		static  sf::Clock textTimer;			//文本显示计时器
		static bool text_drawing = false;		//是否显示文本，主要用于防止在程序启动时就显示文本

		//判断是否按下了组合键ctrl:17 shift:16 T:84   并判断是否为桌宠状态。  使用GetKeyState()函数是为了在穿透状态下也能接受热键
		if ((GetKeyState(17) & 0x8000) && (GetKeyState(16) & 0x8000) && (GetKeyState(84) & 0x8000) && data::cfg["decoration"]["desktop_pet"].asBool())
		{
			//判断是否刚按下T
			if (T_keystate == false)
			{
				//设置是否鼠标穿透，并重置文本显示的计时器
				if (is_mouse_through) {
					SetWindowLong(catHwnd, GWL_EXSTYLE, ret);
					is_mouse_through = false;
					textTimer.restart();
				}
				else
				{
					SetWindowLong(catHwnd, GWL_EXSTYLE, ret | WS_EX_LAYERED | WS_EX_TRANSPARENT);
					is_mouse_through = true;
					textTimer.restart();
				}
			}
			//设置T键状态， 在第一次按下组合键时才开启文本显示
			T_keystate = true;
			text_drawing = true;
		}
		//松开键盘后清除T键状态
		else
			T_keystate = false;

		//根据计时器和文本显示状态值来判断当前帧是否显示文本
		if (textTimer.getElapsedTime() < sf::seconds(2) && text_drawing)
		{
			//创建一个用于显示的文本
			sf::Text textForLock;
			textForLock.setFont(catfont);
			textForLock.setFillColor(sf::Color(255, 126, 126));
			textForLock.setCharacterSize(50);
			if (is_mouse_through)
				textForLock.setString(L"窗口锁定，鼠标穿透已开启");
			else
				textForLock.setString(L"窗口解锁，鼠标穿透已关闭");
			window.draw(textForLock);
		}
		//绘制淡出动画
		else if (sf::seconds(2) < textTimer.getElapsedTime() && textTimer.getElapsedTime() < sf::milliseconds(2500) && text_drawing)
		{
			sf::Text textForLock;
			textForLock.setFont(catfont);
			textForLock.setFillColor(sf::Color(255, 126, 126, 255 * ((sf::milliseconds(2500) - textTimer.getElapsedTime()) / sf::milliseconds(500))));
			textForLock.setCharacterSize(50);
			if (is_mouse_through)
				textForLock.setString(L"窗口锁定，鼠标穿透已开启");
			else
				textForLock.setString(L"窗口解锁，鼠标穿透已关闭");
			window.draw(textForLock);
		}
	}

	//拖动窗口
	void moveCatPosition()
	{
		static bool bFirstCLK = false;
		static POINT OldPos, NewPos;
		static int  xOffset, yOffset;
		static RECT subrect;

		//如果左键按下，对猫咪进行移动
		if ((GetKeyState(1) & 0x8000) && (window.hasFocus()))
		{
			if (bFirstCLK == false) {
				GetCursorPos(&OldPos);
				GetWindowRect(catHwnd, &subrect);
				bFirstCLK = true;
			}
			else {

				GetCursorPos(&NewPos);
				xOffset = NewPos.x - OldPos.x;
				yOffset = NewPos.y - OldPos.y;
				//根据偏移量改变窗口位置
				MoveWindow(catHwnd, subrect.left + xOffset, subrect.top + yOffset, subrect.right - subrect.left, subrect.bottom - subrect.top, TRUE);
				UpdateWindow(catHwnd);
			}
		}
		else
			bFirstCLK = false;
	}

	//改变窗口大小
	void moveCatSize()
	{
		static bool rbFirstCLK = false;
		static POINT OldPos, NewPos;
		static int  xOffset, yOffset;
		static RECT subrect;

		//如果右键按下，对猫咪进行缩放
		if ((GetKeyState(2) & 0x8000) && (window.hasFocus()))
		{
			if (rbFirstCLK == false) {
				GetCursorPos(&OldPos);
				rbFirstCLK = true;
			}
			else {
				GetCursorPos(&NewPos);
				xOffset = NewPos.x - OldPos.x;
				yOffset = NewPos.y - OldPos.y;
				GetWindowRect(catHwnd, &subrect);
				//根据一次偏移中移动更多的轴来改变猫咪窗口
				if (xOffset * xOffset > yOffset* yOffset)
					MoveWindow(catHwnd, subrect.left, subrect.top,
						subrect.right + xOffset - subrect.left,
						(subrect.right + xOffset - subrect.left) * data::cfg["decoration"]["window_size"][1].asInt() / data::cfg["decoration"]["window_size"][0].asInt(),
						false);
				else
					MoveWindow(catHwnd, subrect.left, subrect.top,
						 (subrect.bottom + yOffset - subrect.top) * data::cfg["decoration"]["window_size"][0].asInt() / data::cfg["decoration"]["window_size"][1].asInt(),
						subrect.bottom + yOffset - subrect.top,
						false);
				OldPos.x= NewPos.x;
				OldPos.y = NewPos.y;
				UpdateWindow(catHwnd);
			}
		}
		else
			rbFirstCLK = false;
	}


	//当按下启动UI按键时
	void On_Start_UI()
	{
		//如果UI窗口已经启动或者正在启动，则无视此次请求
		if (is_UI_on == false) {
			WCHAR text[32] = { 0 };
			GetWindowText(uiHwnd, text, 32);
			//当识别到已经打开UI窗口时。将UI窗口移到顶层
			if (wcscmp(text, L"Bongo Cat Mver setting") == 0)
				SetWindowPos(uiHwnd, HWND_NOTOPMOST,1,1,1,1, SWP_NOMOVE| SWP_NOSIZE| SWP_SHOWWINDOW);
			//未识别到UI窗口时启动生成UI窗口的线程
			else
				CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UI_Start, NULL, 0, NULL);
		}
	}

	//启动UI
	DWORD WINAPI UI_Start(LPVOID lpParam)
	{
		//设置UI状态
		is_UI_on = true;

		//创建管道通信需要的句柄，并设置安全属性
		//HANDLE hpipe2UIRd, hpipe2UIWt;
		HANDLE hpipe2CATRd=NULL, hpipe2CATWt=NULL;
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = NULL;
		//创建一个单向管道
		if (!CreatePipe(&hpipe2CATRd, &hpipe2CATWt, &sa, 0))
		{
			data::error_msg("与UI建立通信管道失败","一定是哪里出了问题");
			is_UI_on = false;
			return 1;
		}
		if (!SetHandleInformation(hpipe2CATRd, HANDLE_FLAG_INHERIT, 0))
		{
			data::error_msg("与UI建立通信管道失败", "一定是哪里出了问题");
			is_UI_on = false;
			return 1;
		}
	
		//创建UI进程并传递cat窗口句柄
		PROCESS_INFORMATION pi;//pi 获取子进程的信息
		ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.hStdOutput = hpipe2CATWt; //输出由标准输出 -> 输出到管道 
		si.hStdError = hpipe2CATWt;
		si.dwFlags |= STARTF_USESTDHANDLES;

		TCHAR szAppPath[MAX_PATH] = { 0 };//用于传入cat窗口句柄
		wsprintfW(szAppPath,L"%p", catHwnd);
		if (!CreateProcess(L"BongoCatUI.exe", szAppPath, NULL, NULL, true, NULL, NULL, NULL, &si, &pi))
		{
			data::error_msg("创建UI进程的时候发生的问题", "一定是哪里出了问题");
			is_UI_on = false;
			return 1;
		};
		CloseHandle(hpipe2CATWt);

		//父进程从pipeout的读端读取数据
		char buffer[128] = { 0 };
		DWORD readBytes, dwWritten;
		memset(buffer, 0, sizeof(buffer));
		HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		bool bSuccess = ReadFile(hpipe2CATRd, buffer, 128, &readBytes, NULL);
		if (!bSuccess || readBytes == 0) data::error_msg(buffer, "从UI进程获取信息失败");

		//读取成功，将UI进程的窗口句柄字符串转为HWND格式
		else
			uiHwnd=(HWND)strtoull(buffer,NULL,16);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		is_UI_on = false;
		return 0;
	}

	//释放资源的模板函数
	template <typename T>
	void release(T list)
	{
		T currentlist;
		while (list != NULL)
		{
			currentlist = list;
			list = list->next;
			delete currentlist;
		}
	}
	template <>   // 对pkeyList型特例化
	void release(pkeyList list)
	{
		pkeyList currentlist;
		while (list != NULL)
		{
			currentlist = list;
			release(list->notelist);
			list = list->next;
			delete currentlist;
		}
	}


	//根据模式清理内存
	void clearmemory()
	{
		switch (data::cfg["mode"].asInt())
		{
		case STANDARD:
			release(standard::keyboard_key_list);
			release(standard::keyboard_sprite_list);

			release(standard::hand_key_list);
			release(standard::hand_sprite_list);
			release(standard::hand_time_list);

			release(standard::face_key_list);
			release(standard::face_sprite_list);
			release(standard::face_state_list);

			release(standard::sound_key_list);
			release(standard::sound_buffer_list);
			release(standard::sound_list);
			break;
		case KEYBOARD:
			
			release(keyboard::keyboard_key_list);
			release(keyboard::keyboard_sprite_list);

			release(keyboard::righthand_key_list);
			release(keyboard::righthand_sprite_list);
			release(keyboard::righthand_time_list);

			release(keyboard::lefthand_key_list);
			release(keyboard::lefthand_sprite_list);
			release(keyboard::lefthand_time_list);

			release(keyboard::face_key_list);
			release(keyboard::face_sprite_list);
			release(keyboard::face_state_list);

			release(keyboard::sound_key_list);
			release(keyboard::sound_buffer_list);
			release(keyboard::sound_list);
			

			break;
		case LIVE2D:
			release(live2d_98::keyboard_key_list);
			release(live2d_98::keyboard_sprite_list);

			release(live2d_98::hand_key_list);
			release(live2d_98::hand_sprite_list);
			release(live2d_98::hand_time_list);

			release(live2d_98::face_key_list);
			release(live2d_98::face_sprite_list);
			release(live2d_98::face_state_list);

			release(live2d_98::sound_key_list);
			release(live2d_98::sound_buffer_list);
			release(live2d_98::sound_list);
			break;
		default:
			break;
		}
	}

}//namespace cat_main



#endif // !_CATMAIN_H


