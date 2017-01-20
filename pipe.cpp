#include "pipe.h"

int pipe::grade = 0;
int pipe::highest = 0;

pipe::pipe(SDL_Renderer * ren, SDL_Texture * tex) :renderer(ren), texture(tex) {}

void pipe::init() {
	Y = rand() % (SCREEN_HEIGHT - GROUND_HEIGHT - GAP_HEIGHT - 60) + 30;
	X = SCREEN_WIDTH + vx;
}

void pipe::render() {
	renderTexture(texture, renderer, X, Y - width * 6, width, width * 6, SDL_FLIP_VERTICAL);
	renderTexture(texture, renderer, X, Y + gap, width, width * 6);
}

void pipe::move() {
	if (X > -width) {
		if (X > INIT_POS_X - BIRD_HIT_RAD - width && X <= INIT_POS_X - BIRD_HIT_RAD - width + vx) {
			grade += 1;
			cout << grade << endl;
			if (grade > highest) {
				highest = grade;
				cout << "highest!" << endl;
			}
		}
		X -= vx;
	}
}