#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
#include <gctypes.h>
#include "../transform.h"

class GameObject {
public:
  Transform transform;

  void update(f32 deltatime) {
    transform.update();
  }

  virtual void render() { }
  virtual ~GameObject() { }
};

#endif // GAME_OBJECT_H