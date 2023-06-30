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

#include <asndlib.h>
#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>
#include <gccore.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

#include "sfx.h"
#include "logger.h"

#define PAUSE 128
#define READY 64
#define FULL  1
#define NUM_TRACKS 4

/* functions to read the Ogg file from memory */

/* OGG control */
// we use this in the multiplication later to get the size of a frame
#define CHANNELS 			2

#define SAMPLING_RATE 44100
#define BIT_DEPTH 		16				// 2 bytes
#define DUR_SECONDS		0.017			// 17ms (equates to ~60fps)

#define READ_SAMPLES 4096
#define MAX_PCMOUT 	 4096 // maximum size to read ogg samples
#define STACKSIZE		 8192

static struct OggData {
	OggVorbis_File vf;  // Vorbis File
	vorbis_info *vi;    // Vorbis Info
	int current_section;

  struct {
    char *mem;
    int size;
    int pos;
  } file;

  // OGG thread control
  u8 stack[STACKSIZE];
  lwpq_t queue = LWP_TQUEUE_NULL;
  lwp_t thread = LWP_THREAD_NULL;
  int running = 0;

	// OGG file operation
	bool open;
	int mode;
	int eof;
	int flag;
	int volume;
	int seek_time;

	s32 voice;

	/* OGG buffer control */
	short pcmout[2][READ_SAMPLES + MAX_PCMOUT * CHANNELS]; /* take 4k out of the data segment, not the stack */
	int pcmout_pos;
	int pcm_indx;
} tracks[NUM_TRACKS];

static int f_read(void *punt, int bytes, int blocks, OggData *ogg) {
	int b, c;
	if (bytes * blocks <= 0)
		return 0;

	blocks = bytes * blocks;
	c = 0;

	while (blocks > 0) {
		b = blocks;
		if (b > MAX_PCMOUT)
			b = MAX_PCMOUT;

		if (ogg->file.size == 0)
			return -1;

		if ((ogg->file.pos + b) > ogg->file.size)
			b = ogg->file.size - ogg->file.pos;

		if (b > 0) {
			memcpy(punt, ogg->file.mem + ogg->file.pos, b);
			ogg->file.pos += b;
		}

		if (b <= 0)
			return c / bytes;

		c += b;
		blocks -= b;
	}

	return c / bytes;
}

static int f_seek(OggData *ogg, ogg_int64_t offset, int mode) {
	if (ogg == NULL)
		return -1;

	mode &= 3;

	if (ogg->file.size == 0)
		return -1;

	if (mode == 0) {
		if (offset >= ogg->file.size) {
			ogg->file.pos = ogg->file.size;
			return -1;
		} else if ((offset) < 0) {
			ogg->file.pos = 0;
			return -1;
		} else {
			ogg->file.pos = offset;
		}
	}

	if (mode == 1) {
		if ((ogg->file.pos + offset) >= ogg->file.size) {
			ogg->file.pos = ogg->file.size;
			return -1;
		} else if ((ogg->file.pos + offset) < 0) {
			ogg->file.pos = 0;
			return -1;
		} else {
			ogg->file.pos += offset;
		}
	}

	if (mode == 2) {
		if ((ogg->file.size + offset) >= ogg->file.size) {
			ogg->file.pos = ogg->file.size;
			return -1;
		} else if ((ogg->file.size + offset) < 0) {
			ogg->file.pos = 0;
			return -1;
		} else {
			ogg->file.pos = ogg->file.size + offset;
		}
	}

	return 0;
}

static int f_close(void *f) {
  OggData* ogg = (OggData*)f;

	ogg->file.size = 0;
	ogg->file.pos = 0;
	ogg->open = false;

	if (ogg->file.mem) {
		free(ogg->file.mem);
		ogg->file.mem = NULL;
	}

	return 0;
}

static long f_tell(OggData *ogg) {
	return ogg->file.pos;
}

static void mem_open(OggData& ogg, s32 voice, char * oggbuf, int size) {
	ogg.voice = voice;
	ogg.file.mem = oggbuf;
	ogg.file.size = size;
	ogg.file.pos = 0;
	ogg.open = true;
}

