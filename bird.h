#pragma once
#include "cleanup.h"
#include "base.h"
#include "pipe.h"

class bird //鸟类，定义了鸟的特征和行为
{
public:
	bird() = delete;
    //构造函数，参数是渲染器、飞行状态下的三种贴图、死亡状态下的贴图、鸟flap时的音效、鸟碰撞时的音效
	bird(SDL_Renderer * ren, SDL_Texture * tex1, SDL_Texture * tex2, SDL_Texture * tex3, SDL_Texture * tex_d, Mix_Chunk * flap, Mix_Chunk * hit);
    void render();//渲染出鸟的图像
	void init();  //设置鸟的初始状态
	void flap();  //每次flap时改变鸟状态
	void fall();  //鸟自由下落时的运动状态
	bool checkHit(const pipe& p); //检查是否碰到柱子
	bool checkHitBounce(const pipe& p); //在Bounce模式下，撞到柱子上下表面可以反弹，撞到柱子侧面才算死

private:
	int x = INIT_POS_X; //鸟的初始横坐标
	int y = INIT_POS_Y; //鸟的初始纵坐标
	int r = BIRD_HIT_RAD;//用于判断鸟是否碰撞的圆半径
	double vx = HORIZONTAL_SPEED;//鸟相对于柱子水平移动的速度（屏幕显示的效果是柱子水平移动而鸟水平静止）
	double vy = 0; //鸟的竖直方向速度，默认值为0
	double angle = 0; //鸟的图像倾斜的角度，默认值为0
	SDL_Renderer * renderer = nullptr; //渲染器
	vector<SDL_Texture*> texList = { nullptr, nullptr, nullptr, nullptr };//一个列表，包含了鸟4种状态下的贴图
	Mix_Chunk * chk_flap = nullptr;//鸟flap时的音效
	Mix_Chunk * chk_hit = nullptr; //鸟碰撞时的音效
public:
	STATE state = START; //表明鸟状态的枚举变量，默认为START
};
