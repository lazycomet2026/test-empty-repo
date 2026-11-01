#pragma once

#include "catfunc.h"
using namespace std;



//建立一个数量为N的音频循环队列
pSoundqueue creatSoundQueue(int n)
{
	//创建第一个节点
	pSoundqueue queuesoundplay = new soundqueue;
	queuesoundplay->index = 0;
	//保存第一个节点
	pSoundqueue first = queuesoundplay;
	for (int i = 1; i < n; i++)
	{
		queuesoundplay->next = new soundqueue;
		queuesoundplay = queuesoundplay->next;
		queuesoundplay->index = i;
	}
	//将最后一个节点并链接到队列开始
	queuesoundplay->next = first;
	//返回头节点
	return first;
}

//对索引为index的按键进行读取
pkeyList read_key_from_cfg(string index)
{
	pkeyList pkeylistcurrent = NULL;
	pkeyList key_list = NULL;
	if (!data::cfg[index][0].isNull())
	{
		pkeylistcurrent = new keyList;
		key_list = pkeylistcurrent;

		for (int i = 0; !data::cfg[index][i].isNull(); i++)
		{
			pkeylistcurrent->index = i;
			pkeylistcurrent->notelist = NULL;

			//创建组合键链表
			if (!data::cfg[index][i][0].isNull())
			{
				//创建第一个单键节点
				pkeylistcurrent->notelist = new keyNote;
				pkeylistcurrent->notelist->key = data::cfg[index][i][0].asInt();
				pkeylistcurrent->notelist->next = NULL;

				//创建一个对组合键链表进行创建用的临时指针
				pkeyNote current_note = pkeylistcurrent->notelist;
				for (int j = 1; !data::cfg[index][i][j].isNull(); j++)
				{
					current_note->next = new keyNote;
					current_note = current_note->next;
					current_note->key = data::cfg[index][i][j].asInt();
				}
				current_note->next = NULL;
			}
			if (!data::cfg[index][i + 1].isNull())
			{
				pkeylistcurrent->next = new keyList;
				pkeylistcurrent = pkeylistcurrent->next;
			}
		}
		if (pkeylistcurrent)
			pkeylistcurrent->next = NULL;
	}
	return key_list;
}
//对索引为index,index2的按键进行读取
pkeyList read_key_from_cfg(string index, string index2)
{
	pkeyList pkeylistcurrent = NULL;
	pkeyList key_list = NULL;
	if (!data::cfg[index][index2][0].isNull())
	{
		pkeylistcurrent = new keyList;
		key_list = pkeylistcurrent;

		for (int i = 0; !data::cfg[index][index2][i].isNull(); i++)
		{
			pkeylistcurrent->index = i;
			pkeylistcurrent->notelist = NULL;
			//创建组合键链表
			if (!data::cfg[index][index2][i][0].isNull())
			{
				//创建第一个单键节点
				pkeylistcurrent->notelist = new keyNote;
				pkeylistcurrent->notelist->key = data::cfg[index][index2][i][0].asInt();
				pkeylistcurrent->notelist->next = NULL;

				//创建一个对组合键链表进行创建用的临时指针
				pkeyNote current_note = pkeylistcurrent->notelist;
				for (int j = 1; !data::cfg[index][index2][i][j].isNull(); j++)
				{
					current_note->next = new keyNote;
					current_note = current_note->next;
					current_note->key = data::cfg[index][index2][i][j].asInt();
				}
				current_note->next = NULL;
			}
			if (!data::cfg[index][index2][i + 1].isNull())
			{
				pkeylistcurrent->next = new keyList;
				pkeylistcurrent = pkeylistcurrent->next;
			}
		}
		if (pkeylistcurrent)
			pkeylistcurrent->next = NULL;
	}
	return key_list;
}
//对索引为index,index2的结点进行图片读取
pspriteList read_img(string index, string index2)
{
	pspriteList pspritelistcurrent = NULL;
	pspriteList sprite_list = NULL;


	if (!data::cfg[index][index2][0].isNull())
	{
		pspritelistcurrent = new spriteList;
		pspritelistcurrent->index = 0;

		pspritelistcurrent->sprite.setTexture(data::load_texture2("img/" + index + "/" + index2 + "/0.png"));
		pspritelistcurrent->next = NULL;
		sprite_list = pspritelistcurrent;
	}
	else return NULL;
	for (int i = 1; !data::cfg[index][index2][i].isNull(); i++)
	{
		pspritelistcurrent->next = new spriteList;
		pspritelistcurrent = pspritelistcurrent->next;
		pspritelistcurrent->index = i;

		pspritelistcurrent->sprite.setTexture(data::load_texture2("img/" + index + "/" + index2 + "/" + std::to_string(i) + ".png"));
	}
	pspritelistcurrent->next = NULL;
	return sprite_list;
}

