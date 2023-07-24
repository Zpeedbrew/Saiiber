#include "game_scene.h"

#include <fat.h>
#include <malloc.h>
#include <sdcard/wiisd_io.h>

#include <cstdio>

#include "../exmath.h"
#include "../fnt.h"
#include "../gameobjects/block.h"
#include "../gameobjects/camera.h"
#include "../gameobjects/saber.h"
#include "../gfx.h"
#include "../input.h"
#include "../logger.h"
#include "../resource/beatmap.h"
#include "../resource/model.h"
#include "../sfx.h"
#include "menu_scene.h"

// TODO: Lighting
static GXColor LightColors[] = {
    {0xFF, 0xFF, 0xFF, 0xFF},  // Light color 1
    {0x80, 0x80, 0x80, 0xFF},  // Ambient 1
    {0x80, 0x80, 0x80, 0xFF}   // Material 1
};

GameScene::GameScene(std::string dir, BeatmapInfo info, Mode mode, Rank rank)
    : beatmap(dir, info) {
  LOG_DEBUG("GameScene constructor\n");

  if (beatmap.loadMap(mode, rank) != 0) {
    LOG_ERROR("Failed to load beatmap\n");
    Scene::ChangeScene<MenuScene>();
    // TODO: Change this to a failure scene
    return;
  }

  std::string path = dir + "/" + info._songFilename;
  beatmap.voice = SFX_Load(path.c_str());
  if (beatmap.voice == -1) {
    LOG_ERROR("Failed to load song %s\n", path);
    Scene::ChangeScene<MenuScene>();  // TODO: Change this to a failure scene
    return;
  }

  // TODO: Decide on the way we should load the blocks of the chart.
  // Personally, I was thinking we might just be able to make a gameobject for
  // all of them then have a constant velocity for them to move down the screen
  // we can use a culling frustum to filter the ones out that are off screen
}

GameScene::~GameScene() {}

void GameScene::init() {
  GFX_SetBlendMode(MODE_BLEND);
  GFX_SetWriteBuffers(true, true, true);
  GFX_TextureMatrix(false);

  // Camera setup
  camera = std::make_shared<Camera>();
  camera->update(0.0f);
  GFX_OutputMatrix(view);
  gameObjects.push_back(camera);

  // 640 x 480
  LOG_DEBUG("Ortho loaded between %u and %u\n", SCREEN_WIDTH, SCREEN_HEIGHT);

  // So THIS is C++
  auto& green = gameObjects.emplace_back(std::make_shared<Block>());
  green->transform->position = {-2.0f, 1.0f, -10.0f};
  green->transform->scale = { 0.5f, 0.5f, 0.5f };

  redSaber = std::make_shared<Saber>(redMote.transform);
  blueSaber = std::make_shared<Saber>(blueMote.transform);
  gameObjects.push_back(redSaber);
  gameObjects.push_back(blueSaber);

  LOG_DEBUG("Loaded GameScene\n");
}

float xRot = 0.0f;
float yRot = 0.0f;

void GameScene::update(f32 deltatime) {
#ifdef _DEBUG
  camera->freecam(deltatime);
#endif

  // TODO: Check intersection between sabers and blocks
  for (size_t i = 0; i < gameObjects.size(); i++) {
    gameObjects[i]->update(deltatime);
  }
}

// This one originally written by shagkur
void SetLight() {
  guVector lpos;
  GXLightObj lobj;

  lpos.x = 0;
  lpos.y = 0;
  lpos.z = 2.0f;

  guVecMultiply(view, &lpos, &lpos);

  GX_InitLightPos(&lobj, lpos.x, lpos.y, lpos.z);
  GX_InitLightColor(&lobj, LightColors[0]);
  GX_LoadLightObj(&lobj, GX_LIGHT0);

  // set number of rasterized color channels
  GX_SetNumChans(1);
  GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0,
                 GX_DF_CLAMP, GX_AF_NONE);
  GX_SetChanAmbColor(GX_COLOR0A0, LightColors[1]);
  GX_SetChanMatColor(GX_COLOR0A0, LightColors[2]);
}

void GameScene::render() {
#ifdef _DEBUG
  camera->render();
#endif

  SetLight();
  for (size_t i = 0; i < gameObjects.size(); i++) {
    gameObjects[i]->render();
  }
}
