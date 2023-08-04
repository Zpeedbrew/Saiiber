#include "exmath.h"

float minf(float a, float b) {
  if (a < b) return a;
  return b;
}

float maxf(float a, float b) {
  if (a > b) return a;
  return b;
}

guQuaternion guQuatConjugate(guQuaternion& q) {
  return {-q.x, -q.y, -q.z, q.w};
}

void _guQuatMtx(Mtx& mtx, guQuaternion& q) {
  f32 ww = q.w * q.w;
  f32 xx = q.x * q.x;
  f32 yy = q.y * q.y;
  f32 zz = q.z * q.z;

  mtx[0][0] = xx - yy - zz + ww;
  mtx[1][1] = -xx + yy - zz + ww;
  mtx[2][2] = -xx - yy + zz + ww;

  f32 tmp1 = q.x * q.y;
  f32 tmp2 = q.z * q.w;
  mtx[1][0] = 2.0f * (tmp1 + tmp2);
  mtx[0][1] = 2.0f * (tmp1 - tmp2);

  tmp1 = q.x * q.z;
  tmp2 = q.y * q.w;
  mtx[2][0] = 2.0f * (tmp1 - tmp2);
  mtx[0][2] = 2.0f * (tmp1 + tmp2);

  tmp1 = q.y * q.z;
  tmp2 = q.x * q.w;
  mtx[2][1] = 2.0f * (tmp1 + tmp2);
  mtx[1][2] = 2.0f * (tmp1 - tmp2);
}

void guQuatAddVec(guVector& v, guQuaternion& q, guQuaternion& dst) {
  // create pseudo quaternion from V
  guQuaternion vq{v.x, v.y, v.z, 0};
  guQuatAdd(&vq, &q, &dst);
}

guVector operator+(const guVector& left, const guVector& right) {
  return {left.x + right.x, left.y + right.y, left.z + right.z};
}

guVector operator-(const guVector& left, const guVector& right) {
  return {left.x - right.x, left.y - right.y, left.z - right.z};
}

guVector operator/(const guVector& v, const float scalar) {
  return { v.x / scalar, v.y / scalar, v.z / scalar};
}

guVector operator*(const float scalar, const guVector& v) {
  return {scalar * v.x, scalar * v.y, scalar * v.z};
}

guVector operator*(const guQuaternion& left, const guVector& right) {
  guVector xyz = {left.x, left.y, left.z};

  guVector t;
  guVecCross(&xyz, &right, &t);
  t *= 2.0f;

  guVector ct;
  guVecCross(&xyz, &t, &ct);

  return right + (left.w * t) + ct;
}

bool operator==(guVector& left, guVector& right) {
  return left.x == right.x && left.y == right.y && left.z == right.z;
}

bool operator!=(guVector& left, guVector& right) {
  return !(left == right);
}

guVector& operator*=(guVector& v, const float scalar) {
  v.x *= scalar;
  v.y *= scalar;
  v.z *= scalar;
  return v;
}

guVector& operator*=(guVector& left, guVector& right) {
  left.x *= right.x;
  left.y *= right.y;
  left.z *= right.z;
  return left;
}

guVector operator-(const guVector& v) {
  return { -v.x, -v.y, -v.z };
}

guVector& operator+=(guVector& left, guVector right) {
  left.x += right.x;
  left.y += right.y;
  left.z += right.z;
  return left;
}

guVector& operator-=(guVector& left, guVector right) {
  left.x -= right.x;
  left.y -= right.y;
  left.z -= right.z;
  return left;
}

float angleInRadians(int lo, int hi, long measured) {
  float x = (hi - lo);
  return (float)(measured / x) * M_PI;
}