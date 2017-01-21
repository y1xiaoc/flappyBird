#include "base.h"

using namespace std;

SDL_ERROR::SDL_ERROR() :runtime_error(strlen(SDL_GetError()) ? SDL_GetError() : "Unknown Error.") {};
SDL_ERROR::SDL_ERROR(const char * msg) :runtime_error((string(msg) + SDL_GetError()).c_str()) {};
SDL_ERROR::SDL_ERROR(const string & msg) :runtime_error(msg + SDL_GetError()) {};

clock_t myClock() {
	return clock() * 1000 / CLOCKS_PER_SEC;
}

string getResPath(string path) {
	return string(SDL_GetBasePath()) + "res\\" + path;
}

SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren) {
	SDL_Texture *texture = IMG_LoadTexture(ren, file.c_str());
	if (texture == nullptr) {
		throw SDL_ERROR("load texture error: ");
	}
	return texture;
}

Mix_Chunk* loadSound(const std::string &file) {
	Mix_Chunk *sound = Mix_LoadWAV(file.c_str());
	if (sound == nullptr) {
		throw SDL_ERROR("load sound error: ");
	}
	return sound;
}

void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, int w, int h) {
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	SDL_RenderCopy(ren, tex, NULL, &rect);
}

void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, int w, int h, double angle) {
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	SDL_RenderCopyEx(ren, tex, NULL, &rect, angle, NULL, SDL_FLIP_NONE);
}

void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, int w, int h, SDL_RendererFlip flip) {
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	SDL_RenderCopyEx(ren, tex, NULL, &rect, NULL, NULL, flip);
}

void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y) {
	int w, h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	renderTexture(tex, ren, x, y, w, h);
}

void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, double angle) {
	int w, h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	renderTexture(tex, ren, x, y, w, h, angle);
}

void renderTexture(SDL_Texture * tex, SDL_Renderer * ren, int x, int y, SDL_RendererFlip flip) {
	int w, h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	renderTexture(tex, ren, x, y, w, h, flip);
}