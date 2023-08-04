#ifndef EXMATH_H
#define EXMATH_H
/**
 * Extended Math (exmath)
 * Author: TheSunCat
 * A collection of useful mathematical functions and constants.
 */

#include <math.h>
#include <ogc/gu.h>

#undef guQuatMtx
#undef guQuatAdd

#define guQuatMtx _guQuatMtx
#define guQuatAdd _guQuatAdd

/* Typically, in a formal Mathematical setting, min/max would mean "magnitude,"
 * or the distance from 0. However, the < and > operators in C++ specifically
 * only compare the leftmost and rightmost. */
// LEFTMOST value
float minf(float a, float b);
// RIGHTMOST value
float maxf(float a, float b); 

guQuaternion guQuatConjugate(guQuaternion& q);
void _guQuatMtx(guQuaternion& q, Mtx& mtx);

void _guQuatAdd(guQuaternion& q1, guQuaternion& q2, guQuaternion& dst);
void _guQuatAdd(guVector& v, guQuaternion& q, guQuaternion& dst);

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

/* Nunchuk accelerometer value to radian conversion.   Normalizes the measured
 * value into the range specified by hi-lo.  Most of the time lo should be 0,
 * and hi should be 1023. lo         the lowest possible reading from the
 * Nunchuk hi         the highest possible reading from the Nunchuk measured the
 * actual measurement */
float angleInRadians(int lo, int hi, long measured);

#endif  // EX_MATH