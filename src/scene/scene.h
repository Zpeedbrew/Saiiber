#ifndef SCENE_H
#define SCENE_H
#include "../gfx.h"
#include <gctypes.h>

class Scene {
protected:
  bool loaded = false;

public:
  static Scene* currentScene;
  static Scene* overlayScene;

  virtual ~Scene() { }

  inline static void SetScene(Scene* scene) {
    if (currentScene != NULL)
      delete currentScene;
    currentScene = scene;
    currentScene->init();
  }

  inline static void SetOverlayScene(Scene* scene) {
    if (overlayScene != NULL)
      delete overlayScene;
    overlayScene = scene;
    overlayScene->init();
  }

  inline static void Update(f32 deltatime) {
    if (overlayScene != NULL)
      overlayScene->update(deltatime);
    else
      currentScene->update(deltatime);
  }

  inline static void Render() {
    if (overlayScene != NULL)
      overlayScene->render();
    else
      currentScene->render();
  }

  virtual void init() { }
  virtual void update(f32 deltatime) { }
  virtual void render() { }

  inline bool isLoaded() { return loaded; }
};

#endif // SCENE_H