// some of this code (mostly the inilation scpits) contain some code fromb a grrlib template exsample. https://github.com/GRRLIB/GRRLIB/tree/master/examples/template

/*
#include <grrlib.h>
#include <stdlib.h>
#include <math.h>
#include <wiiuse/wpad.h>


#include "Letter_Gothic_Std_14_Bold_png.h"
#define RED 0xFF0000FF
#define BLUE 0x0000FFFF

int main() {
    int camZ=13.0f;

    GRRLIB_Init();
    WPAD_Init();


    GRRLIB_texImg *tex_font = GRRLIB_LoadTexture(Letter_Gothic_Std_14_Bold_png);
    GRRLIB_InitTileSet(tex_font, 11, 24, 32);


    GRRLIB_Settings.antialias = true;

    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);

    while(1) {
        GRRLIB_2dMode();
        WPAD_ScanPads();
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) exit(0); 
        GRRLIB_Camera3dSettings(0.0f,0.0f,camZ, 0,1,0, 0,0,0); //want this to go back to menu
        GRRLIB_SetLightAmbient(0x333333FF);

        GRRLIB_3dMode(0.1, 1000, 45, 0, 1);
        // Switch to 2D Mode to display text
        GRRLIB_2dMode();
        //title text 
        //need to button stuff eventuly.
        /*
        exit(0)
    
