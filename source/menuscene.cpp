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
#include "play_png.h"
#include "exit_png.h"
#include "settings_png.h"
#include "credits_png.h"

//debug
#include "fps.h"
//definations
#define RED 0xFF0000FF
#define WHITE 0xFFFFFFFF
#define BLUE 0x0000FFFF
#define BLACK 0x000000FF
//varibiles
float cursorX, cursorY;
#include "menu_scene.h"

#include <dirent.h>

#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "../fnt.h"
#include "../gfx.h"
#include "../input.h"
#include "../logger.h"
#include "../resource/beatmap.h"
#include "../sfx.h"
#include "../ui.h"
#include "game_scene.h"
#define SONGPATH "sd:/Songs"  
	
    GRRLIB_Init();
    GRRLIB_Settings.antialias = true;
    GRRLIB_SetBackgroundColour(BLACK); 
    //font intit
    GRRLIB_texImg *tex_font = GRRLIB_LoadTexture(Letter_Gothic_Std_14_Bold_png);
    GRRLIB_InitTileSet(tex_font, 11, 24, 32);
    //image init
    GRRLIB_texImg *tex_cur = GRRLIB_LoadTexture(hand_png);
    GRRLIB_texImg *tex_play = GRRLIB_LoadTexture(play_png);
    GRRLIB_texImg *tex_dis = GRRLIB_LoadTexture(discord_png);
    GRRLIB_texImg *tex_gr = GRRLIB_LoadTexture(grrlib_logo_png);
    GRRLIB_texImg *tex_qr = GRRLIB_LoadTexture(qr_png);
    GRRLIB_texImg *tex_logo = GRRLIB_LoadTexture(logo_png);
    GRRLIB_texImg *tex_exit = GRRLIB_LoadTexture(exit_png);
    GRRLIB_texImg *tex_set = GRRLIB_LoadTexture(settings_png);
    GRRLIB_texImg *tex_cred = GRRLIB_LoadTexture(credits_png);
	GRRLIB_Model* model = GRRLIB_ReadOBJ("sd:/data/head_chord.obj");


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
        GRRLIB_DrawImg(0,200,tex_play,0,0.5,0.5,RED);
        if(GRRLIB_PtInRect(0,200,192,68,cursorX,cursorY) && WPAD_ButtonsDown(0) & WPAD_BUTTON_A;
        //GRRLIB_PtInRect() fun button stuff
        GRRLIB_DrawImg(25,260,tex_set,0,1,1,RED);
        if(GRRLIB_PtInRect(25,260, ,) && WPAD_ButtonsDown(0) & WPAD_BUTTON_A ) ;{ fun button stuff
        //crediits buttons 
        GRRLIB_DrawImg(25,320,tex_cred,0,1,1,RED);
        //if(GRRLIB_PtInRect(13,367, ,) && WPAD_ButtonsDown(0) & WPAD_BUTTON_A );
        //exit button
        GRRLIB_DrawImg(0,380,tex_exit,0,1,1,RED);
        if(GRRLIB_PtInRect(0,380,50,50,cursorX,cursorY) && WPAD_ButtonsDown(0) & WPAD_BUTTON_A) break;
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
        GRRLIB_Render();
       }    
    //free fonts
    GRRLIB_FreeTexture(tex_font);
    //free imgaes
	GRRLIB_FreeTexture(tex_exit)
    GRRLIB_FreeTexture(tex_dis);
    GRRLIB_FreeTexture(tex_gr);
    GRRLIB_FreeTexture(tex_cur);  
	GRRLIB_FreeTexture(tex_qr);  
    GRRLIB_Exit(); 
    exit(0);
}


//graphics
#include <grrlib.h>
//font
#include "Letter_Gothic_Std_14_Bold_png.h"
#include "beon_png.h"
#include "logo_png.h"
//images
#include "hand_png.h"
#include "discord_png.h"
#include "grrlib_logo_png.h"
#include "play_png.h"
#include "exit_png.h"
#include "settings_png.h"
#include "credits_png.h"
//debug
#include "fps.h"
#include "menu_scene.h"
#include <dirent.h>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "../fnt.h"
#include "../gfx.h"
#include "../input.h"
#include "../logger.h"
#include "../resource/beatmap.h"
#include "../sfx.h"
#include "../ui.h"
#include "game_scene.h"
#define SONGPATH "sd:/Songs"

#define RED 0xFF0000FF
#define WHITE 0xFFFFFFFF
#define BLUE 0x0000FFFF
#define BLACK 0x000000FF

struct MenuSceneImpl {
  // Don't need to worry about cleaning up this pointer, It's managed elsewhere
  MenuScene* menu;
  std::vector<std::pair<std::string, BeatmapInfo>> beatmaps;
  bool needReload = true;

