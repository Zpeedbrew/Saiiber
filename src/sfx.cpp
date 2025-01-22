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

#define PAUSED 128
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

static FILE* trackfile;

static struct OggData {
	OggVorbis_File vf;  // Vorbis File
	vorbis_info *vi;    // Vorbis Info
	int current_section;

  struct {
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
} track;

// out is a pointer to the buffer to be filled with the data
// bps is the number of bytes per sample (2 for 16-bit stereo)
// samples is the number of samples to read
// void* is a pointer to the datasource (in this case we just use the static trackfile)
static int f_read(void* out, int bps, int samples, void*) {
	if (bps * samples <= 0 || feof(trackfile))
		return 0;

	if (track.file.size == 0)
		return -1;

	size_t samplesRead = fread(out, bps, samples, trackfile);
	track.file.pos += samplesRead * bps;
	return samplesRead;
}

// Yes, I know this is redundant. I kept it as an example for myself.
static int f_seek(void*, ogg_int64_t offset, int seektype) {
	if (track.file.size == 0)
		return -1;

	int ret = fseek(trackfile, offset, seektype);

	switch (seektype) {
	case SEEK_SET:
		if (offset >= track.file.size) {
			track.file.pos = track.file.size;
			return -1;
		}
		
		if (offset < 0) {
			track.file.pos = 0;
			return -1;
		}

		track.file.pos = offset;
		return ret;

	case SEEK_CUR:
		if ((track.file.pos + offset) >= track.file.size) {
			track.file.pos = track.file.size;
			return -1;
		}

		if ((track.file.pos + offset) < 0) {
			track.file.pos = 0;
			return -1;
		}

		track.file.pos += offset;
		return ret;

	case SEEK_END:
		if ((track.file.size + offset) >= track.file.size) {
			track.file.pos = track.file.size;
			return -1;
		}

		if ((track.file.size + offset) < 0) {
			track.file.pos = 0;
			return -1;
		}

		track.file.pos = track.file.size + offset;
		return ret;
	}

	return ret;
}

static int f_close(void*) {
	track.file.size = 0;
	track.file.pos = 0;
	track.open = false;

	fclose(trackfile);
	trackfile = NULL;
	return 0;
}

static long f_tell(void*) {
	return track.file.pos;
}

static void mem_close(OggData& ogg) {
  // it is a memory file descriptor?
	track.file.size = 0;
	track.open = false;
  f_close(&track.file);
}

static ov_callbacks callbacks = {
	(size_t (*)(void *, size_t, size_t, void *))  f_read,
	(int (*)(void *, ogg_int64_t, int))           f_seek,
	(int (*)(void *))                             f_close,
	(long (*)(void *))                            f_tell
};

static void ogg_add_callback(s32 voice) {
	if (!track.running) {
		ASND_StopVoice(track.voice);
		return;
	}

	if (track.flag & PAUSED)
		return; // Ogg is paused

	if (track.pcm_indx >= READ_SAMPLES) {
		if (ASND_AddVoice(track.voice,
				(void *) track.pcmout[track.pcmout_pos],
				track.pcm_indx << 1) == 0) {
			track.pcmout_pos ^= 1;
			track.pcm_indx = 0;
			track.flag = 0;
			LWP_ThreadSignal(track.queue);
		}
	} else if (track.flag & READY) {
    track.flag &= ~READY;
    LWP_ThreadSignal(track.queue);
	}
}

static void * ogg_player_thread(void* arg) {
	int first_time = 1;
	long ret;

	//init
	LWP_InitQueue(&track.queue);

	track.vi = ov_info(&track.vf, -1);

	ASND_PauseVoice(track.voice, 0);

	track.pcm_indx = 0;
	track.pcmout_pos = 0;
	track.eof = 0;
	track.flag = 0;
	track.current_section = 0;

	track.running = 1;

	s32 fmt = track.vi->channels == 2  ? VOICE_STEREO_16BIT : VOICE_MONO_16BIT;

	while (!track.eof && track.running) {
		if (track.flag)
			LWP_ThreadSleep(track.queue); // wait only when i have samples to send
  
    // wait to all samples are sent
		if (track.flag == 0) {
			if (ASND_TestPointer(track.voice, track.pcmout[track.pcmout_pos])
					&& ASND_StatusVoice(track.voice) != SND_UNUSED) {
				track.flag |= READY;
				continue;
			}

			if (track.pcm_indx < READ_SAMPLES) {
				track.flag = 3;

				if (track.seek_time >= 0) {
					ov_time_seek(&track.vf, track.seek_time);
					track.seek_time = -1;
				}

				ret = ov_read(&track.vf,
                    (char *) &track.pcmout[track.pcmout_pos][track.pcm_indx],
                    MAX_PCMOUT, &track.current_section);

				track.flag &= READY | PAUSED;

				// EOF or Error
				if (ret <= 0) {
					if (ret != OV_HOLE)
						track.eof = 1; // stops
				} else {
					/* we don't bother dealing with sample rate changes, etc, but
					 you'll have to*/
					track.pcm_indx += ret >> 1; //get 16 bits samples
				}
			}
			else
				track.flag = FULL;
		}

		if (track.flag == FULL) {
			if (ASND_StatusVoice(track.voice) == SND_UNUSED || first_time) {
				first_time = 0;
				ASND_SetVoice(track.voice, fmt, track.vi->rate, 0,
						(void *) track.pcmout[track.pcmout_pos],
						track.pcm_indx << 1, track.volume,
						track.volume, ogg_add_callback);
				track.pcmout_pos ^= 1;
				track.pcm_indx = 0;
				track.flag = 0;
			}
		}
		usleep(100);
	}

	ov_clear(&track.vf);
	track.pcm_indx = 0;
	track.open = false;
	return 0;
}

void SFX_Init() {
	ASND_Init();
}

void SFX_Stop(s32 voice) {
	ASND_StopVoice(voice);
	track.running = 0;

	if(track.thread != LWP_THREAD_NULL) {
		if(track.queue != LWP_TQUEUE_NULL)
			LWP_ThreadSignal(track.queue);
		LWP_JoinThread(track.thread, NULL);
		track.thread = LWP_THREAD_NULL;
	}

	if(track.queue != LWP_TQUEUE_NULL) {
		LWP_CloseQueue(track.queue);
		track.queue = LWP_TQUEUE_NULL;
	}
}

int SFX_Play(s32 voice, int time_pos) {
	SFX_Stop(voice);
	ASND_Pause(0);

	track.mode = 0;
	track.eof = 0;
	track.volume = 127;
	track.flag = 0;
	track.seek_time = -1;

	if (time_pos > 0)
		track.seek_time = time_pos;

	if (ov_open_callbacks((void *)&track, &track.vf, NULL, 0, callbacks) < 0) {
		mem_close(track); // mem_close() can too close files from devices
		track.open = false;
		track.running = 0;
		return -1;
	}

  if (LWP_CreateThread(&track.thread, ogg_player_thread,
      NULL, track.stack, STACKSIZE, 80) == -1) {
    track.running = 0;
    ov_clear(&track.vf);
		track.open = false;
    return -1;
  }

	return 0;
}

// returns voice or -1 if there was an error
int SFX_Load(const char* path) {
  trackfile = fopen(path, "rb");
  if (!trackfile) {
    printf("Failed to open track file\n");
    return -1;
  }

  fseek(trackfile, 0, SEEK_END);
  int size = ftell(trackfile);
  fseek(trackfile, 0, SEEK_SET);

	track.voice = ASND_GetFirstUnusedVoice();
	if (track.voice < 0) {
		printf("No voices available\n");
		return -1;
	}

	track.file.size = size;
	track.file.pos = 0;
	track.open = true;
	return track.voice;
}

void SFX_Pause(s32 voice) {
	track.flag |= PAUSED;
}

void SFX_Unpause(s32 voice) {
  if (track.flag & PAUSED) {
    track.flag |= READY;
    track.flag &= ~PAUSED;
    if (track.running > 0)
      LWP_ThreadSignal(track.queue);
  }
}

int SFX_Status(s32 voice) {
	if (track.running == 0)
		return OGG_STATUS_ERR; // Error

	if (track.eof)
		return OGG_STATUS_EOF; // EOF

	if (track.flag & PAUSED)
		return OGG_STATUS_PAUSED; // paused

  return OGG_STATUS_RUNNING; // running
}

void SFX_Volume(s32 voice, int volume) {
	track.volume = volume;
	ASND_ChangeVolumeVoice(voice, volume, volume);
}

s32 SFX_GetTime(s32 voice) {
	int ret;
	if (track.running == 0 || !track.open)
		return -1;

	ret = ((s32) ov_time_tell(&track.vf));

	return ret;
}

void SFX_Seek(s32 voice, s32 time_pos) {
	if (time_pos >= 0)
		track.seek_time = time_pos;
}

void SFX_Cleanup() {
	for (int i = 0; i < NUM_TRACKS; i++) {
		SFX_Stop(i);
	}

	ASND_End();
}