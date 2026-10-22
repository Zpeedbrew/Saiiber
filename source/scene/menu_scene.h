#ifndef MENU_SCENE_H
#define MENU_SCENE_H
#include <vector>
#include <memory>

#include "scene.h"
#include "../ui/gui_element.h"

struct MenuSceneImpl;

class MenuScene : public Scene {
 private:
  std::vector<std::unique_ptr<GuiElement>> guiElements;
  std::unique_ptr<MenuSceneImpl> impl;

 public:
  MenuScene();
  ~MenuScene();

  void addElement(std::unique_ptr<GuiElement> element);

  void update(f32 deltatime);
  void render();
  void reset();
};

#endif  // MENU_SCENE_H