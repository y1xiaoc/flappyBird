#include <iostream>
#include <string>
#include <vector>

#include "cleanup.h"
#include "base.h"
#include "pipe.h"
#include "bird.h"

using namespace std;

int main(int, char**) {

	srand(time(NULL));

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		throw SDL_ERROR("init error: ");
	}

	SDL_Window * win = SDL_CreateWindow("Something", 300, 50, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (win == nullptr) {
		SDL_Quit();
		throw SDL_ERROR("create window error: ");
	}

	SDL_Renderer * ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr) {
		cleanup(win);
		SDL_Quit();
		throw SDL_ERROR("create renderer error: ");
	}

	SDL_Texture * background = loadTexture(getResPath("background.png"), ren);
	SDL_Texture * birdimg1 = loadTexture(getResPath("bird.png"), ren);
	SDL_Texture * birdimg2 = loadTexture(getResPath("bird2.png"), ren);
	SDL_Texture * birdimg3 = loadTexture(getResPath("bird3.png"), ren);
	SDL_Texture * birdimg_d = loadTexture(getResPath("bird_d.png"), ren);
	SDL_Texture * ground = loadTexture(getResPath("ground.png"), ren);
	SDL_Texture * pipeimg = loadTexture(getResPath("pipe.png"), ren);
	
	bird myBird(ren, birdimg1, birdimg2, birdimg3, birdimg_d);
	vector<pipe> pipeList = { pipe(ren, pipeimg), pipe(ren, pipeimg) };
	SDL_Event e;
	bool quit = false;
	unsigned t = 0;

	while (!quit) {

		if (myBird.state == FLYING) {
			if (t % (600 / HORIZONTAL_SPEED) == 0) {
				pipeList[0].init();
			}
			if (t % (600 / HORIZONTAL_SPEED) == (600 / HORIZONTAL_SPEED / 2)) {
				pipeList[1].init();
			}

			for (auto&p : pipeList) {
				if (myBird.checkHit(p)) { //to use bouncing mode, change checkHit to checkHitBounce
					cout << "hit!" << endl;
				}
			}
		}

		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN) {
				if (e.key.keysym.sym == SDLK_ESCAPE) {
					quit = true;
					break;
				}
				if (myBird.state == DYING) {
					myBird.init();
					pipe::grade = 0;
					for (auto& p : pipeList) {
						p.X = -p.width;
					}
				}
				else {
					myBird.flap();
				}
			}
		}
		myBird.fall();

		SDL_RenderClear(ren);
		renderTexture(background, ren, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT * 7 / 8);

		for (auto& p : pipeList) {
			if (myBird.state == FLYING) {
				p.move();
			}
			p.render();
		}

		for (int i = 0; i < 35; ++i) {
			int pos = (i*SCREEN_WIDTH / 20 - t*HORIZONTAL_SPEED) % (SCREEN_WIDTH * 21 / 20) - SCREEN_WIDTH / 20;
			renderTexture(ground, ren, pos, SCREEN_HEIGHT * 7 / 8, SCREEN_WIDTH / 20, SCREEN_HEIGHT / 8);
		}

		myBird.render();

		if (myBird.state == DYING) {
			;//render something;
		}
		else {
			t++;
		}

		SDL_RenderPresent(ren);
		SDL_Delay(1);

	}

	cleanup(background, birdimg1, birdimg2, birdimg3, birdimg_d, pipeimg, ground, ren, win);
	SDL_Quit();

	return 0;
}