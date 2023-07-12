#include "input.h"

Wiimote blueMote;
Wiimote redMote;

bool Input::isButtonDown(int button) {
  return WPAD_ButtonsDown(WPAD_CHAN_ALL) & button;
}

bool Input::isButtonHeld(int button) {
  return WPAD_ButtonsHeld(WPAD_CHAN_ALL) & button;
}

bool Input::wasButtonDown(int button) {
  return blueMote.wasButtonDown(button) || redMote.wasButtonDown(button);
}

void Input::Update(float delta) {
  WPAD_ScanPads();
  blueMote.update(delta);
  redMote.update(delta);
}
