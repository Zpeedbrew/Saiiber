#ifndef GAME_SCENE_H
#define GAME_SCENE_H
#include "scene.h"
#include <string>
#include <utility>
#include <vector>

class GameObject;
class BeatmapInfo;
class Beatmap;

enum class Mode;
enum class Rank;

class GameScene : public Scene {
private:
  std::vector<GameObject*> gameObjects;
  Beatmap* beatmap;

public:
  GameScene(std::pair<std::string, BeatmapInfo> pair, Mode mode, Rank rank);
  ~GameScene();

  void init();
  void update(f32 deltatime);
  void render();
};

#endif // GAME_SCENE_H