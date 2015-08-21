/*

   Name:
   drv_hp.c

   Description:
   Mikmod driver for output to HP 9000 series /dev/audio

   Portability:

   HP-UX only

   Written by Lutz Vieweg <lkv@mania.robin.de>
   based on drv_raw.c

   Feel free to distribute just as you like.
   No warranties at all.

 */

#include <stdio.h>
#include <stdlib.h>

#include <sys/audio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "mikmod.h"

static int fd;
#define RAWBUFFERSIZE 16384
static char RAW_DMABUF[RAWBUFFERSIZE];

static BOOL HP_IsThere(void)
{
    return 1;
}

static BOOL HP_Init(void)
{
    int flags;

    if (!(md_mode & DMODE_16BITS)) {
	myerr = "sorry, this driver supports 16-bit linear output only";
	return 0;
    }
    if ((fd = open("/dev/audio", O_WRONLY | O_NDELAY, 0)) < 0) {
	myerr = "unable to open /dev/audio";
	return 0;
    }
    if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
	myerr = "unable to set non-blocking mode for /dev/audio";
	return 0;
    }
    flags |= O_NDELAY;
    if (fcntl(fd, F_SETFL, flags) < 0) {
	myerr = "unable to set non-blocking mode for /dev/audio";
	return 0;
    }
    if (ioctl(fd, AUDIO_SET_DATA_FORMAT, AUDIO_FORMAT_LINEAR16BIT)) {
	myerr = "unable to select 16bit-linear sample format";
	return 0;
    }
    if (ioctl(fd, AUDIO_SET_SAMPLE_RATE, md_mixfreq)) {
	myerr = "unable to select requested sample-rate";
	return 0;
    }
    if (ioctl(fd, AUDIO_SET_CHANNELS, (md_mode & DMODE_STEREO) ? 2 : 1)) {
	myerr = "unable to select requested number of channels";
	return 0;
    }
    /*
       choose between:
       AUDIO_OUT_SPEAKER
       AUDIO_OUT_HEADPHONE
       AUDIO_OUT_LINE
     */
    if (ioctl(fd, AUDIO_SET_OUTPUT,
	      AUDIO_OUT_SPEAKER | AUDIO_OUT_HEADPHONE | AUDIO_OUT_LINE)) {
	myerr = "unable to select audio output";
	return 0;
    } {
	struct audio_describe description;
	struct audio_gains gains;
	float volume = 1.0;
	if (ioctl(fd, AUDIO_DESCRIBE, &description)) {
	    myerr = "unable to get audio description";
	    return 0;
	}
	if (ioctl(fd, AUDIO_GET_GAINS, &gains)) {
	    myerr = "unable to get gain values";
	    return 0;
	}
	gains.transmit_gain = (int) ((float) description.min_transmit_gain +
				   (float) (description.max_transmit_gain
					 - description.min_transmit_gain)
				     * volume);
	if (ioctl(fd, AUDIO_SET_GAINS, &gains)) {
	    myerr = "unable to set gain values";
	    return 0;
	}
    }

    if (ioctl(fd, AUDIO_SET_TXBUFSIZE, RAWBUFFERSIZE * 8)) {
	myerr = "unable to set transmission buffer size";
	return 0;
    }
    if (!VC_Init()) {
	close(fd);
	return 0;
    }
    return 1;
}



static void HP_Exit(void)
{
    VC_Exit();
    close(fd);
}


static void HP_Update(void)
{
    VC_WriteBytes(RAW_DMABUF, RAWBUFFERSIZE);
    write(fd, RAW_DMABUF, RAWBUFFERSIZE);
}


DRIVER drv_hp =
{
    NULL,
    "HP-UX /dev/audio",
    "MikMod HP-UX /dev/audio driver v1.10",
    HP_IsThere,
    VC_SampleLoad,
    VC_SampleUnload,
    HP_Init,
    HP_Exit,
    VC_PlayStart,
    VC_PlayStop,
    HP_Update,
    VC_VoiceSetVolume,
    VC_VoiceSetFrequency,
    VC_VoiceSetPanning,
    VC_VoicePlay,
    MD_BlankFunction,
    MD_BlankFunction,
    MD_BlankFunction
};
