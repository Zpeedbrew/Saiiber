#include "menu_scene.h"
#include "../resource/beatmap.h"
#include "../fnt.h"
#include "../input.h"

const int GREEN = 0x00FF00FF;
const int WHITE = 0xFFFFFFFF;

static enum State {
  MAIN_MENU,
  QUICKPLAY,
  VERSUS,
  PRACTICE,
  SETTINGS,
} curState, prevState;

static BeatmapData data;

struct ButtonData {
  int color;
  s16 x, y;
  s16 width, height;
};

static struct _mainmenu {
  ButtonData quickplay;
  ButtonData versus;
  ButtonData practice;
  ButtonData settings;

  void update(f32 delta) {
    if (blueMote.transform.intersects(quickplay.x, quickplay.y,
    quickplay.width, quickplay.height)) {
      quickplay.color = GREEN;
    } else {
      quickplay.color = WHITE;
    }

    if (blueMote.transform.intersects(versus.x, versus.y,
    versus.width, versus.height)) {
      versus.color = GREEN;
    } else {
      versus.color = WHITE;
    }

    if (blueMote.transform.intersects(practice.x, practice.y,
    practice.width, practice.height)) {
      practice.color = GREEN;
    } else {
      practice.color = WHITE;
    }

    if (blueMote.transform.intersects(settings.x, settings.y,
    settings.width, settings.height)) {
      settings.color = GREEN;
    } else {
      settings.color = WHITE;
    }

    if (blueMote.buttonDown(WIIMOTE_A)) {
      if (quickplay.color == GREEN)
        curState = QUICKPLAY;
      else if (versus.color == GREEN)
        curState = VERSUS;
      else if (practice.color == GREEN)
        curState = PRACTICE;
      else if (settings.color == GREEN)
        curState = SETTINGS;
    }

    if (blueMote.buttonDown(WIIMOTE_HOME)) {
      exit(0);
    }
  }

  void render() {
    // Title
    FNT_SetColor(0xFF0000FF);
    FNT_SetScale(2.0f);
    int width = FNT_GetStringWidth("Saiiber");
    s16 middle = (SCREEN_WIDTH / 2) - (width / 2);
    FNT_DrawString("Saiiber", middle, 100);

    // Quickplay
    FNT_SetScale(1.0f);
    FNT_SetColor(quickplay.color);
    FNT_DrawString("Quickplay", quickplay.x, quickplay.y);

    // Versus
    FNT_SetColor(versus.color);
    FNT_DrawString("Versus", versus.x, versus.y);

    // Practice
    FNT_SetColor(practice.color);
    FNT_DrawString("Practice", practice.x, practice.y);

    // Settings
    FNT_SetColor(settings.color);
    FNT_DrawString("Settings", settings.x, settings.y);
  }
} MainMenu;

MenuScene::MenuScene() {
  data.loadMapData("sd:/Songs/Tuxedo - Do It");

  FNT_SetScale(1.0f);

  MainMenu.quickplay.color = WHITE;
  MainMenu.quickplay.x = 100;
  MainMenu.quickplay.y = 200;
  MainMenu.quickplay.width = FNT_GetStringWidth("Quickplay");
  MainMenu.quickplay.height = FNT_GetStringHeight("Quickplay");

  MainMenu.versus.color = WHITE;
  MainMenu.versus.x = 100;
  MainMenu.versus.y = 250;
  MainMenu.versus.width = FNT_GetStringWidth("Versus");
  MainMenu.versus.height = FNT_GetStringHeight("Versus");

  MainMenu.practice.color = WHITE;
  MainMenu.practice.x = 100;
  MainMenu.practice.y = 300;
  MainMenu.practice.width = FNT_GetStringWidth("Practice");
  MainMenu.practice.height = FNT_GetStringHeight("Practice");

  MainMenu.settings.color = WHITE;
  MainMenu.settings.x = 100;
  MainMenu.settings.y = 350;
  MainMenu.settings.width = FNT_GetStringWidth("Settings");
  MainMenu.settings.height = FNT_GetStringHeight("Settings");

  curState = MAIN_MENU;
  prevState = MAIN_MENU;
}

void MenuScene::init() {
  GFX_EnableLighting(false);
  GFX_SetBlendMode(MODE_BLEND);
  GFX_EnableAlphaTest(false);
  GFX_TextureMatrix(false);
  GFX_SetWriteBuffers(true, false, false);

  Mtx44 ortho;
  guOrtho(ortho, 0, SCREEN_HEIGHT, 0, SCREEN_WIDTH, 0, 1);
  GX_LoadProjectionMtx(ortho, GX_ORTHOGRAPHIC);

	guMtxIdentity(view);
  GFX_ModelViewMatrix(view);
}

MenuScene::~MenuScene() {

}

void MenuScene::update(f32 deltatime) {
  switch (curState) {
    case MAIN_MENU:
      MainMenu.update(deltatime);
      break;
    case QUICKPLAY:
      break;
    case VERSUS:
      break;
    case PRACTICE:
      break;
    case SETTINGS:
      break;
  }
}

void MenuScene::render() {
  switch (curState) {
    case MAIN_MENU:
      MainMenu.render();
      break;
    case QUICKPLAY:
      break;
    case VERSUS:
      break;
    case PRACTICE:
      break;
    case SETTINGS:
      break;
  }
}