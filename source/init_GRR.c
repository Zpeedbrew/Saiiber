#include "input.h"

void graphics_init() {
GRRLIB_Init();
GRRLIB_Settings.antialias = true;
GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF); 
GRRLIB_SetLightAmbient(0x333333FF);

}
