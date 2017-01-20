#pragma once
#include <iostream>
#include "cleanup.h"
#include "base.h"
#include "pipe.h"

class bird
{
public:
	bird() = delete;
	bird(SDL_Renderer * ren, SDL_Texture * tex1, SDL_Texture * tex2, SDL_Texture * tex3, SDL_Texture * tex_d, Mix_Chunk * flap, Mix_Chunk * hit);
	void render();
	void init();
	void flap();
	void fall();
	bool checkHit(const pipe& p);
	bool checkHitBounce(const pipe& p);

private:
	int x = INIT_POS_X;
	int y = INIT_POS_Y;
	int r = BIRD_HIT_RAD;
	double vx = HORIZONTAL_SPEED;
	double vy = 0;
	double angle = 0;
	SDL_Renderer * renderer = nullptr;
	vector<SDL_Texture*> texList = { nullptr, nullptr, nullptr, nullptr };
	Mix_Chunk * chk_flap = nullptr;
	Mix_Chunk * chk_hit = nullptr;
public:
	STATE state = START;
};