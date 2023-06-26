#ifndef TEXTURE_H
#define TEXTURE_H
#include <gctypes.h>

enum TextureFormat {
	TEX_FMT_RGBA16,
	TEX_FMT_RGBA32,
	TEX_FMT_I8,
	TEX_FMT_IA4,
};

void LoadTexture(const char* filename, TextureFormat type, int slot, bool linear);
#endif // TEXTURE_H