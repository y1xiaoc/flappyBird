#include "bird.h"

bird::bird(SDL_Renderer * ren, SDL_Texture * tex) :renderer(ren), texture(tex) {}

void bird::render() {
	renderTexture(texture, renderer, x - BIRD_IMG_LEFT, y - BIRD_IMG_UP,
		BIRD_IMG_LEFT + BIRD_IMG_RIGHT, BIRD_IMG_UP + BIRD_IMG_DOWN, angle);
}

void bird::init() {
	x = INIT_POS_X;
	y = INIT_POS_Y;
	vx = HORIZONTAL_SPEED;
	vy = 0;
	angle = 0;
	state = START;
}

void bird::flap() {
	vy = -FLAP_SPEED;
	angle = atan(vy / vx) * 180 / 3.1415926;
	state = FLYING;
}

void bird::fall() {
	switch (state) {
	case START:
		y += vy;
		vy += 0.3*(rand() % 3 - 1);
		vy *= 0.95;
		if (vy > 3) { vy = 3; }
		if (vy < -3) { vy = -3; }
		if (y > INIT_POS_Y + 30) {
			y = INIT_POS_Y + 30;
			vy = -vy;
		}
		if (y < INIT_POS_Y - 30) {
			y = INIT_POS_Y - 30;
			vy = -vy;
		}
		break;
	case FLYING:
		y += vy;
		vy += GRAVITY_ACCELERATION;
		if (y > SCREEN_HEIGHT - GROUND_HEIGHT) {
			y = SCREEN_HEIGHT - GROUND_HEIGHT;
			vy = -vy * 0.5;
		}
		angle = atan(vy / vx) * 180 / 3.1415926;
		break;
	case DYING:
		if (y < SCREEN_HEIGHT - GROUND_HEIGHT) {
			y += vy;
			vy += GRAVITY_ACCELERATION;
		}
		angle = atan(vy / (vx*0.1)) * 180 / 3.1415926;
		break;
	}

}

bool bird::checkHit(const pipe & p) {
	if ((x + r) < p.X || (x - r) > p.X + p.width) {
		return false;
	}
	else if (x > p.X && x < p.X + p.width) {
		if ((y - r) > p.Y && (y + r) < p.Y + p.gap) {
			return false;
		}
		else {
			state = DYING;
			return true;
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
			return true;
		}
	}
}

bool bird::checkHitBounce(const pipe & p) {
	if ((x + r) < p.X || (x - r / 2) > p.X + p.width) {
		return false;
	}
	else if (x > p.X && (x - r / 2) < p.X + p.width) {
		if ((y - r) < p.Y) {
			y = 2 * (p.Y + r) - y;
			vy = -vy*0.8;
			cout << "Bounce!" << endl;
			return false;
		}
		else if ((y + r) > p.Y + p.gap) {
			y = 2 * (p.Y - r + p.gap) - y;
			vy = -vy*0.8;
			cout << "Bounce!" << endl;
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
			return true;
		}
	}
}
