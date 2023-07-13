#include "input.h"

Wiimote blueMote;
Wiimote redMote;

// Buttons are only ever in the "d" register for the first frame they are pressed
// It is safe to use this to check for individual button presses
bool Input::isButtonDown(int button) {
  return blueMote.isButtonDown(button) || redMote.isButtonDown(button);
}

bool Input::isButtonHeld(int button) {
  return blueMote.isButtonHeld(button) || redMote.isButtonHeld(button);
}

bool Input::isButtonUp(int button) {
  return blueMote.isButtonUp(button) || redMote.isButtonUp(button);
}

void Input::Update(float delta) {
  WPAD_ScanPads();
  blueMote.update(delta);
  redMote.update(delta);
}
