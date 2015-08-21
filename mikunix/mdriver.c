/*

   Name:
   MDRIVER.C

   Description:
   These routines are used to access the available soundcard drivers.

   Portability:
   All systems - all compilers

 */
#include <stdio.h>
#include <stdlib.h>
#include "mikmod.h"

DRIVER *firstdriver = NULL, *md_driver;

UWORD md_device = 0;
UWORD md_mixfreq = 44100;
UWORD md_mode = 0;
UWORD md_dmabufsize = 8192;
UBYTE md_numchn = 0;
UBYTE md_bpm = 125;

static void dummyplay(void)
{
}

void (*md_player) (void) = dummyplay;

static FILE *sl_fp;
static SWORD sl_old;
static UWORD sl_infmt;
static UWORD sl_outfmt;
static SWORD sl_buffer[1024];

static BOOL isplaying = 0;


void SL_Init(FILE * fp, UWORD infmt, UWORD outfmt)
{
    sl_old = 0;
    sl_fp = fp;
    sl_infmt = infmt;
    sl_outfmt = outfmt;
}


void SL_Exit(void)
{
}


void SL_Load(void *buffer, ULONG length)
{
    SBYTE *bptr = (SBYTE *) buffer;
    SWORD *wptr = (SWORD *) buffer;
    UWORD stodo;
    int t;

    /* compute number of samples to load */
    if (sl_outfmt & SF_16BITS)
	length >>= 1;

    while (length) {

	stodo = (length < 1024) ? length : 1024;

	if (sl_infmt & SF_16BITS) {
	    fread(sl_buffer, sizeof(SWORD), stodo, sl_fp);
#ifdef MM_BIG_ENDIAN
	    if (!(sl_infmt & SF_BIG_ENDIAN))
		swab((char *) sl_buffer, (char *) sl_buffer, stodo << 1);
#else
	    /* assume machine is little endian by default */
	    if (sl_infmt & SF_BIG_ENDIAN)
		swab((char *) sl_buffer, (char *) sl_buffer, stodo << 1);
#endif
	} else {
	    SBYTE *s;
	    SWORD *d;

	    fread(sl_buffer, sizeof(SBYTE), stodo, sl_fp);

	    s = (SBYTE *) sl_buffer;
	    d = sl_buffer;
	    s += stodo;
	    d += stodo;

	    for (t = 0; t < stodo; t++) {
		s--;
		d--;
		*d = (*s) << 8;
	    }
	}

	if (sl_infmt & SF_DELTA) {
	    for (t = 0; t < stodo; t++) {
		sl_buffer[t] += sl_old;
		sl_old = sl_buffer[t];
	    }
	}
	if ((sl_infmt ^ sl_outfmt) & SF_SIGNED) {
	    for (t = 0; t < stodo; t++) {
		sl_buffer[t] ^= 0x8000;
	    }
	}
	if (sl_outfmt & SF_16BITS) {
	    for (t = 0; t < stodo; t++)
		*(wptr++) = sl_buffer[t];
	} else {
	    for (t = 0; t < stodo; t++)
		*(bptr++) = sl_buffer[t] >> 8;
	}

	length -= stodo;
    }
}


void MD_InfoDriver(void)
{
    int t;
    DRIVER *l;

    /* list all registered devicedrivers: */

    for (t = 1, l = firstdriver; l != NULL; l = l->next, t++) {
	printf("%d. %s\n", t, l->Version);
    }
}


void MD_RegisterDriver(DRIVER * drv)
{
    if (firstdriver == NULL) {
	firstdriver = drv;
	drv->next = NULL;
    } else {
	drv->next = firstdriver;
	firstdriver = drv;
    }
}


SWORD MD_SampleLoad(FILE * fp, ULONG size, ULONG reppos, ULONG repend, UWORD flags)
{
    SWORD result = md_driver->SampleLoad(fp, size, reppos, repend, flags);
    SL_Exit();
    return result;
}


void MD_SampleUnLoad(SWORD handle)
{
    md_driver->SampleUnLoad(handle);
}

void MD_PatternChange(void)
{
    md_driver->PatternChange();
}

void MD_Mute(void)
{
    md_driver->Mute();
}

void MD_UnMute(void)
{
    md_driver->UnMute();
}

void MD_BlankFunction(void)
{
}

BOOL MD_Init(void)
{
    UWORD t;

    /* if md_device==0, try to find a device number */

    if (md_device == 0) {

	for (t = 1, md_driver = firstdriver; md_driver != NULL; md_driver = md_driver->next, t++) {
	    if (md_driver->IsPresent())
		break;
	}

	if (md_driver == NULL) {
	    myerr = "You don't have any of the supported sound-devices";
	    return 0;
	}
	md_device = t;
    }
    /* if n>0 use that driver */

    for (t = 1, md_driver = firstdriver; md_driver != NULL && t != md_device; md_driver = md_driver->next, t++);

    if (md_driver == NULL) {
	myerr = "Device number out of range";
	return 0;
    }
    return (md_driver->Init());
}


void MD_Exit(void)
{
    md_driver->Exit();
}


void MD_PlayStart(void)
{
    /* safety valve, prevents entering
       playstart twice: */

    if (isplaying)
	return;
    md_driver->PlayStart();
    isplaying = 1;
}


void MD_PlayStop(void)
{
    /* safety valve, prevents calling playStop when playstart
       hasn't been called: */

    if (isplaying) {
	isplaying = 0;
	md_driver->PlayStop();
    }
}


void MD_SetBPM(UBYTE bpm)
{
    md_bpm = bpm;
}


void MD_RegisterPlayer(void (*player) (void))
{
    md_player = player;
}


void MD_Update(void)
{
    if (isplaying)
	md_driver->Update();
}


void MD_VoiceSetVolume(UBYTE voice, UBYTE vol)
{
    md_driver->VoiceSetVolume(voice, vol);
}


void MD_VoiceSetFrequency(UBYTE voice, ULONG frq)
{
    md_driver->VoiceSetFrequency(voice, frq);
}


void MD_VoiceSetPanning(UBYTE voice, ULONG pan)
{
    md_driver->VoiceSetPanning(voice, pan);
}


void MD_VoicePlay(UBYTE voice, SWORD handle, ULONG start, ULONG size, ULONG reppos, ULONG repend, UWORD flags)
{
    md_driver->VoicePlay(voice, handle, start, size, reppos, repend, flags);
}
