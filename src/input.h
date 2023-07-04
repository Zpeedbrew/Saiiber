#ifndef INPUT_H
#define INPUT_H
#include "wiimote.h"
											
#define WIIMOTE_2							0x0001
#define WIIMOTE_1							0x0002
#define WIIMOTE_B							0x0004
#define WIIMOTE_A							0x0008
#define WIIMOTE_MINUS						0x0010
#define WIIMOTE_HOME						0x0080
#define WIIMOTE_LEFT						0x0100
#define WIIMOTE_RIGHT						0x0200
#define WIIMOTE_DOWN						0x0400
#define WIIMOTE_UP							0x0800
#define WIIMOTE_PLUS						0x1000
											
#define WIIMOTE_Z					(0x0001<<16)
#define WIIMOTE_C					(0x0002<<16)

extern Wiimote blueMote;
extern Wiimote redMote;

#endif // INPUT_H