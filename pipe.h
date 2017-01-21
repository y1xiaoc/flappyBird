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
	SDL_Renderer * renderer = nullptr;
	SDL_Texture * texture = nullptr;
	Mix_Chunk * chk_score = nullptr;
public:
	int width = PIPE_WIDTH;
	int X = -width;
	int Y = 0;
	int gap = GAP_HEIGHT;
	int vx = HORIZONTAL_SPEED;
	static int grade;
	static int highest;
};
