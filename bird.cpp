#include "bird.h"

//构造函数
bird::bird(SDL_Renderer * ren, SDL_Texture * tex1, SDL_Texture * tex2, SDL_Texture * tex3, SDL_Texture * tex_d, Mix_Chunk * flap, Mix_Chunk * hit, Mix_Chunk * bounce) :renderer(ren), texList({ tex1, tex2, tex3, tex_d }),chk_flap(flap), chk_hit(hit), chk_bounce(bounce) {};

//渲染出鸟的图像：如果鸟的状态是START，就把鸟放在x、y所表示的位置，并利用texList和系统时间使鸟的图像在三种飞行贴图中转换，实现扇翅膀的效果；如果鸟的状态是FLYING，myClock()后除的数减小，翅膀扇得更快；如果鸟的状态是DYING，使用死亡状态的贴图，不扇翅膀。
void bird::render() {
	if (state == START) {
		renderTexture(texList[(myClock() / 150) % 3], renderer, x - BIRD_IMG_LEFT, y - BIRD_IMG_UP,
			BIRD_IMG_LEFT + BIRD_IMG_RIGHT, BIRD_IMG_UP + BIRD_IMG_DOWN, angle);
	}
	else if (state == FLYING) {
		renderTexture(texList[(myClock() / 100) % 3], renderer, x - BIRD_IMG_LEFT, y - BIRD_IMG_UP,
			BIRD_IMG_LEFT + BIRD_IMG_RIGHT, BIRD_IMG_UP + BIRD_IMG_DOWN, angle);
	}
	else {
		renderTexture(texList[3], renderer, x - BIRD_IMG_LEFT, y - BIRD_IMG_UP,
			BIRD_IMG_LEFT + BIRD_IMG_RIGHT, BIRD_IMG_UP + BIRD_IMG_DOWN, angle);
	}
}

//设置鸟的初始状态
void bird::init() {
	x = INIT_POS_X;
	y = INIT_POS_Y;
	vx = HORIZONTAL_SPEED;
	vy = 0;
	angle = 0;
	state = START;
	bounce = false;
}

//每次flap时改变鸟状态：竖直方向上的速度变为设置好的FLAP_SPEED，根据鸟竖直方向和水平方向上的速度之比确定鸟倾斜的角度，状态变成FLYING，播放flap音效
void bird::flap() {
	vy = -FLAP_SPEED;
	angle = atan(vy / vx) * 180 / 3.1415926;
	state = FLYING;
	Mix_PlayChannel(-1, chk_flap, 0);
}

//鸟的运动状态
void bird::fall() {
	switch (state) {
	case START:  //如果鸟的状态为START，在竖直方向小幅度上下摆动
		y = INIT_POS_Y + 10 * sin(double(myClock())/250);
		break;
	case FLYING: //如果鸟的状态为FLYING，每一帧鸟竖直移动的量等于速度vy，每一帧竖直方向速度vy变化的量等于重力加速度减去空气阻力带来的速度减小量
		y += vy;
            vy += GRAVITY_ACCELERATION - vy * (abs((double)vy)) * AIR_RESISTANCE;
		if (y > SCREEN_HEIGHT - GROUND_HEIGHT) {   //如果与地面碰撞，下一帧的y为当前y关于地面的镜面反射，速度vy反向，大小减小一半。如果不是在bounce模式下，则判定为死亡，播放碰撞音效；如果在bounce模式下，播放反弹音效
			y = 2 * (SCREEN_HEIGHT - GROUND_HEIGHT) - y;
			vy = -vy * 0.5;
			if (!bounce) {
				state = DYING;
				Mix_PlayChannel(-1, chk_hit, 0);
			}
			else {
				Mix_PlayChannel(-1, chk_bounce, 0);
			}
		}
		angle = atan((vy - 10) / vx) * 180 / 3.1415926;
		if (angle < -20) { angle = -20; }
		break;
	case DYING:
		if (y < SCREEN_HEIGHT - GROUND_HEIGHT) {
			y += vy;
			if (y > SCREEN_HEIGHT - GROUND_HEIGHT) { y = SCREEN_HEIGHT - GROUND_HEIGHT; }
			vy += GRAVITY_ACCELERATION;
		}
		angle = atan(vy / (vx*0.1)) * 180 / 3.1415926;
		break;
	}

}

bool bird::checkHit(const pipe & p) {
	if (!bounce) {
		if ((x + r) < p.X || (x - r) > p.X + p.width) {
			return false;
		}
		else if (x > p.X && x < p.X + p.width) {
			if ((y - r) > p.Y && (y + r) < p.Y + p.gap) {
				return false;
			}
			else {
				if ((y - r) < p.Y) {
					y = p.Y + r;
					vy = -vy * 0.5;
					state = DYING;
					Mix_PlayChannel(-1, chk_hit, 0);
					return true;
				}
				else if ((y + r) > p.Y + p.gap) {
					y = p.Y - r + p.gap;
					vy = -vy * 0.5;
					state = DYING;
					Mix_PlayChannel(-1, chk_hit, 0);
					return true;
				}
				else {
					return false;
				}
			}
		}
		else {
			if (y > p.Y && y < p.Y + p.gap
				&& (pow(x - p.X, 2) + pow(y - p.Y, 2))>pow(r, 2)
				&& (pow(x - p.X - p.width, 2) + pow(y - p.Y, 2)) > pow(r, 2)
				&& (pow(x - p.X, 2) + pow(y - p.Y - p.gap, 2)) > pow(r, 2)
				&& (pow(x - p.X - p.width, 2) + pow(y - p.Y - p.gap, 2)) > pow(r, 2)) {
				return false;
			}
			else {
				state = DYING;
				Mix_PlayChannel(-1, chk_hit, 0);
				return true;
			}
		}
	}

	else {
		if ((x + r) < p.X || (x - r / 2) >= p.X + p.width) {
			return false;
		}
		else if (x > p.X && (x - r / 2) < p.X + p.width) {
			if ((y - r) < p.Y) {
				y = 2 * (p.Y + r) - y;
				vy = -vy * 0.8;
				Mix_PlayChannel(-1, chk_bounce, 0);
				return false;
			}
			else if ((y + r) > p.Y + p.gap) {
				y = 2 * (p.Y - r + p.gap) - y;
				vy = -vy * 0.8;
				Mix_PlayChannel(-1, chk_bounce, 0);
				return false;
			}
			else {
				return false;
			}
		}
		else {
			if (y > p.Y && y < p.Y + p.gap
				&& (pow(x - p.X, 2) + pow(y - p.Y, 2))>pow(r, 2)
				&& (pow(x - p.X - p.width, 2) + pow(y - p.Y, 2)) > pow(r, 2)
				&& (pow(x - p.X, 2) + pow(y - p.Y - p.gap, 2)) > pow(r, 2)
				&& (pow(x - p.X - p.width, 2) + pow(y - p.Y - p.gap, 2)) > pow(r, 2)) {
				return false;
			}
			else {
				state = DYING;
				Mix_PlayChannel(-1, chk_hit, 0);
				return true;
			}
		}
	}
}
