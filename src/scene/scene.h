#ifndef SCENE_H
#define SCENE_H
#include <gctypes.h>

#include <memory>

class Scene {
 protected:
  bool loaded = false;

 public:
  static std::unique_ptr<Scene> currentScene;
  static std::unique_ptr<Scene> overlayScene;

  virtual ~Scene() {}

  template <class T, typename... Args>
  inline static void ChangeScene(Args&&... args) {
    currentScene = std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    currentScene->init();
  }

  template <class T>
  inline static void ChangeScene(T* scene) {
    currentScene.reset(scene);
    currentScene->init();
  }

  template <class T>
  inline static void SetOverlayScene(T* scene) {
    overlayScene.reset(scene);
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

  virtual void init() {}
  virtual void update(f32 deltatime) {}
  virtual void render() {}

  inline bool isLoaded() { return loaded; }
};

#endif  // SCENE_H