/**
 * A lot of this was graciously provided by CavEx source
 * https://github.com/xtreme8000/CavEX/blob/master/source/platform/wii/texture.c
 *
 * I couldn't do it without this. Thanks!
 */

#include "gfx.h"

#include <fat.h>
#include <gccore.h>
#include <malloc.h>
#include <sdcard/wiisd_io.h>
#include <string.h>
#include <wiiuse/wpad.h>

#include "logger.h"
#include "resource/texture.h"

#define DEFAULT_FIFO_SIZE (256 * 1024)
#define CORNBLUE \
  { 0x64, 0x95, 0xED, 0xFF }
#define BLACK \
  { 0x00, 0x00, 0x00, 0xFF }

static GXColor background = CORNBLUE;
static uint8_t colors[256 * 3] ATTRIBUTE_ALIGN(32);
static void* frameBuffer[3];
static u32 bufferIndex = 0;

static GXRModeObj* rmode;
static volatile mqmsg_t current_frame = NULL;

static mqmsg_t frame = NULL;
static mqbox_t frame_draw;
static mqbox_t frame_empty;

int SCREEN_WIDTH = 704;
int SCREEN_HEIGHT = 480;

Mtx44 projection;
Mtx view = {{1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f}};

void GFX_OutputMatrix(Mtx matrix) {
  LOG_DEBUG("Matrix\n");
  LOG_DEBUG("%f %f %f %f\n", matrix[0][0], matrix[0][1], matrix[0][2],
            matrix[0][3]);
  LOG_DEBUG("%f %f %f %f\n", matrix[1][0], matrix[1][1], matrix[1][2],
            matrix[1][3]);
  LOG_DEBUG("%f %f %f %f\n", matrix[2][0], matrix[2][1], matrix[2][2],
            matrix[2][3]);
}

#include "beon_png.h"
#include "models_png.h"

void loadAllTextures() {
  // LoadTextureFromFile("note.png", TEX_FMT_RGBA32, GX_TEXMAP0);
  // LoadTextureFromFile("loading2.png", TEX_FMT_RGBA32, GX_TEXMAP1);

  LoadPNGFromMemory(models_png, models_png_size, TEX_FMT_RGBA32, GX_TEXMAP0);
  LoadPNGFromMemory(beon_png, beon_png_size, TEX_FMT_I8, GX_TEXMAP2);

  GX_DrawDone();  // Wait for GPU to finish
}

