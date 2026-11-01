#include "header.hpp"
#include "../catfunc.h"
#include "SFML/Graphics/Texture.hpp"

namespace keyboard
{
	Json::Value stopemoticon_value,stopsound_value;

	bool is_emoticonkeep,is_soundkeep;
	sf::Sprite bg, left_up,right_up;

	sf::Clock key_clock;				//用来判断按键先后的计时器
	sf::Time init_time;

	pkeyList keyboard_key_list;			//键盘特效的组合键链表
	pspriteList keyboard_sprite_list;	//键盘特效的图片链表

	pkeyList righthand_key_list;		//右手的组合键链表
	pspriteList righthand_sprite_list;	//右手sprite的链表
	ptimeList righthand_time_list;		//右手的计时器链表

	pkeyList  lefthand_key_list;		//左手的组合键链表
	pspriteList lefthand_sprite_list;	//左手sprite的链表
	ptimeList lefthand_time_list;		//左手的计时器链表

	pkeyList face_key_list;				//表情的组合键链表
	pspriteList face_sprite_list;		//表情sprite的链表
	pstateList face_state_list;			//记录表情状态的链表

	pkeyList sound_key_list;			//音频的组合键链表
	psoundBufferList sound_buffer_list;	//音频的缓冲链表
	//pSoundqueue soundplayqueue;			//音频的循环队列	同时播放单键音频时用
	psoundList sound_list;				//音频对象的链表	用于重新播放音频


	bool init()
	{
		sound_list = NULL;

		key_clock.restart();	//初始化时间
		init_time = key_clock.getElapsedTime();

		is_emoticonkeep = data::cfg["decoration"]["emoticonKeep"].asBool();
		is_soundkeep = data::cfg["decoration"]["soundKeep"].asBool();
		stopemoticon_value = data::cfg["decoration"]["emoticonClear"];
		stopsound_value=data::cfg["decoration"]["soundClear"];


		//读取键盘按键
		keyboard_key_list = read_key_from_cfg("keyboard", "keyboard");
		//读取键盘图片
		keyboard_sprite_list= read_img("keyboard", "keyboard");

		//读取右手按键
		righthand_key_list=read_key_from_cfg("keyboard", "righthand");
		//读取右手图片
		righthand_sprite_list = read_img("keyboard", "righthand");
		//创建右手计时器链表
		righthand_time_list = creat_timelist("keyboard", "righthand");

		//读取左手按键
		lefthand_key_list = read_key_from_cfg("keyboard", "lefthand");
		//读取左手图片
		lefthand_sprite_list = read_img("keyboard", "lefthand");
		//创建左手计时器链表
		lefthand_time_list = creat_timelist("keyboard", "lefthand");

		//读取表情按键
		face_key_list= read_key_from_cfg("keyboard", "face");
		//读取表情图片
		face_sprite_list= read_img("keyboard", "face");
		//创建表情状态链表
		face_state_list = creat_statelist("keyboard", "face");

		//读取音频按键
		sound_key_list= read_key_from_cfg("keyboard", "sounds");
		//读取音频文件
		sound_buffer_list = read_sound("keyboard", "sounds");
		if(!is_soundkeep)
		{
			sound_list = creat_soundlist("keyboard", "sounds");
			psoundBufferList psoundBuffer = sound_buffer_list;
			psoundList psound = sound_list;
			while(psound !=NULL)
			{
				psound->sound.setBuffer(psoundBuffer->soundbuffer);
				psound = psound->next;
				psoundBuffer = psoundBuffer->next;
			}
		}



		//设置左右手静止动作
		left_up.setTexture(data::load_texture("img/keyboard/lefthand/leftup.png"));
		right_up.setTexture(data::load_texture("img/keyboard/righthand/rightup.png"));

		bg.setTexture(data::load_texture("img/keyboard/bg.png"));

		return true;
	}

	void draw()
	{
		window.draw(bg);

		//根据音频播放模式来播放音频
		if (is_soundkeep)
			playsound(sound_key_list,sound_buffer_list, soundplayqueue);
		else
			playsound(sound_key_list, sound_list);
		//如果按下了取消音频键则停止所有音频
		bool stop=true;
		for (Json::Value& v : stopsound_value)
			stop = stop&&(GetKeyState(v.asInt()) & 0x8000);
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
		//监测并绘制右手
		drawhand(righthand_key_list, righthand_sprite_list, righthand_time_list, key_clock, init_time, right_up);
		//监测并绘制右手
		drawhand(lefthand_key_list, lefthand_sprite_list, lefthand_time_list, key_clock, init_time, left_up);



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
	}
}; // namespace keyboard




