// some of this code (mostly the inilation scpits) contain some code fromb a grrlib template exsample. https://github.com/GRRLIB/GRRLIB/tree/master/examples/template

#include <grrlib.h>
#include <stdlib.h>
#include <math.h>
#include <wiiuse/wpad.h>
#include "input.h"


#include "Letter_Gothic_Std_14_Bold_png.h"
#define RED 0xFF0000FF
#define BLUE 0x0000FFFF

int k() {
    int camZ=13.0f;
    InitInput();
    GRRLIB_Init();

    GRRLIB_texImg *tex_font = GRRLIB_LoadTexture(Letter_Gothic_Std_14_Bold_png);
    GRRLIB_InitTileSet(tex_font, 11, 24, 32);


    GRRLIB_Settings.antialias = true;

    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);

    while(1) {
        WPAD_ScanPads();
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break; 
        GRRLIB_Camera3dSettings(0.0f,0.0f,camZ, 0,1,0, 0,0,0); 
        GRRLIB_SetLightAmbient(0x333333FF);

        GRRLIB_3dMode(0.1, 1000, 45, 0, 1);
        // Switch to 2D Mode to display text
        GRRLIB_2dMode();
        //title text 
        //need to button stuff eventuly.
        //exit(0)
    }
    
    GRRLIB_Exit();
    exit(0);
}
    
