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

int main() {
    int camZ=13.0f;

    GRRLIB_Init();
    WPAD_Init();


    GRRLIB_texImg *tex_font = GRRLIB_LoadTexture(Letter_Gothic_Std_14_Bold_png);
    GRRLIB_texImg *tex_back = GRRLIB_LoadTexture(back_png);

    GRRLIB_InitTileSet(tex_font, 11, 24, 32);


    GRRLIB_Settings.antialias = true;

    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);

    while(1) {
        GRRLIB_2dMode();
        WPAD_ScanPads();
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break; //goes back to home i hope

        GRRLIB_Camera3dSettings(0.0f,0.0f,camZ, 0,1,0, 0,0,0);
        GRRLIB_SetLightAmbient(0x333333FF);

        //GRRLIB_3dMode(0.1, 1000, 45, 0, 1); dont have anyhing 32 here
        // Switch to 2D Mode to display text
        GRRLIB_2dMode();
        //title text 
        GRRLIB_Printf((640-(21*21))/2, 20, tex_font, RED, 5, "settings");
        GRRLIB_Printf((640-(16*14))/2, 400, tex_font, RED, 2, "return to home");
        //need to button stuff eventuly.
        /*
        exit(0)
        */
        GRRLIB_Printf((640-(16*14))/2, 70, tex_font, RED, 3, "volume,");
        GRRLIB_Printf((640-(16*14))/2, 150, tex_font, RED, 3, "settings");
        GRRLIB_Printf((640-(16*14))/2, 200, tex_font, RED, 3, "rumble"):
        GRRLIB_Printf((640-(16*14))/2, 200, tex_font, RED, 3, "rumble"); //will make text etherir say on  or off

        GRRLIB_Printf((640-(16*14))/2, 250, tex_font, RED, 3, "thanks to the wii hombrewing community");
        GRRLIB_Printf((640-(16*14))/2, 300, tex_font, RED, 3, "check out GRRLIB at https://github.com/GRRLIB/GRRLIB");
        GRRLIB_Printf((640-(16*14))/2, 350, tex_font, RED, 3, "check out source code at https://github.com/Zpeedbrew/Saiiber/");
        //debug text neeed to make togglable
        /*
        GRRLIB_Printf((640-(16*14))/2, 350, tex_font, 0xFF0000FF, 2, "current version:1.6");
        GRRLIB_Printf((640-(16*14))/2, 350, tex_font, 0xFF0000FF, 2, "rendering engine:GRRLIB");
        */



        GRRLIB_Render();
    }
    GRRLIB_FreeTexture(tex_font);
    GRRLIB_FreeTexture(tex_back);
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);
}
