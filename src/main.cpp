#include <asndlib.h>
#include <fat.h>
#include <gccore.h>
#include <malloc.h>
#include <sdcard/wiisd_io.h>
#include <string.h>
#include <wiiuse/wpad.h>

#include <cstdio>
#include <cstdlib>

#include "fnt.h"
#include "gfx.h"
#include "input.h"
#include "logger.h"
#include "resource/beatmap.h"
#include "scene/game_scene.h"
#include "scene/loading_scene.h"
#include "scene/menu_scene.h"
#include "scene/scene.h"
#include "sfx.h"

static bool running = true;

std::unique_ptr<Scene> Scene::currentScene(nullptr);
std::unique_ptr<Scene> Scene::overlayScene(nullptr);

bool vsync = false;
float gpuWait, vsyncWait;

FILE* logfile;

void LOG_Init() {
  // generate "saiiber-HH-MM-DD-MM-YYYY.log"
  time_t t = time(NULL);
  struct tm* tm = localtime(&t);
  char filename[256];
  sprintf(filename, "saiiber-%04d-%02d-%02d-%02d%02d%02d.log",
          tm->tm_year + 1900, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min,
          tm->tm_sec);
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

void reload(u32 irq, void* ctx) { running = false; }

// When shutdown is invoked we can no longer write to the log file.
void shutdown() {
  Input::enabled = false;
  running = false;
}

int main(int argc, char** argv) {
  if (!fatInitDefault()) return -1;

  if (!fatMountSimple("sd", &__io_wiisd)) return -1;

  LOG_Init();
  GFX_Init();
  SFX_Init();
  Wiimote_Init();
  FNT_Init();

  SYS_SetResetCallback(reload);
  SYS_SetPowerCallback(shutdown);

  // TODO: When the scene changes we want to have a loading scene.
  // Append all dynamic resources to a list for the loading scene
  //  - This should be done in ctor of the scene after the loading scene
  // Load all of the resources in a separate thread (while showing loading)
  // This will be necessary for the song and chart files

  // TODO: Continue with game and keep attempting to connect controllers
  //  in separate thread until one is established. Change the gamemode options
  //  based on whether or not a second controller is connected.

  int err = blueMote.awaitConnect(0, 10000);
  if (err != WPAD_CONNECTED)
    LOG_ERROR("BlueMote failed to connect with error: %d.\n", err);

  // 2000 because when debugging I don't usually connect it anyway
  err = redMote.awaitConnect(1, 2000);
  if (err != WPAD_CONNECTED)
    LOG_ERROR("RedMote failed to connect with error: %d.\n", err);

#ifdef _DEBUG
  std::string path = "sd:/Songs/Tuxedo - Do It";
  BeatmapInfo info;
  if (GetInfoFromDir(path.c_str(), info) != 0) {
    LOG_DEBUG("Failed to load beatmap in %s\n", path.c_str());
    return -1;
  }

  // std::string dir, BeatmapInfo info, Mode mode, Rank rank
  Scene::ChangeScene<GameScene>(path, info, Mode::Standard, Rank::Hard);

#else
  Scene::ChangeScene<MenuScene>();

#endif

  u64 lastTime = SYS_Time();
  u64 timeNow = 0;
  f32 deltatime = 0;

  LOG_DEBUG("Beginning game loop\n");
  while (running) {
    timeNow = SYS_Time();
    deltatime = (timeNow - lastTime) / 1000000.0f;

    // do state scan once per frame
    Input::Update(deltatime);
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