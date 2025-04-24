#ifndef SABER_H
#define SABER_H
#include "gameobject.h"
#include <ogc/gx.h>

class Model;

class Saber:public GameObject {
 private:
  static std::unique_ptr<Model>model;
  glm::mat4 texMtx;
  GXColor color;

  bool inView;
  glm::u16vec4 ppSource;

 public:
  Saber(std::shared_ptr<Transform>transform);
  void update(f32 deltatime)override;
  void render()override;
  void setColor(u8 r,u8 g,u8 b,u8 a);
  void postProcess(void*buf);
};
#endif  // SABER_H