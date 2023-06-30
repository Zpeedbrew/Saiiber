#include "game_scene.h"
#include <cstdio>
#include <fat.h>
#include <sdcard/wiisd_io.h>
#include <malloc.h>

#include "../gfx.h"
#include "../sfx.h"
#include "../logger.h"
#include "../input.h"
#include "../resource/model.h"
#include "../resource/beatmap.h"

#include "../gameobjects/block.h"

// TODO: Lighting
static GXColor LightColors[] = {
		{ 0xFF, 0xFF, 0xFF, 0xFF }, // Light color 1
		{ 0x80, 0x80, 0x80, 0xFF }, // Ambient 1
		{ 0x80, 0x80, 0x80, 0xFF }  // Material 1
};

GameScene::GameScene(const char* songdir) {
  // load note model only once
  if (Block::model == NULL)
    Block::model = new Model("block.obj");  

  LoadedMapData* mapData = (LoadedMapData*)malloc(sizeof(LoadedMapData));
  if (GetMapData(mapData, songdir) < 0) {
    LOG_ERROR("Failed to load map data\n");
    return;
  }

  char path[256];
  sprintf(path, "%s/%s", songdir, mapData->getInfo()._songFilename.c_str());
  int voice = SFX_Load(path);
  if (voice == -1) {
    LOG_ERROR("Failed to load song %s\n", path);
    return;
  }

  map = new LoadedMap(mapData, (char*)NULL, voice);
  
  // TODO: Decide on the way we should load the blocks of the chart.
  // Personally, I was thinking we might just be able to make a gameobject for all of them
  // then have a constant velocity for them to move down the screen
  // we can use a culling frustum to filter the ones out that are off screen
}

GameScene::~GameScene() {
  for (size_t i = 0; i < gameObjects.size(); i++) {
    delete gameObjects[i];
  }

  delete map;
}


void GameScene::init() {
  GFX_EnableLighting(false);
  GFX_SetBlendMode(MODE_BLEND);
  GFX_EnableAlphaTest(true);
  GFX_SetWriteBuffers(true, true, true);

  // Set up the projection matrix
  // This creates a perspective matrix with a view angle of 90
  // an aspect ratio that matches the screen, and z-near
  guPerspective(projection, 60.0f, (f32)SCREEN_WIDTH / (f32)SCREEN_HEIGHT, 0.1f, 300.0f);
  GX_LoadProjectionMtx(projection, GX_PERSPECTIVE);

  // Set up the view matrix
	guVector cam = {0.0F, 3.0F,  0.0F},
			      up = {0.0F, 1.0F,  0.0F},
          look = {0.0F, 0.0F, -1.0F};
  guLookAt(view, &cam, &up, &look);

  // guMtxIdentity(view);
  // guMtxTrans(view, 0, 2.0f, 0);
  // guMtxRotAxis(view, 'x', 10.0f);
  guMtxRotDeg(view, 'x', 10.0f);
  GFX_OutputMatrix(view);

  // 640 x 480
  LOG_DEBUG("Ortho loaded between %u and %u\n", SCREEN_WIDTH, SCREEN_HEIGHT);

  Block* green = new Block();
  green->transform.position = { -2.0f, 1.0f, -10.0f };

  gameObjects.push_back(green);

  LOG_DEBUG("Loaded GameScene\n");
}

float xRot = 0.0f;
float yRot = 0.0f;
void GameScene::update(f32 deltatime) {
  for (size_t i = 0; i < gameObjects.size(); i++) {
    gameObjects[i]->update(deltatime);
  }
}

// This one originally written by shagkur
void SetLight(Mtx view, GXColor litcol, GXColor ambcol, GXColor matcol)
{
	guVector lpos;
	GXLightObj lobj;

	lpos.x = 0;
	lpos.y = 0;
	lpos.z = 2.0f;

	guVecMultiply(view,&lpos,&lpos);

	GX_InitLightPos(&lobj,lpos.x,lpos.y,lpos.z);
	GX_InitLightColor(&lobj,litcol);
	GX_LoadLightObj(&lobj,GX_LIGHT0);

	// set number of rasterized color channels
	GX_SetNumChans(1);
	GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG,
    GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
	GX_SetChanAmbColor(GX_COLOR0A0, ambcol);
	GX_SetChanMatColor(GX_COLOR0A0, matcol);
}

void GameScene::render() {
  GFX_EnableTextures(false);
  GX_Begin(GX_LINES, GX_VTXFMT1, 2);
  GX_Position3s16(0, -100, 0);
  GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);
  GX_TexCoord2u16(0, 0);

  GX_Position3s16(0, 0, 0);
  GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);
  GX_TexCoord2u16(0, 0);
  GX_End();
  GFX_EnableTextures(true);

	//SetLight(view, LightColors[0], LightColors[1], LightColors[2]);
  for (size_t i = 0; i < gameObjects.size(); i++) {
    Mtx modelview;
    guMtxConcat(view, gameObjects[i]->transform.matrix, modelview);
    GFX_ModelViewMatrix(modelview);
    gameObjects[i]->render();
  }
}
