#include "header.hpp"
#include "SFML/Graphics/Texture.hpp"


#include<CubismFramework.hpp>
#include"LAppAllocator.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "myUserModel.hpp"
#include"LAppPal.hpp"
#include"../catfunc.h"




using namespace Csm;

extern void writeConfigFromUI();


namespace live2d_98
{
	//csm的全局变量
	CubismFramework::Option   _cubismOption;
	// allocator.
	LAppAllocator   _cubismAllocator;
	//live2d信息输出 Message output function.
	static   void   PrintMessage(const   Csm::csmChar* message);

	bool is_l2d_horizontal_flip;
	//模型
	myUserModel* myModel;

	Json::Value mouse_leftkey_value, mouse_rightkey_value, mouse_sidekey_value, stopemoticon_value,stopsound_value;

	int offset_x, offset_y;				//图片文件偏移值
	int hand_offset_x, hand_offset_y;	//手整体偏移值
	int horizontal, vertical;

	double scale;

	bool is_mouse, is_left_handed, is_emoticonkeep, is_workarea, is_mouse_force_move, is_soundkeep;

	sf::Sprite bg, device, mouse_left, mouse_right, mouse_side, up;
	sf::Texture atextex;				//右手纹理

	int mouse_state = 0;				//鼠标状态

	//window
	double letter_x, letter_y, s_height, s_width;

	sf::Clock key_clock;
	sf::Time init_time;

	pkeyList keyboard_key_list;			//键盘特效的组合键链表
	pspriteList keyboard_sprite_list;	//键盘特效的图片链表

	pkeyList  hand_key_list;			//手的组合键链表
	pspriteList hand_sprite_list;		//手sprite的链表
	ptimeList hand_time_list;			//手的计时器链表

	pkeyList face_key_list;				//表情的组合键链表
	pspriteList face_sprite_list;		//表情sprite的链表
	pstateList face_state_list;			//记录表情状态的链表

	pkeyList sound_key_list;			//音频的组合键链表
	psoundBufferList sound_buffer_list;	//音频的缓冲链表
	//pSoundqueue soundplayqueue;			//音频的循环队列	同时播放单键音频时用
	psoundList sound_list;				//音频对象的链表	用于重新播放音频


	//arm lines color
	int red_value;
	int green_value;
	int blue_value;

	//workarea
	int top_left;
	int right_bottom;




