#ifndef EXMATH_H
#define EXMATH_H
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <gctypes.h>
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

struct orient_t;
struct vec3w_t;

/**
 * Some quick notes:
 * 1. guQuatNormalize is already optimized for fast inverse square root
 * 2. All of the ps_* functions are optimized for SIMD (paired single)
*/

/* Typically, in a formal Mathematical setting, min/max would mean "magnitude,"
 * or the distance from 0. However, the < and > operators in C++ specifically
 * only compare the leftmost and rightmost. */
float minf(float a, float b); // LEFTMOST value
float maxf(float a, float b); // RIGHTMOST value

glm::quat operator-(const orient_t& a, const orient_t& b);
glm::vec3 operator-(const vec3w_t& a, const vec3w_t& b);

orient_t& operator+=(orient_t& a, const orient_t& b);
orient_t& operator-=(orient_t& a, const orient_t& b);

vec3w_t& operator+=(vec3w_t& a, const vec3w_t& b);
vec3w_t& operator-=(vec3w_t& a, const vec3w_t& b);

/// @brief Normalizes the measured value into the range specified by hi-lo.
/// @param lo the lowest possible reading from the Nunchuk
/// @param hi the highest possible reading from the Nunchuk
/// @param me the actual measurement
/// @return Nunchuk accelerometer value to radian conversion.
float angleInRadians(int lo, int hi, long me);

#endif  // EX_MATH