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

void _guQuatMtx(guQuaternion& q, Mtx& mtx) {
  double sqw = q.w * q.w;
  double sqx = q.x * q.x;
  double sqy = q.y * q.y;
  double sqz = q.z * q.z;

  // inverse square length is only required if not already normalised
  // since sqw + sqx + sqy + sqz = 1 / invs * invs
  double invs = 1 / (sqx + sqy + sqz + sqw);
  mtx[0][0] = (sqx - sqy - sqz + sqw) * invs;
  mtx[1][1] = (-sqx + sqy - sqz + sqw) * invs;
  mtx[2][2] = (-sqx - sqy + sqz + sqw) * invs;

  double tmp1 = q.x * q.y;
  double tmp2 = q.z * q.w;
  mtx[1][0] = 2.0 * (tmp1 + tmp2) * invs;
  mtx[0][1] = 2.0 * (tmp1 - tmp2) * invs;

  tmp1 = q.x * q.z;
  tmp2 = q.y * q.w;
  mtx[2][0] = 2.0 * (tmp1 - tmp2) * invs;
  mtx[0][2] = 2.0 * (tmp1 + tmp2) * invs;

  tmp1 = q.y * q.z;
  tmp2 = q.x * q.w;
  mtx[2][1] = 2.0 * (tmp1 + tmp2) * invs;
  mtx[1][2] = 2.0 * (tmp1 - tmp2) * invs;
}

void _guQuatAdd(guQuaternion& q1, guQuaternion& q2, guQuaternion& dst) {
  dst.x = q1.x + q2.x;
  dst.y = q1.y + q2.y;
  dst.z = q1.z + q2.z;
  dst.w = q1.w + q2.w;
}

void _guQuatAdd(guVector& v, guQuaternion& q, guQuaternion& dst) {
  // create pseudo quaternion from V
  guQuaternion vq{v.x, v.y, v.z, 0};
  _guQuatAdd(vq, q, dst);
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