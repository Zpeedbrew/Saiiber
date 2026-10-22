#ifndef GUI_LIST_H
#define GUI_LIST_H
#include <memory>
#include <vector>

#include "button.h"

class Scene;

class GuiList : public GuiElement {
 private:
  std::vector<std::unique_ptr<GuiButton>> buttons;
  std::function<void(int button, u32 choice, GuiButton& element)> _buttonPressedCallback;
  u32 choice = 0;

 public:
  static const GuiElementType type = GuiElementType::List;

  GuiList();
  ~GuiList();

  void add(const char* text, s16 x, s16 y, f32 scale);
  inline void add(const char* text, s16 x, s16 y) { add(text, x, y, 1.0f); }

  GuiButton* get(u32 index);

  inline void onButtonPressed(std::function<void(int, u32, GuiButton&)> callback) {
    _buttonPressedCallback = callback;
  }

  void update(f32 deltatime) override;
  void render() override;
};

#endif  // GUI_LIST_H