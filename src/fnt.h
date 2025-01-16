/*
 * Font (fnt.h)
 * Author: Beemer
 * Implementation for FNT loading
 */

#ifndef FNT_H
#define FNT_H
#include <gctypes.h>

extern void FNT_Init();

extern void FNT_SetScale(float scale);
extern void FNT_SetScale(float x, float y);

extern void FNT_SetColor(int color);
extern void FNT_SetColor(u8 r, u8 g, u8 b, u8 a);

extern u16 FNT_GetStringWidth(const char* str, float scale = -1.0f);
extern u16 FNT_GetStringHeight(float scale = -1.0f);

extern void FNT_DrawString(const char *str, s16 x, s16 y);

#endif // FNT_H