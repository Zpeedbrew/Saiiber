#include "exmath.h"

#include <wiiuse/wiiuse.h>

float minf(float a, float b) {
  if (a < b) return a;
  return b;
}

float maxf(float a, float b) {
  if (a > b) return a;
  return b;
}

glm::quat operator-(const orient_t &a, const orient_t &b) {
  // Yaw of the Quat is the Pitch of the Orientation
  // Pitch of the Quat is the Yaw of the Orientation
  // Roll is the same for both
  return glm::quat(glm::vec3(glm::radians(a.pitch - b.pitch),
                             glm::radians(a.yaw - b.yaw),
                             glm::radians(a.roll - b.roll)));
}

glm::vec3 operator-(const vec3w_t &a, const vec3w_t &b) {
  return glm::vec3((f32)a.x - (f32)b.x, (f32)a.y - (f32)b.y,
                   (f32)a.z - (f32)b.z);
}

orient_t &operator+=(orient_t &a, const orient_t &b) {
  a.yaw += b.yaw;
  a.pitch += b.pitch;
  a.roll += b.roll;
  return a;
}

orient_t &operator-=(orient_t &a, const orient_t &b) {
  a.yaw -= b.yaw;
  a.pitch -= b.pitch;
  a.roll -= b.roll;
  return a;
}

vec3w_t &operator+=(vec3w_t &a, const vec3w_t &b) {
  a.x += b.x;
  a.y += b.y;
  a.z += b.z;
  return a;
}

vec3w_t &operator-=(vec3w_t &a, const vec3w_t &b) {
  a.x -= b.x;
  a.y -= b.y;
  a.z -= b.z;
  return a;
}

float angleInRadians(int lo, int hi, long measured) {
  float x = (hi - lo);
  return (float)(measured / x) * M_PI;
}