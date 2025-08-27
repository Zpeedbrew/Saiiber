#include "game_scene.h"

#include <fat.h>
#include <malloc.h>
#include <memory.h>
#include <sdcard/wiisd_io.h>

#include <cstdio>
#include <functional>

#include "../exmath.h"
#include "../fnt.h"
#include "../gameobjects/block.h"
#include "../gameobjects/camera.h"
#include "../gameobjects/saber.h"
#include "../gameobjects/stage.h"
#include "../gfx.h"
#include "../input.h"
#include "../logger.h"
#include "../resource/beatmap.h"
#include "../resource/model.h"
#include "../sfx.h"
#include "menu_scene.h"

static void *ogg_buffer = NULL;
static u32 ogg_len = 0;

// TODO: Lighting
static GXColor LightColors[] = {
    {0x00, 0x00, 0xff, 0xFF},  // Light color 1
    {0x80, 0x80, 0x80, 0xFF},  // Ambient 1
    {0x80, 0x80, 0x80, 0xFF}   // Material 1
};

GameScene::GameScene(std::string dir, BeatmapInfo info, Mode mode, Rank rank)
    : beatmap(dir, info) {
  LOG_DEBUG("GameScene constructor\n");
  this->mode = mode;

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

void GameScene::music()
{
  FILE *ogg_file = fopen("sd:/music.ogg", "rb");
    if (ogg_file == NULL) {
    //printf("Failed to open music.ogg!\n");
    //return;
    }

    oggvorbis_play(ogg_buffer, ogg_len, 0, 0);




}

void GameScene::init() {

  //ASND_Init();
  GFX_SetBlendMode(MODE_BLEND);
  GFX_SetWriteBuffers(true, true, true);
  GFX_Texture(TEX_NONE);

  
  GFX_SetPostProcessCallback(
      std::bind(&GameScene::postProcess, this, std::placeholders::_1));

  // Camera setup
  camera = std::make_shared<Camera>();
  gameObjects.push_back(camera);

  // So THIS is C++
  auto& refblock = gameObjects.emplace_back(std::make_shared<Block>());
  refblock->transform->position = {-2.0f, 1.0f, -10.0f};
  refblock->transform->scale = {0.5f, 0.5f, 0.5f};
  block = std::dynamic_pointer_cast<Block>(refblock);

  redSaber = std::make_shared<Saber>(redMote.transform);
  blueSaber = std::make_shared<Saber>(blueMote.transform);
  redSaber->setColor(255, 0, 0, 127);
  blueSaber->setColor(0, 0, 255, 127);
  //want to add a way to make saber custom coulor

  // place the saber in front of the view
  glm::vec3 front = camera->transform->position + camera->transform->forward();

#ifdef _DEBUG
  redSaber->transform->position = glm::vec3{0.0f, -0.25f, -0.5f} + front;
  gameObjects.push_back(redSaber);
  mode = Mode::OneSaber; // This is safe because we already loaded beatmap
#else
  switch (mode) {
    case Mode::OneSaber:
      redSaber->transform->position = glm::vec3{0.0f, -0.25f, -0.5f} + front;
      gameObjects.push_back(redSaber);
      break;
    case Mode::Standard:
    default:
      redSaber->transform->position = glm::vec3{-0.25f, -0.25f, -0.5f} + front;
      blueSaber->transform->position = glm::vec3{0.25f, -0.25f, -0.5f} + front;
      gameObjects.push_back(redSaber);
      gameObjects.push_back(blueSaber);
      break;
  }
#endif

  auto& stage = gameObjects.emplace_back(std::make_shared<Stage>());
  stage->transform->position = {0.0f, 0.0f, -2.0f};

  LOG_DEBUG("Loaded GameScene\n");
}

float xRot = 0.0f;
float yRot = 0.0f;

void GameScene::update(f32 deltatime) {
if(redMote.isButtonDown(WPAD_BUTTON_HOME))
{
exit(0);
}
#ifdef _DEBUG
  camera->freecam(deltatime);

  if (!calibrated) {
    // lock the position of the saber in place until calibrated
    glm::vec3 front = camera->transform->position + camera->transform->forward();
    redSaber->transform->position = glm::vec3{0.0f, -0.25f, -0.5f} + front;
    redSaber->transform->update();

    if (redMote.isButtonDown(WPAD_BUTTON_A)) {
      calibrated = redMote.calibrate();
      LOG_DEBUG("Calibrated Redmote\n");
      // TODO: start the game.
    }
  }

#else
  if (!calibrated) {
    switch (mode) {
      case Mode::Standard:
        calibrated = redMote.calibrate() && blueMote.calibrate();
        break;
      case Mode::OneSaber:
        calibrated = redMote.calibrate();
    }
  }
#endif

  // At any point we need to allow the player to recalibrate the remote
  if (redMote.isButtonDown(WPAD_BUTTON_A)) {
    // reset position
    glm::vec3 front = camera->transform->position + camera->transform->forward();

    switch (mode) {
      case Mode::Standard:
        redMote.transform->position = glm::vec3{-0.25f, -0.25f, -0.5f} + front;
        break;
      case Mode::OneSaber:
        redMote.transform->position = glm::vec3{0.0f, -0.25f, -0.5f} + front;
        break;
    }

    redMote.calibrate();
    LOG_DEBUG("Recalibrated Redmote\n");
  }

  // At any point we need to allow the player to recalibrate the remote
  if (blueMote.isButtonDown(WPAD_BUTTON_A)) {
    glm::vec3 front = camera->transform->position + camera->transform->forward();
    switch (mode) {
      case Mode::Standard:
        blueMote.transform->position = glm::vec3{0.25f, -0.25f, -0.5f} + front;
        break;
      case Mode::OneSaber:
        blueMote.transform->position = glm::vec3{0.25f, -0.25f, -0.5f} + front;
        break;
    }
    
    blueMote.calibrate();
    LOG_DEBUG("Recalibrated Bluemote\n");
  }

  if (block != NULL) block->transform->rotate(0.0f, 0.0f, deltatime);

  // TODO: Check intersection between sabers and blocks
  for (size_t i = 0; i < gameObjects.size(); i++) {
    gameObjects[i]->update(deltatime);
  }
}

// This one originally written by shagkur
void SetLight() {
  glm::vec3 lpos(0, 0, 2.0f);
  GXLightObj lobj;

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
  SetLight();
  for (size_t i = 0; i < gameObjects.size(); i++) {
    gameObjects[i]->render();
  }
}

void GameScene::postProcess(void* buf) {
  //redSaber->postProcess();
  //blueSaber->postProcess(buf);
}
