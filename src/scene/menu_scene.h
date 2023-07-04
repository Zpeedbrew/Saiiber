#ifndef MENU_SCENE_H
#define MENU_SCENE_H
#include "scene.h"

class MenuScene : public Scene {
public:
  MenuScene();
  ~MenuScene();
  void init();
  void update(f32 deltatime);
  void render();
};

#endif // MENU_SCENE_H