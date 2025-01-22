#include "wiimote.h"

#define LO_PASS_VALUE 4.0f

// Quick few notes...
// According to the Motion Plus documentation
// Relative to standing the Wiimote towards the sky...
// Roll is the forward/backwards tilt of the remote
// Pitch is the left/right tilt of the remote
// Yaw is the left and right rotation of the remote
// I am not exactly sure if the motion plus accounts for the offset
//  around the base of the remote, where the attachment is.

// To translate that into our Transform, that would mean
// Yaw of the remote is the Pitch of the Transform
// Pitch of the remote is the Yaw of the Transform
// Roll is the same for both

#include <string.h>
#include <unistd.h>
#include <wiiuse/wpad.h>

#include <cmath>

#include "exmath.h"
#include "gfx.h"
#include "logger.h"
#include "transform.h"

// top left is 0,0
static glm::vec3 SENSOR =
    glm::vec3(SCREEN_WIDTH / 2.0f, (f32)SCREEN_HEIGHT, 0.0f);

Wiimote::Wiimote() : transform(new Transform()), history(10) {}

void Wiimote::nextFrame() {
  orient_t orient;
  vec3w_t accel;
  ir_t ir;

  WPAD_Orientation(chan, &orient);
  WPAD_Accel(chan, &accel);
  WPAD_IR(chan, &ir);

  if (history.size() >= 10) history.pop_front();
  history.push_back(current);

  // Acceleration is always +/- 512
  // With the exception of the gravity (z) which is +315
  // Except that when the remote is sat on it's face, it's +103

  bool flipped = abs(orient.pitch) >= 90.0f;

  // Fix gimbal lock by preventing pitch from going passed 90 degrees
  current.orient = glm::quat(
      glm::vec3(glm::radians(glm::clamp(-orient.pitch, -89.9f, 89.9f)),
                glm::radians(flipped ? -orient.yaw : orient.yaw),
                glm::radians(-orient.a_roll)));
  current.accel = glm::vec3((f32)accel.x, (f32)accel.y, (f32)accel.z);
  current.accel -= zeroes.accel;

  // We use the raw position because it goes out of bounds of the screen
  // z is questionable distance in meters from screen
  current.cursor = glm::vec3(ir.ax, ir.ay, ir.z);
  current.pos_valid = ir.raw_valid;

  // Angle corresponds to the rotation of the wiimote, we don't need it.
}

// Calibrate Zeroes relative to a standing saber and remote.
bool Wiimote::calibrate() {
  zeroes.orient = current.orient;
  zeroes.accel = current.accel;
  zeroes.cursor = current.cursor;

  // clear history
  history.clear();

  return true;
}

