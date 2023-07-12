#ifndef MENU_SCENE_H
#define MENU_SCENE_H
#include <vector>

#include "scene.h"
#include "../ui/gui_element.h"

class MenuScene : public Scene {
 private:
  std::vector<GuiElement*> staticElements;
  std::vector<GuiElement*> navElements;
  unsigned int choice = 0;

 public:

  MenuScene();
  ~MenuScene();

  void update(f32 deltatime);
  void render();
};

#endif  // MENU_SCENE_H