//对索引为index,index2的结点进行音频读取
psoundBufferList read_sound(string index, string index2)
{
	psoundBufferList psoundbufferlistcurrent = NULL;
	psoundBufferList soundbuffer_list = NULL;


	if (!data::cfg[index][index2][0].isNull())
	{
		psoundbufferlistcurrent = new soundBufferList;
		psoundbufferlistcurrent->index = 0;


		if (!psoundbufferlistcurrent->soundbuffer.loadFromFile(("img/" + index + "/" + index2 + "/" + std::to_string(0) + ".wav")))
			if (!psoundbufferlistcurrent->soundbuffer.loadFromFile(("img/" + index + "/" + index2 + "/" + std::to_string(0) + ".ogg")))
				if (!psoundbufferlistcurrent->soundbuffer.loadFromFile(("img/" + index + "/" + index2 + "/" + std::to_string(0) + ".flac")))
					;






		psoundbufferlistcurrent->next = NULL;
		soundbuffer_list = psoundbufferlistcurrent;
	}
	else return NULL;
	for (int i = 1; !data::cfg[index][index2][i].isNull(); i++)
	{
		psoundbufferlistcurrent->next = new soundBufferList;
		psoundbufferlistcurrent = psoundbufferlistcurrent->next;
		psoundbufferlistcurrent->index = i;

		if (!psoundbufferlistcurrent->soundbuffer.loadFromFile(("img/" + index + "/" + index2 + "/" + std::to_string(i) + ".wav")))
			if (!psoundbufferlistcurrent->soundbuffer.loadFromFile(("img/" + index + "/" + index2 + "/" + std::to_string(i) + ".ogg")))
				if (!psoundbufferlistcurrent->soundbuffer.loadFromFile(("img/" + index + "/" + index2 + "/" + std::to_string(i) + ".flac")))
					;
	}
	psoundbufferlistcurrent->next = NULL;
	return soundbuffer_list;
}



//对索引为index,index2的结点创建计时链表
ptimeList creat_timelist(string index, string index2)
{
	ptimeList ptimeListcurrent = NULL;
	ptimeList time_list = NULL;


	if (!data::cfg[index][index2][0].isNull())
	{
		ptimeListcurrent = new timeList;
		ptimeListcurrent->index = 0;
		ptimeListcurrent->time = sf::seconds(0);
		ptimeListcurrent->next = NULL;
		time_list = ptimeListcurrent;
	}
	else return NULL;

	for (int i = 1; !data::cfg[index][index2][i].isNull(); i++)
	{
		ptimeListcurrent->next = new timeList;
		ptimeListcurrent = ptimeListcurrent->next;
		ptimeListcurrent->index = i;
		ptimeListcurrent->time = sf::seconds(0);
	}
	ptimeListcurrent->next = NULL;
	return time_list;
}

//对索引为index,index2的结点创建状态链表
pstateList creat_statelist(string index, string index2)
{
	pstateList pstateListcurrent = NULL;
	pstateList state_list = NULL;


	if (!data::cfg[index][index2][0].isNull())
	{
		pstateListcurrent = new stateList;
		pstateListcurrent->index = 0;
		pstateListcurrent->state = false;
		pstateListcurrent->next = NULL;
		state_list = pstateListcurrent;
	}
	else return NULL;

	for (int i = 1; !data::cfg[index][index2][i].isNull(); i++)
	{
		pstateListcurrent->next = new stateList;
		pstateListcurrent = pstateListcurrent->next;
		pstateListcurrent->index = i;
		pstateListcurrent->state = false;
	}
	pstateListcurrent->next = NULL;
	return state_list;
}

