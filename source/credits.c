// some of this code (mostly the inilation scpits) contain some code fromb a grrlib template exsample. https://github.com/GRRLIB/GRRLIB/tree/master/examples/template
#include <grrlib.h>
#include <stdlib.h>
#include <math.h>
#include <wiiuse/wpad.h>
//colors
#define RED 0xFF0000FF
#define BLUE 0x0000FFFF
#include "Letter_Gothic_Std_14_Bold_png.h"
#include "back_png.h"
#include "hand_png.h"
//grlib logo

int o() {
    GRRLIB_Init();
    WPAD_Init();
    GRRLIB_texImg *tex_font = GRRLIB_LoadTexture(Letter_Gothic_Std_14_Bold_png);
    GRRLIB_texImg *tex_back = GRRLIB_LoadTexture(back_png);
    GRRLIB_texImg *tex_cur = GRRLIB_LoadTexture(hand_png);
    GRRLIB_InitTileSet(tex_font, 11, 24, 32);
    GRRLIB_Settings.antialias = true;
    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);
    GRRLIB_SetLightAmbient(0x333333FF);
    while(1) {
        GRRLIB_2dMode();
        WPAD_ScanPads();
        //if(WPAD_ButtonsDown(0) & WPAD_BUTTON_PLUS) break; //goes back to home i hope
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break; //goes back to home i hope
        GRRLIB_2dMode();
        //title text 
        GRRLIB_Printf(100, 20, tex_font, RED, 5, "credits");
        GRRLIB_Printf(100, 400, tex_font, RED, 2, "return to home");
        //need to button stuff eventuly.
        GRRLIB_Printf(640, 100, tex_font, RED, 3, "devolped by Guinea7pig, Beemer,");
        GRRLIB_Printf(100, 150, tex_font, RED, 3, "settings");
        GRRLIB_Printf(100, 200, tex_font, RED, 3, "thanks to Crayon2000 and theGRRlIB dev team + the exsample projects");
        GRRLIB_Printf(100, 250, tex_font, RED, 3, "thanks to the wii hombrewing community");
        GRRLIB_Printf(100, 300, tex_font, RED, 3, "check out GRRLIB at https://github.com/GRRLIB/GRRLIB");
        GRRLIB_Printf(100, 350, tex_font, RED, 3, "check out source code at https://github.com/Zpeedbrew/Saiiber/");
        //image displaying 
        //GRRLIB_Draw_img(xpos, ypos,img,scale,scale,rot,0xFFFFFFFF);
        GRRLIB_Render();
    }
    GRRLIB_FreeTexture(tex_font);
    GRRLIB_FreeTexture(tex_back);
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB
    exit(0);
}
