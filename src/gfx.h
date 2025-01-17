#ifndef GFX_H
#define GFX_H
#include <stddef.h>
#include <functional>
#include "exmath.h"
#define MODELFMT GX_VTXFMT0
#define GUIFMT GX_VTXFMT1
#define FONTFMT GX_VTXFMT2
#define LINEFMT GX_VTXFMT3

#define PERSPECTIVE 0
#define ORTHOGRAPHIC 1

enum BlendMode {
  MODE_BLEND,
  MODE_ADD,
  MODE_SUB,
  MODE_BLEND3,
  MODE_INVERT,
  MODE_TRANSPARENT,
  MODE_OFF
};

// Texture Maps map to GX_TEXMAP0-7
enum TextureMap {
  TEX_MODEL = 0,
  TEX_GUI = 1,
  TEX_FONT = 2,
  TEX_GUI2 = 3,
  POST_PROCESS = 4,
  TEX_NONE
};

typedef std::function<void(void*)> GFXPostProcessCallback;

extern int SCREEN_WIDTH, SCREEN_HEIGHT;
extern glm::mat4 projection;
extern glm::mat4 view;

// framebuffer operations
void GFX_Init();
void GFX_Cleanup();
void GFX_FlipBuffers();
void GFX_CopyBuffers(u32 cnt);
void GFX_Finish(bool vsync);
void GFX_SetPostProcessCallback(GFXPostProcessCallback callback);

void GFX_Texture(TextureMap texmap);
void GFX_Texture(TextureMap texmap, glm::mat4& texMtx);

// settings
void GFX_EnableAlphaTest(bool enable);
void GFX_EnableLighting(bool enable);
void GFX_EnableCulling(bool enable);
void GFX_EnableColor(bool enable);
void GFX_EnableAlpha(bool enable);
void GFX_DepthRange(float near, float far);

// active elements
void GFX_SetBlendMode(BlendMode mode);
void GFX_DepthFunc(u8 func);
void GFX_SetWriteBuffers(bool color, bool depth, bool depth_test);

// matrices
// Expects non-transposed matrices
void GFX_Projection(glm::mat4 projection, int type);
void GFX_ModelMatrix(glm::mat4& model);
void GFX_ModelViewMatrix(glm::mat4& model, glm::mat4& _view);
void GFX_OutputMatrix(glm::mat4& matrix);
void GFX_OutputVector(glm::vec3& vec);

#ifdef SHADERS
void GFX_NormalMatrix(glm::mat4 model);
#endif

#endif  // GFX_H