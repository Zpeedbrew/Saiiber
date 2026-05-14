// some of this code (mostly the inilation scpits) contain some code fromb a grrlib template exsample. https://github.com/GRRLIB/GRRLIB/tree/master/examples/template
//graphics intit
#include <grrlib.h>
#include <stdlib.h>
//math
#include <math.h>
//input
#include "input.h"
#include <wiiuse/wpad.h>
//colors
#define RED 0xFF0000FF
#define BLUE 0x0000FFFF
#define WHITE 0xFFFFFFFF
//fonts
#include "Letter_Gothic_Std_14_Bold_png.h"
#include "back_png.h"
#include "hand_png.h"
#include "grrlib_logo_png.h"
#include "qr_png.h"
//grlib logo

int o() {
    //graphics
    GRRLIB_Init();
    GRRLIB_InitTileSet(tex_font, 11, 24, 32);
    GRRLIB_Settings.antialias = true;
    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);
    GRRLIB_SetLightAmbient(0x333333FF);
    InitInput();
    //font init
    GRRLIB_texImg *tex_font = GRRLIB_LoadTexture(Letter_Gothic_Std_14_Bold_png);
    //image
    GRRLIB_texImg *tex_back = GRRLIB_LoadTexture(back_png);
    GRRLIB_texImg *tex_gr = GRRLIB_LoadTexture(grrlib_logo_png);
    GRRLIB_texImg *tex_qr = GRRLIB_LoadTexture(qr_png);

    GRRLIB_texImg *tex_cur = GRRLIB_LoadTexture(hand_png);
    while(1) {
        GRRLIB_2dMode();
        WPAD_ScanPads();        
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break; //goes back to home i hope
        //title text 
        GRRLIB_Printf(100, 20, tex_font, BLUE, 5, "credits");
        GRRLIB_Printf(100, 400, tex_font, RED, 2, "return to home");
        //need to button stuff eventuly.
        GRRLIB_Printf(640, 100, tex_font, RED, 3, "devolped by Guinea7pig, Beemer,");
        GRRLIB_Printf(100, 150, tex_font, RED, 3, "settings");
        GRRLIB_Printf(100, 200, tex_font, RED, 3, "thanks to Crayon2000 and theGRRlIB dev team + the exsample projects");
        GRRLIB_Printf(100, 250, tex_font, RED, 3, "thanks to the wii hombrewing community");
        GRRLIB_Printf(100, 300, tex_font, RED, 3, "check out GRRLIB at https://github.com/GRRLIB/GRRLIB");
        GRRLIB_Printf(100, 350, tex_font, RED, 3, "check out source code at https://github.com/Zpeedbrew/Saiiber/");
        //draw image
         GRRLIB_DrawImg(30,425,tex_qr,0,0.1,0.1,WHITE);

        //image displaying 
        //GRRLIB_Draw_img(xpos, ypos,img,scale,scale,rot,0xFFFFFFFF);
        GRRLIB_Render();
    }
    GRRLIB_FreeTexture(tex_font);
    GRRLIB_FreeTexture(tex_back);
    GRRLIB_FreeTexture(tex_gr);
    GRRLIB_FreeTexture(tex_q);
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB
    exit(0);
}
