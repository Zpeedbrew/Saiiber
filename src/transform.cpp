#include "transform.h"

#include "gfx.h"
#include "logger.h"

glm::vec3 VEC_FORWARD(0.0f, 0.0f, -1.0f);
glm::vec3 VEC_UP(0.0f, 1.0f, 0.0f);

Transform::Transform()
    : rotation(1.0f, 0.0f, 0.0f, 0.0f),
      scale(1.0f, 1.0f, 1.0f),
      position(0.0f, 0.0f, 0.0f),
      matrix(1.0f) {}

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
  // This is verifiably correct!
  matrix = glm::translate(position) * glm::toMat4(rotation) * glm::scale(scale);
}

glm::vec3 Transform::forward() {
  glm::vec3 f = glm::normalize(rotation * VEC_FORWARD);
  return f;
}

glm::vec3 Transform::right() {
  glm::vec3 r = glm::normalize(glm::cross(forward(), VEC_UP));
  return r;
}

void Transform::rotate(glm::quat quaternion) {
  rotation = glm::normalize(rotation * quaternion);
}

void Transform::rotate(glm::vec3 euler) {
  rotate(glm::quat(glm::radians(euler)));
}

void Transform::rotate(float pitch, float yaw, float roll) {
  rotate(glm::vec3(pitch, yaw, roll));
}

void Transform::setRotation(glm::vec3 euler) {
  rotation = glm::normalize(glm::quat(glm::radians(euler)));
}

void Transform::setRotation(float pitch, float yaw, float roll) {
  setRotation(glm::vec3(pitch, yaw, roll));
}

void Transform::lerpRotation(float dt, float yaw, float pitch, float roll) {
  rotation = glm::normalize(
      glm::lerp(rotation,
                glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw),
                                    glm::radians(roll))),
                dt));
}
