#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include "constants.h"
#include "controls.h"
#include "random.h"
#define MAX_HUTS_ALLOWED 6
#define MAX_HUT_HEIGHT (window_height/MAX_HUTS_ALLOWED*2)
#define touches(r1,r2) (r1.x==r2.x || r1.y==r2.y || r1.x+r1.w==r2.x+r2.w || r1.y+r1.h==r2.y+r2.h || r1.x+r1.w == r2.x || r1.x+r1.w==r2.x)
#define HIGH_SPEED 30
#define SPEED_STEP 0.5
#define GEAR_MAX_SPEED 10


enum side{LEFT,RIGHT};
enum jump{NO_JUMP,UP,DOWN};
extern short state;
extern SDL_Window* window;
extern SDL_Renderer* renderer;
SDL_Texture* road;
int window_width, window_height;
struct car{
    SDL_Texture* texture;
    SDL_Rect rect;
    SDL_Rect original_rect;;
    float distance_moved;
    float pixels_moved;
    float health;
    short brakes_on;
    short move_forward;
    short move_backward;
    short jump;
    short gear;
    float speed;
    float pixels_speed;
    float angle;
}user;

struct object{
    SDL_Texture* texture;
    SDL_Rect rect;
    SDL_Color baseColor;
    short side;
    short active;
} huts[MAX_HUTS_ALLOWED],boundary[2];

short active_huts = 0;
void loadResources(){
    road = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_UNKNOWN,SDL_TEXTUREACCESS_TARGET,window_width,window_height);
    SDL_SetRenderTarget(renderer,road);
    SDL_SetRenderDrawColor(renderer,0,0,0,0xFF);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer,0xFF,0xFF,0xFF,0xFF);
    SDL_Rect left_line = {window_width/5,0,window_width*0.03,window_height}, right_line = {4*window_width/5,0,window_width*0.03,window_height};
    SDL_RenderFillRect(renderer,&right_line);
    SDL_RenderFillRect(renderer,&left_line);
    boundary[0].rect = left_line, boundary[0].active = YES;
    boundary[1].rect = right_line, boundary[1].active = YES;
    SDL_SetRenderTarget(renderer,NULL);
    SDL_Surface* tmp = IMG_Load("images/car-yellow-design.png");
    user.texture = SDL_CreateTextureFromSurface(renderer,tmp);
    user.rect.h = window_height * 0.25;
    user.rect.w = window_width*0.10;
    user.rect.x = window_width/2;
    user.rect.y = window_height-tmp->h-10;
    user.original_rect = user.rect;
    SDL_FreeSurface(tmp);
}