  BeatmapList loadSongs();
  void MainMenu();
  void SongSelect();
  void ModeSelect(int i);
  void DifficultySelect(int i, Mode mode);
};

struct ButtonData {
  const char* text;
  s16 pos[2];
};

BeatmapList MenuSceneImpl::loadSongs() {
  BeatmapList beatmaps;

  // list directory
  DIR* dir = opendir(SONGPATH);
  if (dir == NULL) {
    LOG_ERROR("Failed to open songs directory!\n");
    return beatmaps;
  }

  struct dirent* ent;
  while ((ent = readdir(dir)) != NULL) {
    if (ent->d_type == DT_DIR && ent->d_name[0] != '.') {
      // check for beatmap
      std::string path = std::string(SONGPATH) + "/" + ent->d_name;

      BeatmapInfo info;
      if (GetInfoFromDir(path.c_str(), info) != 0) {
        LOG_DEBUG("Failed to load beatmap in %s\n", ent->d_name);
        continue;
      }

      beatmaps.emplace_back(std::string(path), info);
    }
  }

  return beatmaps;
}

void MenuSceneImpl::DifficultySelect(int i, Mode mode) {
  auto& song = beatmaps[i];

  int width = FNT_GetStringWidth("Difficulty Select", 2.0f);
  s16 middle = (SCREEN_WIDTH / 2) - (width / 2);
  auto title =
      std::make_unique<GuiText>("Difficulty Select", middle, 100, 2.0f);
  title->setPosition(middle, 100);
  title->setColor(0xFF0000FF);

  int diffFlags = song.second.getDifficulties(mode);
  int yIdx = 0;

  auto buttons = std::make_unique<GuiList>();
  if (diffFlags & (1 << (int)Rank::Easy))
    buttons->add("Easy", 100, 200 + (50 * yIdx++));

  if (diffFlags & (1 << (int)Rank::Normal))
    buttons->add("Normal", 100, 200 + (50 * yIdx++));

  if (diffFlags & (1 << (int)Rank::Hard))
    buttons->add("Hard", 100, 200 + (50 * yIdx++));

  if (diffFlags & (1 << (int)Rank::Expert))
    buttons->add("Expert", 100, 200 + (50 * yIdx++));

  if (diffFlags & (1 << (int)Rank::ExpertPlus))
    buttons->add("Expert+", 100, 200 + (50 * yIdx++));

  buttons->onButtonPressed([=](int button, u32 choice, GuiButton& element) {
    if (button == WIIMOTE_BUTTON_B) {
      ModeSelect(i);
      return;
    }

    if (button == WIIMOTE_BUTTON_A) {
      Rank rank = RankFromString(element.getText());
      Scene::ChangeScene<GameScene>(song.first, song.second, mode, rank);
    }
  });

  menu->reset();
  menu->addElement(std::move(title));
  menu->addElement(std::move(buttons));
}

void MenuSceneImpl::ModeSelect(int i) {
  auto& song = beatmaps[i];

  int width = FNT_GetStringWidth("Mode Select", 2.0f);
  s16 middle = (SCREEN_WIDTH / 2) - (width / 2);
  auto title = std::make_unique<GuiText>("Mode Select", middle, 100, 2.0f);
  title->setPosition(middle, 100);
  title->setColor(0xFF0000FF);

  int yIdx = 0;
  int modeFlags = song.second.getModes();

  auto buttons = std::make_unique<GuiList>();
  if (modeFlags & (1 << (int)Mode::Standard))
    buttons->add("Standard", 100, 200 + (yIdx++ * 50));

  if (modeFlags & (1 << (int)Mode::OneSaber))
    buttons->add("One Saber", 100, 200 + (yIdx++ * 50));

  if (modeFlags & (1 << (int)Mode::NoArrows))
    buttons->add("No Arrows", 100, 200 + (yIdx++ * 50));

  if (modeFlags & (1 << (int)Mode::ThreeSixty))
    buttons->add("360", 100, 200 + (yIdx++ * 50));

  if (modeFlags & (1 << (int)Mode::Ninety))
    buttons->add("90", 100, 200 + (yIdx++ * 50));

  if (modeFlags & (1 << (int)Mode::Lightshow))
    buttons->add("Lightshow", 100, 200 + (yIdx++ * 50));

  if (modeFlags & (1 << (int)Mode::Lawless))
    buttons->add("Lawless", 100, 200 + (yIdx++ * 50));

  buttons->onButtonPressed([=](int button, u32 choice, GuiButton& element) {
    if (button == WIIMOTE_BUTTON_B) {
      SongSelect();
      return;
    }

    if (button == WIIMOTE_BUTTON_A) {
      Mode mode = ModeFromString(element.getText());
      DifficultySelect(i, mode);
    }
  });

  menu->reset();
  menu->addElement(std::move(title));
  menu->addElement(std::move(buttons));
}