void GFX_Init() {
  VIDEO_Init();
  WPAD_Init();

  rmode = VIDEO_GetPreferredMode(NULL);
  frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
  frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
  frameBuffer[2] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

  MQ_Init(&frame_draw, 3);
  MQ_Init(&frame_empty, 3);

  MQ_Send(frame_empty, frameBuffer[0], MQ_MSG_BLOCK);
  MQ_Send(frame_empty, frameBuffer[1], MQ_MSG_BLOCK);
  frame = frameBuffer[2];

#ifdef MODERN_ASPECT
  if (CONF_GetAspectRatio() == CONF_ASPECT_16_9) {
    rmode->viWidth = 678;
    SCREEN_WIDTH = 802;
  } else {
    rmode->viWidth = 672;
    SCREEN_HEIGHT = 640;
  }

  if (VIDEO_GetCurrentTvMode() == VI_PAL) {
    // screenMode->viHeight = VI_MAX_HEIGHT_PAL;
    rmode->viXOrigin = (VI_MAX_WIDTH_PAL - rmode->viWidth) / 2;
    rmode->viYOrigin = (VI_MAX_HEIGHT_PAL - rmode->viHeight) / 2;
  } else {
    // screenMode->viHeight = VI_MAX_HEIGHT_NTSC;
    rmode->viXOrigin = (VI_MAX_WIDTH_NTSC - rmode->viWidth) / 2;
    rmode->viYOrigin = (VI_MAX_HEIGHT_NTSC - rmode->viHeight) / 2;
  }

  s8 hoffset = 0;
  CONF_GetDisplayOffsetH(&hoffset);
  rmode->viXOrigin += hoffset;
#endif

  VIDEO_Configure(rmode);
  VIDEO_SetNextFramebuffer(frameBuffer[0]);
  VIDEO_SetPreRetraceCallback(GFX_CopyBuffers);
  VIDEO_SetBlack(false);
  VIDEO_Flush();

  VIDEO_WaitVSync();

  LOG_DEBUG("Video initialized\n");

  // Initialize the GX subsystem
  void* gp_fifo = memalign(32, DEFAULT_FIFO_SIZE);
  memset(gp_fifo, 0, DEFAULT_FIFO_SIZE);

  GX_Init(gp_fifo, DEFAULT_FIFO_SIZE);
  GX_SetCopyClear(background, GX_MAX_Z24);

  // designate the viewing area
  GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
  GX_SetDispCopyYScale(GX_GetYScaleFactor(rmode->efbHeight, rmode->xfbHeight));
  // GX_SetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
  GX_SetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
  GX_SetDispCopyDst(rmode->fbWidth, rmode->xfbHeight);
  GX_SetCopyFilter(rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);
  GX_SetFieldMode(
      rmode->field_rendering,
      ((rmode->viHeight == 2 * rmode->xfbHeight) ? GX_ENABLE : GX_DISABLE));

  GX_SetCullMode(GX_CULL_NONE);
  GX_CopyDisp(frameBuffer[0], GX_TRUE);
  GX_SetDispCopyGamma(GX_GM_1_0);

  GX_InvalidateTexAll();
  GX_ClearVtxDesc();
  GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
  GX_SetVtxDesc(GX_VA_NRM, GX_DIRECT);
  GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
  GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

  // Models
  // s16 needs 1 bit for +/- and 15 bits of precision
  GX_SetVtxAttrFmt(MODELFMT, GX_VA_POS, GX_POS_XYZ, GX_S16, 15);
  GX_SetVtxAttrFmt(MODELFMT, GX_VA_NRM, GX_NRM_XYZ, GX_S16, 15);
  GX_SetVtxAttrFmt(MODELFMT, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
  GX_SetVtxAttrFmt(MODELFMT, GX_VA_TEX0, GX_TEX_ST, GX_U16, 16);

  // GUI
  GX_SetVtxAttrFmt(GUIFMT, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
  GX_SetVtxAttrFmt(GUIFMT, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
  GX_SetVtxAttrFmt(GUIFMT, GX_VA_TEX0, GX_TEX_ST, GX_U16, 8);

  // FONT
  GX_SetVtxAttrFmt(FONTFMT, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
  GX_SetVtxAttrFmt(FONTFMT, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
  GX_SetVtxAttrFmt(FONTFMT, GX_VA_TEX0, GX_TEX_ST, GX_U8, 8);

  // lines?
  GX_SetVtxAttrFmt(LINEFMT, GX_VA_POS, GX_POS_XYZ, GX_S16, 15);
  GX_SetVtxAttrFmt(LINEFMT, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

  GX_SetArray(GX_VA_CLR0, colors, 3 * sizeof(uint8_t));
  GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);

  // we will only need 1 channel for each
  GX_SetNumChans(1);
  GX_SetNumTexGens(1);
  GX_SetNumTevStages(1);

  GFX_BindTexture(TEX_MODEL);
  GFX_EnableTexture(true);
  GFX_EnableAlphaTest(true);

  loadAllTextures();

  LOG_DEBUG("GX initialized\n");
}

void GFX_Cleanup() {
  LOG_DEBUG("Cleaning up.\n");
  free(frameBuffer[0]);
  free(frameBuffer[1]);
}

void GFX_EnableCulling(bool enable) {
  GX_SetCullMode(enable ? GX_CULL_BACK : GX_CULL_NONE);
}

void GFX_EnableAlphaTest(bool enable) {
  if (enable) {
    GX_SetAlphaCompare(GX_GEQUAL, 16, GX_AOP_AND, GX_ALWAYS, 0);
    GX_SetZCompLoc(GX_FALSE);
  } else {
    GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GX_SetZCompLoc(GX_TRUE);
  }
}

// it's set to modulate in initialize
bool light = false;
bool texture = false;

// toggles texturing
// Forces light to be enabled if texturing is disabled
void GFX_EnableTexture(bool enable) {
  texture = enable;
  GX_SetTevOp(GX_TEVSTAGE0,
              texture ? (light ? GX_MODULATE : GX_REPLACE) : GX_PASSCLR);
}

void GFX_EnableLighting(bool enable) {
  if (!enable && !texture) {
    LOG_ERROR("Cannot disable lighting when texture is disabled.\n");
    return;
  }

  light = enable;
  GX_SetTevOp(GX_TEVSTAGE0,
              light ? (texture ? GX_MODULATE : GX_PASSCLR) : GX_REPLACE);
}

bool texMtxEnabledLast = true;

void GFX_BindTexture(TextureMap texmap) {
  GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, (u32)texmap, GX_COLOR0A0);
}

void GFX_TextureMatrix(bool enable, Mtx tex) {
  if (enable) {
    static Mtx flipped = {{1.0f, 0.0f, 0.0f, 0.0f},
                          {0.0f, -1.0f, 0.0f, 0.0f},
                          {0.0f, 0.0f, 1.0f, 0.0f}};

    Mtx convert;
    guMtxConcat(tex, flipped, convert);
    GX_LoadTexMtxImm(tex, GX_TEXMTX0, GX_MTX2x4);
  }

  if (enable != texMtxEnabledLast)
    GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0,
                      enable ? GX_TEXMTX0 : GX_IDENTITY);

  texMtxEnabledLast = enable;
}

void GFX_NormalMatrix(Mtx model) {
  Mtx nrm;
  guMtxInverse(model, nrm);
  guMtxTranspose(nrm, nrm);
  GX_LoadNrmMtxImm(nrm, GX_PNMTX0);
}

void GFX_Projection(Mtx44 projection, int type) {
  GX_LoadProjectionMtx(projection, type);
}

void GFX_ModelViewMatrix(Mtx model, Mtx _view) {
  if (_view == NULL) _view = view;

  Mtx modelview;
  guMtxIdentity(modelview);
  guMtxConcat(_view, model, modelview);
  GX_LoadPosMtxImm(modelview, GX_PNMTX0);
}

u8 lastDepthFunc = GX_LEQUAL;
u8 lastDepth = GX_TRUE;
u8 lastDepthTest = GX_TRUE;
void GFX_SetWriteBuffers(bool color, bool depth, bool depth_test) {
  GX_SetColorUpdate(color ? GX_TRUE : GX_FALSE);

  lastDepth = depth ? GX_TRUE : GX_FALSE;
  lastDepthTest = depth_test ? GX_TRUE : GX_FALSE;
  GX_SetZMode(lastDepthTest, lastDepthFunc, lastDepth);
}

void GFX_DepthFunc(u8 func) {
  switch (func) {
    default:
    case GX_LEQUAL:
      lastDepthFunc = GX_LEQUAL;
      break;
    case GX_EQUAL:
      lastDepthFunc = GX_EQUAL;
      break;
  }

  GX_SetZMode(lastDepthTest, lastDepthFunc, lastDepth);
}

// Sets the blend mode
// but the result of which is enabled and disabled by
// GX_SetColorUpdate and GX_SetAlphaUpdate
void GFX_SetBlendMode(BlendMode mode) {
  // src_pix is the color from the texture (with added GX_Color from TevStages)
  // dst_pix is the color already in the framebuffer (black if nothing)
  // blend function is:
  // src_pix * src_factor + dst_pix * dst_factor

  switch (mode) {
    // This is interpolated blending
    case MODE_BLEND:
      GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA,
                      GX_LO_NOOP);
      break;

    case MODE_ADD:
      GX_SetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP);
      break;

    case MODE_SUB:
      GX_SetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_BM_SUBTRACT);
      break;

    // source being black 0, 0, 0
    // destination being brown 127 (0.5), 64 (0.25), 0 (0)
    // result being just black, basically...
    case MODE_BLEND3:
      GX_SetBlendMode(GX_BM_BLEND, GX_BL_DSTCLR, GX_BL_SRCCLR, GX_LO_NOOP);
      break;

    case MODE_INVERT:
      GX_SetBlendMode(GX_BM_LOGIC, GX_BL_ZERO, GX_BL_ZERO, GX_LO_INV);
      break;

    case MODE_OFF:
      GX_SetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP);
      break;
  }
}