	bool init()
	{
		sound_list = NULL;
		while (!is_opengl_inited)
			Sleep(100);//等待openGL初始化完毕


		key_clock.restart();//初始化时间
		init_time = key_clock.getElapsedTime();



		//赋值
		red_value = data::cfg["decoration"]["armLineColor"][0].asInt();
		green_value = data::cfg["decoration"]["armLineColor"][1].asInt();
		blue_value = data::cfg["decoration"]["armLineColor"][2].asInt();

		is_emoticonkeep = data::cfg["decoration"]["emoticonKeep"].asBool();
		is_soundkeep = data::cfg["decoration"]["soundKeep"].asBool();
		stopemoticon_value = data::cfg["decoration"]["emoticonClear"];
		stopsound_value = data::cfg["decoration"]["soundClear"];

		is_mouse = data::cfg["standard"]["mouse"].asBool();
		is_mouse_force_move = data::cfg["decoration"]["mouse_force_move"].asBool();
		is_left_handed = data::cfg["decoration"]["leftHanded"].asBool();

		mouse_leftkey_value = data::cfg["standard"]["mouse_left"];
		mouse_rightkey_value = data::cfg["standard"]["mouse_right"];
		mouse_sidekey_value = data::cfg["standard"]["mouse_side"];

		hand_offset_x= data::cfg["standard"]["hand_offset"][0].asInt();
		hand_offset_y=data::cfg["standard"]["hand_offset"][1].asInt();

		if (is_mouse)
		{
			offset_x = (data::cfg["decoration"]["offsetX"])[0].asInt();
			offset_y = (data::cfg["decoration"]["offsetY"])[0].asInt();
			scale = (data::cfg["decoration"]["scalar"])[0].asDouble();
		}
		else
		{
			offset_x = (data::cfg["decoration"]["offsetX"])[1].asInt();
			offset_y = (data::cfg["decoration"]["offsetY"])[1].asInt();
			scale = (data::cfg["decoration"]["scalar"])[1].asDouble();
		}

		//获取主屏宽高
		RECT desktop;
		const HWND h_desktop = GetDesktopWindow();
		GetWindowRect(h_desktop, &desktop);
		horizontal = desktop.right;
		vertical = desktop.bottom;
		//根据workarea设定指定区域
		if (data::cfg["workarea"]["workarea"].asBool()) {
			letter_x = data::cfg["workarea"]["top_left"][0].asInt();
			letter_y = data::cfg["workarea"]["top_left"][1].asInt();
			s_width = data::cfg["workarea"]["right_bottom"][0].asInt() - letter_x;
			s_height = data::cfg["workarea"]["right_bottom"][1].asInt() - letter_y;

		}
		else
		{
			s_width = horizontal;
			s_height = vertical;
			letter_x = 0;
			letter_y = 0;
		}



		//导入sprites
		if (!atextex.loadFromFile("img/standard/arm.png"))
		{
			data::error_msg("File not successfully loaded", "Error loading files");
		}
		up.setTexture(data::load_texture("img/standard/up.png"));
		if (is_mouse)
		{
			bg.setTexture(data::load_texture("img/standard/l2dmousebg.png"));
			device.setTexture(data::load_texture("img/standard/mouse.png"), true);
			mouse_left.setTexture(data::load_texture("img/standard/mouse_left.png"), true);
			mouse_right.setTexture(data::load_texture("img/standard/mouse_right.png"), true);
			mouse_side.setTexture(data::load_texture("img/standard/mouse_side.png"), true);
		}
		else
		{
			bg.setTexture(data::load_texture("img/standard/l2dtabletbg.png"));
			device.setTexture(data::load_texture("img/standard/tablet.png"), true);
			mouse_left.setTexture(data::load_texture("img/standard/tablet_left.png"), true);
			mouse_right.setTexture(data::load_texture("img/standard/tablet_right.png"), true);
		}
		//读取键盘按键
		keyboard_key_list = read_key_from_cfg("standard", "keyboard");
		//读取键盘图片
		keyboard_sprite_list = read_img("standard", "keyboard");

		//读取手按键
		hand_key_list = read_key_from_cfg("standard", "hand");
		//读取手图片
		hand_sprite_list = read_img("standard", "hand");
		//创建手计时器链表
		hand_time_list = creat_timelist("standard", "hand");

		//读取表情按键
		face_key_list = read_key_from_cfg("standard", "face");
		//读取表情图片
		face_sprite_list = read_img("standard", "face");
		//创建表情状态链表
		face_state_list = creat_statelist("standard", "face");

		//读取音频按键
		sound_key_list = read_key_from_cfg("standard", "sounds");
		//读取音频文件
		sound_buffer_list = read_sound("standard", "sounds");
		if (!is_soundkeep)
		{
			sound_list = creat_soundlist("standard", "sounds");
			psoundBufferList psoundBuffer = sound_buffer_list;
			psoundList psound = sound_list;
			while (psound != NULL)
			{
				psound->sound.setBuffer(psoundBuffer->soundbuffer);
				psound = psound->next;
				psoundBuffer = psoundBuffer->next;
			}
		}

		device.setScale(scale, scale);
		mouse_left.setScale(scale, scale);
		mouse_right.setScale(scale, scale);
		mouse_side.setScale(scale, scale);

		GetCursorPos(&point);//初始化鼠标坐标


		return true;


	}




