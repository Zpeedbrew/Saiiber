//graphics
#include <grrlib.h>
#include <stdlib.h>
//math
#include <math.h>
//input
#include <wiiuse/wpad.h>
#include "input.h"
#include "fat.h"
//font
#include "Letter_Gothic_Std_14_Bold_png.h"
#include "beon_png.h"
#include "logo_png.h"
//images
#include "hand_png.h"
#include "discord_png.h"
#include "grrlib_logo_png.h"
#include "qr_png.h"
#include "play_png.h"
#include "exit_png.h"
#include "settings_png.h"

//#include "qr2_png.h"

//debug
#include "fps.h"
//definations
#define RED 0xFF0000FF
#define WHITE 0xFFFFFFFF
#define BLUE 0x0000FFFF
#define BLACK 0x000000FF
//varibiles
// GRRLIB_Model* saber = GRRLIB_LoadObjFromString((const char*)saber_obj);

float cursorX, cursorY;
int main() {
    //Scene currentScene = SCENE_MENU;
    //graphics library init
	//graphics_init();
    
	GRRLIB_Init();
    GRRLIB_Settings.antialias = true;
    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF); 
    //audio init
    //ASND_Init();
	//MP3Player_Init();
    //controller init
    InitInput();
    //font intit
    GRRLIB_texImg *tex_font = GRRLIB_LoadTexture(Letter_Gothic_Std_14_Bold_png);
    //GRRLIB_texImg *tex_beon = GRRLIB_LoadTexture(Letter_Gothic_Std_14_Bold_png);
    GRRLIB_InitTileSet(tex_font, 11, 24, 32);
    //GRRLIB_InitTileSet(tex_beon, 11, 24, 32); //tileset probs wrong
    //image init
    GRRLIB_texImg *tex_cur = GRRLIB_LoadTexture(hand_png);
    GRRLIB_texImg *tex_play = GRRLIB_LoadTexture(play_png);
    GRRLIB_texImg *tex_dis = GRRLIB_LoadTexture(discord_png);
    GRRLIB_texImg *tex_gr = GRRLIB_LoadTexture(grrlib_logo_png);
    GRRLIB_texImg *tex_qr = GRRLIB_LoadTexture(qr_png);
    GRRLIB_texImg *tex_logo = GRRLIB_LoadTexture(logo_png);
    GRRLIB_texImg *tex_exit = GRRLIB_LoadTexture(exit_png);
    GRRLIB_texImg *tex_set = GRRLIB_LoadTexture(settings_png);


	GRRLIB_SetMidHandle(tex_cur, true);
    while(1) {
        WPAD_ScanPads();
        GetIRPointer(0,&cursorX, &cursorY);
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)break;
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_MINUS);
		{
		GRRLIB_ScrShot("sd:/saiiber.png");
		WPAD_Rumble(0,1);
		}
        //title text 
        GRRLIB_DrawImg(10,10,tex_logo,0,1,1,WHITE);

        //menu text
        GRRLIB_DrawImg(0,160,tex_play,0,0.5,0.5,RED);
        //if(GRRLIB_PtInRect(1,167,80,200,cursorX,cursorY) && WPAD_ButtonsDown(0) & WPAD_BUTTON_A;
        //GRRLIB_PtInRect() fun button stuff
        GRRLIB_DrawImg(25,220,tex_set,0,1,1,RED);
        //if(GRRLIB_PtInRect( , , ,) && WPAD_ButtonsDown(0) & WPAD_BUTTON_A );{ fun button stuff
        //if(GRRLIB_PtInRect( , , ,) && WPAD_ButtonsDown(0) & WPAD_BUTTON_A ) ;
        //crediits buttons
        GRRLIB_Printf(20,350,tex_font,RED, 2,"credits");
        //if(GRRLIB_PtInRect(13,367, ,) && WPAD_ButtonsDown(0) & WPAD_BUTTON_A );
        //exit button
        GRRLIB_DrawImg(0,370,tex_exit,0,1,1,RED);
        if(GRRLIB_PtInRect(10,400,50,50,cursorX,cursorY) && WPAD_ButtonsDown(0) & WPAD_BUTTON_A) break;
		//credits text
        GRRLIB_Printf(10,450,tex_font,BLUE,1,"made by guinea7pig");
        GRRLIB_Printf(465,450,tex_font,RED,1,"made with grrlib");
        //images
        GRRLIB_DrawImg(220,450,tex_dis,0,0.4,0.4,WHITE);
        GRRLIB_DrawImg(385,450,tex_gr,0,0.2,0.2,WHITE);
        //debug text neeed to make togglable
		//FPS = CalculateFrameRate();
        //GRRLIB_Printf(0,30,tex_font,RED,1,"fps: %d", FPS);
        GRRLIB_Printf(500,30,tex_font,RED,1,"curx: %f", cursorX);
        GRRLIB_Printf(500,50,tex_font,BLUE,1,"cury %f", cursorY);
        if (cursorX >= 0 && cursorY >= 0) GRRLIB_DrawImg(cursorX,cursorY,tex_cur,0,1,1,WHITE);
	    //GRRLIB_ObjectView()//controls object paremters
        GRRLIB_Render();
       }    
    //free fonts
    GRRLIB_FreeTexture(tex_font);
    //GRRLIB_FreeTexture(tex_beon);
    //free imgaes
    GRRLIB_FreeTexture(tex_dis);
    GRRLIB_FreeTexture(tex_gr);
    GRRLIB_FreeTexture(tex_cur);  
	GRRLIB_FreeTexture(tex_qr);  
    GRRLIB_Exit(); 
    exit(0);
}
