#ifndef INPUT_H
#define INPUT_H

#include <wiiuse/wpad.h>

// Initialize and Update
void InitInput();
void UpdateInput();

// Button Checks
bool ButtonPressed(int chan, int button);
bool ButtonHeld(int chan, int button);

// IR Tracking (New)
void GetIRPointer(int chan, float *x, float *y);

#endif
