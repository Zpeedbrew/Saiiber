#ifndef GUI_TEXT_H
#define GUI_TEXT_H
#include "gui_element.h"

class GuiText : public GuiElement {
private:
  const char* text;
  f32 _scale;
  int _color;

public:
  GuiText(const char* text, s16 x, s16 y, f32 scale);
  ~GuiText();

  void update(f32 deltatime);
  void render();

  inline void setColor(int color) { _color = color; }
  inline void setText(const char* text) { this->text = text; }
  inline void setScale(float scale) { _scale = scale; }
};

#endif // GUI_TEXT_H