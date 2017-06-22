#include<SDL.h>
#include<SDL_ttf.h>
int mainOld(int argc, char *argv[]){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win=SDL_CreateWindow("Test Phase",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,800,800,SDL_WINDOW_SHOWN);
    SDL_Delay(2000);
}