//获取时间链表中 时间值最大的结点的索引 （表示最新按下）
int max_time(ptimeList timelist)
{
	sf::Time maxtime;
	int index = 0;
	if (timelist != NULL)
		maxtime = timelist->time;
	for (int i = 0; timelist != NULL; i++)
	{
		if (timelist->time > maxtime)
		{
			maxtime = timelist->time;
			index = i;
		};
		timelist = timelist->next;
	}
	return index;
}

//绘制手部图片，同时动作时只播放最后触发的动作
void drawhand(pkeyList pkeylistcurrent, pspriteList pspriteListcurrent, ptimeList ptimeListcurrent, sf::Clock clock, sf::Time init_time, sf::Sprite up)
{
	int hand_state = -1;
	ptimeList ptime = ptimeListcurrent; //记录指针初始数值
	//判断右手当前状态
	for (int i = 0; pkeylistcurrent != NULL; i++)
	{
		//创建一个用于临时操作的指针
		pkeyNote pkeynotecurrent = pkeylistcurrent->notelist;
		//创建一个用于存储判断结果的bool值
		bool bpress = true;
		while (pkeynotecurrent != NULL && bpress)
		{
			bpress = GetKeyState(pkeynotecurrent->key) & 0x8000;//todo:解决空值时访问冲突的bug
			pkeynotecurrent = pkeynotecurrent->next;
		}
		//如果一个组合键被按下，那么bpress=true;
		if (bpress) {
			hand_state = i;
			pkeylistcurrent->state = true;
			//如果一个键是新按下的，就保存这个键的按下时间
			if (pkeylistcurrent->last_state == false)
				ptimeListcurrent->time = clock.getElapsedTime();
		}
		else
			//否则则时间置为初始化时的时间
			ptimeListcurrent->time = init_time;
		pkeylistcurrent->last_state = pkeylistcurrent->state;
		pkeylistcurrent->state = false;
		//指针递增
		pkeylistcurrent = pkeylistcurrent->next;
		ptimeListcurrent = ptimeListcurrent->next;
	}
	if (hand_state != -1) {
		for (int i = 0; i < max_time(ptime); i++)
			pspriteListcurrent = pspriteListcurrent->next;
		window.draw(pspriteListcurrent->sprite);
	}
	else
		window.draw(up);
}
//绘制键盘特效，同时触发时所有图片都绘制
void drawkeyboard(pkeyList pkeylistcurrent, pspriteList pspriteListcurrent)
{
	//判断键盘当前状态
	for (int i = 0; pkeylistcurrent != NULL; i++)
	{
		//创建一个用于临时操作的指针
		pkeyNote pkeynotecurrent = pkeylistcurrent->notelist;
		//创建一个用于存储判断结果的bool值
		bool bpress = true;
		while (pkeynotecurrent != NULL && bpress)
		{
			bpress = GetKeyState(pkeynotecurrent->key) & 0x8000;//todo:解决空值时访问冲突的bug
			pkeynotecurrent = pkeynotecurrent->next;
		}
		//如果一个组合键被按下，那么bpress=true;
		if (bpress) {
			;
			pkeylistcurrent->state = true;
			window.draw(pspriteListcurrent->sprite);
		}
		pkeylistcurrent->last_state = pkeylistcurrent->state;
		pkeylistcurrent->state = false;
		//指针递增
		pkeylistcurrent = pkeylistcurrent->next;
		pspriteListcurrent = pspriteListcurrent->next;
	}
}

