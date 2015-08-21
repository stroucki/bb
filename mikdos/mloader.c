
/*

   Name:
   MLOADER.C

   Description:
   These routines are used to access the available module loaders

   Portability:
   All systems - all compilers

 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mikmod.h"


FILE *modfp;
UNIMOD of;

LOADER *firstloader = NULL;


UWORD finetune[16] =
{
    8363, 8413, 8463, 8529, 8581, 8651, 8723, 8757,
    7895, 7941, 7985, 8046, 8107, 8169, 8232, 8280
};





void ML_InfoLoader(void)
{
    int t;
    LOADER *l;

    /* list all registered devicedrivers: */

    for (t = 1, l = firstloader; l != NULL; l = l->next, t++) {
	printf("%d. %s\n", t, l->version);
    }
}


void ML_RegisterLoader(LOADER * ldr)
{
    LOADER *l;

    if (firstloader == NULL) {
	firstloader = ldr;
	ldr->next = NULL;
    } else {
	ldr->next = firstloader;
	firstloader = ldr;
    }
}



void *MyMalloc(size_t size)
/*
   Same as malloc, but sets error variable ml_errno when it failed
 */
{
    void *d;

    d = malloc(size);
    if (d == NULL) {
	myerr = "Error allocating structure";
    }
    return d;
}



void *MyCalloc(size_t nitems, size_t size)
/*
   Same as calloc, but sets error variable ml_errno when it failed
 */
{
    void *d;

    d = calloc(nitems, size);
    if (d == NULL) {
	myerr = "Error allocating structure";
    }
    return d;
}



BOOL ReadComment(UWORD len)
{
    int t;

    if (len) {
	if (!(of.comment = (char *) MyMalloc(len + 1)))
	    return 0;
	fread(of.comment, len, 1, modfp);
	of.comment[len] = 0;

	/* strip any control-characters in the comment: */

	for (t = 0; t < len; t++) {
	    if (of.comment[t] < 32)
		of.comment[t] = ' ';
	}
    }
    return 1;
}



BOOL AllocPatterns(void)
{
    int s, t, tracks = 0;

    /* Allocate track sequencing array */

    if (!(of.patterns = (UWORD *) MyCalloc((ULONG) of.numpat * of.numchn, sizeof(UWORD))))
	return 0;
    if (!(of.pattrows = (UWORD *) MyCalloc(of.numpat, sizeof(UWORD))))
	return 0;

    for (t = 0; t < of.numpat; t++) {

	of.pattrows[t] = 64;

	for (s = 0; s < of.numchn; s++) {
	    of.patterns[(t * of.numchn) + s] = tracks++;
	}
    }

    return 1;
}


BOOL AllocTracks(void)
{
    if (!(of.tracks = (UBYTE **) MyCalloc(of.numtrk, sizeof(UBYTE *))))
	return 0;
    return 1;
}



BOOL AllocInstruments(void)
{
    UWORD t;

    if (!(of.instruments = (INSTRUMENT *) MyCalloc(of.numins, sizeof(INSTRUMENT))))
	return 0;
    return 1;
}


BOOL AllocSamples(INSTRUMENT * i)
{
    UWORD u, n;

    if (n = i->numsmp) {
	if (!(i->samples = (SAMPLE *) MyCalloc(n, sizeof(SAMPLE))))
	    return 0;

	for (u = 0; u < n; u++) {
	    i->samples[u].panning = 128;
	    i->samples[u].handle = -1;
	}
    }
    return 1;
}


char *DupStr(char *s, UWORD len)
/*
   Creates a CSTR out of a character buffer of 'len' bytes, but strips
   any terminating non-printing characters like 0, spaces etc.
 */
{
    UWORD t;
    char *d = NULL;

    /* Scan for first printing char in buffer [includes high ascii up to 254] */

    while (len) {
	if (!(s[len - 1] >= 0 && s[len - 1] <= 0x20))
	    break;
	len--;
    }

    if (len) {

	/* When the buffer wasn't completely empty, allocate
	   a cstring and copy the buffer into that string, except
	   for any control-chars */

	if ((d = (char *) malloc(len + 1)) != NULL) {
	    for (t = 0; t < len; t++)
		d[t] = (s[t] >= 0 && s[t] < 32) ? ' ' : s[t];
	    d[t] = 0;
	}
    }
    return d;
}



