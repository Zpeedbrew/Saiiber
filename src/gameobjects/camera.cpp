#include "camera.h"
#include "../gfx.h"
#include "../logger.h"
Camera::Camera() {
  transform->position = glm::vec3(0.0F,2.0F,2.0f);// transform->rotate(0.0f, -90.0f, 0.0f);
// This took actually forever to figure out...
// Creates the desired matrix (like how guPerspective works)
projection=glm::perspectiveFovLH_ZO(glm::radians(fov),(f32)SCREEN_WIDTH,(f32)SCREEN_HEIGHT,-300.0f,-0.1f);
// Needs to be -1.0f because we're facing -1.0f in the Z axis
projection[2][3]=-1.0f;
// This is also verifiably correct
glm::vec3 pos=transform->position;
glm::vec3 target=pos+transform->forward();
view = glm::lookAt(pos, target, VEC_UP);
GFX_Projection(projection, PERSPECTIVE);
}
void Camera::update(f32 deltatime) {
GameObject::update(deltatime);
if(fov<1.0f)fov=1.0f;
if(fov>45.0f)fov=45.0f;
glm::vec3 pos=transform->position;
glm::vec3 target=pos+transform->forward();
view=glm::lookAt(pos,target,VEC_UP);
projection=glm::perspectiveFovLH_ZO(glm::radians(fov),(f32)SCREEN_WIDTH,(f32)SCREEN_HEIGHT,-300.0f,-0.1f);
projection[2][3]=-1.0f;
}
void Camera::zoom(float amount){fov+=amount;}
#ifndef _DEBUG
void Camera::render() {
GFX_Projection(projection,PERSPECTIVE);
}
#else
#include <iostream>
#include "../fnt.h"
#include "../input.h"
glm::vec3 lastPos;
glm::vec3 lastRot;
//possibly should make thoes there own string 
//char titleString[15] = "Saiiber";
//char controlstring[39]="Position:0.000,0.000,0.000";
//char controlsting2[39]="Rotation:0.000,0.000,0.000";
//char fpstring[39]="n/a fps";
char versionString[4]="1.4";
//stuff under here is debug
char turningString[15] = "false"; // forgoet what this is
char controlstring[39]="Position:0.000,0.000,0.000";
char rotString[39]="Rotation:0.000,0.000,0.000";

void Camera::render() {
  FNT_SetColor(0xFF0000FF);
  FNT_SetScale(1.0f);
  u16 height = FNT_GetStringHeight();
  FNT_DrawString(turningString,0,0);
  FNT_DrawString(posString,0,height+2);
  FNT_DrawString(rotString,0,height+2+height+2);
  FNT_DrawString(versionString,0,height+12+height+12);
  GFX_Projection(projection,PERSPECTIVE);
}
// TODO: Fix drift...
// also want to make movespeed a full number
void Camera::freecam(f32 deltatime){
  static float movespeed=6.5f;
  bool turning=Input::isButtonHeld(WIIMOTE_BUTTON_B);
  glm::vec3 front = transform->forward();
  if(Input::isButtonDown(WIIMOTE_BUTTON_A) ||
    Input::isButtonHeld(WIIMOTE_BUTTON_A))
    movespeed=9.0f;

  if (Input::isButtonUp(WIIMOTE_BUTTON_A))movespeed=5.9f;

  if (Input::isButtonDown(WIIMOTE_BUTTON_B))
    sprintf(turningString,"Turning:True");
  if (Input::isButtonUp(WIIMOTE_BUTTON_B))
    sprintf(turningString,"Turning:False");

  if (Input::isButtonDown(WIIMOTE_BUTTON_UP) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_UP)) {
    if (turning) {
      // leftmost positive value
      float diff = minf(0.5f*deltatime,89.0f-pitch);
      pitch+=diff;
      transform->rotate(diff,0.0f,0.0f);
    } else
      transform->position+=movespeed*deltatime*transform->forward();
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_DOWN) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_DOWN)) {
    if (turning) {
      // rightmost negative value
      float diff=maxf(-0.5f*deltatime,-89.0f-pitch);
      pitch+=diff;
      transform->rotate(diff,0.0f,0.0f);
    } else
      transform->position+=-movespeed*deltatime*transform->forward();
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_LEFT) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_LEFT)){
    if (turning)
      transform->rotate(0.0f,0.5f*deltatime,0.0f);
    else
      transform->position+=movespeed*-deltatime*transform->right();
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_RIGHT) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_RIGHT)){
    if (turning)
      transform->rotate(0.0f,-0.5f*deltatime,0.0f);
    else
      transform->position+=movespeed*deltatime*transform->right();
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_ONE) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_ONE)){
    transform->position+=movespeed*deltatime*VEC_UP;
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_PLUS) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_PLUS)){
    zoom(-0.5f*deltatime);
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_MINUS) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_MINUS)){
    zoom(0.5f*deltatime);
  }

  if (Input::isButtonDown(WIIMOTE_BUTTON_TWO) ||
      Input::isButtonHeld(WIIMOTE_BUTTON_TWO)){
    transform->position+=-movespeed*deltatime*VEC_UP;
  }

  if (transform->position != lastPos) {
    sprintf(posString,"Position:%.3f,%.3f,%.3f",transform->position.x,
            transform->position.y,transform->position.z);
    lastPos=transform->position;
  }

  // TODO:Fix
  if (front != lastRot) {
    sprintf(rotString,"Rotation:%.3f,%.3f,%.3f",front.x,front.y,front.z);
    lastRot=front;
  }
}
#endif
