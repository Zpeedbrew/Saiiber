#include "wiimote.h"
#include <wiiuse/wpad.h>

#include "exmath.h"
#include <unistd.h>
#include <cmath>

Wiimote::Wiimote(int channel) : chan(channel)
{
    // make sure we connect
    while(1)
    {
        WPAD_ScanPads();
        WPADData* data = WPAD_Data(chan);
        if (data->err == WPAD_ERR_NONE) break;

        sleep_for(50);
    }

    calibrateZeroes();

    initGyroKalman(&rollData,Q_angle,Q_gyro,R_angle);
    initGyroKalman(&pitchData,Q_angle,Q_gyro,R_angle);
    initGyroKalman(&yawData,Q_angle,Q_gyro,R_angle);

    yawRK.val_i_3 = 0;  yawRK.val_i_2 = 0;  yawRK.val_i_1 = 0;  yawRK.previous = 0;
    lastread = millis();

    // TODO: push all data to transform after updating
    // add quaternion to Transform and replace all of the
    //  roll pitch yaw stuff with it
}


void Wiimote::calibrateZeroes()
{
    long y0 = 0, p0 = 0, r0 = 0;
    long xa0 = 0, ya0 = 0, za0 = 0;
    int avg = 300;

    for (int i=0; i < avg; i++) {
        rawReadData();

        y0 += yaw;
        r0 += roll;
        p0 += pitch;

        xa0 += ax_m;
        ya0 += ay_m;
        za0 += az_m;

        sleep_for(10);
    }

    yaw0 = y0 / (avg/2); roll0 = r0 / (avg/2); pitch0 = p0 / (avg/2);
    xAcc0 = xa0 / (avg/2); yAcc0 = ya0 / (avg/2); zAcc0 = za0 / (avg/2);
}

void Wiimote::initGyroKalman(struct GyroKalman *kalman, const float Q_angle, const float Q_gyro, const float R_angle)
{
    kalman->Q_angle = Q_angle; kalman->Q_gyro  = Q_gyro;  kalman->R_angle = R_angle;
    kalman->P_00 = 0; kalman->P_01 = 0; kalman->P_10 = 0; kalman->P_11 = 0;
}

void Wiimote::predict(struct GyroKalman *kalman, double dotAngle, double dt) {
    kalman->x_angle += dt * (dotAngle - kalman->x_bias);
    kalman->P_00 += -1 * dt * (kalman->P_10 + kalman->P_01) + dt*dt * kalman->P_11 + kalman->Q_angle;
    kalman->P_01 += -1 * dt * kalman->P_11;
    kalman->P_10 += -1 * dt * kalman->P_11;
    kalman->P_11 += kalman->Q_gyro;
}

double Wiimote::updateKalman(struct GyroKalman *kalman, double angle_m) {
    const double y = angle_m - kalman->x_angle;
    const double S = kalman->P_00 + kalman->R_angle;
    const double K_0 = kalman->P_00 / S;
    const double K_1 = kalman->P_10 / S;
    kalman->x_angle += K_0 * y; kalman->x_bias  += K_1 * y;
    kalman->P_00 -= K_0 * kalman->P_00; kalman->P_01 -= K_0 * kalman->P_01; kalman->P_10 -= K_1 * kalman->P_00; kalman->P_11 -= K_1 * kalman->P_01;
    return kalman->x_angle;
}

double Wiimote::computeRungeKutta4(struct RungeKutta *rk, double val_i_0) {
    rk->previous += 0.16667*(rk->val_i_3 + 2*rk->val_i_2 + 2*rk->val_i_1 + val_i_0);
    rk->val_i_3 = rk->val_i_2;
    rk->val_i_2 = rk->val_i_1;
    rk->val_i_1 = val_i_0;
    return rk->previous;
}


void Wiimote::update(f32 dt) {
    readData();

    // TODO not handling slow/fast mode, can this cause problems or does WPAD do that for me?
    readingsX = roll;
    readingsY = pitch;
    readingsZ = yaw;

    // predict the future
    // *Doctor Who theme plays*
    predict(&rollData, DegToRad(readingsX), dt);
    predict(&pitchData, DegToRad(readingsY), dt);

    orient.x = RadToDeg(updateKalman(&rollData, accelAngleX));
    orient.y = RadToDeg(updateKalman(&rollData, accelAngleX));
    orient.z = RadToDeg(computeRungeKutta4(&yawRK, DegToRad(readingsZ) * dt));
}

void Wiimote::readData() {
    rawReadData();

    // apply calibration values
    yaw -= yaw0; roll -= roll0; pitch -= pitch0;
    ax_m -= xAcc0; ay_m -= yAcc0; az_m = zAcc0;

    az_m += 211; // account for the gravity of the situation (720?)

    // The real purpose of this method is to convert the accelometer values into usable angle values.
    // see app note: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf
    // That paper gives the math for implementing a tilt sensor using 3-axis accelerometers.  Roll(X) and pitch(Y) are directly
    // applicable.  Yaw(Z) is not since there is no 'tilt' with respect to the earth.
    // Once the accelerometer values have been biased to zero (by subtracting calibration value above), then they should fall in a range from
    // -512 to +511.
    double x = angleInRadians(-512, 511, ax_m), y = angleInRadians(-512, 511, ay_m), z = angleInRadians(-512, 511, az_m);
    // compute values that are used in multiple places
    double xSquared = x*x; double ySquared = y*y; double zSquared = z*z;
    accelAngleX = (accelAngleX * 0.8 + atan(x / sqrt(ySquared + zSquared))*0.2);
    accelAngleY = (accelAngleY * 0.8 + atan(y / sqrt(xSquared + zSquared))*0.2);
    accelAngleZ = (accelAngleZ * 0.8 + atan(sqrt(xSquared + ySquared)/ z)*0.2);
    // filter readings using low pass filter for acceleromters to remove jitter
    // big percentage of previous value plus smaller percentage of current value
    // effect is delay large changes from reaching the output but at cost of reduced sensitivity
}

void Wiimote::rawReadData()
{
    WPADData* wd = WPAD_Data(chan);

    // MotionPlus
    yaw =   wd->exp.mp.ry / wmpFastToDegreePerSec;
    roll =  wd->exp.mp.rz / wmpFastToDegreePerSec;
    pitch = wd->exp.mp.rx / wmpFastToDegreePerSec;

    // TODO not handling slow/fast mode, can this cause problems or does WPAD do that for me?
    /*slowPitch = data[3] & 1;
    slowYaw = data[3] & 2;
    slowRoll = data[4] & 2;*/

    // This function processes the byte array from the wii nunchuck (Wiimote?).
    ax_m = wd->accel.x;
    ay_m = wd->accel.y;
    az_m = wd->accel.z;
}

void Wiimote_Init() {
  WPAD_Init();
  WPAD_SetDataFormat(-1, WPAD_FMT_BTNS_ACC_IR);
  WPAD_SetMotionPlus(-1, 1);
}