static void mem_close(OggData& ogg) {
  // it is a memory file descriptor?
	ogg.file.size = 0;
	ogg.open = false;
  f_close(&ogg.file);
}

static ov_callbacks callbacks = {
	(size_t (*)(void *, size_t, size_t, void *))  f_read,
	(int (*)(void *, ogg_int64_t, int))           f_seek,
	(int (*)(void *))                             f_close,
	(long (*)(void *))                            f_tell
};

static void ogg_add_callback(s32 voice) {
	OggData& ogg = tracks[voice];

	if (!ogg.running) {
		ASND_StopVoice(ogg.voice);
		return;
	}

	if (ogg.flag & 128)
		return; // Ogg is paused

	if (ogg.pcm_indx >= READ_SAMPLES) {
		if (ASND_AddVoice(ogg.voice,
				(void *) ogg.pcmout[ogg.pcmout_pos],
				ogg.pcm_indx << 1) == 0) {
			ogg.pcmout_pos ^= 1;
			ogg.pcm_indx = 0;
			ogg.flag = 0;
			LWP_ThreadSignal(ogg.queue);
		}
	} else if (ogg.flag & 64) {
    ogg.flag &= ~64;
    LWP_ThreadSignal(ogg.queue);
	}
}

static void * ogg_player_thread(void* arg) {
  OggData* ogg = (OggData*) arg;

	int first_time = 1;
	long ret;

	//init
	LWP_InitQueue(&ogg->queue);

	ogg->vi = ov_info(&ogg->vf, -1);

	ASND_PauseVoice(ogg->voice, 0);

	ogg->pcm_indx = 0;
	ogg->pcmout_pos = 0;
	ogg->eof = 0;
	ogg->flag = 0;
	ogg->current_section = 0;

	ogg->running = 1;

	while (!ogg->eof && ogg->running) {
		if (ogg->flag)
			LWP_ThreadSleep(ogg->queue); // wait only when i have samples to send
  
    // wait to all samples are sent
		if (ogg->flag == 0) {
			if (ASND_TestPointer(ogg->voice, ogg->pcmout[ogg->pcmout_pos])
					&& ASND_StatusVoice(ogg->voice) != SND_UNUSED) {
				ogg->flag |= 64;
				continue;
			}

			if (ogg->pcm_indx < READ_SAMPLES) {
				ogg->flag = 3;

				if (ogg->seek_time >= 0) {
					ov_time_seek(&ogg->vf, ogg->seek_time);
					ogg->seek_time = -1;
				}

				ret = ov_read(&ogg->vf,
                    (char *) &ogg->pcmout[ogg->pcmout_pos][ogg->pcm_indx],
                    MAX_PCMOUT, &ogg->current_section);

				ogg->flag &= 192;
				if (ret == 0) {
					/* EOF */
					if (ogg->mode & 1)
						ov_time_seek(&ogg->vf, 0); // repeat
					else
						ogg->eof = 1; // stops
				} else if (ret < 0) {
					/* error in the stream.  Not a problem, just reporting it in
					 case we (the app) cares.  In this case, we don't. */
					if (ret != OV_HOLE) {
						if (ogg->mode & 1)
							ov_time_seek(&ogg->vf, 0); // repeat
						else
							ogg->eof = 1; // stops
					}
				} else {
					/* we don't bother dealing with sample rate changes, etc, but
					 you'll have to*/
					ogg->pcm_indx += ret >> 1; //get 16 bits samples
				}
			}
			else
				ogg->flag = 1;
		}

		if (ogg->flag == 1) {
			if (ASND_StatusVoice(ogg->voice) == SND_UNUSED || first_time) {
				first_time = 0;
				if (ogg->vi->channels == 2) {
					ASND_SetVoice(ogg->voice, VOICE_STEREO_16BIT, ogg->vi->rate, 0,
							(void *) ogg->pcmout[ogg->pcmout_pos],
							ogg->pcm_indx << 1, ogg->volume,
							ogg->volume, ogg_add_callback);
					ogg->pcmout_pos ^= 1;
					ogg->pcm_indx = 0;
					ogg->flag = 0;
				} else {
					ASND_SetVoice(ogg->voice, VOICE_MONO_16BIT, ogg->vi->rate, 0,
							(void *) ogg->pcmout[ogg->pcmout_pos],
							ogg->pcm_indx << 1, ogg->volume,
							ogg->volume, ogg_add_callback);
					ogg->pcmout_pos ^= 1;
					ogg->pcm_indx = 0;
					ogg->flag = 0;
				}
			}
		}
		usleep(100);
	}

	ov_clear(&ogg->vf);
	ogg->pcm_indx = 0;
	ogg->open = false;
	return 0;
}

