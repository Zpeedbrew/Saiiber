#include "input.h"

void InitInput() {
    WPAD_Init();
    // Enable Buttons, Accelerometer, and IR data
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
    // Optional: Set virtual resolution to match your screen (e.g., 640x480)
    WPAD_SetVRes(WPAD_CHAN_0, 640, 480);
}

// Get pointer coordinates
void GetIRPointer(int chan, float *x, float *y) {
    WPADData *data = WPAD_Data(chan);
    if (data->ir.valid) {
        *x = data->ir.x;
        *y = data->ir.y;
    } else {
        *x = -1.0f; // Indicates the remote isn't pointing at the sensor bar
        *y = -1.0f;
    }
}
