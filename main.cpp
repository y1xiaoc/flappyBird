#include <string>
#include <vector>
#include <sstream>
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
	if (TTF_Init() != 0) {
		throw SDL_ERROR("init ttf error: ");
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
	SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

	SDL_Rect fillScreen = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	SDL_Texture * img_background = loadTexture(getResPath("background.png"), ren);
	SDL_Texture * img_ground = loadTexture(getResPath("ground.png"), ren);
	SDL_Texture * img_pipe = loadTexture(getResPath("pipe.png"), ren);
	SDL_Texture * img_board = loadTexture(getResPath("board.png"), ren);

	SDL_Texture * img_bird1 = loadTexture(getResPath("bird.png"), ren);
	SDL_Texture * img_bird2 = loadTexture(getResPath("bird2.png"), ren);
	SDL_Texture * img_bird3 = loadTexture(getResPath("bird3.png"), ren);
	SDL_Texture * img_bird_d = loadTexture(getResPath("bird_d.png"), ren);

	Mix_Chunk * snd_flap = loadSound(getResPath("flap.wav"));
	Mix_Chunk * snd_score = loadSound(getResPath("score.wav"));
	Mix_Chunk * snd_hit = loadSound(getResPath("hit.wav"));
	Mix_Chunk * snd_bounce = loadSound(getResPath("bounce.wav"));
	Mix_Chunk * snd_final = loadSound(getResPath("final.wav"));

	TTF_Font * font = TTF_OpenFont(getResPath("Humor-Sans.ttf").c_str(), 50);
	if (font == nullptr) { throw SDL_ERROR("load font error: "); }
	
	stringstream ss;
	SDL_Texture * txt_score = nullptr;
	SDL_Texture * txt_best = nullptr;
	SDL_Texture * txt_restart = createText("press any key to restart", font, { 0,0,0,255 }, ren);
	SDL_SetTextureBlendMode(txt_restart, SDL_BLENDMODE_BLEND);
	SDL_Color WHITE = { 255,255,255,255 };
	SDL_Color BLACK = { 0,0,0,255 };
	
	bird myBird(ren, img_bird1, img_bird2, img_bird3, img_bird_d, snd_flap, snd_hit, snd_bounce);
	vector<pipe> pipeList = { pipe(ren, img_pipe, snd_score), pipe(ren, img_pipe, snd_score) };
	SDL_Event e;
	bool quit = false;
	unsigned ground_t = 0;
	unsigned dying_t = 0;

	while (!quit) {

		if (myBird.state == FLYING) {
			if (ground_t % (600 / HORIZONTAL_SPEED) == 0) {
				pipeList[0].init();
			}
			if (ground_t % (600 / HORIZONTAL_SPEED) == (600 / HORIZONTAL_SPEED / 2)) {
				pipeList[1].init();
			}

			for (auto&p : pipeList) {
				myBird.checkHit(p);
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
					if (dying_t > 20) {
						myBird.init();
						pipe::grade = 0;
						for (auto& p : pipeList) {
							p.X = -p.width;
						}
					}
				}
				else {
					if (e.key.keysym.sym == SDLK_b) {
						myBird.bounce = true;
					}
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
			int pos = (i*SCREEN_WIDTH / 20 - ground_t*HORIZONTAL_SPEED) % (SCREEN_WIDTH * 21 / 20) - SCREEN_WIDTH / 20;
			renderTexture(img_ground, ren, pos, SCREEN_HEIGHT * 7 / 8, SCREEN_WIDTH / 20, SCREEN_HEIGHT / 8);
		}

		myBird.render();

		if (myBird.state == DYING) {
			//flash white
			int alpha = dying_t > 20 ? 85 : (255 * (30 - dying_t) / 30);
			SDL_SetRenderDrawColor(ren, 255, 255, 255, alpha);
			SDL_RenderFillRect(ren, &fillScreen);
			SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);

			//board drop
			int board_y = dying_t < 25 ? -int((dying_t - 5) * (dying_t - 41))/2 : 160;
			renderTexture(img_board, ren, (SCREEN_WIDTH - BOARD_WIDTH) / 2, board_y, BOARD_WIDTH, BOARD_HEIGHT);

			//render score
			int tWidth = 0;
			int tHight = 0;
			TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

			ss << pipe::grade;
			txt_score = createText(ss.str(), font, WHITE, ren);			
			SDL_QueryTexture(txt_score, NULL, NULL, &tWidth, &tHight);
			renderTexture(txt_score, ren, (SCREEN_WIDTH - tWidth) / 2, board_y+47);
			ss.str("");

			ss << pipe::highest;
			txt_best = createText(ss.str(), font, WHITE, ren);
			SDL_QueryTexture(txt_best, NULL, NULL, &tWidth, &tHight);
			renderTexture(txt_best, ren, (SCREEN_WIDTH - tWidth) / 2, board_y + 117);
			ss.str("");

			if (dying_t > 20) {
				alpha = dying_t < 70 ? 255 * (dying_t - 20) / 50 : 255 * (1.2 + 0.8 * cos(double(dying_t - 70) / 30))/2;
				SDL_SetTextureAlphaMod(txt_restart, alpha);
				renderTexture(txt_restart, ren, 50, 400, SCREEN_WIDTH - 100, 30);
			}

			if (dying_t == 10) {
				Mix_PlayChannel(-1, snd_final, 0);
			}

			dying_t++;
		}
		else {
			TTF_SetFontStyle(font, TTF_STYLE_BOLD);
			int tWidth = 0;
			int tHight = 0;
			
			ss << pipe::grade;
			TTF_SetFontOutline(font, 1 );
			txt_score = createText(ss.str(), font, BLACK, ren);
			SDL_QueryTexture(txt_score, NULL, NULL, &tWidth, &tHight);
			renderTexture(txt_score, ren, (SCREEN_WIDTH - tWidth) / 2, 100);
			
			TTF_SetFontOutline(font, 0);
			txt_score = createText(ss.str(), font, WHITE, ren);
			//SDL_QueryTexture(txt_score, NULL, NULL, &tWidth, &tHight);
			renderTexture(txt_score, ren, (SCREEN_WIDTH - tWidth) / 2, 100);

			ss.str("");
			dying_t = 0;
			ground_t++;
		}

		SDL_RenderPresent(ren);
		SDL_Delay(1);

	}
	cleanup(txt_best, txt_restart, txt_score, font);
	cleanup(snd_final, snd_flap, snd_hit, snd_score);
	cleanup(img_background, img_bird1, img_bird2, img_bird3, img_bird_d, img_pipe, img_ground, ren, win);
	TTF_Quit();
	IMG_Quit();
	Mix_Quit();
	SDL_Quit();

	return 0;
}