#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <dinput.h>

#include <time.h>
#include <windows.h>
#include <math.h>
#include <string.h>

#include "../myUserModel.hpp"
#include <SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include "json/json.h"
//#include<atlimage.h>
//#include <assert.h>
#include <dwmapi.h>
#pragma comment (lib, "dwmapi.lib")


//定义全局宏
#define MAX_KEYS_NUM 50
#define VERSION "Bongo Cat Mver v0.1.4"

//定义消息
#define UIWM_WRITECONFIG 0x0400+2                   //写config文件并通知cat窗口读取
#define UIWM_WRITECONFIG_AND_RELOAD_FILE 0x0400+3   //写config文件并通知cat窗口重新读取素材文件
#define CATWM_ACTIVE	0x0400+4					//向自己发送消息以保持激活状态





//公用的全局变量
extern HWND catHwnd;			//猫咪窗口句柄
extern HWND uiHwnd;				//UI窗口句柄
extern HINSTANCE hInst;			//猫咪窗口实例


extern sf::RenderWindow window;	//猫咪窗口对象
extern sf::Font catfont;		//全局的字体对象

extern int WIDTH;				//猫咪窗口的宽度
extern int HEIGHT;				//猫咪窗口的高度

extern bool is_load_success;	//todo···	
extern bool is_opengl_inited;	//告知opengl初始化情况
extern bool is_UIThread_on;		//一个用于控制同时只能启动一个设置窗口的bool值



//定义用于存放一个组合键的单键的结构，并将被用来组成一个表示一个组合键的链表。单键应理解为只有一个键的组合键
typedef struct keyNote {
	int key;
	keyNote* next;
}*pkeyNote;
//定义用于存放一个组合键的结构，并将被用来组成一个表示一个组合键的链表。
typedef struct keyList{
	int index;
	keyNote* notelist;
	keyList* next;
	bool state;	//标识该键当前的点击状态
	bool last_state;	//标识该键上一帧的点击状态
}*pkeyList;
//定义用于存放一组图片的链表结点。
typedef struct spriteList {
	int index;
	sf::Sprite sprite;
	spriteList* next;
}*pspriteList;
//定义用于存放一组计时数据的链表结点。
typedef struct timeList {
	int index;
	sf::Time time;
	timeList* next;
}*ptimeList;
//定义用于存放布尔状态的链表结点。
typedef struct stateList {
	int index;
	bool state;
	stateList* next;
}*pstateList;
//定义用于存放音频缓存的链表结点
typedef struct soundBufferList {
	int index;
	sf::SoundBuffer soundbuffer;
	soundBufferList* next;
}*psoundBufferList;
typedef struct soundList {
	int index;
	sf::Sound sound;
	soundList* next;
}*psoundList;
//定义用于播放音频的循环队列
typedef struct soundqueue {
	sf::Sound sound;
	int index;
	soundqueue* next;
}*pSoundqueue;




extern pSoundqueue soundplayqueue;//一个全局的音频队列

//主DInput COM对象
extern LPDIRECTINPUT8 lpdi ;
//键盘设备
extern LPDIRECTINPUTDEVICE8 lpdimouse;
//获取的鼠标数据放在此结构中
extern DIMOUSESTATE mouse_state_d7;
//鼠标坐标
extern POINT point;









namespace data
{
extern Json::Value cfg;

void error_msg(std::string error, std::string title);

bool init();

sf::Texture &load_texture(std::string path);
sf::Texture &load_texture2(std::string path);
}; // namespace data


namespace standard
{
	bool init();

	void draw();

	//暴露用于release的指针
	extern pkeyList keyboard_key_list;			//键盘特效的组合键链表
	extern pspriteList keyboard_sprite_list;	//键盘特效的图片链表

	extern pkeyList hand_key_list;			//手的组合键链表
	extern pspriteList hand_sprite_list;	//手sprite的链表
	extern ptimeList hand_time_list;		//手的计时器链表

	extern pkeyList face_key_list;				//表情的组合键链表
	extern pspriteList face_sprite_list;		//表情sprite的链表
	extern pstateList face_state_list;			//记录表情状态的链表

	extern pkeyList sound_key_list;				//音频的组合键链表
	extern psoundBufferList sound_buffer_list;	//音频的缓冲链表
	extern psoundList sound_list;				//音频对象的链表	用于重新播放音频
}; // namespace standard


namespace keyboard
{
	bool init();

	void draw();

	extern sf::SoundBuffer soundBuffer[50];

	//暴露用于release的指针
	extern pkeyList keyboard_key_list;			//键盘特效的组合键链表
	extern pspriteList keyboard_sprite_list;	//键盘特效的图片链表

