#ifndef SABER_H
#define SABER_H
#include "gameobject.h"

class Model;

class Saber : public GameObject {
 private:
  static std::unique_ptr<Model> model;
  Mtx texMtx;

 public:
  Saber(std::shared_ptr<Transform> transform);
  void update(f32 deltatime) override;
  void render() override;
};

#endif  // SABER_H