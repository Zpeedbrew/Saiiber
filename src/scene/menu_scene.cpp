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
#include "../transform.h"
#include "../ui.h"
#include "game_scene.h"
#define SONGPATH "sd:/Songs"

const int GREEN = 0x00FF00FF;
const int WHITE = 0xFFFFFFFF; //the text on this is by piture so the color dose not actully matter

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
  s16 middle = (SCREEN_WIDTH /  1.5)- (width / 2);
  auto title =
      std::make_unique<GuiText>("Difficulty Select", middle, 100, 1.5);
  title->setPosition(middle, 100);
  title->setColor(GREEN);

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
    buttons->add("Impossible", 100, 200 + (50 * yIdx++));

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

/* TODO: Make Modifiers
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
*/

  buttons->onButtonPressed([=](int button, u32 choice, GuiButton& element) {
    if (button == WIIMOTE_BUTTON_B) {
      SongSelect();
    return;
    }
    if (button == WIIMOTE_BUTTON_1) {
       Scene::ChangeScene<DebugScene>()
      return;   
    }    
    
    if (button == WIIMOTE_BUTTON_2) {
    Scene::ChangeScene<LoadingScene>()
      return;
    }

    if (button == WIIMOTE_BUTTON_2) {
    return -1;
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

  int width = FNT_GetStringWidth("Saiiber", 3.0f);
  s16 middle = (SCREEN_WIDTH / 2) - (width / 2);
  auto title = std::make_unique<GuiText>("Saiiber", middle, 100, 3.0f);
  title->setPosition(middle, 100);
  title->setColor(0xFF0000FF);

  auto buttons = std::make_unique<GuiList>();
  buttons->add("Quickplay", 100, 200, 1.0f);
  buttons->add("Versus", 100, 250);
  buttons->add("Practice", 100, 300);
  buttons->add("Settings", 100, 350);
  buttons->add("Quit", 100, 400  );


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

  GFX_Texture(TEX_NONE);
  GFX_SetBlendMode(MODE_BLEND);
  GFX_EnableAlphaTest(false);
  GFX_SetWriteBuffers(true, false, false);

  glm::mat4 ortho = glm::transpose(
      glm::ortho(0.0f, (f32)SCREEN_WIDTH, (f32)SCREEN_HEIGHT, 0.0f));
  GFX_Projection(ortho, GX_ORTHOGRAPHIC);

  glm::vec3 camPos(0.0f, 0.0f, 0.0f);
  glm::vec3 camUp(0.0f, 1.0f, 0.0f);
  glm::vec3 camAt(0.0f, 0.0f, -1.0f);
  view = glm::lookAt(camPos, camAt, camUp);

  glm::mat4 origin(1.0f);
  GFX_ModelMatrix(origin);
}

// proper pimpl idiom requires destructor
MenuScene::~MenuScene() {}

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
/*
void MainMenu::quit(){
  if (button == WIIMOTE_BUTTON_HOME) {
    return -1;
     }
  }
*/
/*
void MainMenu::Debug(){
 if (button == WIIMOTE_BUTTON_1) {
   Scene::ChangeScene<DebugScene>()
    }
if (button == WIIMOTE_BUTTON_2) {
   Scene::ChangeScene<LoadingScene>()
    }
}
*/