//绘制可持续的表情，有特殊的取消方式
void drawface(pkeyList pkeylistcurrent, pspriteList pspriteListcurrent, pstateList pstateListcurrent)
{
	int index = -1;
	pstateList statelist = pstateListcurrent;	//记录指针初始值
	pspriteList   spriteList = pspriteListcurrent;
	//判断按键当前状态
	for (int i = 0; pkeylistcurrent != NULL; i++)
	{
		//创建一个用于临时操作的指针
		pkeyNote pkeynotecurrent = pkeylistcurrent->notelist;
		//创建一个用于存储判断结果的bool值
		bool bpress = true;
		while (pkeynotecurrent != NULL && bpress)
		{
			bpress = GetKeyState(pkeynotecurrent->key) & 0x8000;//todo:解决空值时访问冲突的bug
			pkeynotecurrent = pkeynotecurrent->next;
		}
		//如果一个组合键被按下，那么bpress=true;
		if (bpress) {
			;
			pkeylistcurrent->state = true;
			if (pkeylistcurrent->last_state == false)
				//当一个表情按下时记录当前索引
				index = i;
		}
		pkeylistcurrent->last_state = pkeylistcurrent->state;
		pkeylistcurrent->state = false;
		//指针递增
		pkeylistcurrent = pkeylistcurrent->next;
		pspriteListcurrent = pspriteListcurrent->next;
		pstateListcurrent = pstateListcurrent->next;
	}
	//根据索引清理状态
	pstateListcurrent = statelist;
	if (index != -1)
	{
		for (int i = 0; i < index; i++)
		{
			pstateListcurrent->state = false;
			pstateListcurrent = pstateListcurrent->next;
		}
		if (pstateListcurrent->state == false)
			pstateListcurrent->state = true;
		else
			pstateListcurrent->state = false;
		pstateListcurrent = pstateListcurrent->next;
		while (pstateListcurrent != NULL)
		{
			pstateListcurrent->state = false;
			pstateListcurrent = pstateListcurrent->next;
		}
	}
	//根据状态值播放表情
	while (statelist != NULL && statelist->state == false)
	{
		statelist = statelist->next;
		spriteList = spriteList->next;
	}
	if (statelist != NULL)
	{
		window.draw(spriteList->sprite);
	}
}

//播放可单键同时播放的音频
void playsound(pkeyList pkeylistcurrent, psoundBufferList psoundBufferListcurrent, pSoundqueue psoundqueuecurrent)
{
	psoundBufferList psoundbuffer = psoundBufferListcurrent; //记录指针初始数值
	//判断音频按键当前状态
	for (int i = 0; pkeylistcurrent != NULL; i++)
	{
		//创建一个用于临时操作的指针
		pkeyNote pkeynotecurrent = pkeylistcurrent->notelist;
		//创建一个用于存储判断结果的bool值
		bool bpress = true;
		while (pkeynotecurrent != NULL && bpress)
		{
			bpress = GetKeyState(pkeynotecurrent->key) & 0x8000;//todo:解决空值时访问冲突的bug
			pkeynotecurrent = pkeynotecurrent->next;
		}
		//如果一个组合键被按下，那么bpress=true;
		if (bpress) {
			pkeylistcurrent->state = true;
			//如果一个键是新按下的，就播放这个音频

			if (pkeylistcurrent->last_state == false)
				//todo
			{

				//向后查询100位音频状态，查询不到则退出

				for (int counter = 0; psoundqueuecurrent->sound.getStatus() == sf::Sound::Playing && counter < 100; counter++)
					psoundqueuecurrent = psoundqueuecurrent->next;

				psoundqueuecurrent->sound.setBuffer(psoundBufferListcurrent->soundbuffer);
				psoundqueuecurrent->sound.play();
				psoundqueuecurrent = psoundqueuecurrent->next;
			}
		}
		pkeylistcurrent->last_state = pkeylistcurrent->state;
		pkeylistcurrent->state = false;
		//指针递增
		pkeylistcurrent = pkeylistcurrent->next;
		psoundBufferListcurrent = psoundBufferListcurrent->next;
	}
}

//以按下同一个键位的时候中断当前键位音频的方式进行播放
void playsound(pkeyList pkeylistcurrent, psoundList psoundcurrent)
{
	//判断音频按键当前状态
	for (int i = 0; pkeylistcurrent != NULL; i++)
	{
		//创建一个用于临时操作的指针
		pkeyNote pkeynotecurrent = pkeylistcurrent->notelist;
		//创建一个用于存储判断结果的bool值
		bool bpress = true;
		while (pkeynotecurrent != NULL && bpress)
		{
			bpress = GetKeyState(pkeynotecurrent->key) & 0x8000;//todo:解决空值时访问冲突的bug
			pkeynotecurrent = pkeynotecurrent->next;
		}
		//如果一个组合键被按下，那么bpress=true;
		if (bpress) {
			pkeylistcurrent->state = true;
			//如果一个键是新按下的，就播放这个音频
			if (pkeylistcurrent->last_state == false)
			{
				psoundcurrent->sound.play();
			}
		}
		pkeylistcurrent->last_state = pkeylistcurrent->state;
		pkeylistcurrent->state = false;
		//指针递增
		pkeylistcurrent = pkeylistcurrent->next;
		psoundcurrent = psoundcurrent->next;
	}
}


