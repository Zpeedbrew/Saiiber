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
//colors
#define version 1.6
#define RED 0xFF0000FF
#define WHITE 0xFFFFFFFF
#define BLUE 0x0000FFFF
#define BLACK 0x000000FF
//font
#include "Letter_Gothic_Std_14_Bold_png.h"
//images
#include "back_png.h"
#include "hand_png.h"
int e() {
    //graphics init
    GRRLIB_Init();
    GRRLIB_InitTileSet(tex_font, 11, 24, 32);
    GRRLIB_Settings.antialias = true;
    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);
    GRRLIB_SetLightAmbient(0x333333FF);
    //input init
    InitInput();


    //loading font
    GRRLIB_texImg *tex_font = GRRLIB_LoadTexture(Letter_Gothic_Std_14_Bold_png);
    loading textures
    GRRLIB_texImg *tex_back = GRRLIB_LoadTexture(back_png);
    while(1) {
        GRRLIB_2dMode();
        WPAD_ScanPads();
        GetIRPointer(0,&cursorX, &cursorY);
       if (cursorX >= 0 && cursorY >= 0) {
       GRRLIB_DrawImg(cursorX,cursorY,tex_cur,0,1,1,WHITE);
       }
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break; //goes back to home i hope
        //title text 
        GRRLIB_Printf(215, 20, tex_font, RED, 5, "settings");
        GRRLIB_Printf(10, 70, tex_font, RED, 3, "volume,");
        // music/sound slider
        GRRLIB_Printf(10, 150, tex_font, RED, 3, "settings");
        GRRLIB_Printf(10, 200, tex_font, RED, 3, "rumble %f");//
        GRRLIB_Printf(10, 200, tex_font, RED, 3, "rumble"); //will make text etherir say on  or off
        GRRLIB_Printf(10, 400, tex_font, RED, 2, "return to home");
        //load text
        GRRLIB_DrawImg(10,20,tex_back,0,1,1,WHITE);
        GRRLIB_Render();
    }
    GRRLIB_FreeTexture(tex_font);
    GRRLIB_FreeTexture(tex_back);
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);
}
