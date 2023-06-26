/**
 * A lot of this was graciously provided by CavEx source
 * https://github.com/xtreme8000/CavEX/blob/master/source/platform/wii/texture.c
 * 
 * I couldn't do it without this. Thanks!
*/

#include "gfx.h"
#include <gccore.h>
#include <malloc.h>
#include <string.h>
#include <fat.h>
#include <sdcard/wiisd_io.h>
#include <wiiuse/wpad.h>

#include "logger.h"
#include "resource/texture.h"

#define DEFAULT_FIFO_SIZE (256 * 1024)

static GXColor background = { 0x64, 0x95, 0xED, 0xFF };
static uint8_t colors[256 * 3] ATTRIBUTE_ALIGN(32);
static void* frameBuffer[3];
static u32 bufferIndex = 0;

static GXRModeObj *rmode;
static volatile mqmsg_t current_frame = NULL;

static mqmsg_t frame = NULL;
static mqbox_t frame_draw;
static mqbox_t frame_empty;

int SCREEN_WIDTH = 704;
int SCREEN_HEIGHT = 480;

Mtx44 projection;
Mtx view = {
	{1.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, 1.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 1.0f, 0.0f}
};

void GFX_OutputMatrix(Mtx matrix) {
	LOG_DEBUG("Matrix\n");
	LOG_DEBUG("%f %f %f %f\n", matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]);
	LOG_DEBUG("%f %f %f %f\n", matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3]);
	LOG_DEBUG("%f %f %f %f\n", matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]);
}

void loadAllTextures() {
  if (!fatInitDefault()) {
    LOG_ERROR("Failed to initialize FAT\n");
    return;
  }

  if (!fatMountSimple("sd", &__io_wiisd)) {
    LOG_ERROR("Failed to mount SD card\n");
    return;
  }

  LoadTexture("note.png", TEX_FMT_RGBA32, GX_TEXMAP0, false);
  LoadTexture("loading2.png", TEX_FMT_RGBA32, GX_TEXMAP1, false);
  
  fatUnmount("sd");
  GX_DrawDone(); // Wait for GPU to finish
}

void GFX_Setup() {
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

/*
	if(CONF_GetAspectRatio() == CONF_ASPECT_16_9) {
		rmode->viWidth = 678;
		SCREEN_WIDTH = 802;
	} else {
		rmode->viWidth = 672;
		SCREEN_HEIGHT = 640;
	}

	if(VIDEO_GetCurrentTvMode() == VI_PAL) {
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
*/

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
  GX_SetDispCopyYScale(
    GX_GetYScaleFactor(rmode->efbHeight, rmode->xfbHeight));
	//GX_SetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
  GX_SetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
  GX_SetDispCopyDst(rmode->fbWidth, rmode->xfbHeight);
  GX_SetCopyFilter(rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);
  GX_SetFieldMode(rmode->field_rendering, ((rmode->viHeight == 2 * rmode->xfbHeight) ? GX_ENABLE : GX_DISABLE));

  GX_SetCullMode(GX_CULL_NONE);
  GX_CopyDisp(frameBuffer[0], GX_TRUE);
  GX_SetDispCopyGamma(GX_GM_1_0);

  GX_InvalidateTexAll();
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX8);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

  // models
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 15);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U16, 16);

	// gui
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_U16, 8);
  
  // font drawing
	GX_SetVtxAttrFmt(GX_VTXFMT2, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT2, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT2, GX_VA_TEX0, GX_TEX_ST, GX_U16, 8);

  // TODO: Access colors from renderer
	GX_SetArray(GX_VA_CLR0, colors, 3 * sizeof(uint8_t));
  GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);

  // we will only need 1 channel for each
  GX_SetNumChans(1);
	GX_SetNumTexGens(1);
	GX_SetNumTevStages(1);

  GFX_BindTexture(TEX_MODEL);
  GFX_EnableTextures(true);
  GFX_EnableAlphaTest(true);
	
	loadAllTextures();

  LOG_DEBUG("GX initialized\n");
}

