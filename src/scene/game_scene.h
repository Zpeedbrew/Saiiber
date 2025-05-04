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
class Camera;
class Block;
class GameScene : public Scene {
private:
std::shared_ptr<Camera>camera;
std::shared_ptr<Saber>redSaber;
std::shared_ptr<Saber>blueSaber;
//debugging purposes
std::shared_ptr<Block>block;
std::vector<std::shared_ptr<GameObject>>gameObjects;
Beatmap beatmap;
Mode mode;
bool calibrated=false;
public:
GameScene(std::string directory,BeatmapInfo info,Mode mode,Rank rank);
~GameScene();
void init();
void update(f32 deltatime);
void render();
void postProcess(void* buf);
};
#endif  // GAME_SCENE_H