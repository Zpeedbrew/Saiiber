#ifndef LOADING_SCENE_H
#define LOADING_SCENE_H
#include "scene.h"

class LoadingScene : public Scene {
private:
  Scene* nextScene;

public:
  LoadingScene(Scene* nextScene);
  ~LoadingScene();
  
  void init();
  void update(f32 deltatime);
  void render();
};

#endif // LOADING_SCENE_H