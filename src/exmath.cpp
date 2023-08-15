#include "exmath.h"

float minf(float a, float b) {
  if (a < b) return a;
  return b;
}

float maxf(float a, float b) {
  if (a > b) return a;
  return b;
}

float angleInRadians(int lo, int hi, long measured) {
  float x = (hi - lo);
  return (float)(measured / x) * M_PI;
}