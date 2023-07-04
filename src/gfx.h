#ifndef GFX_H
#define GFX_H
#include <ogc/gu.h>
#include <ogc/gx.h>
#include <stddef.h>

enum BlendMode {
  MODE_BLEND,
  MODE_BLEND2,
  MODE_BLEND3,
  MODE_INVERT,
  MODE_OFF
};

enum TextureMap {
  TEX_MODEL,
  TEX_GUI,
  TEX_GUI2,
  TEX_FONT,
  TEX_NONE
};

extern int SCREEN_WIDTH, SCREEN_HEIGHT;
extern Mtx44 projection;
extern Mtx view;

// framebuffer operations
extern void GFX_Init();
extern void GFX_Cleanup();
extern void GFX_FlipBuffers(float* gpu_wait, float* vsync_wait);
extern void GFX_CopyBuffers(u32 cnt);
extern void GFX_Finish(bool vsync);

// settings
extern void GFX_EnableAlphaTest(bool enable);
extern void GFX_EnableCulling(bool enable);
extern void GFX_EnableLighting(bool enable);
extern void GFX_EnableBGColor(bool enable);
extern void GFX_DepthRange(float near, float far);

// active elements
extern void GFX_BindTexture(TextureMap texmap);
extern void GFX_SetBlendMode(BlendMode mode);
extern void GFX_DepthFunc(u8 func);
extern void GFX_SetWriteBuffers(bool color, bool depth, bool depth_test);

//matrices
extern void GFX_ModelViewMatrix(Mtx mv);
extern void GFX_TextureMatrix(bool enable, Mtx matrix = NULL);

extern void GFX_OutputMatrix(Mtx matrix);

#endif // GFX_H