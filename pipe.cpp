#include "pipe.h"

int pipe::grade = 0; //设置分数初值为0
int pipe::highest = 0; //设置最高分初值为0

//构造函数
pipe::pipe(SDL_Renderer * ren, SDL_Texture * tex, Mix_Chunk * chk) :renderer(ren), texture(tex), chk_score(chk) {};

//设置管子初始状态的横坐标、纵坐标，把管子放在屏幕的最右边，纵坐标随机产生
void pipe::init() {
	Y = rand() % (SCREEN_HEIGHT - GROUND_HEIGHT - GAP_HEIGHT - 60) + 30;
	X = SCREEN_WIDTH + vx;
}

//渲染出管子的图像，上下各有一根管子，上面的管子翻转，管子的长度是宽度的6倍
void pipe::render() {
	renderTexture(texture, renderer, X, Y - width * 6, width, width * 6, SDL_FLIP_VERTICAL);
	renderTexture(texture, renderer, X, Y + gap, width, width * 6);
}

//使管子水平移动
void pipe::move() {
	if (X > -width) {
        //判断是否得分，如果现在鸟没有通过管子而在下一帧一定会通过管子，就判定为得分，分数、最高分相应改变，播放得分音效
		if (X > INIT_POS_X - BIRD_HIT_RAD - width && X <= INIT_POS_X - BIRD_HIT_RAD - width + vx) {
			grade += 1;
			Mix_PlayChannel(-1, chk_score, 0);
			if (grade > highest) {
				highest = grade;
			}
		}
		X -= vx; //单位时间内管子横坐标向左移动的量等于水平速度
	}
}
