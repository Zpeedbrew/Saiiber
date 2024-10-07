#ifndef saber_H
#define saber_H
#include "gameobject.h"

class Model;

class Block : public GameObject {
private:
  Mtx texMtx;

public:
  static Model* model;

  Saber();
  ~Saber();
  void update(f32 deltatime);
  void render();

  void setTexMtx(s16 x, s16 y, s8 w, s16 h); //i hope this the c and why beacuse thats what im setting it to
//is this the width. hight
};

#endif // BLOCK_H
