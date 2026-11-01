// header.h: 标准系统包含文件的包含文件，
// 或特定于项目的包含文件
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <time.h>
#include <math.h>
#include "json.h"
#include "resource.h"

//一些全局定义
#define VERSION L"Bongo Cat Mver v0.1.4"



//全局变量
extern HWND wpfHwnd;
extern HWND hWnd;


#ifdef _DEBUG
extern HANDLE g_hOutput;
#endif // DEBUG

namespace data
{
    extern Json::Value cfg;
    void error_msg(std::string error, std::string title);
    bool init();
}
