#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include "RenderWin.hpp"


RenderWin::RenderWin(const char* title,int w,int h)
:window(nullptr), renderer(nullptr)
{
    window = SDL_CreateWindow(title , SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window , -1 , SDL_RENDERER_SOFTWARE);
};

SDL_Texture* RenderWin::Load(const char* filePath){

    SDL_Surface* surface = nullptr;
    surface = SDL_LoadBMP(filePath);
    if (surface == nullptr) {
        std::cout << "failed to load surface " << SDL_GetError() << std::endl;
    };
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);
    return texture;

};

void RenderWin::Free(){
    SDL_DestroyWindow(window);
};

void RenderWin::Clear()
{
    SDL_RenderClear(renderer);
};

void RenderWin::Render(SDL_Texture* tex)
{
    SDL_RenderCopy(renderer , tex, nullptr, nullptr);
};

void RenderWin::Display()
{
    SDL_RenderPresent(renderer);
};
