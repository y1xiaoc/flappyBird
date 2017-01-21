#pragma once
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_mixer/SDL_mixer.h>

using namespace std;


const int SCREEN_HEIGHT = 640; //设置屏幕高度
const int SCREEN_WIDTH = 480;  //设置屏幕宽度
const int GROUND_HEIGHT = 80 + 20;//设置地面高度。实际地面高度为80，为了判断鸟的碰撞要加上鸟的半径20。

const int BIRD_IMG_LEFT = 30;     //图片中鸟的左边缘与质心的距离
const int BIRD_IMG_RIGHT = 25;    //图片中鸟的右边缘与质心的距离
const int BIRD_IMG_UP = 22;       //图片中鸟的上边缘与质心的距离
const int BIRD_IMG_DOWN = 22;     //图片中鸟的下边缘与质心的距离
const int BIRD_HIT_RAD = 20;      //游戏中鸟近似为一个半径为20的圆
const int INIT_POS_X = 200;       //设置鸟的初始横坐标
const int INIT_POS_Y = 300;       //设置鸟的初始纵坐标

const int PIPE_WIDTH = 85;        //设置管子宽度
const int GAP_HEIGHT = 150;       //设置上下两个管子之间距离
const int BOARD_WIDTH = 160;
const int BOARD_HEIGHT = 184;

const int HORIZONTAL_SPEED = 3;   //设置管子水平移动的速度
const int FLAP_SPEED = 11;        //设置鸟每次flap时向上的初速度
const double GRAVITY_ACCELERATION = 0.7;//设置重力加速度
const double AIR_RESISTANCE = 0.001;//设置空气阻力系数

enum STATE { START, FLYING, DYING }; //STATE是用于决定鸟当前状态的枚举类

class SDL_ERROR :public runtime_error //SDL_ERROR是继承于runtime_error的异常类，用于显示SDL相关的错误信息
{
public:
	SDL_ERROR();
	SDL_ERROR(const char * msg);
	SDL_ERROR(const string & msg);
};

clock_t myClock();

string getResPath(string path); //用于获取资源存储位置的函数

SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren); //将图片文件生成贴图的函数

Mix_Chunk* loadSound(const std::string &file);

SDL_Texture* createText(const std::string &message, TTF_Font * font, SDL_Color color, SDL_Renderer *renderer);

//用于渲染贴图的函数，参数是贴图、渲染器、目标位置的横坐标、目标位置的纵坐标、目标的宽度、目标的高度
void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, int w, int h);

//用于渲染贴图的函数，参数是贴图、渲染器、目标位置的横坐标、目标位置的纵坐标、目标的宽度、目标的高度、目标倾斜的角度
void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, int w, int h, double angle);

//用于渲染贴图的函数，参数是贴图、渲染器、目标位置的横坐标、目标位置的纵坐标、目标的宽度、目标的高度、目标是否翻转的枚举变量
void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, int w, int h, SDL_RendererFlip);

//用于渲染贴图的函数，参数是贴图、渲染器、目标位置的横坐标、目标位置的纵坐标，目标的宽度和高度自动设置为贴图的宽度和高度
void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y);

//用于渲染贴图的函数，参数是贴图、渲染器、目标位置的横坐标、目标位置的纵坐标、目标倾斜的角度，目标的宽度和高度自动设置为贴图的宽度和高度
void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, double angle);

//用于渲染贴图的函数，参数是贴图、渲染器、目标位置的横坐标、目标位置的纵坐标、目标的宽度、目标的高度、目标是否翻转的枚举变量，目标的宽度和高度自动设置为贴图的宽度和高度
void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, SDL_RendererFlip);
