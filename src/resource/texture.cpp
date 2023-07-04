/** 
 * Texture
 * Author: Beemer
 * Implementation for PNG loading
 */

#include "texture.h"
#include <stddef.h>
#include <limits.h>
#include <assert.h>
#include <gccore.h>
#include <malloc.h>

#include "lodepng.h"
#include "../gfx.h"
#include "../logger.h"

#define ASSET_PATH "sd:/apps/saiiber/assets"

#define rgba16(r, g, b, a)                                                     \
	(((int)(a) << 12) | ((int)(r) << 8) | ((int)(g) << 4) | (int)(b))
#define rgb16(r, g, b) (0x8000 | ((int)(r) << 10) | ((int)(g) << 5) | (int)(b))
#define xy16(x, y) (((int)(x) << 8) | (int)(y))
#define ia8(i, a) (((int)(a) << 4) | (int)(i))

int alphacmp(uint8_t* col, uint8_t* alpha) {
	return	(col[0] - alpha[0])
				+ (col[1] - alpha[1])
				+ (col[2] - alpha[2]);
}


void* tex_conv_rgb32(uint8_t* image, size_t width, size_t height, int alpha) {
	assert(image && (width % 4) == 0 && (height % 4) == 0);
	uint16_t* output = (uint16_t*)memalign(32, width * height * sizeof(uint16_t) * 2);

	if(!output)
		return NULL;

	size_t output_idx = 0;

	for(size_t y = 0; y < height; y += 4) {
		for(size_t x = 0; x < width; x += 4) {
			for(size_t by = 0; by < 4; by++) {
				for(size_t bx = 0; bx < 4; bx++) {
					uint8_t* col = image + (x + bx + (y + by) * width) * 4;
					if (alphacmp(col, (uint8_t*)&alpha) == 0)
						col[3] = 0;

					output[output_idx++] = xy16(col[3], col[0]);
				}
			}

			for(size_t by = 0; by < 4; by++) {
				for(size_t bx = 0; bx < 4; bx++) {
					uint8_t* col = image + (x + bx + (y + by) * width) * 4;
					output[output_idx++] = xy16(col[1], col[2]);
				}
			}
		}
	}

	DCFlushRange(output, width * height * sizeof(uint16_t) * 2);

	return output;
}

void* tex_conv_rgb16(uint8_t* image, size_t width, size_t height, int alpha) {
	assert(image && (width % 4) == 0 && (height % 4) == 0);
	uint16_t* output = (uint16_t*)memalign(32, width * height * sizeof(uint16_t));

	if(!output)
		return NULL;

	size_t output_idx = 0;

	for(size_t y = 0; y < height; y += 4) {
		for(size_t x = 0; x < width; x += 4) {
			for(size_t by = 0; by < 4; by++) {
				for(size_t bx = 0; bx < 4; bx++) {
					uint8_t* col = image + (x + bx + (y + by) * width) * 4;
					int alpha = col[3] >> 5;

					if (alphacmp(col, (uint8_t*)&alpha) == 0)
						alpha = 0;

					if(alpha < 7) {
						output[output_idx++] = rgba16(col[0] >> 4, col[1] >> 4,
													  col[2] >> 4, alpha);
					} else {
						output[output_idx++]
							= rgb16(col[0] >> 3, col[1] >> 3, col[2] >> 3);
					}
				}
			}
		}
	}

	DCFlushRange(output, width * height * sizeof(uint16_t));

	return output;
}

void* tex_conv_i8(uint8_t* image, size_t width, size_t height) {
	assert(image && (width % 8) == 0 && (height % 4) == 0);
	uint8_t* output = (uint8_t*)memalign(32, width * height * sizeof(uint8_t));

	if(!output)
		return NULL;

	size_t output_idx = 0;

	for(size_t y = 0; y < height; y += 4) {
		for(size_t x = 0; x < width; x += 8) {
			for(size_t by = 0; by < 4; by++) {
				for(size_t bx = 0; bx < 8; bx++) {
					uint8_t* column = image + (x + bx + (y + by) * width) * 4;
					output[output_idx++]
						= ((int)column[0] + (int)column[1] + (int)column[2]) / 3;
				}
			}
		}
	}

	DCFlushRange(output, width * height * sizeof(uint8_t));

	return output;
}

