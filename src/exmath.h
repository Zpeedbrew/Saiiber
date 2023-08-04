#ifndef EXMATH_H
#define EXMATH_H
#include <math.h>
#include <ogc/gu.h>

#undef guQuatMtx

#define guMtxRotate _guQuatMtx
#define guQuatMtx _guQuatMtx

/**
 * Some quick notes:
 * 1. guQuatNormalize is already optimized for fast inverse square root
 * 2. All of the ps_* functions are optimized for SIMD (paired single)
*/

/* Typically, in a formal Mathematical setting, min/max would mean "magnitude,"
 * or the distance from 0. However, the < and > operators in C++ specifically
 * only compare the leftmost and rightmost. */
// LEFTMOST value
float minf(float a, float b);
// RIGHTMOST value
float maxf(float a, float b); 

guQuaternion guQuatConjugate(guQuaternion& q);

// assumes the quaternion is normalized
void _guQuatMtx(Mtx& mtx, guQuaternion& q);

void guQuatAddVec(guVector& v, guQuaternion& q, guQuaternion& dst);

guVector operator-(const guVector& v);

guVector operator+(const guVector& left, const guVector& right);
guVector operator-(const guVector& left, const guVector& right);
guVector operator/(const guVector& v, const float scalar);
guVector operator*(const float scalar, const guVector& v);

guVector operator*(const guQuaternion& left, const guVector& right);

bool operator==(guVector& left, guVector& right);
bool operator!=(guVector& left, guVector& right);

guVector& operator*=(guVector& v, const float scalar);
guVector& operator*=(guVector& left, guVector& right);

guVector& operator+=(guVector& left, guVector right);
guVector& operator-=(guVector& left, guVector right);


/// @brief Normalizes the measured value into the range specified by hi-lo.
/// @param lo the lowest possible reading from the Nunchuk
/// @param hi the highest possible reading from the Nunchuk
/// @param me the actual measurement
/// @return Nunchuk accelerometer value to radian conversion.
float angleInRadians(int lo, int hi, long me);

#endif  // EX_MATH