#include "input.h"

void InitInput() {
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
    WPAD_SetVRes(WPAD_CHAN_0, 640, 480);
}

void GetIRPointer(int chan, float *x, float *y) {
    WPADData *data = WPAD_Data(chan);
    if (data != NULL && data->ir.valid) {
        *x = data->ir.x;
        *y = data->ir.y;
    } else {
        *x = -1.0f; 
        *y = -1.0f;
    }
}
