#include "fnt.h"
#include "beon_fnt.h"
#include <string.h>
#include <map>
#include <istream>
#include "gfx.h"
#include "exmath.h"

// We can have more fonts if we want to load them here
// Just set make a "SetFont" function and an Enum for the fonts that get loaded

struct CharInfo {
  int x, y;
  int width, height;
  int xoffset, yoffset;
  int xadvance;
};

std::map<char, CharInfo> font;
Mtx44 screen;
Mtx screenView;

static float xScale = 1.0f;
static float yScale = 1.0f;
static u8 r = 255, g = 255, b = 255, a = 255;

// util class for looking at the memory as if it were a stream
struct membuf: std::streambuf {
  membuf(char* base, std::ptrdiff_t n) {
    this->setg(base, base, base + n);
  }
};

void FNT_Init() {
  membuf sbuf((char*)beon_fnt, beon_fnt_size);
  std::istream in(&sbuf);

  std::string line;
  while (std::getline(in, line)) {
    // check if line starts with "char"
    if (line.find("char") != 0)
      continue;

    int id = 0;
    CharInfo info;
    sscanf(line.c_str(), "char id=%d x=%d y=%d width=%d height=%d xoffset=%d yoffset=%d xadvance=%d",
      &id, &info.x, &info.y, &info.width, &info.height, &info.xoffset, &info.yoffset, &info.xadvance);
    font[id] = info;
  }

  guMtx44Identity(screen);
  guOrtho(screen, 0, SCREEN_HEIGHT, 0, SCREEN_WIDTH, 0, 1);

  guMtxIdentity(screenView);
}

void FNT_SetScale(float scale) {
  xScale = scale;
  yScale = scale;
}

void FNT_SetScale(float x, float y) {
  xScale = x;
  yScale = y;
}

void FNT_SetColor(int color) {
  r = (color >> 24) & 0xFF;
  g = (color >> 16) & 0xFF;
  b = (color >> 8) & 0xFF;
  a = color & 0xFF;
}

void FNT_SetColor(u8 _r, u8 _g, u8 _b, u8 _a) {
  r = _r; g = _g;
  b = _b; a = _a;
}

s16 FNT_GetStringWidth(const char* str, float scale) {
  int width = 0;

  if (scale == 1.0f)
    scale = xScale;

  // add xadvance for all characters except last
  int len = strlen(str);
  for (int i = 0; i < len - 1; i++) {
    CharInfo info = font[toupper(str[i])];
    width += info.xadvance;
  }

  width += font[toupper(str[len - 1])].width;
  return width * scale;
}

int max(int a, int b) {
  return a > b ? a : b;
}

s16 FNT_GetStringHeight(const char* str, float scale) {
  int height = 0;

  if (scale == 1.0f)
    scale = yScale;

  int len = strlen(str);
  for (int i = 0; i < len; i++) {
    CharInfo info = font[toupper(str[i])];
    height = max(height, info.height);
  }

  return height * scale;
}

void FNT_DrawString(const char *str, s16 x, s16 y) {
  GFX_Projection(screen, GX_ORTHOGRAPHIC);

  GFX_BindTexture(TEX_FONT);
  GFX_EnableTexture(true);
  GFX_EnableLighting(false);

  GFX_ModelViewMatrix(screenView, screenView);
  GFX_TextureMatrix(false);
  GFX_SetBlendMode(MODE_SOURCE);

  int offsetX = 0;
  int len = strlen(str);
  for (int i = 0; i < len; i++) {
    CharInfo info = font[toupper(str[i])];
    s16 x1 = x + info.xoffset + offsetX;
    s16 x2 = x1 + (info.width * xScale);

    s16 y1 = y + info.height + info.yoffset;
    s16 y2 = y1 - (info.height * yScale);

    // texture is 256 in width and height
    u8 u2 = info.x + info.width;
    u8 v2 = info.y + info.height;

    GX_Begin(GX_QUADS, FONTFMT, 4);

    GX_Position3s16(x1, y2, 0);
    GX_Normal3s8(0,0,0);
    GX_Color4u8(r, g, b, a);
    GX_TexCoord2u8(info.x, info.y);

    GX_Position3s16(x2, y2, 0);
    GX_Normal3s8(0,0,0);
    GX_Color4u8(r, g, b, a);
    GX_TexCoord2u8(u2, info.y);

    GX_Position3s16(x2, y1, 0);
    GX_Normal3s8(0,0,0);
    GX_Color4u8(r, g, b, a);
    GX_TexCoord2u8(u2, v2);

    GX_Position3s16(x1, y1, 0);
    GX_Normal3s8(0,0,0);
    GX_Color4u8(r, g, b, a);
    GX_TexCoord2u8(info.x, v2);

    GX_End();
    
    // I believe xadvance is the width + kerning
    offsetX += (info.xadvance * xScale);
  }

  GFX_Projection(projection, GX_PERSPECTIVE);
}