	extern pkeyList righthand_key_list;			//右手的组合键链表
	extern pspriteList righthand_sprite_list;	//右手sprite的链表
	extern ptimeList righthand_time_list;		//右手的计时器链表

	extern pkeyList  lefthand_key_list;			//左手的组合键链表
	extern pspriteList lefthand_sprite_list;	//左手sprite的链表
	extern ptimeList lefthand_time_list;		//左手的计时器链表

	extern pkeyList face_key_list;				//表情的组合键链表
	extern pspriteList face_sprite_list;		//表情sprite的链表
	extern pstateList face_state_list;			//记录表情状态的链表

	extern pkeyList sound_key_list;				//音频的组合键链表
	extern psoundBufferList sound_buffer_list;	//音频的缓冲链表
	extern psoundList sound_list;				//音频对象的链表	用于重新播放音频

}; // namespace keyboard


namespace live2d_98
{
	bool init();

	void draw();

	extern myUserModel* myModel;

	//暴露用于release的指针
	extern pkeyList keyboard_key_list;			//键盘特效的组合键链表
	extern pspriteList keyboard_sprite_list;	//键盘特效的图片链表

	extern pkeyList hand_key_list;			//手的组合键链表
	extern pspriteList hand_sprite_list;	//手sprite的链表
	extern ptimeList hand_time_list;		//手的计时器链表

	extern pkeyList face_key_list;				//表情的组合键链表
	extern pspriteList face_sprite_list;		//表情sprite的链表
	extern pstateList face_state_list;			//记录表情状态的链表

	extern pkeyList sound_key_list;				//音频的组合键链表
	extern psoundBufferList sound_buffer_list;	//音频的缓冲链表
	extern psoundList sound_list;				//音频对象的链表	用于重新播放音频
}; // namespace live2d_standard



























//因为live2d的logo不适合交给用户处理，所以应放在资源文件
//一个用于从资源中加载png文件的函数
/*CImage* LoadCImage(UINT nID, LPCTSTR lpszType, HINSTANCE hInstance)
{
	CImage* pImage = NULL;
	// 兼容bmp的加载
	if (RT_BITMAP == lpszType)
	{
		pImage = new CImage();
		pImage->LoadFromResource(hInstance, nID);
		if (!pImage->IsNull())
		{
			return pImage;
		}
		else
		{
			delete pImage;
			pImage = NULL;
			return pImage;
		}
	}
	CString strLog;
	HRSRC hRsrc = ::FindResource(hInstance, MAKEINTRESOURCE(nID), lpszType);
	assert(hRsrc != NULL);
	if (NULL == hRsrc)
	{
		return NULL;
	}
	DWORD dwSize = ::SizeofResource(hInstance, hRsrc);
	LPBYTE lpRsrc = (LPBYTE)::LoadResource(hInstance, hRsrc);
	assert(lpRsrc != NULL);
	if (NULL == hRsrc)
	{
		return NULL;
	}
	// 后面采用流加载的方式使用到了CreateStreamOnHGlobal，它需要使用HGLOBAL内存
	HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, dwSize);
	if (NULL == hMem)
	{
		::FreeResource(lpRsrc);
		return NULL;
	}
	LPBYTE pMem = (LPBYTE)::GlobalLock(hMem);
	if (NULL == pMem)
	{
		::GlobalUnlock(hMem);
		::GlobalFree(hMem);
		::FreeResource(lpRsrc);
		return NULL;
	}
	memcpy(pMem, lpRsrc, dwSize);
	IStream* pStream = NULL;
	HRESULT hr = ::CreateStreamOnHGlobal(hMem, FALSE, &pStream);
	if (pStream != NULL && hr == S_OK)
	{
		pImage = new CImage();
		HRESULT hrs = pImage->Load(pStream);
		pStream->Release();
		// 释放资源
		::GlobalUnlock(hMem);
		::GlobalFree(hMem);
		::FreeResource(lpRsrc);
		if (hrs == S_OK)
		{
			// 处理图片中的透明效果
			if (pImage->GetBPP() == 32)
			{
				for (int i = 0; i < pImage->GetWidth(); i++)
				{
					for (int j = 0; j < pImage->GetHeight(); j++)
					{
						unsigned char* pucColor = reinterpret_cast<unsigned char*>(pImage->GetPixelAddress(i, j));
						pucColor[0] = pucColor[0] * pucColor[3] / 255;
						pucColor[1] = pucColor[1] * pucColor[3] / 255;
						pucColor[2] = pucColor[2] * pucColor[3] / 255;
					}
				}
			}
			return pImage;
		}
		else
		{
			delete pImage;
			pImage = NULL;
			return pImage;
		}
	}
	else
	{   // 释放资源
		::GlobalUnlock(hMem);
		::GlobalFree(hMem);
		::FreeResource(lpRsrc);
		return NULL;
	}
}
*/