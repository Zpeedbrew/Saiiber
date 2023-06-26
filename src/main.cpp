#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <malloc.h>
#include <gccore.h>
#include <fat.h>
#include <asndlib.h>
#include <sdcard/wiisd_io.h>

#include <wiiuse/wpad.h>

#include "gfx.h"
#include "sfx.h"
#include "input.h"
#include "logger.h"

#include "scene/scene.h"
#include "scene/loading_scene.h"
#include "scene/menu_scene.h"
#include "scene/game_scene.h"

static bool running = true;

Scene* Scene::currentScene = NULL;
Scene* Scene::overlayScene = NULL;
Wiimote blueMote(0);
Wiimote redMote(1);

bool vsync = false;
float gpuWait, vsyncWait;

int main(int argc, char **argv) {
  GFX_Setup();
  SFX_Setup();
  Wiimote_Init();

  // TODO: When the scene changes, instead of changing to the new scene we need to load resources.
  // I don't want to have to do "new LoadingScene(new OtherScene())."
  // Instead we should have each scene LIST the resources they need
  //  either statically (templates) or in the ctor, then we
  //  can have a loading scene that loads all the resources in a separate thread

  // For static resources we should just load when the game loads
  // Probably do this for the block model, saber model, texture maps, etc.

  // For dynamic resources we should load when the scene is created
  // and destroy when the scene is destroyed
  // This will be necessary for the song and chart files

  // TODO: create song directory and give GameScene one of em
  Scene::SetScene(new LoadingScene(new GameScene("sd:/apps/saiiber/assets")));

  u64 lastTime = SYS_Time();
  u64 timeNow = 0;
  f32 deltatime = 0;

  LOG_DEBUG("Beginning game loop\n");
	while(running) {
    timeNow = SYS_Time();
    deltatime = (timeNow - lastTime) / 1000.0f;

    Scene::Update(deltatime);

    GFX_FlipBuffers(&gpuWait, &vsyncWait);

    GX_SetViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 1);
    Scene::Render();
    GFX_Finish(false);
    lastTime = timeNow;  
	}

  LOG_DEBUG("Exiting.");
  GFX_Cleanup();
	return 0;
}