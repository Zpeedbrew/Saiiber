#ifndef INPUT_H
#define INPUT_H
#include "wiimote.h"

extern Wiimote blueMote;
extern Wiimote redMote;

namespace Input {
  extern bool enabled;
  extern bool isButtonDown(int button);
  extern bool isButtonUp(int button);
  extern bool isButtonHeld(int button);
  extern void Update(float delta);
};

#endif // INPUT_H