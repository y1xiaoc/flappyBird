#pragma once
#include <utility>
#include "base.h"

//利用了c++11的新特性，cleanup函数可以接受可变个数的参数，递归的将这些指针指向的东西全都释放掉。
//对于每个特化情况，若是空指针则略过，否则调用SDL库中对应的函数将指针指向的对象释放。
//利用了forward来避免拷贝。

template<typename T, typename... Args>
void cleanup(T * t, Args&&... args) {
	cleanup(t);
	cleanup(std::forward<Args>(args)...);
}

template<>
inline void cleanup<SDL_Window>(SDL_Window *win){
	if (!win){
		return;
	}
	SDL_DestroyWindow(win);
}

template<>
inline void cleanup<SDL_Renderer>(SDL_Renderer *ren) {
	if (!ren) {
		return;
	}
	SDL_DestroyRenderer(ren);
}

template<>
inline void cleanup<SDL_Texture>(SDL_Texture *tex) {
	if (!tex) {
		return;
	}
	SDL_DestroyTexture(tex);
}

template<>
inline void cleanup<SDL_Surface>(SDL_Surface *surf) {
	if (!surf) {
		return;
	}
	SDL_FreeSurface(surf);
}

template<>
inline void cleanup<Mix_Chunk>(Mix_Chunk *chunk) {
	if (!chunk) {
		return;
	}
	Mix_FreeChunk(chunk);
}

template<>
inline void cleanup<TTF_Font>(TTF_Font *font) {
	if (!font) {
		return;
	}
	TTF_CloseFont(font);
}
