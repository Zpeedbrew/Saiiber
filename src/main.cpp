#include <asndlib.h>
#include <fat.h>
#include <gccore.h>
#include <malloc.h>
#include <ogc/isfs.h>
#include <sdcard/wiisd_io.h>
#include <string.h>
#include <wiiuse/wpad.h>
#include <stdlib.h>


#include <cstdio>
#include <cstdlib>

//#include <grrlib.h>

#include "fnt.h"
#include "gfx.h"
#include "input.h"
#include "logger.h"
#include "resource/beatmap.h"
#include "scene/debug_scene.h"
#include "scene/game_scene.h"
#include "scene/loading_scene.h"
#include "scene/menu_scene.h"
#include "scene/scene.h"
#include "sfx.h"

#ifdef _DEBUG
#include <debug.h>
#include <network.h>
#endif


static bool running = true;

std::unique_ptr<Scene> Scene::currentScene(nullptr);
std::unique_ptr<Scene> Scene::overlayScene(nullptr);

bool vsync = false;

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
  char buffer[256];

  va_list args;
  va_start(args, format);
  vsnprintf(buffer, 256, format, args);
  va_end(args);

  if (logfile != NULL) {
    fprintf(logfile, buffer);
    fflush(logfile);
  }

  printf(buffer);
}

// This might be the controller reload?
void reload(u32 irq, void* ctx) { LOG_DEBUG("Reloading %u\n", irq); }

// When shutdown is invoked we can no longer write to the log file.
void shutdown() {
  WPAD_Shutdown();
  Input::enabled = false;
  running = false;
}

const char* tcp_localip = "192.168.1.119";
const char* tcp_netmask = "255.255.255.0";
const char* tcp_gateway = "192.168.1.1";

int main(int argc, char** argv) {
  s32 err;

#ifdef _DEBUG
  err = if_config("192.168.1.121", "255.255.255.0", "192.168.1.1", true, 5);
  DEBUG_Init(GDBSTUB_DEVICE_TCP, 5656);
  _break();
#endif

  if (!fatInitDefault()) return -1;
  if (!fatMountSimple("sd", &__io_wiisd)) return -1;
  LOG_Init();

  if (err != 0) {
    LOG_ERROR("Failed to configure network with error: %d.\n", err);
    
  } else
    LOG_DEBUG("Network configured successfully.\n");

  err = WPAD_Init();
  if (err != 0) {
    LOG_DEBUG("Failed to initialize WPAD with error: %s.\n",
    WPAD_GetError(err));
    return -1;
  } else
    LOG_DEBUG("WPAD initialized successfully.\n");

  s32 mperr = WPAD_SetMotionPlus(WPAD_CHAN_ALL, TRUE);
  s32 dferr = WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
  LOG_DEBUG("WPAD_SetDataFormat: %d\n", dferr);
  LOG_DEBUG("WPAD_SetMotionPlus: %d\n", mperr);

  GFX_Init();
  SFX_Init();
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

  /*err = blueMote.awaitConnect(1, 0);
  if (err != WPAD_CONNECTED)
    LOG_ERROR("BlueMote failed to connect with error: %d.\n", err);
  */

  redMote.assignChannel(0);

#ifdef _DEBUG
  std::string path = "sd:/Songs/Tuxedo - Do It";
  BeatmapInfo info;
  if (GetInfoFromDir(path.c_str(), info) != 0) {
    LOG_DEBUG("Failed to load beatmap in %s\n", path.c_str());
    return -1;
  }

  // std::string dir, BeatmapInfo info, Mode mode, Rank rank
  //want to change to MenuScene
  //Scene::ChangeScene<GameScene>(path, info, Mode::Standard, Rank::Hard);
  Scene::ChangeScene<MenuScene>();
#else
  //Scene::ChangeScene<MenuScene>();
  Scene::ChangeScene<GameScene>(path, info, Mode::Standard, Rank::Hard);

#endif

  u64 lastTime = SYS_Time();
  u64 timeNow = 0;
  f32 deltatime = 0;

  LOG_DEBUG("Beginning game loop\n");
  while (running) {
    timeNow = SYS_Time();  // each tick is 16nanoseconds
    deltatime = (timeNow - lastTime) * 0.000000016f;  // convert to seconds
    lastTime = timeNow;

    // do state scan once per frame
    Input::Update(deltatime);
    Scene::Update(deltatime);

    GFX_FlipBuffers();
    Scene::Render();
    GFX_Finish(false);
  }

  WPAD_Shutdown();
  GFX_Cleanup();
  SFX_Cleanup();

  LOG_DEBUG("Exiting.\n");
  fatUnmount("sd");
  return 0;
}
