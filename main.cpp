#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include "RenderWin.hpp"

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) > 0)
    {
        std::cout << "Could not init SDL" << SDL_GetError() << std::endl;
    };

    RenderWin window("progressbar",640,480);
    SDL_Texture* tex = window.Load("/mnt/c/Users/ajman/images/bootlogo.bmp");
    window.Clear();
    window.Render(tex);
    window.Display();
    SDL_Delay(3000);

    

}
