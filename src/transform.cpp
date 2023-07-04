#include "transform.h"
#include <ogc/gx.h>

static guVector xAxis = { 1.0f, 0.0f, 0.0f };
static guVector yAxis = { 0.0f, 1.0f, 0.0f };
static guVector zAxis = { 0.0f, 0.0f, 1.0f };

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

void Transform::update() {
  guMtxIdentity(matrix);
  guMtxTrans(matrix, position.x, position.y, position.z);

  if (rotation.x != 0.0f)
    guMtxRotAxisDeg(matrix, &xAxis, rotation.x);
  if (rotation.y != 0.0f)
    guMtxRotAxisDeg(matrix, &yAxis, rotation.y);
  if (rotation.z != 0.0f)
    guMtxRotAxisDeg(matrix, &zAxis, rotation.z);

  guMtxScale(matrix, scale.x, scale.y, scale.z);
}