	void draw()
	{

		//绘制live2d模型
		LAppPal::UpdateTime();
		CubismMatrix44 projection;
		int l2dwidth = data::cfg["decoration"]["window_size"][0].asInt(), l2dheight = data::cfg["decoration"]["window_size"][1].asInt();
		double modelscale = data::cfg["decoration"]["l2d_correct"].asDouble();

		projection.Scale(modelscale * 1.0f, modelscale * static_cast<float>(l2dwidth) / static_cast<float>(l2dheight));

		const CubismMatrix44    saveProjection = projection;
		myModel->Update();
		myModel->Draw(projection);



		//绘制背景
		window.draw(bg);

		double x, y;
		if (is_mouse_force_move)
		{
			DInput_Read_Mouse();
			point.x += mouse_state_d7.lX;
			point.y += mouse_state_d7.lY;
			//UpdateInputState();
			if (point.x < letter_x)point.x = letter_x;
			if (point.x > (s_width + letter_x))point.x = s_width + letter_x;
			if (point.y < letter_y)point.y = letter_y;
			if (point.y > (s_width + letter_x))point.y = s_height + letter_y;
			double fx = ((double)point.x - letter_x) / s_width;
			if (is_left_handed)
				fx = 1 - fx;
			double fy = ((double)point.y - letter_y) / s_height;
			fx = min(fx, 1.0);
			fx = max(fx, 0.0);
			fy = min(fy, 1.0);
			fy = max(fy, 0.0);
			x = -97 * fx + 44 * fy + 184;
			y = -76 * fx - 40 * fy + 324;
			if (is_l2d_horizontal_flip)
				myModel->SetDragging(1 - 2 * fx, 1 - 2 * fy);
			else
				myModel->SetDragging(1 - 2 * fx, 1 - 2 * fy);
		}
		else
			if (GetCursorPos(&point))
			{
				double fx = ((double)point.x - letter_x) / s_width;
				if (is_left_handed)
					fx = 1 - fx;
				double fy = ((double)point.y - letter_y) / s_height;
				fx = min(fx, 1.0);
				fx = max(fx, 0.0);
				fy = min(fy, 1.0);
				fy = max(fy, 0.0);
				x = -97 * fx + 44 * fy + 184;
				y = -76 * fx - 40 * fy + 324;
				if (is_l2d_horizontal_flip)
					myModel->SetDragging(1 - 2 * fx, 1 - 2 * fy);
				else
					myModel->SetDragging(1 - 2 * fx, 1 - 2 * fy);
			}
			else { x = y = 0; }

		double mpos0, mpos1;
		double dx = -38;
		double dy = -50;
		std::vector<double>pss2;
		//计算右手的贝塞尔曲线数组
		setrighthand(x, y, &mpos0, &mpos1, &pss2, dx, dy);


		//对右手进行位置处理
		for (int i = 0; i < 26; i++)
		{
			pss2[i * 2] += hand_offset_x;
			pss2[i * 2 + 1] += hand_offset_y;
		}


		//设置坐标
		device.setPosition(mpos0 + dx + offset_x+ hand_offset_x, mpos1 + dy + offset_y+ hand_offset_y);
		mouse_left.setPosition(mpos0 + dx + offset_x+ hand_offset_x, mpos1 + dy + offset_y+ hand_offset_y);
		mouse_right.setPosition(mpos0 + dx + offset_x+ hand_offset_x, mpos1 + dy + offset_y+ hand_offset_y);
		mouse_side.setPosition(mpos0 + dx + offset_x+ hand_offset_x, mpos1 + dy + offset_y+ hand_offset_y);

		// drawing mouse
		if (is_mouse) {
			window.draw(device);
			for (Json::Value& v : mouse_leftkey_value)
				if (GetKeyState(v.asInt()) & 0x8000)
				{
					window.draw(mouse_left);
					break;
				}
			for (Json::Value& v : mouse_rightkey_value)
				if (GetKeyState(v.asInt()) & 0x8000)
				{
					window.draw(mouse_right);
					break;
				}
			for (Json::Value& v : mouse_sidekey_value)
				if (GetKeyState(v.asInt()) & 0x8000)
				{
					window.draw(mouse_side);
					break;
				}


		}

		//设置右手形状，绘制右手纹理
		sf::ConvexShape fill(26);
		for (int i = 0; i < 26; i++)
		{
			fill.setPoint(i, sf::Vector2f(pss2[i * 2], pss2[i * 2 + 1]));
		}
		fill.setTexture(&atextex);
		window.draw(fill);



		drawline(pss2, red_value, green_value, blue_value);



		//根据音频播放模式来播放音频
		if (is_soundkeep)
			playsound(sound_key_list, sound_buffer_list, soundplayqueue);
		else
			playsound(sound_key_list, sound_list);
		//如果按下了取消音频键则停止所有音频
		bool stop = true;
		for (Json::Value& v : stopsound_value)
			stop = stop && (GetKeyState(v.asInt()) & 0x8000);
		if (stop) {
			if (is_soundkeep)
				for (int i = 0; i < 50; i++)
				{
					soundplayqueue->sound.stop();
					soundplayqueue = soundplayqueue->next;
				}
			else
			{
				psoundList psound = sound_list;
				while (psound != NULL)
				{
					psound->sound.stop();
					psound = psound->next;
				}
			}
		}

		//监测并绘制键盘
		drawkeyboard(keyboard_key_list, keyboard_sprite_list);
		//监测并绘制手
		drawhand(hand_key_list, hand_sprite_list, hand_time_list, key_clock, init_time, up);

		if (is_emoticonkeep)
		{
			//如果按下了清除表情的键，则清除表情状态
			stop = true;
			for (Json::Value& v : stopemoticon_value)
				stop = stop && (GetKeyState(v.asInt()) & 0x8000);
			if (stop)
				{
					pstateList statelist = face_state_list;
					while (statelist != NULL)
					{
						statelist->state = false;
						statelist = statelist->next;
					}
				}
			//以按键取消的方式绘制表情
			drawface(face_key_list, face_sprite_list, face_state_list);
		}
		else
			//以按键特效同样的方式绘制表情
			drawkeyboard(face_key_list, face_sprite_list);



		//使用数位笔时绘制数位笔
		if (!is_mouse) {
			for (Json::Value& v : mouse_leftkey_value)
				if (GetKeyState(v.asInt()) & 0x8000)
				{
					mouse_state = 1;
					break;
				}
			for (Json::Value& v : mouse_rightkey_value)
				if (GetKeyState(v.asInt()) & 0x8000)
				{
					mouse_state = 2;
					break;
				}
			switch (mouse_state)
			{
			case 1:
				window.draw(mouse_left);
				break;
			case 2:
				window.draw(mouse_right);
				break;
			default:
				window.draw(device);
				break;
			}
			mouse_state = 0;
		}


	}



	void   PrintMessage(const   Csm::csmChar* message)
	{

		printf("%s什么嘛你射得还蛮准的", message);
		return;
	}




}; // namespace live2d_98






