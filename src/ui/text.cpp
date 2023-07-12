#include "text.h"

#include "../fnt.h"

GuiText::GuiText(const char* text, s16 x, s16 y, f32 scale = 1.0f)
    : GuiElement(x, y), text(text), _scale(scale) {}

GuiText::~GuiText() {}

void GuiText::update(f32 deltatime) {}

void GuiText::render() {
  FNT_SetColor(_color);
  FNT_SetScale(_scale);
  FNT_DrawString(text, _pos[0], _pos[1]);
}