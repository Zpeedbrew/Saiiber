// some of this code (mostly the inilation scpits) contain some code fromb a grrlib template exsample. https://github.com/GRRLIB/GRRLIB/tree/master/examples/template
//graphics
#include <grrlib.h>
#include <stdlib.h>
//math
#include <math.h>
//input
#include <wiiuse/wpad.h>
#include "input.h"
//font
#include "Letter_Gothic_Std_14_Bold_png.h"
#include "beon_png.h"
//images
#include "hand_png.h"
#include "discord_png.h"
#include "grrlib_logo_png.h"
#include "qr_png.h"
//debug
#include "fps.h"
//#include "qr2_png.h"
//definations
#define version 1.6
#define RED 0xFF0000FF
#define WHITE 0xFFFFFFFF
#define BLUE 0x0000FFFF
#define BLACK 0x000000FF
//varibiles
float cursorX, cursorY;
//voids 
/*
void Changescene() {
    GRRLIB_FreeTexture(tex_font);
    GRRLIB_FreeTexture(tex_beon);
    //free imgaes
    GRRLIB_FreeTexture(tex_dis);
    GRRLIB_FreeTexture(tex_gr);
    GRRLIB_FreeTexture(tex_qr);
    GRRLIB_FreeTexture(tex_cur);  
*/
int main() {
    //Scene currentScene = SCENE_MENU;
    //graphics library init
    GRRLIB_Init();
    GRRLIB_Settings.antialias = true;
    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF); 
    GRRLIB_SetLightAmbient(0x333333FF);
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
    GRRLIB_texImg *tex_dis = GRRLIB_LoadTexturePNG(discord_png);
    GRRLIB_texImg *tex_gr = GRRLIB_LoadTexturePNG(grrlib_logo_png);
    GRRLIB_texImg *tex_qr = GRRLIB_LoadTexturePNG(qr_png);
	GRRLIB_SetMidHandle(tex_cur, true);
	GRRLIB_Model* model = GRRLIB_ReadOBJ("sd:/data/head_chord.obj");
    while(1) {
        GRRLIB_2dMode();
        WPAD_ScanPads();
        GetIRPointer(0,&cursorX, &cursorY);
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)break;
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_MINUS) GRRLIB_ScrShot("sd:/saiiber.png");
        //title text 
        GRRLIB_Printf(99,20,tex_font,RED,5,"Saii");
        GRRLIB_Printf(315,20,tex_font,BLUE,5,"ber");
        //menu text
        GRRLIB_Printf(20,200,tex_font,RED,2,"play");
        if(GRRLIB_PtInRect(1,167,80,200,cursorX,cursorY) && WPAD_ButtonsDown(0) & WPAD_BUTTON_A ) 
        //GRRLIB_PtInRect() fun button stuff
        GRRLIB_Printf(20,250,tex_font,RED,2,"settings");
        //if(GRRLIB_PtInRect( , , ,) && WPAD_ButtonsDown(0) & WPAD_BUTTON_A );{ fun button stuff
        GRRLIB_Printf(20,300,tex_font, RED,2,"quick play");
        if(GRRLIB_PtInRect( , , ,) && WPAD_ButtonsDown(0) & WPAD_BUTTON_A ) { }
        //crediits buttons
        GRRLIB_Printf(20,350,tex_font,RED, 2,"credits");
        if(GRRLIB_PtInRect(13,367, ,) && WPAD_ButtonsDown(0) & WPAD_BUTTON_A ){ };
        //exit button
        GRRLIB_Printf(20,400,tex_font,RED,2,"exit");
        if(GRRLIB_PtInRect(10,400,50,50,cursorX,cursorY) && WPAD_ButtonsDown(0) & WPAD_BUTTON_A)break;
        GRRLIB_Printf(10,450,tex_font,BLUE,1,"made by guinea7pig");
        GRRLIB_Printf(460,450,tex_font,RED,1,"made with grrlib");
        //images
        GRRLIB_DrawImg(10,425,tex_dis,0,1,1,WHITE);
        GRRLIB_DrawImg(315,400,tex_gr,0,1,1,WHITE);    
        //debug text neeed to make togglable
        //GRRLIB_Printf(0,30,tex_font,RED,1,"fps: %d", FPS);
        GRRLIB_Printf(500,30,tex_font,RED,1,"curx: %f", cursorX);
        GRRLIB_Printf(500,50,tex_font,BLUE,1,"cury %f", cursorY);
        //FPS = CalculateFrameRate();
        if (cursorX >= 0 && cursorY >= 0) GRRLIB_DrawImg(cursorX,cursorY,tex_cur,0,1,1,WHITE);
	    
		GRRLIB_3dMode();
		GRRLIB_DrawO3dBJ()
		GRRLIB_ObjectView()//controls object paremters
        */
       }
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
