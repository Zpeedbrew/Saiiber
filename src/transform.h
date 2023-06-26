#ifndef TRANSFORM_H
#define TRANSFORM_H
#include <ogc/gu.h>

class Transform {
public:
  guVector position = { 0, 0, 0 };
  guVector rotation = { 0, 0, 0 };
  guVector scale = { 1.0f, 1.0f, 1.0f };
  
  Mtx matrix;

  void update() {
    static guVector xAxis = { 1.0f, 0.0f, 0.0f };
    static guVector yAxis = { 0.0f, 1.0f, 0.0f };
    static guVector zAxis = { 0.0f, 0.0f, 1.0f };

    guMtxIdentity(matrix);
    guMtxTransApply(matrix, matrix, position.x, position.y, position.z);

    if (rotation.x != 0.0f)
      guMtxRotAxisDeg(matrix, &xAxis, rotation.x);
    if (rotation.y != 0.0f)
      guMtxRotAxisDeg(matrix, &yAxis, rotation.y);
    if (rotation.z != 0.0f)
      guMtxRotAxisDeg(matrix, &zAxis, rotation.z);

    guMtxScaleApply(matrix, matrix, scale.x, scale.y, scale.z);
  }

};

#endif // TRANSFORM_H