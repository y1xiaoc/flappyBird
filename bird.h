#pragma once
#include <iostream>
#include "cleanup.h"
#include "base.h"
#include "pipe.h"

class bird //鸟类，定义了鸟的特征和行为
{
public:
	bird() = delete;
    //构造函数，参数是渲染器、飞行状态下的三种贴图、死亡状态下的贴图
	bird(SDL_Renderer * ren, SDL_Texture * tex1, SDL_Texture * tex2, SDL_Texture * tex3, SDL_Texture * tex_d);
	void render();//渲染出鸟的图像的函数
	void init();
	void flap();
	void fall();
	bool checkHit(const pipe& p);
	bool checkHitBounce(const pipe& p);

private:
	int x = INIT_POS_X;
	int y = INIT_POS_Y;
	int r = BIRD_HIT_RAD;
	double vx = HORIZONTAL_SPEED;
	double vy = 0;
	double angle = 0;
	SDL_Renderer * renderer = nullptr;
	vector<SDL_Texture*> texList = { nullptr, nullptr, nullptr, nullptr };
public:
	STATE state = START;
};