void GFX_EnableColor(bool enable) {
  GX_SetColorUpdate(enable ? GX_TRUE : GX_FALSE);
}

void GFX_EnableAlpha(bool enable) {
  GX_SetAlphaUpdate(enable ? GX_TRUE : GX_FALSE);
}

void GFX_DepthRange(float near, float far) {
  GX_SetViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, near, far);
}

void GFX_CopyBuffers(u32 cnt) {
  mqmsg_t input_frame;

  if (MQ_Receive(frame_draw, &input_frame, MQ_MSG_NOBLOCK)) {
    VIDEO_SetNextFramebuffer(input_frame);
    VIDEO_Flush();

    if (current_frame) MQ_Send(frame_empty, current_frame, MQ_MSG_BLOCK);

    current_frame = input_frame;
  }
}

void GFX_FlipBuffers(float* gpu_wait, float* vsync_wait) {
  u64 gpuStart = SYS_Time();
  GX_WaitDrawDone();
  u64 gpuEnd = SYS_Time();

  MQ_Send(frame_draw, frame, MQ_MSG_BLOCK);
  MQ_Receive(frame_empty, &frame, MQ_MSG_BLOCK);

  if (vsync_wait)
    *vsync_wait = (gpuStart - gpuEnd) / 1000.0f;  // conver to seconds
  if (gpu_wait)
    *gpu_wait = (gpuStart - gpuEnd) / 1000.0f;  // convert to seconds
}

void GFX_Finish(bool vsync) {
  GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
  GX_SetColorUpdate(GX_TRUE);
  GX_CopyDisp(frame, GX_TRUE);
  GX_SetDrawDone();

  if (vsync) {
    GX_WaitDrawDone();
    VIDEO_SetNextFramebuffer(frame);
    VIDEO_Flush();
  }
}