void MenuSceneImpl::SongSelect() {
  if (needReload || beatmaps.empty()) {
    beatmaps = loadSongs();
    needReload = false;

    // return to mainmenu when no songs are found
    if (beatmaps.empty()) {
      LOG_DEBUG("No songs found!\n");
      MainMenu();
      return;
    }
  }

  int width = FNT_GetStringWidth("Song Select", 2.0f);
  s16 middle = (SCREEN_WIDTH / 2) - (width / 2);
  auto title = std::make_unique<GuiText>("Song Select", middle, 100, 2.0f);
  title->setPosition(middle, 100);
  title->setColor(0xFF0000FF);

  auto buttons = std::make_unique<GuiList>();
  for (u32 i = 0; i < beatmaps.size(); i++) {
    std::stringstream ss;
    ss << std::setfill(' ') << std::setw(20)
       << beatmaps[i].second._songAuthorName << " - " << std::left
       << std::setw(50) << beatmaps[i].second._songName;
    LOG_DEBUG("Adding Song to SongSelect: %s - %s\n",
              beatmaps[i].second._songAuthorName.c_str(),
              beatmaps[i].second._songName.c_str());
    buttons->add(ss.str().c_str(), 100, 200 + (i * 50), 0.8f);
  }

  buttons->onButtonPressed([=](int button, u32 choice, GuiButton& element) {
    if (button == WIIMOTE_BUTTON_B)
      MainMenu();
    else if (button == WIIMOTE_BUTTON_A)
      ModeSelect(choice);
  });

  // I'm putting reset here because this way we can guarantee that all of the
  //  previous elements are copyable before we delete them (in case they are
  //  needed in the initialization above this)
  menu->reset();
  menu->addElement(std::move(title));
  menu->addElement(std::move(buttons));
}

void MenuSceneImpl::MainMenu() {
  needReload = true;
/*
  int width = FNT_GetStringWidth("Saiiber", 2.0f);
  s16 middle = (SCREEN_WIDTH / 2) - (width / 2);
  auto title = std::make_unique<GuiText>("Saiiber", middle, 100, 2.0f);
  title->setPosition(middle, 100);
  title->setColor(0xFF0000FF);
*/
 GRRLIB_DrawImg(10,10,tex_logo,0,1,1,WHITE);

  auto buttons = std::make_unique<GuiList>();
  buttons->add("Quickplay", 100, 200, 1.0f);
  buttons->add("Versus", 100, 250);
  buttons->add("Practice", 100, 300);
  buttons->add("Settings", 100, 350);

  buttons->onButtonPressed([=](int button, u32 choice, GuiButton& element) {
    if (button == WIIMOTE_BUTTON_A) {
      switch (choice) {
        case 0:
          SongSelect();
          break;
        case 1:
          break;
        case 2:
          break;
        case 3:
          break;
      }
    }
  });

  menu->reset();
  menu->addElement(std::move(title));
  menu->addElement(std::move(buttons));
}

MenuScene::MenuScene() : impl(new MenuSceneImpl()) {
  LOG_DEBUG("MenuScene constructor\n");
  impl->menu = this;
  impl->MainMenu();

  GFX_SetBlendMode(MODE_BLEND);
  GFX_EnableAlphaTest(false);
  GFX_TextureMatrix(false);
  GFX_SetWriteBuffers(true, false, false);

  Mtx44 ortho;
  guOrtho(ortho, 0, SCREEN_HEIGHT, 0, SCREEN_WIDTH, 0, 1);
  GFX_Projection(ortho, GX_ORTHOGRAPHIC);

  guVector camPos = { 0.0f, 0.0f, 0.0f };
  guVector camUp = { 0.0f, 1.0f, 0.0f };
  guVector camAt = { 0.0f, 0.0f, -1.0f };
  guLookAt(view, &camPos, &camUp, &camAt);
  
  Mtx model;
  guMtxIdentity(model);
  GFX_ModelViewMatrix(model);
}

// proper pimpl idiom requires destructor
MenuScene::~MenuScene() { }

void MenuScene::reset() { guiElements.clear(); }

void MenuScene::addElement(std::unique_ptr<GuiElement> element) {
  guiElements.push_back(std::move(element));
}

void MenuScene::update(f32 deltatime) {
  for (auto& element : guiElements) element->update(deltatime);
}

void MenuScene::render() {
  for (auto& element : guiElements) element->render();
}
