#ifndef WIIMOTE_H_INCLUDED
#define WIIMOTE_H_INCLUDED
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <deque>
#include <memory>
#include "exmath.h"

#define WPAD_CONNECT_ERROR -2
#define WPAD_CONNECT_TIMEOUT -1
#define WPAD_CONNECTED 0

struct _wpad_data;
class Transform;

enum Gesture {
  NONE = 0x00,
  UP = 0x01,
  DOWN = 0x02,
  LEFT = 0x04,
  RIGHT = 0x08,
};

struct DataFrame {
  glm::vec3 accel;
  glm::quat orient; // in euler angles

  float angle; // the angle C
  glm::vec3 cursor;
  bool pos_valid;

  glm::vec3 velocity;
  glm::vec3 direction;
};

class Wiimote {
 public:
  Wiimote();
  void update(f32 dt);
  bool calibrate();

  inline int getChannel() { return chan; }

  /*
   * The registers these functions compare against are all for single frames.
   * Use isButtonHeld if you want to check subsequent frames for a button press.
   */
  bool isButtonDown(int button);
  bool isButtonUp(int button);
  bool isButtonHeld(int button);

  void assignChannel(int channel);

  std::shared_ptr<Transform> transform;

  Gesture getLastGesture();

  std::deque<DataFrame> history;
  DataFrame current;
  DataFrame zeroes;

  void nextFrame();

  int chan = -1;
  bool enabled = false;

  _wpad_data* wd = NULL;
};

extern const char* WPAD_GetError(s32 err);

#endif  // WIIMOTE_H_INCLUDED