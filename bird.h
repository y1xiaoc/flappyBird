#pragma once
#include <iostream>
#include "cleanup.h"
#include "base.h"
#include "pipe.h"

class bird
{
public:
	bird() = delete;
	bird(SDL_Renderer * ren, SDL_Texture * tex);;
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
	SDL_Texture * texture = nullptr;
public:
	STATE state = START;
};