#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H
#include <functional>

#include "gui_element.h"

class Scene;

typedef std::function<void(Scene*)> ClickCallback;

class GuiButton : public GuiElement {
 protected:
  bool _isHovered;
  bool _isPressed;
  bool _isClicked;
  bool _isDisabled;

  u32 _color;
  u32 _hoverColor;
  u32 _pressedColor;
  u32 _disabledColor;

  const char* text;
  f32 _scale;

  ClickCallback _onClick;

 public:
  static const GuiElementType type = GuiElementType::Button;

  GuiButton(const char* text, s16 x, s16 y);

  inline GuiButton(const char* text, s16 x, s16 y, f32 scale)
      : GuiButton(text, x, y) {
    _scale = scale;
  }

  ~GuiButton();

  virtual void update(f32 deltatime);
  virtual void render();

  inline void setScale(s16 scale) { _scale = scale; }
  inline void setColor(u32 color) { _color = color; }
  inline void setHoverColor(u32 color) { _hoverColor = color; }
  inline void setPressedColor(u32 color) { _pressedColor = color; }
  inline void setDisabledColor(u32 color) { _disabledColor = color; }
  inline void onClicked(ClickCallback onClick) { _onClick = onClick; }

  inline void setHovered(bool hovered) { _isHovered = hovered; }
  inline bool isHovered() { return _isHovered; }

  inline void setDisabled(bool disabled) { _isDisabled = disabled; }
  inline bool isDisabled() { return _isDisabled; }

  inline const char* getText() { return text; }

  inline void click(Scene* scene) { _onClick(scene); }
};

#endif  // GUI_BUTTON_H