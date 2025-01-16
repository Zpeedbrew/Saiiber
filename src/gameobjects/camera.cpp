#include "camera.h"

#include "../exmath.h"
#include "../gfx.h"

Camera::Camera() {
  transform->position = {0.0F, 2.0F, 2.0F};
  up = {0.0f, 1.0f, 0.0f};

  guVector direction;
  direction.x = cosf(DegToRad(yaw)) * cosf(DegToRad(pitch));
  direction.y = sinf(DegToRad(pitch));
  direction.z = sinf(DegToRad(yaw)) * cosf(DegToRad(pitch));

  front = direction;
  guVecNormalize(&front);

  guVector target = transform->position + front;

  // Set up the projection matrix
  // This creates a perspective matrix with a view angle of 90
  // an aspect ratio that matches the screen, and z-near
  guPerspective(projection, fov, (f32)SCREEN_WIDTH / (f32)SCREEN_HEIGHT, 0.1f,
                300.0f);
  guLookAt(view, &transform->position, &up, &target);

  GFX_Projection(projection, GX_PERSPECTIVE);
}

void Camera::update(f32 deltatime) {
  GameObject::update(deltatime);

  if (pitch > 89.0f) pitch = 89.0f;
  if (pitch < -89.0f) pitch = -89.0f;

  if (fov < 1.0f) fov = 1.0f;
  if (fov > 45.0f) fov = 45.0f;

  guVector direction;
  direction.x = cosf(DegToRad(yaw)) * cosf(DegToRad(pitch));
  direction.y = sinf(DegToRad(pitch));
  direction.z = sinf(DegToRad(yaw)) * cosf(DegToRad(pitch));

  front = direction;
  guVecNormalize(&front);

  guVector target = transform->position + front;
  guLookAt(view, &transform->position, &up, &target);

  guPerspective(projection, fov, (f32)SCREEN_WIDTH / (f32)SCREEN_HEIGHT, 0.1f,
                300.0f);
  GFX_Projection(projection, GX_PERSPECTIVE);
}

void Camera::zoom(float amount) { fov += amount; }

#ifdef _DEBUG
#include <iostream>

#include "../fnt.h"
#include "../input.h"
guVector lastPos;
guVector lastRot;
float movespeed = 0.5f;

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
      pitch += 0.5f * deltatime;
    else
      transform->position += front * deltatime;
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_DOWN) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_DOWN)) {
    if (turning)
      pitch -= 0.5f * deltatime;
    else
      transform->position += front * movespeed * -deltatime;
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_LEFT) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_LEFT)) {
    if (turning)
      yaw -= 0.5f * deltatime;
    else
      transform->position += right() * movespeed * -deltatime;
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_RIGHT) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_RIGHT)) {
    if (turning)
      yaw += 0.5f * deltatime;
    else
      transform->position += right() * movespeed * deltatime;
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_ONE) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_ONE)) {
    transform->position += up * movespeed * deltatime;
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_PLUS) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_PLUS)) {
    zoom(-0.5f * deltatime);
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_MINUS) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_MINUS)) {
    zoom(0.5f * deltatime);
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_TWO) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_TWO)) {
    transform->position += up * movespeed * -deltatime;
  }

  if (transform->position != lastPos) {
    sprintf(posString, "Position: %.3f, %.3f, %.3f", transform->position.x,
            transform->position.y, transform->position.z);
    lastPos = transform->position;

    update(deltatime);
  }

  if (front != lastRot) {
    sprintf(rotString, "Rotation: %.3f, %.3f, %.3f", front.x, front.y, front.z);
    lastRot = front;

    update(deltatime);
  }
}
#endif