void createHuts(SDL_Renderer *renderer){
    int i;
    for(i=0;i<MAX_HUTS_ALLOWED;i++){
        if(huts[i].active==YES)
            continue; //No need to tamper
        struct object tmpHut;
        tmpHut.texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_UNKNOWN,SDL_TEXTUREACCESS_TARGET,window_width/6,window_height/MAX_HUTS_ALLOWED/2); //A small texture, draw on this from left corner.
        SDL_SetRenderTarget(renderer,tmpHut.texture);
        SDL_Color color = {20+random(235),20+random(235),20+random(235),0xFF};
        SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);
        SDL_SetTextureBlendMode(tmpHut.texture,SDL_BLENDMODE_BLEND); //I don't know why I am doing this, but this does the work.
        //Width and height of base rect to rest upon, these will be same for use in this texture and in pasting in main routine both.
        tmpHut.rect.w = 50+random(window_width/6-50)-10;
        tmpHut.rect.h = 40+random(MAX_HUT_HEIGHT-40);
        short isBuilding = YES;
        int cap_height=0;
        if( tmpHut.rect.h < MAX_HUT_HEIGHT - 70){ //It's small as a hut, otherwise a building
            int i;
            for(i=cap_height=0;i<=tmpHut.rect.w/2;i++,cap_height=i%2?cap_height+1:cap_height){
                SDL_RenderDrawLine(renderer,tmpHut.rect.w/2-i,cap_height,tmpHut.rect.w/2+i,cap_height);
            }
            isBuilding = NO;
        }
        SDL_Point baseMiddle = {tmpHut.rect.w/2,cap_height}; //Middle point of TOP of base
        //Drawing base
        SDL_SetRenderDrawColor(renderer,color.r,color.g,20+random(235),0xFF);
        SDL_Rect rect = {0,baseMiddle.y,tmpHut.rect.w,tmpHut.rect.h};
        SDL_RenderFillRect(renderer,&rect);
        //Deciding position to be used when copying texture in main routine
        if(i < MAX_HUTS_ALLOWED/2){ //Left side
            tmpHut.rect.y = MAX_HUT_HEIGHT  * i + 10; //I need some padding
            tmpHut.rect.x = 2;
        }
        else{
            tmpHut.rect.y =  MAX_HUT_HEIGHT * (i - MAX_HUTS_ALLOWED/2);
            tmpHut.rect.x = window_width - tmpHut.rect.w-2;
        }


        if(isBuilding){ //This is a building, so just turn on the lights.
            SDL_Point light = {baseMiddle.x  -20+random(20), baseMiddle.y};
            do{
                SDL_SetRenderDrawColor(renderer,0xFF,0xFF,0x00,0xF1);
                SDL_Rect rect = {light.x,light.y,tmpHut.rect.w*0.25,5};
                SDL_RenderFillRect(renderer,&rect);
                light.y+=15;
            }while(light.y+15 <= MAX_HUT_HEIGHT-baseMiddle.y);
        }
        else{ //A gate for the hut
                SDL_SetRenderDrawColor(renderer,136,0,21,0xFF);
                SDL_Rect rect = {baseMiddle.x+ -20+random(20),baseMiddle.y+20,tmpHut.rect.w*0.40,tmpHut.rect.h};
                SDL_RenderFillRect(renderer,&rect);
        }
        tmpHut.active = YES;
        if(huts[i].texture!=NULL){ //Must destroy previous one
            SDL_SetRenderTarget(renderer,huts[i].texture);
            SDL_SetRenderDrawColor(renderer,0xFF,0xFF,0xFF,0x00);
            SDL_RenderFillRect(renderer,NULL);
            SDL_DestroyTexture(huts[i].texture);
            huts[i].texture = NULL;

        }
        huts[i]=tmpHut;
    }
    active_huts = i;
    SDL_SetRenderTarget(renderer,NULL);
}


