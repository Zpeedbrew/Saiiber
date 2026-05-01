// some of this code (mostly the inilation scpits) contain some code fromb a grrlib template exsample. https://github.com/GRRLIB/GRRLIB/tree/master/examples/template
#include <grrlib.h>
#include <stdlib.h>
#include <math.h>
#include <ogc/lwp_watchdog.h>  
#include <wiiuse/wpad.h>


#include "Letter_Gothic_Std_14_Bold_png.h"
//#include "cursor_png.h"
#include "discord_png.h"

#define RED 0xFF0000FF
#define BLUE 0x0000FFFF
#difine BLACK 0x000000FF


int main() {
    GRRLIB_Init();
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR); //needed to inatate ir
    GRRLIB_texImg *tex_font = GRRLIB_LoadTexture(Letter_Gothic_Std_14_Bold_png);
    //GRRLIB_texImg *tex_cur = GRRLIB_LoadTexture(cursor_png);
    GRRLIB_texImg *tex_dis = GRRLIB_LoadTexture(discord_png);
    GRRLIB_InitTileSet(tex_font, 11, 24, 32);
    GRRLIB_Settings.antialias = true;
    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);
    while(1) {
        GRRLIB_2dMode();
        WPAD_ScanPads();
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) exit(0);
        GRRLIB_SetLightAmbient(0x333333FF);
        // Switch to 2D Mode to display text
        GRRLIB_2dMode();
        //title text 
        GRRLIB_Printf((640-(21*21))/2, 20, tex_font, 0xFF0000FF, 5, "Saii");
        GRRLIB_Printf((640-(3*3))/2, 20, tex_font, 0x0000FFFF, 5, "ber");
        GRRLIB_Printf((640-(16*14))/2, 100, tex_font, 0xFF0000FF, 2, "play");
        GRRLIB_Printf((640-(16*14))/2, 150, tex_font, 0xFF0000FF, 2, "settings");
        GRRLIB_Printf((640-(16*14))/2, 200, tex_font, 0xFF0000FF, 2, "debug");
        GRRLIB_Printf((640-(16*14))/2, 250, tex_font, 0xFF0000FF, 2, "settings");
        GRRLIB_Printf((640-(16*14))/2, 300, tex_font, 0xFF0000FF, 2, "quick play");
        GRRLIB_Printf((640-(16*14))/2, 350, tex_font, 0xFF0000FF, 2, "credits");
        GRRLIB_Printf((640-(16*14))/2, 400, tex_font, 0xFF0000FF, 2, "exit");
        GRRLIB_Printf((640-(16*14))/2, 400, tex_font, 0xFF0000FF, 1, "made by guinea7pig");
        GRRLIB_Printf((640-(16*14))/2, 400, tex_font, 0xFF0000FF, , "made with grrlib");
        GRRLIB_Draw_img(640-(16*14, 450,tex_dis,1,1,0,0xFFFFFFFF)
        //GRRLIB_Draw_img(xpos, ypos,img,scale,scale,rot,0xFFFFFFFF)

        //grrlib logo
        //display qr code to discord

        //debug text neeed to make togglable
        /*
        GRRLIB_Printf((640-(16*14))/2, 350, tex_font, 0xFF0000FF, 2, "current version:1.6");
        GRRLIB_Printf((640-(16*14))/2, 350, tex_font, 0xFF0000FF, 2, "rendering engine:GRRLIB");
        GRRLIB_Printf((640-(16*14))/2, 400, tex_font, 0xFF0000FF, 1,"fps:");
        GRRLIB_Printf((640-(16*14))/2, 400, tex_font, 0xFF0000FF, 1, "fps"x);
        GRRLIB_Printf((640-(16*14))/2, 400, tex_font, 0xFF0000FF, 1,"cur x");
        GRRLIB_Printf((640-(16*14))/2, 400, tex_font, 0xFF0000FF, 1, curx);
        GRRLIB_Printf((640-(16*14))/2, 400, tex_font, 0xFF0000FF, 1,cury);
        GRRLIB_Printf((640-(16*14))/2, 400, tex_font, 0xFF0000FF, 1,"cur y");

//this code snippet is from https://github.com/GRRLIB/GRRLIB/blob/master/examples/basic_drawing/source/main.c
        static u8 CalculateFrameRate(void) {
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
        */

        //

        GRRLIB_Render();
    }
    GRRLIB_FreeTexture(tex_font);
    //GRRLIB_FreeTexture(tex_cur)
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);
}