void Wiimote::update(f32 dt) {
  if (chan == -1 || !enabled) return;
  nextFrame();

  DataFrame &lastFrame = history.back();

  if (!current.pos_valid)
    current.cursor = lastFrame.cursor;  // set to last known position

  glm::vec3 posCursor = glm::vec3(current.cursor.x, current.cursor.y, 0.0f);

  // direction to the cursor from the sensor
  glm::vec3 dirSensor = glm::normalize(SENSOR - posCursor);

  // direction to the cursor from the remote
  glm::vec3 dirCursor = glm::normalize(current.orient * posCursor);

  // Gets the angle between the ray from the remote to the cursor and the
  // direction to the cursor from the sensor
  float dotWiimote = glm::dot(dirCursor, dirSensor);  // in radians

  // Then we can use Z as the side opposite of the angle
  // and distance from sensor to cursor as another side length
  float dSensor = current.cursor.z;
  float dCursor = glm::distance(posCursor, SENSOR);

  // then find the angle between the two sides using the sine rule
  float aWiimote = glm::asin(dCursor * glm::sin(dotWiimote) / dSensor);
  float aSensor = M_PI - aWiimote - dotWiimote;

  // now use law of cosines to find the distance from the remote to the cursor
  // c2 = a2 + b2 - 2ab cos(C)
  float wmMag = glm::sqrt(dCursor * dCursor + dSensor * dSensor -
                          2 * dCursor * dSensor * cosf(aSensor));

  // Get the real position of the remote
  glm::vec3 wmPos = dirCursor * -wmMag;
  LOG_DEBUG("dirCursor * -wmMag Position: %f %f %f\n", wmPos.x, wmPos.y,
            wmPos.z);

  glm::vec3 accelDiff = current.accel - lastFrame.accel;

  // Apply low pass filter to acceleration difference to remove noise
  accelDiff.x = abs(accelDiff.x) > LO_PASS_VALUE ? accelDiff.x : 0;
  accelDiff.y = abs(accelDiff.y) > LO_PASS_VALUE ? accelDiff.y : 0;
  accelDiff.z = abs(accelDiff.z) > LO_PASS_VALUE ? accelDiff.z : 0;

  current.velocity = lastFrame.velocity + accelDiff * dt;
  current.direction =
      glm::normalize(VEC_FORWARD * current.orient * current.velocity);

  LOG_DEBUG("Cursor Position (%s): %f %f %f", current.pos_valid ? "+" : "-",
            current.cursor.x, current.cursor.y, current.cursor.z);
  // LOG_DEBUG("Wiimote Velocity: %f, %f, %f", current.velocity.x,
  // current.velocity.y, current.velocity.z);
  // LOG_DEBUG("Wiimote Direction: %f, %f, %f", current.direction.x,
  // current.direction.y, current.direction.z);
  // LOG_DEBUG("Wiimote Acceleration: %f, %f, %f", accelDiff.x, accelDiff.y,
  // accelDiff.z);
  // LOG_DEBUG("Wiimote Orient: %f, %f, %f", current.orient.x, current.orient.y,
  // current.orient.z);

  // pos += current.velocity * dt; needs proper positional data from the wiimote

  transform->rotation = current.orient;
  transform->position = glm::vec3(wmPos.x, wmPos.y, transform->position.z);

  transform->update();
}

Gesture Wiimote::getLastGesture() {
  // Approximate Gesture based on history
  Gesture gesture = NONE;

  // Ensure that the gesture is significant enough to recognize

  return gesture;
}

bool Wiimote::isButtonDown(int button) {
  if (chan == -1) return false;
  u32 btns = WPAD_ButtonsDown(chan);
  return (btns & button) != 0;
}

bool Wiimote::isButtonUp(int button) {
  if (chan == -1) return false;
  u32 btns = WPAD_ButtonsUp(chan);
  return (btns & button) != 0;
}

bool Wiimote::isButtonHeld(int button) {
  if (chan == -1) return false;
  u32 btns = WPAD_ButtonsHeld(chan);
  return (btns & button) != 0;
}

const char *GetStatusString(int status) {
  switch (status) {
    case WPAD_STATE_DISABLED:
      return "Disabled";
    case WPAD_STATE_ENABLING:
      return "Enabling";
    case WPAD_STATE_ENABLED:
      return "Enabled";
    default:
      return "Unknown";
  }
}

void Wiimote::assignChannel(int channel) {
  chan = channel;
  wd = WPAD_Data(channel);  // initialize for calibration
  enabled = wd != NULL;     // it shouldn't ever be NULL but just in case

  nextFrame();

  const char *status = GetStatusString(WPAD_GetStatus());
  LOG_DEBUG("Wiimote on channel %d initialized: %s\n", channel, status);
}

const char *WPAD_GetError(s32 err) {
  switch (err) {
    case WPAD_ERR_NONE:
      return "WPAD Initialized successfully";
    case WPAD_ERR_NO_CONTROLLER:
      return "No WPAD controllers found";
    case WPAD_ERR_TRANSFER:
      return "WPAD transfer error";
    case WPAD_ERR_BAD_CHANNEL:
      return "WPAD bad channel";
    case WPAD_ERR_NOT_READY:
      return "WPAD not ready";
    case WPAD_ERR_QUEUE_EMPTY:
      return "WPAD queue empty";
    case WPAD_ERR_BADVALUE:
      return "WPAD bad value";
    case WPAD_ERR_BADCONF:
      return "WPAD bad configuration";
    default:
      return "WPAD unknown error";
  }
}