void* tex_conv_ia4(uint8_t* image, size_t width, size_t height) {
	assert(image && (width % 8) == 0 && (height % 4) == 0);
	uint8_t* output = (uint8_t*)memalign(32, width * height * sizeof(uint8_t));

	if(!output)
		return NULL;

	size_t output_idx = 0;

	for(size_t y = 0; y < height; y += 4) {
		for(size_t x = 0; x < width; x += 8) {
			for(size_t by = 0; by < 4; by++) {
				for(size_t bx = 0; bx < 8; bx++) {
					uint8_t* col = image + (x + bx + (y + by) * width) * 4;
					output[output_idx++] = ia8(
						(((int)col[0] + (int)col[1] + (int)col[2]) / 3) >> 4,
						col[3] >> 4);
				}
			}
		}
	}

	DCFlushRange(output, width * height * sizeof(uint8_t));

	return output;
}

// loads a PNG texture
bool load_png(uint8_t* img, size_t width, size_t height, TextureFormat type,
int slot, int alpha, bool linear) {
  void* output = NULL;
  uint8_t fmt;

  switch (type) {
    case TEX_FMT_RGBA32:
      output = tex_conv_rgb32(img, width, height, alpha);
      fmt = GX_TF_RGBA8;
      break;
    case TEX_FMT_RGBA16:
      output = tex_conv_rgb16(img, width, height, alpha);
      fmt = GX_TF_RGB5A3;
      break;
    case TEX_FMT_I8:
      output = tex_conv_i8(img, width, height);
      fmt = GX_TF_I8;
      break;
    case TEX_FMT_IA4:
      output = tex_conv_ia4(img, width, height);
      fmt = GX_TF_IA4;
      break;
    default:
      LOG_ERROR("Invalid texture format %d\n", type);
      return NULL;
  }

  if (!output)
    return NULL;

  GXTexObj tex;
  uint8_t filter = linear ? GX_LINEAR : GX_NEAR;
	uint8_t wrap = GX_REPEAT; // S and T
  GX_InitTexObj(&tex, output, width, height, fmt, wrap, wrap, GX_FALSE);
  GX_InitTexObjMaxAniso(&tex, GX_ANISO_1); // how many passes to increase detail (increases processing power)
  GX_InitTexObjFilterMode(&tex, filter, filter);
  GX_LoadTexObj(&tex, slot);

  return true;
}

// loads a PNG texture
void LoadTextureFromFile(const char* filename, TextureFormat type, int slot, int alpha, bool linear) {
  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s/%s", ASSET_PATH, filename);
  LOG_DEBUG("Loading texture %s\n", path);

  size_t width, height;
  uint8_t* img;
  if (lodepng_decode32_file(&img, &width, &height, path) > 0) {
    LOG_ERROR("Failed to load png from %s\n", path);
		return;
  }

  if (!load_png(img, width, height, type, slot, alpha, linear)) {
    LOG_ERROR("Failed to convert texture %s\n", filename);
    free(img);
  }
}

void LoadPNGFromMemory(const uint8_t* pngdata, u32 size, TextureFormat type,
int slot, int alpha, bool linear) {
	size_t width, height;
	uint8_t* img;

	// NOTE: Don't need to add the ELF header offset to pngdata
	if (lodepng_decode32(&img, &width, &height, pngdata, size) > 0) {
		LOG_ERROR("Failed to load png from memory\n");
		return;
	}

	if (!load_png(img, width, height, type, slot, alpha, linear)) {
		LOG_ERROR("Failed to convert texture from memory\n");
		free(img);
	}
}

void LoadTextureFromMemory(s32 id, void* data, u32 size, int slot) {
	GXTexObj tex;
	TPLFile file;

	TPL_OpenTPLFromMemory(&file, data, size);
	TPL_GetTexture(&file, id, &tex);
  GX_LoadTexObj(&tex, slot);
}