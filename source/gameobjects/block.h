#ifndef BLOCK_H
#define BLOCK_H
#include "gameobject.h"

class Model;

class Block : public GameObject {
private:
  static std::unique_ptr<Model> model;
  Mtx texMtx;

public:
  Block();
  void update(f32 deltatime);
  void render();
};

#endif // BLOCK_H