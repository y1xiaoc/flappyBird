#pragma once
#include <iostream>
#include "cleanup.h"
#include "base.h"

class pipe
{
public:
	pipe() = delete;
	pipe(SDL_Renderer * ren, SDL_Texture * tex);
	void init();
	void render();
	void move();

private:
	SDL_Renderer * renderer = nullptr;
	SDL_Texture * texture = nullptr;
public:
	int width = PIPE_WIDTH;
	int X = -width;
	int Y = 0;
	int gap = GAP_HEIGHT;
	int vx = HORIZONTAL_SPEED;
	static int grade;
	static int highest;
};