#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_video.h>

class RenderWin 
{
	public:
		RenderWin(const char* title,int w,int h);
		SDL_Texture * Load(const char* filePath);
		void Free();
		void Clear();
		void Render(SDL_Texture* tex);
		void Display();

	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
};

