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
#include "fnt.h"
#include "input.h"
#include "logger.h"

#include "scene/scene.h"
#include "scene/loading_scene.h"
#include "scene/menu_scene.h"
#include "scene/game_scene.h"

#include "resource/beatmap.h"

static bool running = true;

Scene* Scene::currentScene = NULL;
Scene* Scene::overlayScene = NULL;
Wiimote blueMote(0);
Wiimote redMote(1);

bool vsync = false;
float gpuWait, vsyncWait;

FILE* logfile;

void LOG_Init() {
  // generate "saiiber-HH-MM-DD-MM-YYYY.log"
  time_t t = time(NULL);
  struct tm* tm = localtime(&t);
  char filename[256];
  sprintf(filename, "saiiber-%02d-%02d-%02d-%02d-%04d.log",
    tm->tm_hour, tm->tm_min, tm->tm_mday, tm->tm_mon, tm->tm_year + 1900);
  logfile = fopen(filename, "wt");
}

void Logger::log(const char* format, ...) {
  va_list args;
  va_start(args, format);

  if (logfile != NULL) {
    vfprintf(logfile, format, args);
    fflush(logfile);
  } else
    vprintf(format, args);

  va_end(args);
}

void Logger::log_fatal(const char* format, ...) {
  va_list args;
  va_start(args, format);
  
  if (logfile != NULL) {
    vfprintf(logfile, format, args);
    fflush(logfile);
  }
  else
    vprintf(format, args);

  va_end(args);

  if (logfile != NULL)
    fclose(logfile);

  exit(EXIT_FAILURE);
}

void reload(u32 irq, void* ctx) {
  LOG_DEBUG("Reloading\n");
  running = false;
}

void shutdown() {
  LOG_DEBUG("Shutting down\n");
  running = false;
}

int main(int argc, char **argv) {  
  if (!fatInitDefault())
    return -1;

  if (!fatMountSimple("sd", &__io_wiisd))
    return -1;

  LOG_Init();
  GFX_Init();
  SFX_Init();
  Wiimote_Init();
  FNT_Init();

	SYS_SetResetCallback(reload);
	SYS_SetPowerCallback(shutdown);

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
  Scene::SetScene(new MenuScene());

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

  GFX_Cleanup();
  SFX_Cleanup();

  LOG_DEBUG("Exiting.\n");
  fatUnmount("sd");
	return 0;
}