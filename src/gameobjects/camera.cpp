#include "camera.h"

#include "../exmath.h"
#include "../gfx.h"

Camera::Camera() {
  transform->position = {0.0F, 0.0F, 0.0F};

  // Set up the projection matrix
  // This creates a perspective matrix with a view angle of 90
  // an aspect ratio that matches the screen, and z-near
  guPerspective(projection, fov, (f32)SCREEN_WIDTH / (f32)SCREEN_HEIGHT, 0.1f,
                300.0f);
  GFX_Projection(projection, GX_PERSPECTIVE);
}

guVector projectedObject = {0.0f, 0.0f, -300.0f};

void Camera::update(f32 deltatime) {
  GameObject::update(deltatime);

  guPerspective(projection, fov, (f32)SCREEN_WIDTH / (f32)SCREEN_HEIGHT, 0.1f,
                300.0f);
  guLookAt(view, &transform->position, &Transform::UP, &projectedObject);

  GFX_Projection(projection, GX_PERSPECTIVE);
}

void Camera::zoom(float amount) { fov += amount; }

#ifdef _DEBUG
#include <iostream>

#include "../fnt.h"
#include "../input.h"
guVector lastPos;
guVector lastRot;

char turningString[15] = "Turning: False";
char posString[39] = "Position: 0.000, 0.000, 0.000";
char rotString[39] = "Rotation: 0.000, 0.000, 0.000";

void Camera::render() {
  FNT_SetColor(0xFFFFFFFF);
  FNT_SetScale(0.5f);

  s16 height = FNT_GetStringHeight(turningString);

  FNT_DrawString(turningString, 0, 0);
  FNT_DrawString(posString, 0, height + 2);
  FNT_DrawString(rotString, 0, height + 2 + height + 2);
}

void Camera::freecam(f32 deltatime) {
  bool turning = Input::isButtonHeld(WIIMOTE_BUTTON_B);

  if (Input::isButtonDown(WIIMOTE_BUTTON_B))
    sprintf(turningString, "Turning: True");
  if (Input::isButtonUp(WIIMOTE_BUTTON_B))
    sprintf(turningString, "Turning: False");

  if (Input::isButtonDown(WIIMOTE_BUTTON_UP) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_UP)) {
    if (turning)
      transform->rotation += Transform::RIGHT * 1.0f * deltatime;
    else
      transform->position += Transform::FORWARD * 1.0f * deltatime;
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_DOWN) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_DOWN)) {
    if (turning)
      transform->rotation += Transform::RIGHT * -1.0f * deltatime;
    else
      transform->position += Transform::FORWARD * -1.0f * deltatime;
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_LEFT) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_LEFT)) {
    if (turning)
      transform->rotation += Transform::UP * -1.0f * deltatime;
    else
      transform->position += Transform::RIGHT * -1.0f * deltatime;
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_RIGHT) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_RIGHT)) {
    if (turning)
      transform->rotation += Transform::UP * 1.0f * deltatime;
    else
      transform->position += Transform::RIGHT * 1.0f * deltatime;
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_ONE) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_ONE)) {
    transform->position += Transform::UP * 1.0f * deltatime;
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_PLUS) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_PLUS)) {
    zoom(-1.0f * deltatime);
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_MINUS) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_MINUS)) {
    zoom(1.0f * deltatime);
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_TWO) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_TWO)) {
    transform->position += Transform::UP * -1.0f * deltatime;
  }

  if (transform->position != lastPos) {
    sprintf(posString, "Position: %.3f, %.3f, %.3f", transform->position.x,
            transform->position.y, transform->position.z);
    lastPos = transform->position;

    update(deltatime);
    GFX_OutputMatrix(view);
  }

  if (transform->rotation != lastRot) {
    sprintf(rotString, "Rotation: %.3f, %.3f, %.3f", transform->rotation.x,
            transform->rotation.y, transform->rotation.z);
    lastRot = transform->rotation;

    update(deltatime);
    GFX_OutputMatrix(view);
  }
}
#endif