void GFX_Cleanup() {
  LOG_DEBUG("Cleaning up.");
  free(frameBuffer[0]);
  free(frameBuffer[1]);
}

void GFX_EnableTextures(bool enable) {
	GX_SetTevOp(GX_TEVSTAGE0, enable ? GX_MODULATE : GX_PASSCLR);
}

void GFX_EnableLighting(bool enable) {
  GX_SetVtxDesc(GX_VA_CLR0, enable ? GX_INDEX8 : GX_DIRECT);
}

void GFX_EnableCulling(bool enable) {
  GX_SetCullMode(enable ? GX_CULL_BACK : GX_CULL_NONE);
}

void GFX_EnableAlphaTest(bool enable) {
	if(enable) {
		GX_SetAlphaCompare(GX_GEQUAL, 16, GX_AOP_AND, GX_ALWAYS, 0);
		GX_SetZCompLoc(GX_FALSE);
	} else {
		GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
		GX_SetZCompLoc(GX_TRUE);
	}
}

void GFX_BindTexture(TextureMap texmap) {
	switch (texmap) {
	case TEX_MODEL:
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		break;
	case TEX_GUI:
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP1, GX_COLOR0A0);
		break;
	case TEX_GUI2:
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP2, GX_COLOR0A0);
		break;
	case TEX_FONT:
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP3, GX_COLOR0A0);
		break;
	}
}

bool texMtxEnabledLast = true;
void GFX_TextureMatrix(bool enable, Mtx tex) {
	if (enable) {
		static Mtx flipped = { 
			{1.0f, 0.0f, 0.0f, 0.0f}, 
			{0.0f, -1.0f, 0.0f, 0.0f}, 
			{0.0f, 0.0f, 1.0f, 0.0f} 
		};

		Mtx convert;
		guMtxConcat(tex, flipped, convert);
		GX_LoadTexMtxImm(convert, GX_TEXMTX0, GX_MTX2x4);
	}

	if (enable != texMtxEnabledLast)
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, enable ? GX_TEXMTX0 : GX_IDENTITY);

	texMtxEnabledLast = enable;
}

void GFX_ModelViewMatrix(Mtx mv) {
	Mtx modelview;
	guMtxCopy(mv, modelview);
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
	switch(func) {
		default:
		case GX_LEQUAL: lastDepthFunc = GX_LEQUAL; break;
		case GX_EQUAL: lastDepthFunc = GX_EQUAL; break;
	}

	GX_SetZMode(lastDepthTest, lastDepthFunc, lastDepth);
}


void GFX_SetBlendMode(BlendMode mode) {
  switch(mode) {
    case MODE_BLEND:
      GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
      break;
    case MODE_BLEND2:
      GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_NOOP);
      break;
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

void GFX_EnableBGColor(bool enable) {
	GX_SetColorUpdate(enable ? GX_TRUE : GX_FALSE);
}

void GFX_DepthRange(float near, float far) {
	GX_SetViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, near, far);
}

void GFX_CopyBuffers(u32 cnt) {
	mqmsg_t input_frame;

	if(MQ_Receive(frame_draw, &input_frame, MQ_MSG_NOBLOCK)) {
		VIDEO_SetNextFramebuffer(input_frame);
		VIDEO_Flush();

		if(current_frame)
			MQ_Send(frame_empty, current_frame, MQ_MSG_BLOCK);

		current_frame = input_frame;
	}
}

void GFX_FlipBuffers(float* gpu_wait, float* vsync_wait) {
	u64 gpuStart = SYS_Time();
	GX_WaitDrawDone();
	u64 gpuEnd = SYS_Time();

	MQ_Send(frame_draw, frame, MQ_MSG_BLOCK);
	MQ_Receive(frame_empty, &frame, MQ_MSG_BLOCK);

	if(vsync_wait)
		*vsync_wait = (gpuStart - gpuEnd) / 1000.0f; // conver to seconds
	if(gpu_wait)
		*gpu_wait = (gpuStart - gpuEnd) / 1000.0f; // convert to seconds
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