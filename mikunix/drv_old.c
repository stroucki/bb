/*

   MikMOD driver for the Linux Ultrasound Project 

   Portability:
   Linux - Gravis Ultrasound soundcard family

   Warranty:
   Yeah wright!

   Andy Lo-A-Foe <arloafoe@cs.vu.nl>

 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <libgus.h>

#include "mikmod.h"

/* #define DEBUG */

#define MAX_INSTRUMENTS		128	/* Max. instruments loadable   */
#define GUS_CHANNELS		32	/* Max. GUS channels available */
#define SIZE_OF_SEQBUF		(8 * 1024)	/* Size of the sequence buffer */

#define	TRUE	1
#define FALSE	0

#define	CH_FREQ		1
#define CH_VOL		2
#define	CH_PAN		4

int use_card = 0;

typedef unsigned short word;

struct actualPosition {
    word order;
    word row;
};

struct SAMPLE_DATA {		/* This structure is used to hold information  */
    char *sample;		/* about a loaded sample. The sample is also   */
    ULONG length;		/* kept in normal memory. Usefull for spectrum */
    ULONG loopstart;		/* or whatever analyzing of the music (later)  */
    ULONG loopend;
    UWORD flags;
    UWORD active;
};


struct VOICE {			/* A structure to hold the current state        */
    UBYTE in_use;		/* of a GUS voice channel. Again usefull for    */
    int handle;			/* music analyzing                              */
    UWORD flags;
    ULONG start;
    ULONG size;
    ULONG reppos;
    ULONG repend;
    ULONG freq;
    int kick;
    int changes;
    short vol;
    short pan;
};

/* Global declarations follow */

struct SAMPLE_DATA instrs[MAX_INSTRUMENTS];

struct VOICE voices[GUS_CHANNELS];

int ultra_dev = -1;
int gf1flag = -1;
int nr_voices, nr_instrs;
int oldRow = 0, oldOrder = 0;
int ultra_fd;
static UBYTE ULTRA_BPM;


static BOOL UltraIsThere(void)
{
    return TRUE;
}


static BOOL ResetCard(void)
{
    gus_select(gf1flag);
    if (gus_reset(nr_voices, 0) < 0) {
	return FALSE;
    }
    return TRUE;
}


static BOOL UltraInit(void)
{
    struct GUS_STRU_INFO info;
    int i;

    if (!(md_mode & DMODE_16BITS)) {
	md_mode |= DMODE_16BITS;	/* GUS doesn't do 8-bit */
    }
    if (!(md_mode & DMODE_STEREO)) {	/* and also no MONO */
	md_mode |= DMODE_STEREO;
    }
    md_mixfreq = 44100;		/* and also ignore freq. changes */

    md_type = 1;
#ifdef DEBUG
    printf("Detected %d card(s)\n", gus_cards());
#endif
    ultra_dev = use_card;	/* First card */
#ifdef DEBUG
    printf("Using card #%d\n", ultra_dev);
    fflush(stdout);
#endif
    if ((gf1flag = gus_open(ultra_dev, SIZE_OF_SEQBUF, 0)) < 0) {
	switch (errno) {
	case ENOMEM:
	    myerr = "Not enough memory for operation";
	    return 0;
	case EBUSY:
	    myerr = "device or resource busy";
	    return 0;
	default:
	    myerr = "device not found";
	    return 0;
	}
	return -1;
    }
    gus_select(gf1flag);

    ultra_fd = gus_get_handle();

    gus_info(&info, 0);

#ifdef DEBUG
    printf(" at 0x%x, IRQ %d, DMA %d\n", info.port, info.irq, info.dma1);
#endif

    return 1;
}


static unsigned int getSampleType(struct SAMPLE_DATA *smp)
{
    unsigned int result = 0;

    result = ((smp->flags & SF_16BITS) ? GUS_WAVE_16BIT : 0) |
	((smp->flags & SF_DELTA) ? GUS_WAVE_DELTA : 0) |
	((smp->flags & SF_LOOP) ? GUS_WAVE_LOOP : 0) |
	((smp->flags & SF_BIDI) ? GUS_WAVE_BIDIR : 0);
    return result;
}


