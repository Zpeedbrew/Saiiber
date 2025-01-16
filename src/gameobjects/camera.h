#ifndef CAMERA_H
#define CAMERA_H
#include "gameobject.h"

class Camera : public GameObject {
 private:
  float fov = 60.0f;
  guVector front;
  guVector up;

  float yaw = -90.0f;
  float pitch = 0.0f;

 public:
  Camera();
  void update(f32 deltatime) override;
  void zoom(float amount);

  inline guVector right() {
    guVector right;
    guVecCross(&front, &up, &right);
    guVecNormalize(&right);
    return right;
  }

#ifdef _DEBUG
  bool turning = false;
  void freecam(f32 deltatime);
  void render() override;
#endif
};

#endif  // CAMERA_H