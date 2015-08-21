/*

   Name:
   DRV_SGI.C, version 0.2
   Copyright 1996 by Stephan Kanthak, kanthak@i6.informatik.rwth-aachen.de

   Version 0.2 - updated to use new driver API

   Description:
   Mikmod driver for output on SGI audio system (needs libaudio from the
   dmedia package).

   Portability:
   SGI only. Mainly based on voxware driver.

   Fragment configuration:
   =======================

   You can use the environment variables 'MM_SGI_FRAGSIZE' and
   'MM_SGI_BUFSIZE' to override the default size of the audio buffer. If you
   experience crackles & pops, try experimenting with these values.

   Please read README.SGI first before contacting the author because there are
   some things to know about the specials of the SGI audio driver.

 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dmedia/audio.h>
#include "mikmod.h"

#define DEFAULT_SGI_FRAGSIZE  5000
#define DEFAULT_SGI_BUFSIZE   10000

static ALconfig sgi_config;
static ALport sgi_port;
static int sample_factor;
static int sgi_fragsize;
static int sgi_bufsize;
static char *audiobuffer;


static BOOL SGI_IsThere(void)
{
    long params[] =
    {AL_OUTPUT_RATE, 0};

    ALqueryparams(AL_DEFAULT_DEVICE, params, 2);
    if (params[1] != 0)
	return 1;
    else
	return 0;
}

static BOOL SGI_Init(void)
{
    char *env;
    int play_rate;
    int fragsize, numfrags;
    long chpars[] =
    {AL_OUTPUT_RATE, AL_RATE_22050};

    printf("SGI audio driver: ");
    switch (md_mixfreq) {
    case 22050:
	chpars[1] = AL_RATE_22050;
	break;
    case 44100:
	chpars[1] = AL_RATE_44100;
	break;
    default:
	printf("mixing rate not supported.\n");
	return 0;
	break;
    }
    ALsetparams(AL_DEFAULT_DEVICE, chpars, 2);

    if ((sgi_config = ALnewconfig()) == 0) {
	printf("cannot configure sound device.\n");
	return 0;
    }
    if (md_mode & DMODE_16BITS) {
	if (ALsetwidth(sgi_config, AL_SAMPLE_16) < 0) {
	    printf("samplesize of 16 bits not supported.\n");
	    return 0;
	}
	sample_factor = 2;
    } else {
	if (ALsetwidth(sgi_config, AL_SAMPLE_8) < 0) {
	    printf("samplesize of 8 bits not supported.\n");
	    return 0;
	}
	sample_factor = 1;
    }

    if (md_mode & DMODE_STEREO) {
	if (ALsetchannels(sgi_config, AL_STEREO) < 0) {
	    printf("stereo mode not supported.\n");
	    return 0;
	}
    } else {
	if (ALsetchannels(sgi_config, AL_MONO) < 0) {
	    printf("mono mode not supported.\n");
	    return 0;
	}
    }

    if (!VC_Init()) {
	return 0;
    }
    sgi_fragsize = (env = getenv("MM_SGI_FRAGSIZE")) ? atol(env) : DEFAULT_SGI_BUFSIZE;
    sgi_bufsize = (env = getenv("MM_SGI_BUFSIZE")) ? atol(env) : DEFAULT_SGI_BUFSIZE;

    ALsetqueuesize(sgi_config, sgi_bufsize);
    if ((sgi_port = ALopenport("Mod4X", "w", sgi_config)) == 0) {
	printf("cannot open SGI audio port.\n");
	return 0;
    }

    audiobuffer = (char *) MyMalloc(sgi_fragsize * sizeof(char));

    if (audiobuffer == NULL) {
	VC_Exit();
	return 0;
    }
    return 1;
}


static void SGI_Exit(void)
{
    free(audiobuffer);
    VC_Exit();
}


static void SGI_Update(void)
{
    VC_WriteBytes(audiobuffer, sgi_fragsize);
    ALwritesamps(sgi_port, audiobuffer, sgi_fragsize / sample_factor);
}


DRIVER drv_sgi =
{
    NULL,
    "SGI Audio System",
    "SGI Audio System Driver v0.2 - by Stephan Kanthak",
    SGI_IsThere,
    VC_SampleLoad,
    VC_SampleUnload,
    SGI_Init,
    SGI_Exit,
    VC_PlayStart,
    VC_PlayStop,
    SGI_Update,
    VC_VoiceSetVolume,
    VC_VoiceSetFrequency,
    VC_VoiceSetPanning,
    VC_VoicePlay,
    MD_BlankFunction,
    MD_BlankFunction,
    MD_BlankFunction
};
