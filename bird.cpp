#include "bird.h"

//构造函数，传入所有所需素材的指针
bird::bird(SDL_Renderer * ren, SDL_Texture * tex1, SDL_Texture * tex2, SDL_Texture * tex3, SDL_Texture * tex_d, Mix_Chunk * flap, Mix_Chunk * hit, Mix_Chunk * bounce) :renderer(ren), texList({ tex1, tex2, tex3, tex_d }),chk_flap(flap), chk_hit(hit), chk_bounce(bounce) {};

//渲染出鸟的图像
void bird::render() {
	switch (state) { //根据当前鸟的状态决定不同的渲染行为
	case START: //如果鸟的状态是START，就把鸟放在x、y所表示的位置，并利用texList和系统时间使鸟的图像在三种飞行贴图中转换，实现扇翅膀的效果；
		renderTexture(texList[(myClock() / 150) % 3], renderer, x - BIRD_IMG_LEFT, y - BIRD_IMG_UP,
			BIRD_IMG_LEFT + BIRD_IMG_RIGHT, BIRD_IMG_UP + BIRD_IMG_DOWN, angle);
		break;
	case FLYING: //如果鸟的状态是FLYING，myClock()后除的数减小，翅膀扇得更快；
		renderTexture(texList[(myClock() / 100) % 3], renderer, x - BIRD_IMG_LEFT, y - BIRD_IMG_UP,
			BIRD_IMG_LEFT + BIRD_IMG_RIGHT, BIRD_IMG_UP + BIRD_IMG_DOWN, angle);
		break;
	case DYING: //如果鸟的状态是DYING，使用死亡状态的贴图，不扇翅膀。
		renderTexture(texList[3], renderer, x - BIRD_IMG_LEFT, y - BIRD_IMG_UP,
			BIRD_IMG_LEFT + BIRD_IMG_RIGHT, BIRD_IMG_UP + BIRD_IMG_DOWN, angle);
		break;
	}
}

//讲鸟重置为初始状态
void bird::init() {
	x = INIT_POS_X; //重置水平位置
	y = INIT_POS_Y; //重置垂直位置
	vx = HORIZONTAL_SPEED; //重置水平速度
	vy = 0; //重置垂直速度
	angle = 0; //重置角度
	state = START; //重置鸟的状态为开始阶段
	bounce = false; //重置为非弹跳模式
}

//改变鸟的速度状态，使鸟向上飞
void bird::flap() {
	vy = -FLAP_SPEED; //竖直方向上的速度变为设置好的FLAP_SPEED
	state = FLYING; //将状态设置为FLYING，START时调用就会开始游戏
	Mix_PlayChannel(-1, chk_flap, 0); //播放flap音效
}

//每帧调用，更新鸟的运动状态，并算出鸟头角度
void bird::fall() {
	switch (state) { //根据鸟的运动状态不同决定不同行为
	case START:  //如果鸟的状态为START，在竖直方向小幅度上下摆动
		y = INIT_POS_Y + 10 * sin(double(myClock())/250);
		break;
	case FLYING: //如果鸟的状态为FLYING，则模拟抛体运动
		y += vy; //每一帧鸟竖直移动的量等于速度vy
		vy += GRAVITY_ACCELERATION - vy * (abs((double)vy)) * AIR_RESISTANCE; //每一帧竖直方向速度vy变化的量等于重力减去对速度成平方关系的空气阻力
		if (y > SCREEN_HEIGHT - GROUND_HEIGHT) {   //如果与地面碰撞，则反弹
			y = 2 * (SCREEN_HEIGHT - GROUND_HEIGHT) - y; //下一帧的y为当前y关于地面的镜面反射
			vy = -vy * 0.5; //速度vy反向，大小减小一半
			if (!bounce) { //如果不是在bounce模式下，则判定为死亡
				state = DYING; //更新鸟的状态
				Mix_PlayChannel(-1, chk_hit, 0); //播放碰撞音效
			}
			else { //如果在bounce模式下，播放反弹音效
				Mix_PlayChannel(-1, chk_bounce, 0);
			}
		}
		angle = atan((vy - 10) / vx) * 180 / 3.1415926; //算出鸟头的角度，稍微向上斜
		if (angle < -20) { angle = -20; } //但是向上最高20度
		break;
	case DYING:
		if (y < SCREEN_HEIGHT - GROUND_HEIGHT) { //如果鸟的状态为DYING，直接垂直下落，落地为止
			y += vy; //垂直移动vy
			if (y > SCREEN_HEIGHT - GROUND_HEIGHT) { y = SCREEN_HEIGHT - GROUND_HEIGHT; } //不会落到地面以下
			vy += GRAVITY_ACCELERATION; //加速下落
		}
		angle = atan(vy / (vx*0.1)) * 180 / 3.1415926; //vx被乘了0.1，角度接近正负九十
		break;
	}

}

