#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "exmath.h"

extern guVector VEC_FORWARD;
extern guVector VEC_UP;

class Transform {
 public:
  guQuaternion rotation;
  guVector scale;
  guVector position;

  Mtx matrix;

  Transform();
  
  guVector forward();
  guVector right();

  // We have an update for the final matrix update
  // That way we don't have to provide a method to calculate the matrix
  // which would have been used in the render thread, rather than the update
  void update();

  bool intersects(Transform other);
  bool intersects(float x, float y, float z, float width, float height,
                  float depth);
  bool intersects(float x, float y, float width, float height);

  // Rotates transform based on euler angles in degrees
  void rotate(float yaw, float pitch, float roll);
};

#endif  // TRANSFORM_H