#include "menu_scene.h"

#include <dirent.h>
#include <string.h>

#include <map>
#include <vector>

#include "../fnt.h"
#include "../input.h"
#include "../logger.h"
#include "../resource/beatmap.h"
#include "../ui/button.h"
#include "../ui/text.h"
#include "game_scene.h"

#define SONGPATH "sd:/Songs"

const int GREEN = 0x00FF00FF;
const int WHITE = 0xFFFFFFFF;

struct ButtonData {
  const char* text;
  s16 pos[2];
};

BeatmapList loadSongs() {
  BeatmapList beatmaps;

  // list directory
  DIR* dir = opendir(SONGPATH);
  if (dir == NULL) {
    LOG_FATAL("Failed to open songs directory!\n");
    return beatmaps;
  }

  struct dirent* ent;
  while ((ent = readdir(dir)) != NULL) {
    if (ent->d_type == DT_DIR) {
      // check for beatmap
      char path[256];
      sprintf(path, "%s/%s", SONGPATH, ent->d_name);

      BeatmapInfo info;
      if (GetInfoFromDir(path, info) != 0) {
        LOG_DEBUG("Failed to load beatmap in %s\n", ent->d_name);
        continue;
      }

      beatmaps.push_back(std::make_pair(std::string(path), info));
    }
  }

  return beatmaps;
}

class BasicMenu {
 public:
  GuiText title;
  std::vector<GuiButton> buttons;
  int choice = 0;

  BasicMenu(const char* name) : title(name, 100, 100, 2.0f) {}

  virtual void update(f32 delta) {
    if (Input::isButtonDown(WPAD_BUTTON_UP) && choice > 0)
      choice -= 1;

    if (Input::isButtonDown(WPAD_BUTTON_DOWN) && choice < buttons.size() - 1)
      choice += 1;

    for (u32 i = 0; i < buttons.size(); i++)
      buttons.at(i).setHovered(choice == i);
  }

  virtual void render() {
    title.render();

    for (auto& button : buttons) button.render();
  }

  virtual ~BasicMenu(){};
};

static BasicMenu* curMenu = NULL;

void SetMenu(BasicMenu* menu) {
  if (curMenu != NULL) delete curMenu;
  curMenu = menu;
}

class SongSelect;  // forward declaration

class DifficultySelect : public BasicMenu {
 private:
  SongSelect* select;
  BeatmapPair song;
  Mode mode;

 public:
  DifficultySelect(SongSelect* select, BeatmapPair song, Mode mode)
      : BasicMenu("Difficulty Select"), select(select), song(song), mode(mode) {
    int width = FNT_GetStringWidth("Difficulty Select", 2.0f);
    s16 middle = (SCREEN_WIDTH / 2) - (width / 2);
    title.setPosition(middle, 100);
    title.setColor(0xFF0000FF);

    int diffFlags = song.second.getDifficulties(mode);
    int yIdx = 0;

    if (diffFlags & (1 << (int)Rank::Easy))
      buttons.push_back(GuiButton("Easy", 100, 200 + (50 * yIdx++)));

    if (diffFlags & (1 << (int)Rank::Normal))
      buttons.push_back(GuiButton("Normal", 100, 200 + (50 * yIdx++)));

    if (diffFlags & (1 << (int)Rank::Hard))
      buttons.push_back(GuiButton("Hard", 100, 200 + (50 * yIdx++)));

    if (diffFlags & (1 << (int)Rank::Expert))
      buttons.push_back(GuiButton("Expert", 100, 200 + (50 * yIdx++)));

    if (diffFlags & (1 << (int)Rank::ExpertPlus))
      buttons.push_back(GuiButton("Expert+", 100, 200 + (50 * yIdx++)));
  }

  void update(f32 delta) override;
};

class ModeSelect : public BasicMenu {
 private:
  SongSelect* select;
  BeatmapPair song;

