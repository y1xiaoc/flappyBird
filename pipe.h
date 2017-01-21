#pragma once
#include "cleanup.h"
#include "base.h"

class pipe //管子类，定义了管子的特征和行为
{
public:
	pipe() = delete; //屏蔽空构造函数
	pipe(SDL_Renderer * ren, SDL_Texture * tex, Mix_Chunk* chk);//构造函数，参数是渲染器、贴图、得分音效
	void init(); //设置管子的初始状态
	void render(); //渲染出管子的图像
	void move(); //使管子水平移动

private:
	SDL_Renderer * renderer = nullptr; //渲染器
	SDL_Texture * texture = nullptr; //贴图
	Mix_Chunk * chk_score = nullptr; //得分时的音效
public:
	int width = PIPE_WIDTH; //管子宽度
	int X = -width; //管子位置的横坐标
	int Y = 0;      //管子位置的纵坐标
	int gap = GAP_HEIGHT; //上下两根管子的间距
	int vx = HORIZONTAL_SPEED; //管子水平移动的速度
	static int grade; //静态数据成员，分数
	static int highest; //静态数据成员，最高分
};
