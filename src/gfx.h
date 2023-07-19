#ifndef GFX_H
#define GFX_H
#include <ogc/gu.h>
#include <ogc/gx.h>
#include <stddef.h>

enum BlendMode { MODE_BLEND, MODE_BLEND2, MODE_BLEND3, MODE_INVERT, MODE_OFF };

enum TextureMap { TEX_MODEL, TEX_GUI, TEX_GUI2, TEX_FONT, TEX_NONE };

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
void GFX_EnableCulling(bool enable);
void GFX_EnableLighting(bool enable);
void GFX_EnableBGColor(bool enable);
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
void GFX_OutputMatrix(Mtx matrix);

#endif  // GFX_H