 public:
  ModeSelect(SongSelect* select, BeatmapPair song)
      : BasicMenu("Mode Select"), select(select), song(song) {
    int width = FNT_GetStringWidth("Mode Select", 2.0f);
    s16 middle = (SCREEN_WIDTH / 2) - (width / 2);
    title.setPosition(middle, 100);
    title.setColor(0xFF0000FF);

    int yIdx = 0;
    int modeFlags = song.second.getModes();

    if (modeFlags & (1 << (int)Mode::Standard))
      buttons.push_back(GuiButton("Standard", 100, 200 + (yIdx++ * 50)));

    if (modeFlags & (1 << (int)Mode::OneSaber))
      buttons.push_back(GuiButton("One Saber", 100, 200 + (yIdx++ * 50)));

    if (modeFlags & (1 << (int)Mode::NoArrows))
      buttons.push_back(GuiButton("No Arrows", 100, 200 + (yIdx++ * 50)));

    if (modeFlags & (1 << (int)Mode::ThreeSixty))
      buttons.push_back(GuiButton("360", 100, 200 + (yIdx++ * 50)));

    if (modeFlags & (1 << (int)Mode::Ninety))
      buttons.push_back(GuiButton("90", 100, 200 + (yIdx++ * 50)));

    if (modeFlags & (1 << (int)Mode::Lightshow))
      buttons.push_back(GuiButton("Lightshow", 100, 200 + (yIdx++ * 50)));

    if (modeFlags & (1 << (int)Mode::Lawless))
      buttons.push_back(GuiButton("Lawless", 100, 200 + (yIdx++ * 50)));
  }

  virtual void update(f32 delta) override;
};

class SongSelect : public BasicMenu {
 private:
  std::vector<BeatmapPair> beatmaps;

 public:
  SongSelect() : BasicMenu("Song Select") {
    int width = FNT_GetStringWidth("Song Select", 2.0f);
    s16 middle = (SCREEN_WIDTH / 2) - (width / 2);
    title.setPosition(middle, 100);
    title.setColor(0xFF0000FF);

    beatmaps = loadSongs();

    for (u32 i = 0; i < beatmaps.size(); i++) {
      buttons.push_back(GuiButton(beatmaps[i].second._songName.c_str(), 100,
                                  200 + (i * 50), 0.8f));
    }
  }

  void update(f32 delta) override;
};

class MainMenu : public BasicMenu {
 public:
  MainMenu() : BasicMenu("Saiiber") {
    int width = FNT_GetStringWidth("Saiiber", 2.0f);
    s16 middle = (SCREEN_WIDTH / 2) - (width / 2);
    title.setPosition(middle, 100);
    title.setColor(0xFF0000FF);

    buttons.push_back(GuiButton("Quickplay", 100, 200));
    buttons.push_back(GuiButton("Versus", 100, 250));
    buttons.push_back(GuiButton("Practice", 100, 300));
    buttons.push_back(GuiButton("Settings", 100, 350));
  }

  virtual void update(f32 delta) override {
    BasicMenu::update(delta);

    if (Input::isButtonDown(WIIMOTE_BUTTON_A)) {
      switch (choice) {
        case 0:  // Quickplay
        case 1:  // Versus
        case 2:  // Practice
          SetMenu(new SongSelect());
          break;
        case 3:  // Settings
          break;
      }
    }
  }
};

MenuScene::MenuScene() {
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

  SetMenu(new MainMenu());
}

MenuScene::~MenuScene() { delete curMenu; }

void MenuScene::update(f32 deltatime) {
  if (curMenu != NULL) curMenu->update(deltatime);
}

void MenuScene::render() {
  if (curMenu != NULL) curMenu->render();
}

void SongSelect::update(f32 delta) {
  BasicMenu::update(delta);

  if (Input::isButtonDown(WIIMOTE_BUTTON_B)) {
    SetMenu(new MainMenu());
    return;
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_A)) {
    BeatmapPair song = beatmaps[choice];

    // Preserve the SongSelect menu so we don't have to reload it every time
    // we go back
    SongSelect* prev = (SongSelect*)curMenu;
    curMenu = NULL;

    SetMenu(new ModeSelect(prev, song));
  }
}

void ModeSelect::update(f32 delta) {
  BasicMenu::update(delta);

  if (Input::isButtonDown(WIIMOTE_BUTTON_B)) {
    SetMenu(select);
    return;
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_A)) {
    Mode mode = ModeFromString(buttons[choice].getText());
    SetMenu(new DifficultySelect(select, song, mode));
  }
}

void DifficultySelect::update(f32 delta) {
  BasicMenu::update(delta);

  if (Input::isButtonDown(WIIMOTE_BUTTON_B)) {
    SetMenu(new ModeSelect(select, song));
    return;
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_A)) {
    Rank rank = RankFromString(buttons[choice].getText());
    Scene::SetScene(new GameScene(song, mode, rank));
    delete select;
  }
}