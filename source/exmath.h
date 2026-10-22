#ifndef EXMATH_H
#define EXMATH_H
/**
 * Extended Math (exmath)
 * Author: TheSunCat
 * A collection of useful mathematical functions and constants.
*/

#include <gccore.h>
#include <math.h>

const double PI = 3.14159265358979323846;

guVector operator+(const guVector& left, const guVector& right);
guVector operator-(const guVector& left, const guVector& right);
guVector operator/(const guVector& left, const float right);
guVector operator*(const guVector& left, const float scalar);

bool operator!=(guVector& left, guVector& right);

guVector& operator+=(guVector& left, guVector right);
guVector& operator-=(guVector& left, guVector right);

int millis();

void sleep_for(int ms);

// Block until A is pressed
void pressA(int wiimote = 0);

/* Nunchuk accelerometer value to radian conversion.   Normalizes the measured value
 * into the range specified by hi-lo.  Most of the time lo should be 0, and hi should be 1023.
 * lo         the lowest possible reading from the Nunchuk
 * hi         the highest possible reading from the Nunchuk
 * measured   the actual measurement */
float angleInRadians(int lo, int hi, long measured);

#endif // EX_MATH