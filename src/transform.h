#ifndef TRANSFORM_H
#define TRANSFORM_H
#include <ogc/gu.h>

class Transform {
public:
  guVector position = { 0, 0, 0 };
  guVector rotation = { 0, 0, 0 };
  guVector scale = { 1.0f, 1.0f, 1.0f };
  
  Mtx matrix;

  bool intersects(Transform other);
  bool intersects(float x, float y, float z, float width, float height, float depth);
  bool intersects(float x, float y, float width, float height);

  void update();
};

#endif // TRANSFORM_H