int LoadSamples()
{
    struct SAMPLE_DATA *smp;
    gus_instrument_t instrument;
    gus_layer_t layer;
    gus_wave_t wave;
    ULONG length, loopstart, loopend;
    char *sample, *p;
    int i, l;
    unsigned int type;

    for (i = 0; i < nr_instrs; i++) {
	smp = &instrs[i];

	if (!(smp->active)) {
	    continue;
	}
	/*
	   printf("\rLoading sample %02d/%02d...",
	   nr_instrs, i+1);
	   fflush(stdout);
	 */
	length = smp->length;
	loopstart = smp->loopstart;
	loopend = smp->loopend;

	if (smp->flags & SF_16BITS) {
	    length <<= 1;
	    loopstart <<= 1;
	    loopend <<= 1;
	}
	memset(&instrument, 0, sizeof(instrument));
	memset(&layer, 0, sizeof(layer));
	memset(&wave, 0, sizeof(wave));
	instrument.mode = layer.mode = wave.mode = GUS_INSTR_SIMPLE;
	instrument.number.instrument = i;
	instrument.info.layer = &layer;
	layer.wave = &wave;
	type = getSampleType(smp);
	wave.format = (unsigned char) getSampleType(smp);
	wave.begin.ptr = smp->sample;
	wave.loop_start = loopstart << 4;
	wave.loop_end = loopend << 4;
	wave.size = length;
	if (smp->flags & SF_LOOP) {
	    smp->sample[loopend] = smp->sample[loopstart];
	    if (smp->flags & SF_16BITS) {
		smp->sample[loopend - 1] = smp->sample[loopstart - 1];
	    }
	}
	/* Download the sample to GUS RAM */
	if (gus_memory_alloc(&instrument) != 0) {
	    myerr = "error downloading sample.";
	    return -1;
	}
    }
    return 0;
}


SWORD UltraSampleLoad(FILE * fp, ULONG length, ULONG loopstart, ULONG loopend, UWORD flags)
{
    struct SAMPLE_DATA *smp;
    char *p;
    long l;

    smp = &instrs[nr_instrs];
    smp->length = length;
    smp->loopstart = loopstart;
    if (!loopend)
	smp->loopend = length - (flags & SF_16BITS ? 2 : 1);
    else
	smp->loopend = loopend;
    smp->flags = flags;		/* SF_16BITS */
    if ((smp->flags & SF_16BITS)) {
	length <<= 1;
	loopstart <<= 1;
	loopend <<= 1;
    }
    if ((smp->sample = (char *) malloc(length + 16)) == NULL) {
	myerr = "not enough memory to load sample.";
	return -1;
    }
    SL_Init(fp, flags, flags | SF_SIGNED);	/* SF_16BITS */

    l = length;
    p = smp->sample;

    while (l > 0) {
	static UBYTE buffer[8192];
	long todo;
	todo = (l > 8192) ? 8192 : l;
	SL_Load(p, todo);
	p += todo;
	l -= todo;
    }

    smp->active = 1;

    return nr_instrs++;
}


static void UltraSampleUnload(SWORD h)
{

    struct SAMPLE_DATA *smp;
    gus_instrument_t instrument;
#ifdef DEBUG
    printf("Ultra: will unload sample %d\n", h);
#endif
    if (h > MAX_INSTRUMENTS) {
#ifdef DEBUG
	printf("Ultra: sample out of range (%02d)\n", h);
#endif
	return;
    }
    smp = &instrs[h];

    if (!(smp->active))
	return;
    memset(&instrument, 0, sizeof(instrument));
    instrument.mode = GUS_INSTR_SIMPLE;
    instrument.number.instrument = h;
    gus_memory_free(&instrument);
    free(smp->sample);
    nr_instrs--;
    smp->active = 0;
}


static void UltraPlayer(void)
{
    int i, t, control;
    struct VOICE *voice;

    md_player();

    for (t = 0; t < md_numchn; t++) {
	voice = &voices[t];
	if (voice->changes & CH_FREQ)
	    gus_do_voice_frequency(t, voice->freq);
	if (voice->changes & CH_VOL);
	gus_do_voice_volume(t, voice->vol << 8);
	if (voice->changes & CH_PAN)
	    gus_do_voice_pan(t, voice->pan << 6);
	voice->changes = 0;
	if (voice->kick) {
	    voice->kick = 0;
	    if (voice->start > 0)
		gus_do_voice_start_position(t, voice->handle,
					    voice->freq, voice->vol << 8, voice->pan << 6, voice->start << 4);
	    else
		gus_do_voice_start(t, voice->handle, voice->freq, voice->vol << 8, voice->pan << 6);
	}
    }
    gus_do_wait(1);
}


void processPosChange(struct echoData *echo)
{
    extern curmod cur_mod;
    cur_mod.sngpos = echo->data1 + 1;
    cur_mod.patpos = echo->data2;
    update_status_display();
    display_status();
}

void processFreqChange(struct echoData *echo)
{
}

static void ProcessEcho(void)
{
    unsigned char buffer[128];
    unsigned char *ptr;
    int size, t;
    struct echoData echo;

    while ((size = gus_queue_get_echo(buffer, sizeof(buffer))) > 0) {
	ptr = buffer;
	while (size > 7) {
	    switch (*ptr) {
	    case 0x00:
		memcpy(&echo, ptr + 1, sizeof(echo));
		processPosChange(&echo);
		break;
	    case 0x01:
		memcpy(&echo, ptr + 1, sizeof(echo));
		/* processFreqChange(&echo); */
		break;
	    }
	    ptr += 8;
	    size -= 8;
	}
    }


}