//对索引为index,index2的结点创建状态链表
psoundList creat_soundlist(string index, string index2)
{
	psoundList psoundListcurrent = NULL;
	psoundList sound_list = NULL;


	if (!data::cfg[index][index2][0].isNull())
	{
		psoundListcurrent = new soundList;
		psoundListcurrent->index = 0;
		psoundListcurrent->next = NULL;
		sound_list = psoundListcurrent;
	}
	else return NULL;

	for (int i = 1; !data::cfg[index][index2][i].isNull(); i++)
	{
		psoundListcurrent->next = new soundList;
		psoundListcurrent = psoundListcurrent->next;
		psoundListcurrent->index = i;
	}
	psoundListcurrent->next = NULL;
	return sound_list;
}

std::tuple<double, double> bezier(double ratio, std::vector<double>& points, int length)
{
	double fact[22] = { 0.001, 0.001, 0.002, 0.006, 0.024, 0.12, 0.72, 5.04, 40.32, 362.88, 3628.8, 39916.8, 479001.6, 6227020.8, 87178291.2, 1307674368.0, 20922789888.0, 355687428096.0, 6402373705728.0, 121645100408832.0, 2432902008176640.0, 51090942171709440.0 };
	int nn = (length / 2) - 1;
	double xx = 0;
	double yy = 0;

	for (int point = 0; point <= nn; point++)
	{
		double tmp = fact[nn] / (fact[point] * fact[nn - point]) * pow(ratio, point) * pow(1 - ratio, nn - point);
		xx += points[2 * point] * tmp;
		yy += points[2 * point + 1] * tmp;
	}
	return std::make_tuple(xx / 1000, yy / 1000);
}

