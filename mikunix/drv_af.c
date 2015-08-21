/*

   Name:
   DRV_AF.C

   Description:
   Mikmod driver for output on AF audio server.

   Written by Roine Gustafsson <e93_rog@e.kth.se> Oct 25, 1995

   Portability:
   Unixes running Digital AudioFile library, available from
   ftp://crl.dec.com/pub/DEC/AF

   Usage:
   Run the audio server (Aaxp&, Amsb&, whatever)
   Set environment variable AUDIOFILE to something like 'mymachine:0'.
   Remember, stereo is default! See commandline switches.

   I have a version which uses 2 computers for stereo.
   Contact me if you want it.

   THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
   INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
   IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR
   CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
   NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   History:
   ver 1.21 Bugfix: AFFragmentSize could end up uneven.
   Thanks to Marcus Sundberg <e94_msu@e.kth.se>
   ver 1.22 Compatibility: change "AFBuffer" to "audiobuffer"
   Steve McIntyre <stevem@chiark.greenend.org.uk>
   ver 1.30 Compatibility: Use new driver API
   Steve McIntyre <stevem@chiark.greenend.org.uk>
 */

#include <malloc.h>
#include <stdlib.h>
#include <AF/AFlib.h>

#include "mikmod.h"

/* Global variables */

SBYTE *audiobuffer;
static int AFFragmentSize;
AFAudioConn *AFaud;
ATime AFtime;
AC AFac;
AFDeviceDescriptor *AFdesc;

BOOL AF_IsThere(void)
{

    /* I'll think of a detection routine ... somtime */

    return 1;

}

BOOL AF_Init(void)
{
    unsigned long mask;
    AFSetACAttributes attributes;
    int srate;
    ADevice device;
    unsigned int channels;
    AEncodeType type;
    char *server;
    int n;

    AFaud = AFOpenAudioConn("");
    if (AFaud == NULL) {
	myerr = "Cannot open sounddevice.";
	return 0;
    }
    /* Search for a suitable device */
    device = -1;
    for (n = 0; n < AFaud->ndevices; n++) {
	AFdesc = AAudioDeviceDescriptor(AFaud, n);
	if (AFdesc->playNchannels == 2 && md_mode & DMODE_STEREO) {
	    device = n;
	    break;
	}
	if (AFdesc->playNchannels == 1 && !(md_mode & DMODE_STEREO)) {
	    device = n;
	    break;
	}
    }
    if (device == -1) {
	myerr = "Cannot find suitable audio port!";
	AFCloseAudioConn(AFaud);
	return 0;
    }
    attributes.preempt = Mix;
    attributes.start_timeout = 0;
    attributes.end_silence = 0;
    attributes.type = LIN16;	/* In case of an 8bit device, the AF converts the 16 bit data to 8 bit */
    attributes.channels = (md_mode & DMODE_STEREO) ? Stereo : Mono;

    mask = ACPreemption | ACEncodingType | ACStartTimeout | ACEndSilence | ACChannels;
    AFac = AFCreateAC(AFaud, device, mask, &attributes);
    srate = AFac->device->playSampleFreq;

    md_mode |= DMODE_16BITS;	/* This driver only handles 16bits */
    AFFragmentSize = (srate / 40) * 8;	/* Update 5 times/sec */
    md_mixfreq = srate;		/* set mixing freq */

    if (md_mode & DMODE_STEREO) {
	if ((audiobuffer = (SBYTE *) malloc(2 * 2 * AFFragmentSize)) == NULL) {
	    myerr = "Out of memory!";
	    AFCloseAudioConn(AFaud);
	    return 0;
	}
    } else {
	if ((audiobuffer = (SBYTE *) malloc(2 * AFFragmentSize)) == NULL) {
	    myerr = "Out of memory!";
	    AFCloseAudioConn(AFaud);
	    return 0;
	}
    }

    if (!VC_Init()) {
	AFCloseAudioConn(AFaud);
	free(audiobuffer);
	return 0;
    }
    return 1;
}

void AF_PlayStart(void)
{

    AFtime = AFGetTime(AFac);
    VC_PlayStart();

}


void AF_Exit(void)
{

    VC_Exit();
    AFCloseAudioConn(AFaud);
    free(AFBuffer);

}

void AF_Update(void)
{

    UWORD *p, *l, *r;
    int i;


    VC_WriteBytes(AFBuffer, AFFragmentSize);
    if (md_mode & DMODE_STEREO) {
	AFPlaySamples(AFac, AFtime, AFFragmentSize, (unsigned char *) AFBuffer);
	AFtime += AFFragmentSize / 4;
    } else {
	AFPlaySamples(AFac, AFtime, AFFragmentSize, (unsigned char *) AFBuffer);
	AFtime += AFFragmentSize / 2;
    }

}


DRIVER drv_AF =
{
    NULL,
    "AF driver",
    "MikMod AudioFile Driver v1.22",
    AF_IsThere,
    VC_SampleLoad,
    VC_SampleUnload,
    AF_Init,
    AF_Exit,
    AF_PlayStart,
    VC_PlayStop,
    AF_Update,
    VC_VoiceSetVolume,
    VC_VoiceSetFrequency,
    VC_VoiceSetPanning,
    VC_VoicePlay,
    MD_BlankFunction,
    MD_BlankFunction,
    MD_BlankFunction
};