BOOL ML_LoadSamples(void)
{
    UWORD t, u;
    INSTRUMENT *i;
    SAMPLE *s;

    for (t = 0; t < of.numins; t++) {

	i = &of.instruments[t];

	for (u = 0; u < i->numsmp; u++) {

	    s = &i->samples[u];

	    /* sample has to be loaded ? -> increase
	       number of samples and allocate memory and
	       load sample */

	    if (s->length) {

		if (s->seekpos) {
		    _mm_fseek(modfp, s->seekpos, SEEK_SET);
		}
		/* Call the sample load routine of the driver module.
		   It has to return a 'handle' (>=0) that identifies
		   the sample */

		s->handle = MD_SampleLoad(modfp,
					  s->length,
					  s->loopstart,
					  s->loopend,
					  s->flags);

		if (s->handle < 0)
		    return 0;
	    }
	}
    }
    return 1;
}


BOOL ML_LoadHeader(void)
{
    BOOL ok = 0;
    LOADER *l;

    /* Try to find a loader that recognizes the module */

    for (l = firstloader; l != NULL; l = l->next) {
	_mm_rewind(modfp);
	if (l->Test())
	    break;
    }

    if (l == NULL) {
	myerr = "Unknown module format.";
	return 0;
    }
    /* init unitrk routines */

    if (!UniInit())
	return 0;

    /* init module loader */

    if (l->Init()) {
	_mm_rewind(modfp);
	ok = l->Load();
    }
    l->Cleanup();

    /* free unitrk allocations */

    UniCleanup();
    return ok;
}



void ML_XFreeInstrument(INSTRUMENT * i)
{
    UWORD t;

    if (i->samples != NULL) {
	for (t = 0; t < i->numsmp; t++) {
	    if (i->samples[t].handle >= 0) {
		MD_SampleUnLoad(i->samples[t].handle);
	    }
	}
	free(i->samples);
    }
    if (i->insname != NULL)
	free(i->insname);
}


void ML_FreeEx(UNIMOD * mf)
{
    UWORD t;

    if (mf->modtype != NULL)
	free(mf->modtype);
    if (mf->patterns != NULL)
	free(mf->patterns);
    if (mf->pattrows != NULL)
	free(mf->pattrows);

    if (mf->tracks != NULL) {
	for (t = 0; t < mf->numtrk; t++) {
	    if (mf->tracks[t] != NULL)
		free(mf->tracks[t]);
	}
	free(mf->tracks);
    }
    if (mf->instruments != NULL) {
	for (t = 0; t < mf->numins; t++) {
	    ML_XFreeInstrument(&mf->instruments[t]);
	}
	free(mf->instruments);
    }
    if (mf->songname != NULL)
	free(mf->songname);
    if (mf->comment != NULL)
	free(mf->comment);
}



/******************************************

	Next are the user-callable functions

******************************************/


void ML_Free(UNIMOD * mf)
{
    if (mf != NULL) {
	ML_FreeEx(mf);
	free(mf);
    }
}




UNIMOD *ML_LoadFP(FILE * fp)
{
    int t;
    UNIMOD *mf;

    /* init fileptr, clear errorcode, clear static modfile: */

    modfp = fp;
    myerr = NULL;
    memset(&of, 0, sizeof(UNIMOD));

    /* init panning array */

    for (t = 0; t < 32; t++) {
	of.panning[t] = ((t + 1) & 2) ? 255 : 0;
    }

    if (!ML_LoadHeader()) {
	ML_FreeEx(&of);
	return NULL;
    }
    if (!ML_LoadSamples()) {
	ML_FreeEx(&of);
	return NULL;
    }
    if (!(mf = (UNIMOD *) MyCalloc(1, sizeof(UNIMOD)))) {
	ML_FreeEx(&of);
	return NULL;
    }
    /* Copy the static UNIMOD contents
       into the dynamic UNIMOD struct */

    memcpy(mf, &of, sizeof(UNIMOD));

    return mf;
}



UNIMOD *ML_LoadFN(char *filename)
{
    FILE *fp;
    UNIMOD *mf;

    if ((fp = fopen(filename, "rb")) == NULL) {
	myerr = "Error opening file";
	return NULL;
    }
    mf = ML_LoadFP(fp);
    fclose(fp);

    return mf;
}
