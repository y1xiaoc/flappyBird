#pragma once
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

using namespace std;

const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 640;
const int GROUND_HEIGHT = 80 + 20;

const int BIRD_IMG_LEFT = 30;
const int BIRD_IMG_RIGHT = 25;
const int BIRD_IMG_UP = 22;
const int BIRD_IMG_DOWN = 22;
const int BIRD_HIT_RAD = 20;
const int INIT_POS_X = 200;
const int INIT_POS_Y = 300;

const int PIPE_WIDTH = 85;
const int GAP_HEIGHT = 150;

const int BOARD_WIDTH = 160;
const int BOARD_HEIGHT = 184;

const int HORIZONTAL_SPEED = 3;
const int FLAP_SPEED = 11;
const double GRAVITY_ACCELERATION = 0.7;
const double AIR_RESISTANCE = 0.001;

enum STATE { START, FLYING, DYING };

class SDL_ERROR :public runtime_error
{
public:
	SDL_ERROR();
	SDL_ERROR(const char * msg);
	SDL_ERROR(const string & msg);
};

clock_t myClock();

string getResPath(string path);

SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren);

Mix_Chunk* loadSound(const std::string &file);

SDL_Texture* createText(const std::string &message, TTF_Font * font, SDL_Color color, SDL_Renderer *renderer);

void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, int w, int h);
void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, int w, int h, double angle);
void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, int w, int h, SDL_RendererFlip);
void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y);
void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, double angle);
void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, SDL_RendererFlip);