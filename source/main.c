// some of this code (mostly the inilation scpits) contain some code fromb a grrlib template exsample. https://github.com/GRRLIB/GRRLIB/tree/master/examples/template

//graphics
#include <grrlib.h>
#include <stdlib.h>
//math
#include <math.h>
#include <ogc/lwp_watchdog.h> 
//input
#include <wiiuse/wpad.h>
#include "input.h"

//font
#include "Letter_Gothic_Std_14_Bold_png.h"
#include "beon_png.h"

#include "hand_png.h"
#include "discord_png.h"
#include "grrlib_logo_png.h"
#include "qr_png.h"
//#include "qr2_png.h"


#define version 1.6
#define RED 0xFF0000FF
#define WHITE 0xFFFFFFFF
#define BLUE 0x0000FFFF
#define BLACK 0x000000FF

float cursorX, cursorY;

int main() {
    //graphics library init
    GRRLIB_Init();
    GRRLIB_Settings.antialias = true;
    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);
    //controller init
    InitInput();
    //font intit
    GRRLIB_texImg *tex_font = GRRLIB_LoadTexture(Letter_Gothic_Std_14_Bold_png);
    GRRLIB_texImg *tex_beon = GRRLIB_LoadTexture(Letter_Gothic_Std_14_Bold_png);

    GRRLIB_InitTileSet(tex_font, 11, 24, 32);
    GRRLIB_InitTileSet(tex_beon, 11, 24, 32); //tileset probs wrong
    //image init
   GRRLIB_texImg *tex_cur = GRRLIB_LoadTexture(hand_png);
    GRRLIB_texImg *tex_dis = GRRLIB_LoadTexturePNG(discord_png);
    GRRLIB_texImg *tex_gr = GRRLIB_LoadTexturePNG(grrlib_logo_png);
    GRRLIB_texImg *tex_qr = GRRLIB_LoadTexturePNG(qr_png);

    while(1) {
        GRRLIB_2dMode();
        WPAD_ScanPads();
        GetIRPointer(0, &cursorX, &cursorY);
       if (cursorX >= 0 && cursorY >= 0) {
       GRRLIB_DrawImg(cursorX,cursorY,tex_cur,0,1,1,WHITE);
       }

        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break;
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_MINUS) GRRLIB_ScrShot("sd:/saiiber.png");
        GRRLIB_SetLightAmbient(0x333333FF);
        //title text 
        GRRLIB_Printf(99, 20, tex_font, RED, 5, "Saii");
        GRRLIB_Printf(315, 20, tex_font, BLUE, 5, "ber");
        //menu text
        GRRLIB_Printf(10,200, tex_font,RED, 2, "play");
        GRRLIB_Printf(10,250,tex_font,RED, 2, "settings");
        GRRLIB_Printf(10,300,tex_font RED, 2, "quick play");
        GRRLIB_Printf(10,350,tex_font, RED, 2, "credits");
        GRRLIB_Printf(10,400,tex_font, RED, 2, "exit");
        GRRLIB_Printf(32,450,tex_font, BLUE, 1, "made by guinea7pig");
        GRRLIB_Printf(315,450,tex_font, RED ,1, "made with grrlib");
        //images
        GRRLIB_DrawImg(10, 400,tex_dis,0,1,1,WHITE);
        GRRLIB_DrawImg(30, 400,tex_qr,0,1,1,WHITE);
        GRRLIB_DrawImg(315, 400 ,tex_gr,0,0.1,0.1,WHITE);    
        //debug text neeed to make togglable
        GRRLIB_Printf(0, 30, tex_font, RED, 1,"fps: %d", FPS);
        GRRLIB_Printf(500, 30, tex_font, RED, 1,"curx: %d", cursorX);
        GRRLIB_Printf(500, 50, tex_font, RED, 1,"cury %d", curesorY);
        FPS = CalculateFrameRate();
        GRRLIB_Render();
    }
    //free textures
    GRRLIB_FreeTexture(tex_font);
    GRRLIB_FreeTexture(tex_beon);
    //free imgaes
    GRRLIB_FreeTexture(tex_dis);
    GRRLIB_FreeTexture(tex_gr);
    GRRLIB_FreeTexture(tex_gr);

    //GRRLIB_FreeTexture(tex_cur)
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);
}
//this code snippet is from https://github.com/GRRLIB/GRRLIB/blob/master/examples/basic_drawing/source/main.c
  /* static u8 CalculateFrameRate(void) {
    static u8 frameCount = 0;
    static u32 lastTime;
    static u8 FPS = 0;
    const u32 currentTime = ticks_to_millisecs(gettime());

    frameCount++;
    if(currentTime - lastTime > 1000) {
        lastTime = currentTime;
        FPS = frameCount;
        frameCount = 0;
    }
    return FPS;
}
