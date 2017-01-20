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
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		throw SDL_ERROR("init img error: ");
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		throw SDL_ERROR("init mixer error: ");
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


	SDL_Texture * img_background = loadTexture(getResPath("background.png"), ren);
	SDL_Texture * img_ground = loadTexture(getResPath("ground.png"), ren);
	SDL_Texture * img_pipe = loadTexture(getResPath("pipe.png"), ren);

	SDL_Texture * img_bird1 = loadTexture(getResPath("bird.png"), ren);
	SDL_Texture * img_bird2 = loadTexture(getResPath("bird2.png"), ren);
	SDL_Texture * img_bird3 = loadTexture(getResPath("bird3.png"), ren);
	SDL_Texture * img_bird_d = loadTexture(getResPath("bird_d.png"), ren);

	Mix_Chunk * snd_flap = loadSound(getResPath("flap.wav"));
	Mix_Chunk * snd_score = loadSound(getResPath("score.wav"));
	Mix_Chunk * snd_hit = loadSound(getResPath("hit.wav"));
	Mix_Chunk * snd_final = loadSound(getResPath("final.wav"));
	
	bird myBird(ren, img_bird1, img_bird2, img_bird3, img_bird_d, snd_flap, snd_hit);
	vector<pipe> pipeList = { pipe(ren, img_pipe, snd_score), pipe(ren, img_pipe, snd_score) };
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
					Mix_PlayChannel(-1, snd_hit, 0);
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
		renderTexture(img_background, ren, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT * 7 / 8);

		for (auto& p : pipeList) {
			if (myBird.state == FLYING) {
				p.move();
			}
			p.render();
		}

		for (int i = 0; i < 35; ++i) {
			int pos = (i*SCREEN_WIDTH / 20 - t*HORIZONTAL_SPEED) % (SCREEN_WIDTH * 21 / 20) - SCREEN_WIDTH / 20;
			renderTexture(img_ground, ren, pos, SCREEN_HEIGHT * 7 / 8, SCREEN_WIDTH / 20, SCREEN_HEIGHT / 8);
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

	cleanup(snd_final, snd_flap, snd_hit, snd_score);
	cleanup(img_background, img_bird1, img_bird2, img_bird3, img_bird_d, img_pipe, img_ground, ren, win);
	IMG_Quit();
	Mix_Quit();
	SDL_Quit();

	return 0;
}