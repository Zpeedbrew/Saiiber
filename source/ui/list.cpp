#include "list.h"
#include "../input.h"
#include "../logger.h"

GuiList::GuiList() : GuiElement(0, 0) {}

GuiList::~GuiList() {}

void GuiList::add(const char* text, s16 x, s16 y, f32 scale) {
  GuiButton* button = new GuiButton(text, x, y);
  button->setScale(scale);
  buttons.emplace_back(button);
}

GuiButton* GuiList::get(u32 index) {
  if (index < 0 || index > buttons.size() - 1) return NULL;
  return buttons.at(index).get();
}

void GuiList::update(f32 deltatime)
{
  if (Input::isButtonDown(WPAD_BUTTON_UP) && choice > 0) choice -= 1;

  if (Input::isButtonDown(WPAD_BUTTON_DOWN) && choice < buttons.size() - 1)
    choice += 1;

  if (Input::isButtonDown(WIIMOTE_BUTTON_A)) {
    if (_buttonPressedCallback != NULL)
      _buttonPressedCallback(WIIMOTE_BUTTON_A, choice, *buttons.at(choice));
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_B)) {
    if (_buttonPressedCallback != NULL)
      _buttonPressedCallback(WIIMOTE_BUTTON_B, choice, *buttons.at(choice));
  }

  for (u32 i = 0; i < buttons.size(); i++)
    buttons.at(i)->setHovered(choice == i);
}

void GuiList::render() {
  for (auto& button : buttons) button->render();
}