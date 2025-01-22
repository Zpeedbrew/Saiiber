/*
 Copyright (c) 2008 Francisco Mu�oz 'Hermes' <www.elotrolado.net>
 All rights reserved.
 
 Proper (standard) vorbis usage by Tantric, 2009
 Threading modifications/corrections by Tantric, 2009

 Redistribution and use in source and binary forms, with or without 
 modification, are permitted provided that the following conditions are met:

 - Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright notice, 
 this list of conditions and the following disclaimer in the documentation 
 and/or other materials provided with the distribution.
 - The names of the contributors may not be used to endorse or promote products 
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE 
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __OGGPLAYER_H__
#define __OGGPLAYER_H__
#include <gctypes.h>

#define OGG_ONE_TIME         0
#define OGG_INFINITE_TIME    1

#define OGG_STATUS_UNKNOWN 128
#define OGG_STATUS_RUNNING   1
#define OGG_STATUS_ERR      -1
#define OGG_STATUS_PAUSED    2
#define OGG_STATUS_EOF     255

extern void SFX_Init();
extern int SFX_Play(s32 voice, void* buffer, int len, int time_pos);
extern int SFX_Load(const char* filename);
extern void SFX_Stop(s32 voice);

extern void SFX_Pause(s32 voice);
extern void SFX_Unpause(s32 voice);
extern int SFX_Status(s32 voice);
extern void SFX_Volume(s32 voice, int volume);
extern s32 SFX_GetTime(s32 voice);
extern void SFX_Seek(s32 voice, s32 time_pos);

extern void SFX_Cleanup();

#endif