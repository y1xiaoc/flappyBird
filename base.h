#pragma once
#include <string>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

using namespace std;

const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 640;
const int GROUND_HEIGHT = 80 + 20;

const int BIRD_IMG_LEFT = 35;
const int BIRD_IMG_RIGHT = 25;
const int BIRD_IMG_UP = 25;
const int BIRD_IMG_DOWN = 25;
const int BIRD_HIT_RAD = 20;
const int INIT_POS_X = 200;
const int INIT_POS_Y = 300;

const int PIPE_WIDTH = 80;
const int GAP_HEIGHT = 200;

const int HORIZONTAL_SPEED = 3;
const int FLAP_SPEED = 15;
const double GRAVITY_ACCELERATION = 0.98;

enum STATE { START, FLYING, DYING };

class SDL_ERROR :public runtime_error
{
public:
	SDL_ERROR();
	SDL_ERROR(const char * msg);
	SDL_ERROR(const string & msg);
};

string getResPath(string path);
SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren);
void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, int w, int h);
void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, int w, int h, double angle);
void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, int w, int h, SDL_RendererFlip);
void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y);
void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, double angle);
void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, SDL_RendererFlip);