//对一根柱子检查是否碰撞，依据是否在弹跳模式下有不同反应
bool bird::checkHit(const pipe & p) {
	if (!bounce) { //若不在碰撞模式下，碰到管子就判断为死亡
		if ((x + r) < p.X || (x - r) > p.X + p.width) { //远离管子的情况，不会碰撞，直接返回
			return false;
		}
		else if (x > p.X && x < p.X + p.width) { //在管子中间
			if ((y - r) > p.Y && (y + r) < p.Y + p.gap) { //若在管子内部，不会碰撞
				return false;
			}
			else { //否则会碰撞，实现反弹效果
				if ((y - r) < p.Y) { //向上碰管子
					y = p.Y + r; //弹到管子边
					vy = -vy * 0.5; //速度反向并且减半
					state = DYING; //标记为死亡
					Mix_PlayChannel(-1, chk_hit, 0); //播放碰撞音效
					return true;
				}
				else if ((y + r) > p.Y + p.gap) { //向下碰管子
					y = p.Y - r + p.gap; //弹到下管子边
					vy = -vy * 0.5; //速度反向并减半
					state = DYING; //标记为死亡
					Mix_PlayChannel(-1, chk_hit, 0); //播放碰撞音效
					return true;
				}
				else {
					return false;
				}
			}
		}
		else { //若在管子的左右边缘
			if (y > p.Y && y < p.Y + p.gap //在间隙之间
				&& (pow(x - p.X, 2) + pow(y - p.Y, 2))>pow(r, 2)
				&& (pow(x - p.X - p.width, 2) + pow(y - p.Y, 2)) > pow(r, 2)
				&& (pow(x - p.X, 2) + pow(y - p.Y - p.gap, 2)) > pow(r, 2)
				&& (pow(x - p.X - p.width, 2) + pow(y - p.Y - p.gap, 2)) > pow(r, 2)) { //距离管子四角足够远，未碰撞
				return false;
			}
			else { //否则碰撞
				state = DYING; //标记死亡
				Mix_PlayChannel(-1, chk_hit, 0); //播放碰撞音效
				return true;
			}
		}
	}

	else { //若在弹跳模式下，碰管子上下边缘不会判定死亡，而会反弹
		if ((x + r) < p.X || (x - r / 2) >= p.X + p.width) { //远离管子，不碰撞
			return false;
		}
		else if (x > p.X && (x - r / 2) < p.X + p.width) { //管子内部，碰上下边缘反弹
			if ((y - r) < p.Y) { //向上碰管子
				y = 2 * (p.Y + r) - y; //反弹
				vy = -vy * 0.8; //速度反向并削减
				Mix_PlayChannel(-1, chk_bounce, 0); //播放反弹音效
				return false;
			}
			else if ((y + r) > p.Y + p.gap) { //向下碰管子
				y = 2 * (p.Y - r + p.gap) - y; //反弹
				vy = -vy * 0.8; //速度反向并削减
				Mix_PlayChannel(-1, chk_bounce, 0); //播放反弹音效
				return false;
			}
			else {
				return false;
			}
		}
		else { //若撞到管子的左右侧面则判断为死亡
			if (y > p.Y && y < p.Y + p.gap //间隙之间
				&& (pow(x - p.X, 2) + pow(y - p.Y, 2))>pow(r, 2)
				&& (pow(x - p.X - p.width, 2) + pow(y - p.Y, 2)) > pow(r, 2)
				&& (pow(x - p.X, 2) + pow(y - p.Y - p.gap, 2)) > pow(r, 2)
				&& (pow(x - p.X - p.width, 2) + pow(y - p.Y - p.gap, 2)) > pow(r, 2)) { //距离四个角足够远，判断为不碰撞
				return false;
			}
			else { //否则碰撞
				state = DYING; //标记死亡
				Mix_PlayChannel(-1, chk_hit, 0); //播放碰撞音效
				return true;
			}
		}
	}
}
