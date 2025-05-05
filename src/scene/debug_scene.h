#ifndef DEBUG_SCENE_H
#define DEBUG_SCENE_H
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../exmath.h"
#include "scene.h"

class Camera;
class Transform;

class DebugScene : public Scene {
private:
  std::shared_ptr<Camera> camera;
  std::shared_ptr<Transform> modelTransform;

public:
  DebugScene();

  void init();
  void update(f32 deltatime);
  void render();
};

#endif // DEBUG_SCENE_H