static void DoEcho()
{
    struct actualPosition pos;
    unsigned char args[1 + sizeof(pos)];
    pos.order = mp_sngpos;
    pos.row = mp_patpos;

    args[0] = 0x00;
    memcpy(args + 1, (void *) &pos, sizeof(pos));
    gus_do_echo(args, sizeof(args));
}



static void UltraUpdate(void)
{
    fd_set read_fds;

    if (ULTRA_BPM != md_bpm) {
	gus_do_tempo(((md_bpm * 50) / 125) * 1);
	ULTRA_BPM = md_bpm;
    }
    UltraPlayer();
    if (oldRow != mp_sngpos || oldOrder != mp_patpos) {
	DoEcho();
	oldRow = mp_sngpos;
	oldOrder = mp_patpos;
    }
    gus_do_flush();
    FD_ZERO(&read_fds);
    FD_SET(ultra_fd, &read_fds);

    select(ultra_fd + 1, NULL, &read_fds, NULL, NULL);

    if (FD_ISSET(ultra_fd, &read_fds))
	ProcessEcho();
}


static void UltraExit(void)
{
    gus_close(ultra_dev);
    ultra_dev = -1;
    ultra_fd = -1;
}


static void UltraPlayStart(void)
{
    int t;



    for (t = 0; t < GUS_CHANNELS; t++) {
	voices[t].flags = 0;
	voices[t].handle = 0;
	voices[t].size = 0;
	voices[t].start = 0;
	voices[t].reppos = 0;
	voices[t].repend = 0;
	voices[t].changes = 0;
	voices[t].kick = 0;
	voices[t].freq = 10000;
	voices[t].vol = 64;
	voices[t].pan = 8192;
    }

    nr_voices = md_numchn;


    if (ResetCard() == FALSE) {
	exit(-1);
    }
    LoadSamples();

    gus_queue_write_set_size(1024);
    gus_queue_read_set_size(128);

    if (gus_timer_start() < 0) {
    }
    gus_timer_tempo(50);
    ULTRA_BPM = 0;

    for (t = 0; t < nr_voices; t++) {
	gus_do_voice_pan(t, 8192);
	gus_do_voice_volume(t, 50 << 7);
    }

    gus_do_flush();
}


static void UltraPlayStop(void)
{
#ifdef DEBUG
    printf("Ultra: PlayStop called\n");
#endif
    gus_queue_flush();

    gus_queue_write_set_size(0);
    gus_queue_read_set_size(0);

    gus_timer_stop();
}


static void UltraVoiceSetVolume(UBYTE voice, UBYTE vol)
{
    if (vol != voices[voice].vol) {
	voices[voice].vol = vol;
	voices[voice].changes |= CH_VOL;
    }
}


static void UltraVoiceSetFrequency(UBYTE voice, ULONG freq)
{
    if (freq != voices[voice].freq) {
	voices[voice].freq = freq;
	voices[voice].changes |= CH_FREQ;
    }
}


static void UltraVoiceSetPanning(UBYTE voice, UBYTE pan)
{
    if (pan != voices[voice].pan) {
	voices[voice].pan = pan;
	voices[voice].changes |= CH_PAN;
    }
}


static void UltraVoicePlay(UBYTE voice, SWORD handle, ULONG start, ULONG size,
			   ULONG reppos, ULONG repend, UWORD flags)
{
    if (start >= size)
	return;
    if (flags & SF_LOOP) {
	if (repend > size)
	    repend = size;
    }
    voices[voice].flags = flags;
    voices[voice].handle = handle;
    voices[voice].start = start;
    voices[voice].size = size;
    voices[voice].reppos = reppos;
    voices[voice].repend = repend;
    voices[voice].kick = 1;
}


static void UltraDummy()
{
    fflush(stdout);
}

static void UltraPatternChange(void)
{
    gus_do_stop();
    gus_queue_abort_to_stop();
    gus_queue_flush();
}

static void UltraMute(void)
{
    gus_do_stop();
    gus_queue_abort_to_stop();
}

static void UltraUnMute(void)
{
}


DRIVER drv_ultra =
{
    NULL,
    "Linux Ultra driver",
    "Linux Ultra Driver v1.1 - by Andy Lo-A-Foe",
    UltraIsThere,
    UltraSampleLoad,
    UltraSampleUnload,
    UltraInit,
    UltraExit,
    UltraPlayStart,
    UltraPlayStop,
    UltraUpdate,
    UltraVoiceSetVolume,
    UltraVoiceSetFrequency,
    UltraVoiceSetPanning,
    UltraVoicePlay,
    UltraPatternChange,
    UltraMute,
    UltraUnMute
};