//根据参数得到一个手的数组
void setrighthand(double x, double y, double* pmpos0, double* pmpos1, std::vector<double>* ppss2, double dx, double dy)
{
	// initializing pss and pss2 (kuvster's magic)
	int oof = 6;
	std::vector<double> pss = { 211.0, 159.0 };
	double dist = hypot(211 - x, 159 - y);
	double centreleft0 = 211 - 0.7237 * dist / 2;
	double centreleft1 = 159 + 0.69 * dist / 2;
	for (double i = 1; i < oof; i++)
	{
		double p0;
		double p1;
		std::vector<double> bez = { 211, 159, centreleft0, centreleft1, x, y };
		std::tie(p0, p1) = bezier(i / oof, bez, 6);
		pss.push_back(p0);
		pss.push_back(p1);
	}
	pss.push_back(x);
	pss.push_back(y);
	double a = y - centreleft1;
	double b = centreleft0 - x;
	double le = hypot(a, b);
	a = x + a / le * 60;
	b = y + b / le * 60;
	int a1 = 258;
	int a2 = 228;
	dist = hypot(a1 - a, a2 - b);
	double centreright0 = a1 - 0.6 * dist / 2;
	double centreright1 = a2 + 0.8 * dist / 2;
	int push = 20;
	double s = x - centreleft0;
	double t = y - centreleft1;
	le = hypot(s, t);
	s *= push / le;
	t *= push / le;
	double s2 = a - centreright0;
	double t2 = b - centreright1;
	le = hypot(s2, t2);
	s2 *= push / le;
	t2 *= push / le;
	for (double i = 1; i < oof; i++)
	{
		double p0;
		double p1;
		std::vector<double> bez = { x, y, x + s, y + t, a + s2, b + t2, a, b };
		std::tie(p0, p1) = bezier(i / oof, bez, 8);
		pss.push_back(p0);
		pss.push_back(p1);
	}
	pss.push_back(a);
	pss.push_back(b);
	for (double i = oof - 1; i > 0; i--)
	{
		double p0;
		double p1;
		std::vector<double> bez = { 1.0 * a1, 1.0 * a2, centreright0, centreright1, a, b };
		std::tie(p0, p1) = bezier(i / oof, bez, 6);
		pss.push_back(p0);
		pss.push_back(p1);
	}
	pss.push_back(a1);
	pss.push_back(a2);
	*pmpos0 = (a + x) / 2 - 52 - 15;
	*pmpos1 = (b + y) / 2 - 34 + 5;


	const int iter = 25;

	*ppss2 = { pss[0] + dx, pss[1] + dy };
	for (double i = 1; i < iter; i++)
	{
		double p0;
		double p1;
		std::tie(p0, p1) = bezier(i / iter, pss, 38);
		ppss2->push_back(p0 + dx);
		ppss2->push_back(p1 + dy);
	}
	ppss2->push_back(pss[36] + dx);
	ppss2->push_back(pss[37] + dy);
}
//根据数组绘制曲线
void drawline(std::vector<double> pss2, int red_value, int green_value, int blue_value)
{

	// drawing first arm arc
	int shad = 77;
	sf::VertexArray edge(sf::TriangleStrip, 52);
	double width = 7;
	sf::CircleShape circ(width / 2);
	circ.setFillColor(sf::Color(red_value, green_value, blue_value, shad));
	circ.setPosition(pss2[0] - width / 2, pss2[1] - width / 2);

	window.draw(circ);
	for (int i = 0; i < 50; i += 2)
	{
		double vec0 = pss2[i] - pss2[i + 2];
		double vec1 = pss2[i + 1] - pss2[i + 3];
		double dist = hypot(vec0, vec1);
		edge[i].position = sf::Vector2f(pss2[i] + vec1 / dist * width / 2, pss2[i + 1] - vec0 / dist * width / 2);
		edge[i + 1].position = sf::Vector2f(pss2[i] - vec1 / dist * width / 2, pss2[i + 1] + vec0 / dist * width / 2);
		edge[i].color = sf::Color(0, 0, 0, shad);
		edge[i + 1].color = sf::Color(0, 0, 0, shad);
		width -= 0.08;
	}
	double vec0 = pss2[50] - pss2[48];
	double vec1 = pss2[51] - pss2[49];
	double dist = hypot(vec0, vec1);
	edge[51].position = sf::Vector2f(pss2[50] + vec1 / dist * width / 2, pss2[51] - vec0 / dist * width / 2);
	edge[50].position = sf::Vector2f(pss2[50] - vec1 / dist * width / 2, pss2[51] + vec0 / dist * width / 2);
	edge[50].color = sf::Color(red_value, green_value, blue_value, shad);
	edge[51].color = sf::Color(red_value, green_value, blue_value, shad);
	window.draw(edge);
	circ.setRadius(width / 2);
	circ.setPosition(pss2[50] - width / 2, pss2[51] - width / 2);
	window.draw(circ);

	// drawing second arm arc
	sf::VertexArray edge2(sf::TriangleStrip, 52);
	width = 6;
	sf::CircleShape circ2(width / 2);
	circ2.setFillColor(sf::Color(red_value, green_value, blue_value));
	circ2.setPosition(pss2[0] - width / 2, pss2[1] - width / 2);
	window.draw(circ2);
	for (int i = 0; i < 50; i += 2)
	{
		vec0 = pss2[i] - pss2[i + 2];
		vec1 = pss2[i + 1] - pss2[i + 3];
		double dist = hypot(vec0, vec1);
		edge2[i].position = sf::Vector2f(pss2[i] + vec1 / dist * width / 2, pss2[i + 1] - vec0 / dist * width / 2);
		edge2[i + 1].position = sf::Vector2f(pss2[i] - vec1 / dist * width / 2, pss2[i + 1] + vec0 / dist * width / 2);
		edge2[i].color = sf::Color(red_value, green_value, blue_value);
		edge2[i + 1].color = sf::Color(red_value, green_value, blue_value);
		width -= 0.08;
	}
	vec0 = pss2[50] - pss2[48];
	vec1 = pss2[51] - pss2[49];
	dist = hypot(vec0, vec1);
	edge2[51].position = sf::Vector2f(pss2[50] + vec1 / dist * width / 2, pss2[51] - vec0 / dist * width / 2);
	edge2[50].position = sf::Vector2f(pss2[50] - vec1 / dist * width / 2, pss2[51] + vec0 / dist * width / 2);
	edge2[50].color = sf::Color(red_value, green_value, blue_value);
	edge2[51].color = sf::Color(red_value, green_value, blue_value);
	window.draw(edge2);
	circ2.setRadius(width / 2);
	circ2.setPosition(pss2[50] - width / 2, pss2[51] - width / 2);
	window.draw(circ2);
}

