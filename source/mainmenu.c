#include <grrlib.h>
#include <stdlib.h>
#include <math.h>
#include <wiiuse/wpad.h>


#include "Letter_Gothic_Std_14_Bold_png.h"
#include "cursor.h"

int main() {
    float l1=0, l2=0;
    float a=0;
    int camZ=13.0f;

    GRRLIB_Init();
    WPAD_Init();


    GRRLIB_texImg *tex_font = GRRLIB_LoadTexture(Letter_Gothic_Std_14_Bold_png);
    GRRLIB_texImg *tex_cur = GRRLIB_LoadTexture(cursor_png);

    GRRLIB_InitTileSet(tex_font, 11, 24, 32);


    GRRLIB_Settings.antialias = true;

    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);

    while(1) {
        GRRLIB_2dMode();
        WPAD_ScanPads();
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) exit(0);
        if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_PLUS) camZ++;
        if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_MINUS) camZ--;

        GRRLIB_Camera3dSettings(0.0f,0.0f,camZ, 0,1,0, 0,0,0);
        GRRLIB_SetLightAmbient(0x333333FF);

        GRRLIB_3dMode(0.1, 1000, 45, 0, 1);

            // Set all light off to get the sphere no light sourced (only get the vertex color)
            GRRLIB_SetLightOff();
            GRRLIB_ObjectView(sin(l1)*4.0f,0.0f,cos(l1)*4.0f, 0,0,0,1,1,1);
            GRRLIB_DrawSphere(0.2f, 20, 20, true, 0xFF0000FF);
        

        
            // Set all light off to get the sphere no light sourced (only get the vertex color)
            GRRLIB_SetLightOff();
            GRRLIB_ObjectView(0.0f,sin(l2)*4.0f,cos(l2)*4.0f, 0,0,0,1,1,1);
            GRRLIB_DrawSphere(0.2f, 20, 20, true, 0x00FF00FF);
        

        // Set a dummy black light to get the ambient one when no light is selected
        GRRLIB_SetLightDiff(0,(guVector){0.0f,0.0f,0.0f},20.0f,1.0f,0x000000FF);

        
            GRRLIB_SetLightDiff(0,(guVector){sin(l1)*4.0f,0.0f,cos(l1)*4.0f},20.0f,1.0f,0xFF0000FF);
            l1+=0.03f;
        

            GRRLIB_SetLightDiff(1,(guVector){0.0f,sin(l2)*4.0f,cos(l2)*4.0f},20.0f,1.0f,0x00FF00FF);
            l2+=0.05f;

        GRRLIB_ObjectView(0,0,0, a,a*2,a*3,1,1,1);
        

        a+=0.5f;

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
        */

        //

        GRRLIB_Render();
    }
    GRRLIB_FreeTexture(tex_font);
    GRRLIB_FreeTexture(tex_cur)
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);
}
