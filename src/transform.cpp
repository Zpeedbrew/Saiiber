#include "transform.h"

guVector VEC_FORWARD = {0.0f, 0.0f, -1.0f};
guVector VEC_UP = {0.0f, 1.0f, 0.0f};

Transform::Transform()
    : rotation{0, 0, 0, 1},
      scale{1, 1, 1},
      position{0, 0, 0},
      matrix{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}} {}

bool Transform::intersects(float x, float y, float width, float height) {
  return (position.x + scale.x >= x && position.x <= x + width) &&
         (position.y + scale.y >= y && position.y <= y + height);
}

bool Transform::intersects(float x, float y, float z, float width, float height,
                           float depth) {
  return intersects(x, y, width, height) &&
         (position.z + scale.z >= z && position.z <= z + depth);
}

bool Transform::intersects(Transform other) {
  return intersects(other.position.x, other.position.y, other.position.z,
                    other.scale.x, other.scale.y, other.scale.z);
}

void Transform::update() {
  guMtxIdentity(matrix);
  guMtxTrans(matrix, position.x, position.y, position.z);
  guMtxRotate(matrix, rotation);
  guMtxScale(matrix, scale.x, scale.y, scale.z);
}

guVector Transform::forward() { return rotation * VEC_FORWARD; }

guVector Transform::right() {
  guVector right;
  guVector front = forward();

  guVecCross(&front, &VEC_UP, &right);
  guVecNormalize(&right);
  return right;
}

void Transform::rotate(float yaw, float pitch, float roll) {
  guQuaternion y = {0, sinf(DegToRad(yaw) / 2.0f), 0,
                    cosf(DegToRad(yaw) / 2.0f)};

  guQuaternion p = {sinf(DegToRad(pitch) / 2.0f), 0, 0,
                    cosf(DegToRad(pitch) / 2.0f)};

  guQuaternion r = {0, 0, sinf(DegToRad(roll) / 2.0f),
                    cosf(DegToRad(roll) / 2.0f)};

  // Tested and works as intended and very optimally
  guQuaternion q = { 0, 0, 0, 1 };
  guQuatMultiply(&y, &p, &q);
  guQuatMultiply(&q, &r, &q);
  guQuatMultiply(&rotation, &q, &q);
  guQuatNormalize(&q, &rotation);
}
