#ifndef GFX_H
#define GFX_H
#include <ogc/gu.h>
#include <ogc/gx.h>
#include <stddef.h>
#define MODELFMT GX_VTXFMT0
#define GUIFMT GX_VTXFMT1
#define FONTFMT GX_VTXFMT2
#define LINEFMT GX_VTXFMT3

enum BlendMode {
  MODE_BLEND,
  MODE_ADD,
  MODE_SUB,
  MODE_BLEND3,
  MODE_INVERT,
  MODE_OFF
};

enum TextureMap {
  TEX_MODEL = GX_TEXMAP0,
  TEX_GUI = GX_TEXMAP1,
  TEX_FONT = GX_TEXMAP2,
  TEX_GUI2,
  TEX_NONE
};

extern int SCREEN_WIDTH, SCREEN_HEIGHT;
extern Mtx44 projection;
extern Mtx view;

// framebuffer operations
void GFX_Init();
void GFX_Cleanup();
void GFX_FlipBuffers(float* gpu_wait, float* vsync_wait);
void GFX_CopyBuffers(u32 cnt);
void GFX_Finish(bool vsync);

// settings
void GFX_EnableAlphaTest(bool enable);
void GFX_EnableTexture(bool enable);
void GFX_EnableLighting(bool enable);
void GFX_EnableCulling(bool enable);
void GFX_EnableColor(bool enable);
void GFX_EnableAlpha(bool enable);
void GFX_DepthRange(float near, float far);

// active elements
void GFX_BindTexture(TextureMap texmap);
void GFX_SetBlendMode(BlendMode mode);
void GFX_DepthFunc(u8 func);
void GFX_SetWriteBuffers(bool color, bool depth, bool depth_test);

// matrices
void GFX_Projection(Mtx44 projection, int type);
void GFX_ModelViewMatrix(Mtx model, Mtx _view = NULL);
void GFX_TextureMatrix(bool enable, Mtx matrix = NULL);
void GFX_NormalMatrix(Mtx model);
void GFX_OutputMatrix(Mtx matrix);

#endif  // GFX_H