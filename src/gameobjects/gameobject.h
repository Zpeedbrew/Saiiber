#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
#include <gctypes.h>

#include <memory>

#include "../transform.h"

class GameObject {
 public:
  std::shared_ptr<Transform>transform;

  GameObject():transform(new Transform()){}
  GameObject(Transform*transform):transform(transform){}
  GameObject(std::shared_ptr<Transform>transform){
    this->transform=transform;
  }

  virtual void update(f32 deltatime){transform->update();}
  virtual void render(){}
  virtual ~GameObject(){}
};
#endif  // GAME_OBJECT_H