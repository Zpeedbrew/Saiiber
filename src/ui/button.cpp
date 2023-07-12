#include "button.h"

#include "../fnt.h"
#include "../input.h"

GuiButton::GuiButton(const char* text, s16 x, s16 y)
    : GuiElement(x, y), text(text) {
  _scale = 1.0f;
  _isHovered = false;
  _isPressed = false;
  _isClicked = false;
  _isDisabled = false;

  _color = 0xFFFFFFFF;
  _hoverColor = 0x00FF00FF; // green default
  _pressedColor = 0xFFFFFFFF;
  _disabledColor = 0xFFFFFFFF;

  _onClick = NULL;
}

GuiButton::~GuiButton() {}

void GuiButton::update(f32 deltatime) {
  if (_isDisabled) return;
}

void GuiButton::render() {
  FNT_SetColor(_isHovered ? _hoverColor : _color);
  FNT_SetScale(_scale);
  FNT_DrawString(text, _pos[0], _pos[1]);
}