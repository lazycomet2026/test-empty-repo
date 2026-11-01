#pragma once

#include <header.hpp>

using namespace std;



//建立一个数量为N的音频循环队列
pSoundqueue creatSoundQueue(int n);
//对索引为index的按键进行读取
pkeyList read_key_from_cfg(string index);
//对索引为index,index2的按键进行读取
pkeyList read_key_from_cfg(string index, string index2);
//对索引为index,index2的结点进行图片读取
pspriteList read_img(string index, string index2);
//对索引为index,index2的结点进行音频读取
psoundBufferList read_sound(string index, string index2);
//对索引为index,index2的结点创建计时链表
ptimeList creat_timelist(string index, string index2);
//对索引为index,index2的结点创建状态链表
pstateList creat_statelist(string index, string index2);
//对索引为index,index2的结点创建状态链表
psoundList creat_soundlist(string index, string index2);

//获取时间链表中 时间值最大的结点的索引 （表示最新按下）
int max_time(ptimeList timelist);

//绘制手部图片，同时动作时只播放最后触发的动作
void drawhand(pkeyList pkeylistcurrent, pspriteList pspriteListcurrent, ptimeList ptimeListcurrent, sf::Clock clock, sf::Time init_time, sf::Sprite up);

//绘制键盘特效，同时触发时所有图片都绘制
void drawkeyboard(pkeyList pkeylistcurrent, pspriteList pspriteListcurrent);

//绘制可持续的表情，有特殊的取消方式
void drawface(pkeyList pkeylistcurrent, pspriteList pspriteListcurrent, pstateList pstateListcurrent);

//播放可单键同时播放的音频
void playsound(pkeyList pkeylistcurrent, psoundBufferList psoundBufferListcurrent, pSoundqueue psoundqueuecurrent);
//以按下同一个键位的时候中断当前键位音频的方式进行播放
void playsound(pkeyList pkeylistcurrent, psoundList psoundcurrent);



std::tuple<double, double> bezier(double ratio, std::vector<double>& points, int length);

//根据参数得到一个手的数组
void setrighthand(double x, double y, double* pmpos0, double* pmpos1, std::vector<double>* ppss2, double dx, double dy);
//根据数组绘制曲线
void drawline(std::vector<double> pss2, int red_value, int green_value, int blue_value);

//与读取鼠标有关的函数
int DInput_Init_Mouse(void);
int DInput_Read_Mouse(void);
void DInput_Release_Mouse(void);
HRESULT UpdateInputState(void);