void SFX_Setup() {
	ASND_Init();
}

void SFX_Stop(s32 voice) {
	OggData& ogg = tracks[voice];

	ASND_StopVoice(voice);
	ogg.running = 0;

	if(ogg.thread != LWP_THREAD_NULL) {
		if(ogg.queue != LWP_TQUEUE_NULL)
			LWP_ThreadSignal(ogg.queue);
		LWP_JoinThread(ogg.thread, NULL);
		ogg.thread = LWP_THREAD_NULL;
	}

	if(ogg.queue != LWP_TQUEUE_NULL) {
		LWP_CloseQueue(ogg.queue);
		ogg.queue = LWP_TQUEUE_NULL;
	}
}

int SFX_Play(s32 voice, void* buffer, int len, int time_pos) {
	SFX_Stop(voice);
	ASND_Pause(0);

	OggData& ogg = tracks[voice];
	ogg.mode = 0;
	ogg.eof = 0;
	ogg.volume = 127;
	ogg.flag = 0;
	ogg.seek_time = -1;

	if (time_pos > 0)
		ogg.seek_time = time_pos;

	if (ov_open_callbacks((void *)&tracks[voice], &ogg.vf, NULL, 0, callbacks) < 0) {
		mem_close(ogg); // mem_close() can too close files from devices
		ogg.open = false;
		ogg.running = 0;
		return -1;
	}

  if (LWP_CreateThread(&ogg.thread, ogg_player_thread,
      &tracks[voice], ogg.stack, STACKSIZE, 80) == -1) {
    ogg.running = 0;
    ov_clear(&ogg.vf);
		ogg.open = false;
    return -1;
  }

	return 0;
}

// returns voice or -1 if there was an error
int SFX_Load(const char* path) {
  FILE *f = fopen(path, "rb");
  if (!f) {
    printf("Failed to open file\n");
    return -1;
  }

  fseek(f, 0, SEEK_END);
  int size = ftell(f);
  fseek(f, 0, SEEK_SET);

  void* buffer = malloc(size);
  fread(buffer, 1, size, f);
  fclose(f);

	s32 voice = ASND_GetFirstUnusedVoice();

	OggData& ogg = tracks[voice];
  mem_open(ogg, voice, (char *)buffer, size);
	return voice;
}

void SFX_Pause(s32 voice) {
	OggData& ogg = tracks[voice];
	ogg.flag |= 128;
}

void SFX_Unpause(s32 voice) {
	OggData& ogg = tracks[voice];

  if (ogg.flag & 128) {
    ogg.flag |= 64;
    ogg.flag &= ~128;
    if (ogg.running > 0)
      LWP_ThreadSignal(ogg.queue);
  }
}

int SFX_Status(s32 voice) {
	OggData& ogg = tracks[voice];

	if (ogg.running == 0)
		return OGG_STATUS_ERR; // Error

	if (ogg.eof)
		return OGG_STATUS_EOF; // EOF

	if (ogg.flag & 128)
		return OGG_STATUS_PAUSED; // paused

  return OGG_STATUS_RUNNING; // running
}

void SFX_Volume(s32 voice, int volume) {
	OggData& ogg = tracks[voice];

	ogg.volume = volume;
	ASND_ChangeVolumeVoice(voice, volume, volume);
}

s32 SFX_GetTime(s32 voice) {
	OggData& ogg = tracks[voice];

	int ret;
	if (ogg.running == 0 || !ogg.open)
		return -1;

	ret = ((s32) ov_time_tell(&ogg.vf));

	return ret;
}

void SFX_Seek(s32 voice, s32 time_pos) {
	OggData& ogg = tracks[voice];
	if (time_pos >= 0)
		ogg.seek_time = time_pos;
}