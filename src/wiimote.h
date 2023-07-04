#ifndef WIIMOTE_H_INCLUDED
#define WIIMOTE_H_INCLUDED

#include <gccore.h>

#include "transform.h"

// code from https://arduino-projects4u.com/wii-motion-plus/
struct GyroKalman {
  double x_angle, x_bias;  // These variables represent our state matrix x
  double P_00, P_01, P_10, P_11;  // Our error covariance matrix
  /* Q is a 2x2 matrix of the covariance. Because we assume the gyro and
   * accelerometer noise to be independent of each other, the covariances on the
   * / diagonal are 0. Covariance Q, the process noise, from the assumption x =
   * F x + B u + w with w having a normal distribution with covariance Q.
   * (covariance = E[ (X - E[X])*(X - E[X])' ] We assume is linear with dt   */
  double Q_angle, Q_gyro;
  double R_angle;  // Covariance R, our observation noise (from the
                   // accelerometer) Also assumed to be linear with dt
};

struct RungeKutta {
  double val_i_3;
  double val_i_2;
  double val_i_1;
  double previous;
};

static const double radianToDegree = 57.2957795;    // 180/PI
static const double degreeToRadian = 0.0174532925;  // PI/180

// The WMP has two modes: 2000 deg/s (slow) and 500 deg/s (fast).  It is assumed
// that the sensitivity of the output follows the same ratio (it seems that the
// sensitivity is 20 mV/deg/s and 0.5 mV/deg/s), which is to say it's
// proportional to 4:1.
static const int wmpSlowToDegreePerSec = 16;
static const int wmpFastToDegreePerSec = wmpSlowToDegreePerSec / 4;

// WM+ variables  R represents the measurement covariance noise.
// In this case, it is a 1x1 matrix that says that we expect 0.3 rad jitter from
// the accelerometer.
static const float R_angle = .3;  //.3 default

// Q is a 2x2 matrix that represents the process covariance noise. In this case,
// it indicates how much we trust the acceleromter relative to the gyros. You
// should play with different values here as the effects are interesting.  Over
// prioritizing the accelerometers results in fairly inaccurate results.
static const double Q_angle = 0.002;  //(Kalman)
static const double Q_gyro = 0.1;     //(Kalman)

class Wiimote {
 public:
  Wiimote(int channel);

  void update(f32 dt);

  bool buttonDown(u32 button);

  Transform transform;
  guVector orient;

 private:
  void calibrateZeroes();

  void readData();
  void rawReadData();

  /* Initialize the kalman structures.
   * kalman    the kalman data structure
   * Q_angle   the process covariance noise for the accelerometers
   * Q_gyro    the process covariance noise for the gyros
   * R_angle   the measurement covariance noise (jitter in the accelerometers)
   */
  void initGyroKalman(struct GyroKalman *kalman, const float Q_angle,
                      const float Q_gyro, const float R_angle);

  /*
   * The kalman predict method.  See
   * http://en.wikipedia.org/wiki/Kalman_filter#Predict kalman    the kalman
   * data structure dotAngle  Derivitive Of The (D O T) Angle.  This is the
   * change in the angle from the gyro.  This is the value from the Wii
   * MotionPlus, scaled to fast/slow. dt        the change in time, in seconds;
   * in other words the amount of time it took to sweep dotAngle Note: Tom
   * Pycke's ars.c code was the direct inspiration for this.  However, his
   * implementation of this method was inconsistent with the matrix algebra that
   * it came from.  So I went with the matrix algebra and tweaked his
   * implementation here.
   */
  void predict(struct GyroKalman *kalman, double dotAngle, double dt);

  /*
   * The kalman update method.  See
   * http://en.wikipedia.org/wiki/Kalman_filter#Update kalman    the kalman data
   * structure angle_m   the angle observed from the Wii Nunchuk accelerometer,
   * in radians
   */
  double updateKalman(struct GyroKalman *kalman, double angle_m);

  /* Compute the common fourth-order Runge-Kutta algorithm as part of the
   * integrating the gyro's yaw signal.  This will smooth the values a tad. Gyro
   * drift is still a problem, however. rk       the RungaKutta struct to hold
   * previous values val_i_0  the latest raw value to integrate returns the RK4
   * approximation of all values up until time t
   */
  double computeRungeKutta4(struct RungeKutta *rk, double val_i_0);

  // Kalman data structures for each rotational axis
  struct GyroKalman rollData;
  struct GyroKalman pitchData;
  struct GyroKalman yawData;
  struct RungeKutta yawRK;
  int readingsX, readingsY, readingsZ;  // raw 3 axis gyro readings
  int xID;

  // int sensorsign[6] = {1,-1,-1,-1,-1,1};
  // int data[6]; //six data bytes

  int ax_m = 0, ay_m = 0, az_m = 0;
  long yaw = 0, pitch = 0,
       roll = 0;  // normalized (scaled per WM+ mode) rotation values
  long yaw0 = 0, pitch0 = 0, roll0 = 0;  // calibration zeroes
  long xAcc0 = 0, yAcc0 = 0, zAcc0 = 0;

  // TODO do I need this?
  // bool slowYaw, slowPitch, slowRoll;     // WM+ state variables - if true,
  // then in slow (high res) mode

  bool debug = false;
  float accelAngleX = 0;  // Wiimote X angle
  float accelAngleY = 0;  // Wiimote Y angle
  float accelAngleZ = 0;  // Wiimote Z angle

  int chan;
  unsigned long lastread =
      0;  // Internal program state variables last system clock in millis
};

extern void Wiimote_Init();

#endif  // WIIMOTE_H_INCLUDED