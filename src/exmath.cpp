#include "exmath.h"
#include <chrono>
#include <thread>
#include <wiiuse/wpad.h>

guVector operator+(const guVector& left, const guVector& right)
{
    return {left.x + right.x, left.y + right.y, left.z + right.z};
}

guVector operator-(const guVector& left, const guVector& right)
{
    return {left.x - right.x, left.y - right.y, left.z - right.z};
}

guVector operator/(const guVector& left, const float right)
{
    return {left.x / right, left.y / right, left.z / right};
}

guVector makeGuVector(float x, float y, float z)
{
    return {x, y, z};
}

int millis()
{
    auto time = std::chrono::system_clock::now();
    auto since_epoch = time.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch);

    return millis.count();
}

void sleep_for(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void pressA(int wiimote)
{
    while(1)
    {
        WPAD_ScanPads();
        u32 gcPressed = WPAD_ButtonsDown(wiimote);
        if (gcPressed & WPAD_BUTTON_A) break;

        sleep_for(50);
    }
}

float angleInRadians(int lo, int hi, long measured) {
    float x = (hi - lo);
    return (float)(measured/x) * PI;
}