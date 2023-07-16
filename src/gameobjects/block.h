#ifndef BLOCK_H
#define BLOCK_H
#include "gameobject.h"

class Model;

class Block : public GameObject {
private:
  static std::unique_ptr<Model> model;
  Mtx texMtx;

public:
  void update(f32 deltatime);
  void render();

  void setTexMtx(s16 x, s16 y, s16 w, s16 h);
};

#endif // BLOCK_H