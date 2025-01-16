#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
#include <gctypes.h>

#include <memory>

#include "../transform.h"

class GameObject {
 public:
  std::shared_ptr<Transform> transform;

  virtual void update(f32 deltatime) { transform->update(); }

  GameObject() : transform(new Transform()) {}
  GameObject(std::shared_ptr<Transform> transform) {
    this->transform = transform;
  }

  GameObject(Transform* transform) : transform(transform) {}

  virtual void render() {}

  virtual ~GameObject() {}
};

#endif  // GAME_OBJECT_H