#ifndef GUI_ELEMENT_H
#define GUI_ELEMENT_H
#include <gctypes.h>

enum class GuiElementType { Button, List, Slider, Label, Image, Unknown };

class GuiElement {
 protected:
  s16 _pos[2];

 public:
  static const GuiElementType type = GuiElementType::Unknown;

  GuiElement(s16 x, s16 y) : _pos{x, y} {}
  virtual ~GuiElement() {}

  virtual void update(f32 deltatime) = 0;
  virtual void render() = 0;

  inline void setPosition(s16 x, s16 y) { _pos[0] = x; _pos[1] = y; }
  inline s16* getPosition() { return _pos; }
};

template <typename T>
bool isType(T* element, GuiElementType type) {
  return T::type == type;
}

#endif