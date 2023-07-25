#ifndef STAGE_OBJECT_H
#define STAGE_OBJECT_H
#include "gameobject.h"

class Model;

class Stage : public GameObject {
private:
  static std::unique_ptr<Model> model;
  Mtx texMtx;

public:
  Stage();
  void update(f32 deltatime);
  void render();
};

#endif