//初始化鼠标设备
int DInput_Init_Mouse(void)
{
	HRESULT hr;
	if (FAILED(::DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&lpdi, NULL)))
	{
		return 0;
	}

	//创建鼠标设备
	if (lpdi->CreateDevice(GUID_SysMouse, &lpdimouse, NULL) != DI_OK)
		return(0);

	//设置鼠标协作等级，设置为后台/非独占模式
	if (lpdimouse->SetCooperativeLevel(catHwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND) != DI_OK)
		return(0);

	DIPROPDWORD     property;

	property.diph.dwSize = sizeof(DIPROPDWORD);
	property.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	property.diph.dwObj = 0;
	property.diph.dwHow = DIPH_DEVICE;
	property.dwData = 16;


	//设置数据格式,鼠标对应的数据格式为c_dfDIMouse
	if (lpdimouse->SetDataFormat(&c_dfDIMouse) != DI_OK)
		return(0);

	hr = lpdimouse->SetProperty(DIPROP_BUFFERSIZE, &property.diph);

	if FAILED(hr)
	{
		// 失败
		return FALSE;
	}


	//获取鼠标
	if (lpdimouse->Acquire() != DI_OK)
		return(0);


}
//从鼠标读取数据
int DInput_Read_Mouse(void)
{
	//读取鼠标数据
	if (lpdimouse)
	{
		if (lpdimouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state_d7) != DI_OK)
			return(0);
	}
	else
	{
		//如果没有插入鼠标，则将鼠标数据结构置为0
		memset(&mouse_state_d7, 0, sizeof(mouse_state_d7));
		return(0);
	}
	return(1);
}

//释放鼠标
void DInput_Release_Mouse(void)
{
	if (lpdimouse)
	{
		lpdimouse->Unacquire();
		lpdimouse->Release();
	}
}
//更新鼠标数据
HRESULT UpdateInputState(void)
{
	DWORD   i;

	if (lpdimouse != NULL)
	{
		DIDEVICEOBJECTDATA  didod;  // Receives buffered data
		DWORD               dwElements;
		HRESULT             hr;

		while (TRUE)
		{
			dwElements = 1;                     // 每次从缓冲区中读一个数据
			hr = lpdimouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &didod, &dwElements, 0);

			if FAILED(hr)
			{
				// 发生了一个错误
				if (hr == DIERR_INPUTLOST)
				{
					hr = lpdimouse->Acquire();    // 试图重新取回设备
					if FAILED(hr)
					{
						return S_FALSE;         // 失败
					}
				}
			}
			else
				if (dwElements == 1)
				{
					switch (didod.dwOfs)
					{
					case DIMOFS_X:              // X 轴偏移量
						// didod.dwData 里是具体偏移相对值，单位为像素
						point.x += didod.dwData;
						break;
					case DIMOFS_Y:              // Y 轴偏移量
						// didod.dwData 里是具体偏移相对值，单位为像素
						point.y += didod.dwData;
						break;
					case DIMOFS_BUTTON0:        // 0 号键（左键）状态
						// didod.dwData 里是具体状态值
						// 低字节最高位为 1 则表示按下
						// 低字节最高位为 0 表示未按下
						break;
					case DIMOFS_BUTTON1:        // 1 号键（右键）状态
						// 同上
						break;
					case DIMOFS_BUTTON2:        // 2 号键（中键）状态
						// 同上
						break;
					case DIMOFS_BUTTON3:        // 3 号键状态
						// 同上
						break;
					}
				}
				else if (dwElements == 0) break;      // 缓冲区读空
		}
	}
	return S_OK;
}