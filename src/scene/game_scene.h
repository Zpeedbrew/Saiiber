#ifndef GAME_SCENE_H
#define GAME_SCENE_H
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../resource/beatmap.h"
#include "scene.h"

class GameObject;
class Saber;

class GameScene : public Scene {
 private:
  std::shared_ptr<Saber> redSaber;
  std::shared_ptr<Saber> blueSaber;

  std::vector<std::shared_ptr<GameObject>> gameObjects;
  Beatmap beatmap;

 public:
  GameScene(std::string directory, BeatmapInfo info, Mode mode, Rank rank);
  ~GameScene();

  void init();
  void update(f32 deltatime);
  void render();
};

#endif  // GAME_SCENE_H