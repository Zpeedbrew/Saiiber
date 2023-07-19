#include "transform.h"
#include <ogc/gx.h>

guVector copy(const guVector v) {
  return { v.x, v.y, v.z };
}

bool Transform::intersects(float x, float y, float width, float height) {
  return (position.x + scale.x >= x && position.x <= x + width) &&
    (position.y + scale.y >= y && position.y <= y + height);
}

bool Transform::intersects(float x, float y, float z, float width, float height, float depth) {
  return intersects(x, y, width, height)
    && (position.z + scale.z >= z && position.z <= z + depth);
}

bool Transform::intersects(Transform other) {
  return intersects(other.position.x, other.position.y, other.position.z,
    other.scale.x, other.scale.y, other.scale.z);
}

guVector xAxis = { 1.0f, 0.0f, 0.0f };
guVector yAxis = { 0.0f, 1.0f, 0.0f };
guVector zAxis = { 0.0f, 0.0f, 1.0f };

void Transform::update() {
  guMtxIdentity(matrix);
  guMtxTrans(matrix, position.x, position.y, position.z);
  guMtxRotAxisDeg(matrix, &xAxis, rotation.x);
  guMtxRotAxisDeg(matrix, &yAxis, rotation.y);
  guMtxRotAxisDeg(matrix, &zAxis, rotation.z);
  guMtxScale(matrix, scale.x, scale.y, scale.z);
}