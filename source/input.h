#ifndef INPUT_H
#define INPUT_H

#include <wiiuse/wpad.h>

void InitInput();
void UpdateInput();
bool ButtonPressed(int chan, int button);
bool ButtonHeld(int chan, int button);

#endif