void kill(struct car c){
    SDL_Texture* explosion;
    int i,len;
    char path[100] = "images/explosion/frame_";
    char* path_ext = "_delay-0.01s.gif"; //Read only
    for(len=0;path[len];len++)
        ;
    for(i=1; i<=16; i++){
        if(SDL_GetTicks()%100){ //Wait for a while
            i--;
            continue;
        }
        int len_copy = len;
        if(i<10)
            path[len_copy++] = i+'0';
        else
            path[len_copy++] = '0'+i/10,path[len_copy++]='0'+i%10;
        path[len_copy] = '\0';
        int j=0;
        while(path[len_copy++]=path_ext[j++])
            ;
        SDL_Surface* tmp_explosion = IMG_Load(path);
        explosion = SDL_CreateTextureFromSurface(renderer,tmp_explosion);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer,road,NULL,NULL);
        for(j=0;j<active_huts;j++)
            if(huts[j].active)
                SDL_RenderCopyEx(renderer,huts[j].texture,NULL,&huts[j].rect,0,NULL,SDL_FLIP_NONE);
        if(i<=8)
            SDL_RenderCopyEx(renderer,user.texture,NULL,&user.rect,user.angle,NULL,SDL_FLIP_NONE);
        SDL_RenderCopy(renderer,explosion,NULL,&c.rect);
        SDL_RenderPresent(renderer);
    }
}
void startTheGame(void){
    printf("Let's go!\n");
    randomize();
    SDL_GetWindowSize(window,&window_width,&window_height);
    loadResources();
    SDL_Event e;
    SDL_RenderClear(renderer);
    short reset_angle = 0;
    int move_time=SDL_GetTicks(),stop_time=SDL_GetTicks();
    user.angle = 0;
    user.distance_moved = 0;
    user.brakes_on = NO;
    user.move_backward = NO;
    user.move_forward = NO;
    user.speed = 10;
    user.jump = NO;
    user.health = 100;
    user.gear = 1;
    int i;
    while(1){
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT)
                return;
            if(e.type==SDL_KEYDOWN)
                switch(e.key.keysym.sym){
                case SDLK_ESCAPE:
                    for(i=0;i<active_huts;i++)
                        huts[i].active = NO;
                    return;
                case SDLK_m:
                    SDL_SetWindowFullscreen(window,0);
                    SDL_GetWindowSize(window,&window_width,&window_height);
                    break;
                case KSTEER_RIGHT:
                    if(user.rect.x+user.rect.w>=boundary[1].rect.x){
                            reset_angle=YES;
                            break;
                    }
                    if(user.angle<=3)
                        if(user.jump && user.angle<=1)
                            user.angle+=1;
                        else if(!user.jump)
                            user.angle+=1;
                    break;
                case KSTEER_LEFT:
                    if(user.rect.x<=boundary[0].rect.x+boundary[0].rect.w){
                        reset_angle=YES;
                        break;
                    }
                    if(user.angle>=-3)
                        if(user.jump && user.angle>=-1)
                                user.angle-=1;
                            else if(!user.jump)
                                user.angle-=1;
                    break;
                case KSPEED_UP:
                    if(user.speed<=user.gear*GEAR_MAX_SPEED)
                        user.speed+=SPEED_STEP;
                    break;
                case KSPEED_DOWN:
                        user.speed+=SPEED_STEP;
                    break;
                case KJUMP:
                    user.jump = !user.jump?UP:user.jump;
                    break;
                case SDLK_0:
                    printf("Health: %f\tSpeed:%f  Gear:%d\n",user.health,user.speed,user.gear);
                    break;

            }
            if(e.type==SDL_KEYUP)
                switch(e.key.keysym.sym){
                case SDLK_RIGHT:
                case SDLK_LEFT:
                    reset_angle=YES;
                break;
                case SDLK_UP:
                break;
                }
        }
        if(reset_angle)
            if(user.angle>0)
                user.angle--;
            else if(user.angle<0)
                user.angle++;
            else
                reset_angle = NO;
        //Jumping module
        if(user.jump == UP)
                user.rect.h++,user.rect.y--,user.rect.x=user.rect.y%2?user.rect.x-1:user.rect.x,user.rect.w+=1;
        if(user.rect.h >= user.original_rect.h*1.20)
            user.jump = DOWN;
        if(user.jump == DOWN && user.rect.h > user.original_rect.h){
            user.rect.h--,user.rect.y++,user.rect.x=user.rect.y%2?user.rect.x+1:user.rect.x,user.rect.w-=1;
            if(user.rect.h==user.original_rect.h)
                user.jump=NO_JUMP;
        }


        //Moving objects
        if(user.speed!=0)
            for(i=0;i<active_huts;i++){
                huts[i].rect.y+=user.speed;
                user.pixels_moved+=user.speed;
                user.distance_moved = user.pixels_moved/1000;
            }
        if(user.angle!=0)
            user.rect.x+=user.angle;

            /*
        //Gear Module
        if((int)(SDL_GetTicks() - move_time) >= 3000 && speed<=10)
            speed+=2;
        else if((int)(SDL_GetTicks() - stop_time) >= 2000 && speed>=0)
            speed-=2;
        */
        //Managing huts
        createHuts(renderer);
        for(i=0;i<active_huts;i++)
            if(huts[i].rect.y >= window_height){ //Swapping last to first for each side, 'cause houses disappear from bottom and appear from top
                struct object tmp;
                tmp = huts[i];
                tmp.active = NO;
                huts[i] = huts[i<window_height/MAX_HUT_HEIGHT?0:window_height/MAX_HUT_HEIGHT];
                huts[i<window_height/MAX_HUT_HEIGHT?0:window_height/MAX_HUT_HEIGHT] = tmp;
            }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer,road,NULL,NULL);
        SDL_RenderCopyEx(renderer,user.texture,NULL,&user.rect,user.angle,NULL,SDL_FLIP_NONE);
        for(i=0;i<active_huts;i++){
            if(huts[i].active)
                SDL_RenderCopyEx(renderer,huts[i].texture,NULL,&huts[i].rect,0,NULL,SDL_FLIP_NONE);
        }

        //Objects management
        if((user.rect.x+user.rect.w>=boundary[1].rect.x || user.rect.x<=boundary[0].rect.x+boundary[0].rect.w)){
            if(user.speed>=HIGH_SPEED){
               user.health=0;
            }
            else
                user.health--;
        }

        if(user.health<=0){
            kill(user);
            return;
        }

        SDL_RenderPresent(renderer);
    }
}
