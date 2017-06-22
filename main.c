#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<stdio.h>
#include "constants.h"

#define START_RACE              SDL_SetWindowTitle(window,"Fight Racer :: Race"); \
                                SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN); \
                                startTheGame(); \
                                SDL_SetWindowTitle(window,"Fight Racer :: Menu"); \
                                SDL_SetWindowFullscreen(window,0);


//Globals
SDL_Renderer* renderer = NULL;
enum options{START,QUIT};
short state = PAUSED;
SDL_Window* window = NULL;

extern void startTheGame();

struct detailed_texture{
    SDL_Texture* texture;
    SDL_Rect rect;
};

struct menu{
     struct detailed_texture start;
     struct detailed_texture stop;
} paused_menu;

void init(){
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
}

void create_menu(struct detailed_texture* dt,SDL_Texture* texture,int x,int y,int w,int h){
    dt->texture = texture;
    dt->rect.x = x;
    dt->rect.y = y;
    dt->rect.w= w;
    dt->rect.h = h;
}
void load_menu(){
    TTF_Font* font = TTF_OpenFont("fonts/lazy.ttf",50);
    SDL_Color textColor = {0xFF,0xFF,0x00};
    SDL_Color bgColor = {0x00,0x00,0x00};
    SDL_Surface* tmp;
    tmp = TTF_RenderText_Shaded(font,"S     T       A    R      T",textColor,bgColor);
    create_menu(&paused_menu.start,SDL_CreateTextureFromSurface(renderer,tmp),WIDTH/2 - tmp->w/2,HEIGHT/2,tmp->w,tmp->h); //Will create rect for menu and set all the attributes
    SDL_FreeSurface(tmp);
    tmp = TTF_RenderText_Shaded(font,"Q    U     I     T",textColor,bgColor);
    create_menu(&paused_menu.stop,SDL_CreateTextureFromSurface(renderer,tmp),WIDTH/2 - tmp->w/2,HEIGHT/2+100,tmp->w,tmp->h);
    SDL_FreeSurface(tmp);
}

int main(int argc, char **argv){
    init();
    window = SDL_CreateWindow("Fight Racer alpha",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT,SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Event e;
    load_menu();
    short quit = 0;
    while(!quit){
        while(SDL_PollEvent(&e)){
            switch(e.type){
                case SDL_QUIT:
                    quit = 1;
                    break;
            }
            if(state == PAUSED)
            switch(e.type){
                int x,y;
                short option_selected;
                case SDL_MOUSEMOTION:
                    SDL_GetMouseState(&x,&y);
                    if(y<=paused_menu.start.rect.y+paused_menu.start.rect.h){
                        SDL_SetTextureColorMod(paused_menu.start.texture,0xFF,0x00,0x00);
                        SDL_SetTextureColorMod(paused_menu.stop.texture,0xFF,0xFF,0x00);
                        option_selected = START;
                    }
                    else{
                        SDL_SetTextureColorMod(paused_menu.stop.texture,0xFF,0x00,0x00);
                        SDL_SetTextureColorMod(paused_menu.start.texture,0xFF,0xFF,0x00);
                        option_selected = QUIT;
                    }
                    break;
                case SDL_KEYDOWN:
                    switch(e.key.keysym.sym){
                    case SDLK_UP:
                        SDL_SetTextureColorMod(paused_menu.start.texture,0xFF,0x00,0x00);
                        SDL_SetTextureColorMod(paused_menu.stop.texture,0xFF,0xFF,0x00);
                        option_selected = START;
                        break;
                    case SDLK_DOWN:
                        SDL_SetTextureColorMod(paused_menu.stop.texture,0xFF,0x00,0x00);
                        SDL_SetTextureColorMod(paused_menu.start.texture,0xFF,0xFF,0x00);
                        option_selected = QUIT;
                        break;
                    case SDLK_RETURN:
                        switch(option_selected){
                            case START:
                                START_RACE
                                break;
                            case QUIT:
                                quit = 1;
                        }
                    }
                    break;
                    case SDL_MOUSEBUTTONDOWN:
                        switch(option_selected){
                                case START:
                                    START_RACE
                                    break;
                                case QUIT:
                                    quit = 1;
                            }
                        break;

            }
            SDL_SetRenderDrawColor(renderer,0xFF,0xFF,0xFF,0xFF);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer,paused_menu.start.texture,NULL,&paused_menu.start.rect);
            SDL_RenderCopy(renderer,paused_menu.stop.texture,NULL,&paused_menu.stop.rect);
            SDL_RenderPresent(renderer);

        }

    }

}
