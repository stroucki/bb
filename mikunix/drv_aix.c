/*

   Name:
   drv_aix.c

   Description:
   Mikmod driver for output to AIX series audio device

   Portability:

   AIX >= 3.25 with appropriate audio hardware.

   Written by Lutz Vieweg <lkv@mania.robin.de>
   based on drv_raw.c

   Feel free to distribute just as you like.
   No warranties at all.

 */

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/audio.h>

#include "mikmod.h"

struct _track_info {
    unsigned short master_volume;
    unsigned short dither_percent;
    unsigned short reserved[3];
} t_info;
audio_init a_init;
audio_control a_control;
audio_change a_change;


static int fd;
#define RAWBUFFERSIZE 32768
static char RAW_DMABUF[RAWBUFFERSIZE];

static BOOL AIX_IsThere(void)
{
    return 1;
}

static BOOL AIX_Init(void)
{
    float volume = (float) 1.0;
    int flags;

    if (!(md_mode & DMODE_16BITS)) {
	myerr = "sorry, this driver supports 16-bit linear output only";
	return 0;
    }
    if ((fd = open("/dev/acpa0/1", O_WRONLY)) < 3)
	if ((fd = open("/dev/paud0/1", O_WRONLY)) < 3)
	    if ((fd = open("/dev/baud0/1", O_WRONLY)) < 3) {
		myerr = "unable to open audio-device";
		return 0;
	    }
    t_info.master_volume = 0x7fff;
    t_info.dither_percent = 0;

    a_init.srate = md_mixfreq;
    a_init.bits_per_sample = 16;
    a_init.channels = (md_mode & DMODE_STEREO) ? 2 : 1;
    a_init.mode = PCM;
    a_init.flags = FIXED | BIG_ENDIAN | TWOS_COMPLEMENT;
    a_init.operation = PLAY;

    a_change.balance = 0x3fff0000;
    a_change.balance_delay = 0;
    a_change.volume = (long) (volume * (float) 0x7FFFFFFF);
    a_change.volume_delay = 0;
    a_change.monitor = AUDIO_IGNORE;
    a_change.input = AUDIO_IGNORE;
    a_change.output = OUTPUT_1;
    a_change.dev_info = &t_info;

    a_control.ioctl_request = AUDIO_CHANGE;
    a_control.position = 0;
    a_control.request_info = &a_change;

    if (ioctl(fd, AUDIO_INIT, &a_init) == -1) {
	myerr = "configuration (init) of audio device failed";
	return 0;
    }
    if (ioctl(fd, AUDIO_CONTROL, &a_control) == -1) {
	myerr = "configuration (control) of audio device failed";
	return 0;
    }
    a_control.ioctl_request = AUDIO_START;
    a_control.request_info = NULL;
    if (ioctl(fd, AUDIO_CONTROL, &a_control) == -1) {
	myerr = "configuration (start) of audio device failed";
	return 0;
    }

    if (!VC_Init()) {
	close(fd);
	return 0;
    }
    return 1;
}



static void AIX_Exit(void)
{
    VC_Exit();
    close(fd);
}


static void AIX_Update(void)
{
    VC_WriteBytes(RAW_DMABUF, RAWBUFFERSIZE);
    write(fd, RAW_DMABUF, RAWBUFFERSIZE);
}


DRIVER drv_aix =
{
    NULL,
    "AIX audio-device",
    "MikMod AIX audio-device driver v1.10",
    AIX_IsThere,
    VC_SampleLoad,
    VC_SampleUnload,
    AIX_Init,
    AIX_Exit,
    VC_PlayStart,
    VC_PlayStop,
    AIX_Update,
    VC_VoiceSetVolume,
    VC_VoiceSetFrequency,
    VC_VoiceSetPanning,
    VC_VoicePlay,
    MD_BlankFunction,
    MD_BlankFunction,
    MD_BlankFunction
};
