#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "exmath.h"

extern glm::vec3 VEC_FORWARD;
extern glm::vec3 VEC_UP;

class Transform {
 public:
  glm::quat rotation;
  glm::vec3 scale;
  glm::vec3 position;

  glm::mat4 matrix;

  Transform();

  glm::vec3 forward();
  glm::vec3 right();

  // We have an update for the final matrix update
  // That way we don't have to provide a method to calculate the matrix
  // which would have been used in the render thread, rather than the update
  void update();

  bool intersects(Transform other);
  bool intersects(float x, float y, float z, float width, float height,
                  float depth);
  bool intersects(float x, float y, float width, float height);

  /// @brief Adds rotation to current rotation based on a quaternion
  /// @param quaternion The quaternion to multiply by
  void rotate(glm::quat quaternion);

  /// @brief Adds rotation to current rotation based on euler angles in a vector
  /// @param euler glm::vec3 of the pitch, yaw, roll in degrees
  void rotate(glm::vec3 euler);

  /// @brief Adds rotation to current rotation based on euler angles
  /// @param pitch Rotation around the X axis in degrees
  /// @param yaw Rotation around the Y axis in degrees
  /// @param roll Rotation around the Z axis in degrees
  void rotate(float pitch, float yaw, float roll);

  /// @brief Sets the current rotation to the euler angles
  /// @param euler glm::vec3 of the pitch, yaw, roll in degrees
  void setRotation(glm::vec3 euler);

  /// @brief Sets the current rotation to the euler angles
  /// @param pitch Rotation around the X axis in degrees
  /// @param yaw Rotation around the Y axis in degrees
  /// @param roll Rotation around the Z axis in degrees
  void setRotation(float pitch, float yaw, float roll);

  void lerpRotation(float dt, float pitch, float yaw, float roll);
};

#endif  // TRANSFORM_H