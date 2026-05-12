// some of this code (mostly the inilation scpits) contain some code fromb a grrlib template exsample. https://github.com/GRRLIB/GRRLIB/tree/master/examples/template
#include <grrlib.h>
#include <stdlib.h>
#include <math.h>
#include <wiiuse/wpad.h>
//colors
#define RED 0xFF0000FF
#define BLUE 0x0000FFFF
//font
#include "Letter_Gothic_Std_14_Bold_png.h"
//images
#include "back_png.h"

int e() {
    //graphics init
    GRRLIB_Init();
    //input init
    WPAD_Init();
    InitInput();


    //loading font
    GRRLIB_texImg *tex_font = GRRLIB_LoadTexture(Letter_Gothic_Std_14_Bold_png);
    GRRLIB_texImg *tex_back = GRRLIB_LoadTexture(back_png);

    GRRLIB_InitTileSet(tex_font, 11, 24, 32);
    GRRLIB_Settings.antialias = true;
    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);
    GRRLIB_SetLightAmbient(0x333333FF);

    while(1) {
        GRRLIB_2dMode();
        WPAD_ScanPads();
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break; //goes back to home i hope
       GRRLIB_SetLightAmbient(0x333333FF);

        GRRLIB_2dMode();
        //title text 
        GRRLIB_Printf(215, 20, tex_font, RED, 5, "settings");
        GRRLIB_Printf(10, 400, tex_font, RED, 2, "return to home");
        GRRLIB_Printf(10, 70, tex_font, RED, 3, "volume,");
        GRRLIB_Printf(10, 150, tex_font, RED, 3, "settings");
        GRRLIB_Printf(10, 200, tex_font, RED, 3, "rumble");
        GRRLIB_Printf(10, 200, tex_font, RED, 3, "rumble"); //will make text etherir say on  or off

        GRRLIB_Render();
    }
    GRRLIB_FreeTexture(tex_font);
    GRRLIB_FreeTexture(tex_back);
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);
}
