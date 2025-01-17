#ifndef LOADING_SCENE_H
#define LOADING_SCENE_H
#include "scene.h"

template <typename T>
class LoadingScene : public Scene {
 private:
  T* nextScene;

 public:
  LoadingScene(T* nextScene);

  void init();
  void update(f32 deltatime);
  void render();
};

#endif  // LOADING_SCENE_H
