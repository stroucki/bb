/*

   Name:
   MPLAYER.C

   Description:
   The actual modplaying routines
   Portability:
   All systems - all compilers

 */
#include <stdlib.h>
#include "mikmod.h"

#include <math.h>

UNIMOD *pf;			/* <- this modfile is being played */
UWORD reppos;			/* patternloop position */
UWORD repcnt;			/* times to loop */
UWORD vbtick;			/* tick counter */
UWORD patbrk;			/* position where to start a new pattern */
UBYTE patdly;			/* patterndelay counter */
UBYTE patdly2;			/* patterndelay counter */
UWORD numrow;			/* number of rows on current pattern */
SWORD posjmp;			/* flag to indicate a position jump is needed...
				   changed since 1.00: now also indicates the
				   direction the position has to jump to:

				   0: Don't do anything
				   1: Jump back 1 position
				   2: Restart on current position
				   3: Jump forward 1 position
				 */
int forbid;			/* forbidflag */
static int isfirst;

/*
   Set forbid to 1 when you want to modify any of the mp_sngpos, mp_patpos etc.
   variables and clear it when you're done. This prevents getting strange
   results due to intermediate interrupts.
 */


AUDTMP mp_audio[32];		/* max 32 channels */
UBYTE mp_bpm;			/* beats-per-minute speed */
UWORD mp_patpos;		/* current row number (0-255) */
SWORD mp_sngpos;		/* current song position */
UWORD mp_sngspd;		/* current songspeed */
UWORD mp_channel;		/* channel it's working on */
BOOL mp_extspd = 1;		/* extended speed flag, default enabled */
BOOL mp_panning = 1;		/* panning flag, default enabled */
BOOL mp_loop = 0;		/* loop module ? */
UBYTE mp_volume = 100;		/* song volume (0-100) (or user volume) */

static SBYTE globalvolume = 64;	/* global volume */
static UBYTE globalslide;

AUDTMP *a;			/* current AUDTMP it's working on */

float old_bpm;
extern float speed_constant;
extern int quiet;

UWORD mytab[12] =
{
    1712 * 16, 1616 * 16, 1524 * 16, 1440 * 16, 1356 * 16, 1280 * 16,
    1208 * 16, 1140 * 16, 1076 * 16, 1016 * 16, 960 * 16, 907 * 16
};

static UBYTE VibratoTable[32] =
{
    0, 24, 49, 74, 97, 120, 141, 161,
    180, 197, 212, 224, 235, 244, 250, 253,
    255, 253, 250, 244, 235, 224, 212, 197,
    180, 161, 141, 120, 97, 74, 49, 24
};


/* linear periods to frequency translation table: */

UWORD lintab[768] =
{
    16726, 16741, 16756, 16771, 16786, 16801, 16816, 16832, 16847, 16862, 16877, 16892, 16908, 16923, 16938, 16953,
    16969, 16984, 16999, 17015, 17030, 17046, 17061, 17076, 17092, 17107, 17123, 17138, 17154, 17169, 17185, 17200,
    17216, 17231, 17247, 17262, 17278, 17293, 17309, 17325, 17340, 17356, 17372, 17387, 17403, 17419, 17435, 17450,
    17466, 17482, 17498, 17513, 17529, 17545, 17561, 17577, 17593, 17608, 17624, 17640, 17656, 17672, 17688, 17704,
    17720, 17736, 17752, 17768, 17784, 17800, 17816, 17832, 17848, 17865, 17881, 17897, 17913, 17929, 17945, 17962,
    17978, 17994, 18010, 18027, 18043, 18059, 18075, 18092, 18108, 18124, 18141, 18157, 18174, 18190, 18206, 18223,
    18239, 18256, 18272, 18289, 18305, 18322, 18338, 18355, 18372, 18388, 18405, 18421, 18438, 18455, 18471, 18488,
    18505, 18521, 18538, 18555, 18572, 18588, 18605, 18622, 18639, 18656, 18672, 18689, 18706, 18723, 18740, 18757,
    18774, 18791, 18808, 18825, 18842, 18859, 18876, 18893, 18910, 18927, 18944, 18961, 18978, 18995, 19013, 19030,
    19047, 19064, 19081, 19099, 19116, 19133, 19150, 19168, 19185, 19202, 19220, 19237, 19254, 19272, 19289, 19306,
    19324, 19341, 19359, 19376, 19394, 19411, 19429, 19446, 19464, 19482, 19499, 19517, 19534, 19552, 19570, 19587,
    19605, 19623, 19640, 19658, 19676, 19694, 19711, 19729, 19747, 19765, 19783, 19801, 19819, 19836, 19854, 19872,
    19890, 19908, 19926, 19944, 19962, 19980, 19998, 20016, 20034, 20052, 20071, 20089, 20107, 20125, 20143, 20161,
    20179, 20198, 20216, 20234, 20252, 20271, 20289, 20307, 20326, 20344, 20362, 20381, 20399, 20418, 20436, 20455,
    20473, 20492, 20510, 20529, 20547, 20566, 20584, 20603, 20621, 20640, 20659, 20677, 20696, 20715, 20733, 20752,
    20771, 20790, 20808, 20827, 20846, 20865, 20884, 20902, 20921, 20940, 20959, 20978, 20997, 21016, 21035, 21054,
    21073, 21092, 21111, 21130, 21149, 21168, 21187, 21206, 21226, 21245, 21264, 21283, 21302, 21322, 21341, 21360,
    21379, 21399, 21418, 21437, 21457, 21476, 21496, 21515, 21534, 21554, 21573, 21593, 21612, 21632, 21651, 21671,
    21690, 21710, 21730, 21749, 21769, 21789, 21808, 21828, 21848, 21867, 21887, 21907, 21927, 21946, 21966, 21986,
    22006, 22026, 22046, 22066, 22086, 22105, 22125, 22145, 22165, 22185, 22205, 22226, 22246, 22266, 22286, 22306,
    22326, 22346, 22366, 22387, 22407, 22427, 22447, 22468, 22488, 22508, 22528, 22549, 22569, 22590, 22610, 22630,
    22651, 22671, 22692, 22712, 22733, 22753, 22774, 22794, 22815, 22836, 22856, 22877, 22897, 22918, 22939, 22960,
    22980, 23001, 23022, 23043, 23063, 23084, 23105, 23126, 23147, 23168, 23189, 23210, 23230, 23251, 23272, 23293,
    23315, 23336, 23357, 23378, 23399, 23420, 23441, 23462, 23483, 23505, 23526, 23547, 23568, 23590, 23611, 23632,
    23654, 23675, 23696, 23718, 23739, 23761, 23782, 23804, 23825, 23847, 23868, 23890, 23911, 23933, 23954, 23976,
    23998, 24019, 24041, 24063, 24084, 24106, 24128, 24150, 24172, 24193, 24215, 24237, 24259, 24281, 24303, 24325,
    24347, 24369, 24391, 24413, 24435, 24457, 24479, 24501, 24523, 24545, 24567, 24590, 24612, 24634, 24656, 24679,
    24701, 24723, 24746, 24768, 24790, 24813, 24835, 24857, 24880, 24902, 24925, 24947, 24970, 24992, 25015, 25038,
    25060, 25083, 25105, 25128, 25151, 25174, 25196, 25219, 25242, 25265, 25287, 25310, 25333, 25356, 25379, 25402,
    25425, 25448, 25471, 25494, 25517, 25540, 25563, 25586, 25609, 25632, 25655, 25678, 25702, 25725, 25748, 25771,
    25795, 25818, 25841, 25864, 25888, 25911, 25935, 25958, 25981, 26005, 26028, 26052, 26075, 26099, 26123, 26146,
    26170, 26193, 26217, 26241, 26264, 26288, 26312, 26336, 26359, 26383, 26407, 26431, 26455, 26479, 26502, 26526,
    26550, 26574, 26598, 26622, 26646, 26670, 26695, 26719, 26743, 26767, 26791, 26815, 26839, 26864, 26888, 26912,
    26937, 26961, 26985, 27010, 27034, 27058, 27083, 27107, 27132, 27156, 27181, 27205, 27230, 27254, 27279, 27304,
    27328, 27353, 27378, 27402, 27427, 27452, 27477, 27502, 27526, 27551, 27576, 27601, 27626, 27651, 27676, 27701,
    27726, 27751, 27776, 27801, 27826, 27851, 27876, 27902, 27927, 27952, 27977, 28003, 28028, 28053, 28078, 28104,
    28129, 28155, 28180, 28205, 28231, 28256, 28282, 28307, 28333, 28359, 28384, 28410, 28435, 28461, 28487, 28513,
    28538, 28564, 28590, 28616, 28642, 28667, 28693, 28719, 28745, 28771, 28797, 28823, 28849, 28875, 28901, 28927,
    28953, 28980, 29006, 29032, 29058, 29084, 29111, 29137, 29163, 29190, 29216, 29242, 29269, 29295, 29322, 29348,
    29375, 29401, 29428, 29454, 29481, 29507, 29534, 29561, 29587, 29614, 29641, 29668, 29694, 29721, 29748, 29775,
    29802, 29829, 29856, 29883, 29910, 29937, 29964, 29991, 30018, 30045, 30072, 30099, 30126, 30154, 30181, 30208,
    30235, 30263, 30290, 30317, 30345, 30372, 30400, 30427, 30454, 30482, 30509, 30537, 30565, 30592, 30620, 30647,
    30675, 30703, 30731, 30758, 30786, 30814, 30842, 30870, 30897, 30925, 30953, 30981, 31009, 31037, 31065, 31093,
    31121, 31149, 31178, 31206, 31234, 31262, 31290, 31319, 31347, 31375, 31403, 31432, 31460, 31489, 31517, 31546,
    31574, 31602, 31631, 31660, 31688, 31717, 31745, 31774, 31803, 31832, 31860, 31889, 31918, 31947, 31975, 32004,
    32033, 32062, 32091, 32120, 32149, 32178, 32207, 32236, 32265, 32295, 32324, 32353, 32382, 32411, 32441, 32470,
    32499, 32529, 32558, 32587, 32617, 32646, 32676, 32705, 32735, 32764, 32794, 32823, 32853, 32883, 32912, 32942,
    32972, 33002, 33031, 33061, 33091, 33121, 33151, 33181, 33211, 33241, 33271, 33301, 33331, 33361, 33391, 33421
};

#define LOGFAC 2*16

UWORD logtab[] =
{
    LOGFAC * 907, LOGFAC * 900, LOGFAC * 894, LOGFAC * 887, LOGFAC * 881, LOGFAC * 875, LOGFAC * 868, LOGFAC * 862,
    LOGFAC * 856, LOGFAC * 850, LOGFAC * 844, LOGFAC * 838, LOGFAC * 832, LOGFAC * 826, LOGFAC * 820, LOGFAC * 814,
    LOGFAC * 808, LOGFAC * 802, LOGFAC * 796, LOGFAC * 791, LOGFAC * 785, LOGFAC * 779, LOGFAC * 774, LOGFAC * 768,
    LOGFAC * 762, LOGFAC * 757, LOGFAC * 752, LOGFAC * 746, LOGFAC * 741, LOGFAC * 736, LOGFAC * 730, LOGFAC * 725,
    LOGFAC * 720, LOGFAC * 715, LOGFAC * 709, LOGFAC * 704, LOGFAC * 699, LOGFAC * 694, LOGFAC * 689, LOGFAC * 684,
    LOGFAC * 678, LOGFAC * 675, LOGFAC * 670, LOGFAC * 665, LOGFAC * 660, LOGFAC * 655, LOGFAC * 651, LOGFAC * 646,
    LOGFAC * 640, LOGFAC * 636, LOGFAC * 632, LOGFAC * 628, LOGFAC * 623, LOGFAC * 619, LOGFAC * 614, LOGFAC * 610,
    LOGFAC * 604, LOGFAC * 601, LOGFAC * 597, LOGFAC * 592, LOGFAC * 588, LOGFAC * 584, LOGFAC * 580, LOGFAC * 575,
    LOGFAC * 570, LOGFAC * 567, LOGFAC * 563, LOGFAC * 559, LOGFAC * 555, LOGFAC * 551, LOGFAC * 547, LOGFAC * 543,
    LOGFAC * 538, LOGFAC * 535, LOGFAC * 532, LOGFAC * 528, LOGFAC * 524, LOGFAC * 520, LOGFAC * 516, LOGFAC * 513,
    LOGFAC * 508, LOGFAC * 505, LOGFAC * 502, LOGFAC * 498, LOGFAC * 494, LOGFAC * 491, LOGFAC * 487, LOGFAC * 484,
    LOGFAC * 480, LOGFAC * 477, LOGFAC * 474, LOGFAC * 470, LOGFAC * 467, LOGFAC * 463, LOGFAC * 460, LOGFAC * 457,
    LOGFAC * 453, LOGFAC * 450, LOGFAC * 447, LOGFAC * 443, LOGFAC * 440, LOGFAC * 437, LOGFAC * 434, LOGFAC * 431
};

SWORD Interpolate(SWORD p, SWORD p1, SWORD p2, SWORD v1, SWORD v2)
{
    SWORD dp, dv, di;

    if (p1 == p2)
	return v1;

    dv = v2 - v1;
    dp = p2 - p1;
    di = p - p1;

    return v1 + ((SLONG) (di * dv) / dp);
}


UWORD getlinearperiod(UBYTE note, UWORD fine)
{
    return ((10L * 12 * 16 * 4) - ((UWORD) note * 16 * 4) - (fine / 2) + 64);
}


UWORD getlogperiod(UBYTE note, UWORD fine)
{
    UBYTE n, o;
    UWORD p1, p2, i;

    n = note % 12;
    o = note / 12;
    i = (n << 3) + (fine >> 4);	/* n*8 + fine/16 */

    p1 = logtab[i];
    p2 = logtab[i + 1];

    return (Interpolate(fine / 16, 0, 15, p1, p2) >> o);
}


UWORD getoldperiod(UBYTE note, UWORD c2spd)
{
    UBYTE n, o;
    ULONG period;

    if (!c2spd)
	return 4242;		/* <- prevent divide overflow.. (42 eheh) */

    n = note % 12;
    o = note / 12;
    period = ((8363L * mytab[n]) >> o) / c2spd;
    return period;
}



UWORD GetPeriod(UBYTE note, UWORD c2spd)
{
    if (pf->flags & UF_XMPERIODS) {
	return (pf->flags & UF_LINEAR) ? getlinearperiod(note, c2spd) : getlogperiod(note, c2spd);
    }
    return (getoldperiod(note, c2spd));
}



void DoEEffects(UBYTE dat)
{
    UBYTE nib;

    nib = dat & 0xf;

    switch (dat >> 4) {

    case 0x0:			/* filter toggle, not supported */
	break;

    case 0x1:			/* fineslide up */
	if (!vbtick)
	    a->tmpperiod -= (nib << 2);
	break;

    case 0x2:			/* fineslide dn */
	if (!vbtick)
	    a->tmpperiod += (nib << 2);
	break;

    case 0x3:			/* glissando ctrl */
	a->glissando = nib;
	break;

    case 0x4:			/* set vibrato waveform */
	a->wavecontrol &= 0xf0;
	a->wavecontrol |= nib;
	break;

    case 0x5:			/* set finetune */
/*                              a->c2spd=finetune[nib]; */
/*                              a->tmpperiod=GetPeriod(a->note,pf->samples[a->sample].transpose,a->c2spd); */
	break;

    case 0x6:			/* set patternloop */

	if (vbtick)
	    break;

	/* hmm.. this one is a real kludge. But now it
	   works. */

	if (nib) {		/* set reppos or repcnt ? */

	    /* set repcnt, so check if repcnt already is set,
	       which means we are already looping */

	    if (repcnt > 0)
		repcnt--;	/* already looping, decrease counter */
	    else
		repcnt = nib;	/* not yet looping, so set repcnt */

	    if (repcnt)		/* jump to reppos if repcnt>0 */
		mp_patpos = reppos;
	} else {
	    reppos = mp_patpos - 1;	/* set reppos */
	}
	break;


    case 0x7:			/* set tremolo waveform */
	a->wavecontrol &= 0x0f;
	a->wavecontrol |= nib << 4;
	break;

    case 0x8:			/* set panning */
	if (mp_panning) {
	    nib <<= 4;
	    a->panning = nib;
	    pf->panning[mp_channel] = nib;
	}
	break;

    case 0x9:			/* retrig note */

	/* only retrigger if
	   data nibble > 0 */

	if (nib > 0) {
	    if (a->retrig == 0) {

		/* when retrig counter reaches 0,
		   reset counter and restart the sample */

		a->kick = 1;
		a->retrig = nib;
	    }
	    a->retrig--;	/* countdown */
	}
	break;

    case 0xa:			/* fine volume slide up */
	if (vbtick)
	    break;

	a->tmpvolume += nib;
	if (a->tmpvolume > 64)
	    a->tmpvolume = 64;
	break;

    case 0xb:			/* fine volume slide dn */
	if (vbtick)
	    break;

	a->tmpvolume -= nib;
	if (a->tmpvolume < 0)
	    a->tmpvolume = 0;
	break;

    case 0xc:			/* cut note */

	/* When vbtick reaches the cut-note value,
	   turn the volume to zero ( Just like
	   on the amiga) */

	if (vbtick >= nib) {
	    a->tmpvolume = 0;	/* just turn the volume down */
	}
	break;

    case 0xd:			/* note delay */

	/* delay the start of the
	   sample until vbtick==nib */

	if (vbtick == nib) {
	    a->kick = 1;
	} else
	    a->kick = 0;
	break;

    case 0xe:			/* pattern delay */
	if (vbtick)
	    break;
	if (!patdly2)
	    patdly = nib + 1;	/* only once (when vbtick=0) */
	break;

    case 0xf:			/* invert loop, not supported */
	break;
    }
}


void DoVibrato(void)
{
    UBYTE q;
    UWORD temp;

    q = (a->vibpos >> 2) & 0x1f;

    switch (a->wavecontrol & 3) {

    case 0:			/* sine */
	temp = VibratoTable[q];
	break;

    case 1:			/* ramp down */
	q <<= 3;
	if (a->vibpos < 0)
	    q = 255 - q;
	temp = q;
	break;

    case 2:			/* square wave */
	temp = 255;
	break;
    }

    temp *= a->vibdepth;
    temp >>= 7;
    temp <<= 2;

    if (a->vibpos >= 0)
	a->period = a->tmpperiod + temp;
    else
	a->period = a->tmpperiod - temp;

    if (vbtick)
	a->vibpos += a->vibspd;	/* do not update when vbtick==0 */
}



void DoTremolo(void)
{
    UBYTE q;
    UWORD temp;

    q = (a->trmpos >> 2) & 0x1f;

    switch ((a->wavecontrol >> 4) & 3) {

    case 0:			/* sine */
	temp = VibratoTable[q];
	break;

    case 1:			/* ramp down */
	q <<= 3;
	if (a->trmpos < 0)
	    q = 255 - q;
	temp = q;
	break;

    case 2:			/* square wave */
	temp = 255;
	break;
    }

    temp *= a->trmdepth;
    temp >>= 6;

    if (a->trmpos >= 0) {
	a->volume = a->tmpvolume + temp;
	if (a->volume > 64)
	    a->volume = 64;
    } else {
	a->volume = a->tmpvolume - temp;
	if (a->volume < 0)
	    a->volume = 0;
    }

    if (vbtick)
	a->trmpos += a->trmspd;	/* do not update when vbtick==0 */
}


void DoVolSlide(UBYTE dat)
{
    if (!vbtick)
	return;			/* do not update when vbtick==0 */

    a->tmpvolume += dat >> 4;	/* volume slide */
    a->tmpvolume -= dat & 0xf;
    if (a->tmpvolume < 0)
	a->tmpvolume = 0;
    if (a->tmpvolume > 64)
	a->tmpvolume = 64;
}



void DoS3MVolSlide(UBYTE inf)
{
    UBYTE lo, hi;

    if (inf) {
	a->s3mvolslide = inf;
    }
    inf = a->s3mvolslide;

    lo = inf & 0xf;
    hi = inf >> 4;

    if (hi == 0) {
	a->tmpvolume -= lo;
    } else if (lo == 0) {
	a->tmpvolume += hi;
    } else if (hi == 0xf) {
	if (!vbtick)
	    a->tmpvolume -= lo;
    } else if (lo == 0xf) {
	if (!vbtick)
	    a->tmpvolume += hi;
    }
    if (a->tmpvolume < 0)
	a->tmpvolume = 0;
    if (a->tmpvolume > 64)
	a->tmpvolume = 64;
}



void DoXMVolSlide(UBYTE inf)
{
    UBYTE lo, hi;

    if (inf) {
	a->s3mvolslide = inf;
    }
    inf = a->s3mvolslide;

    if (!vbtick)
	return;

    lo = inf & 0xf;
    hi = inf >> 4;

    if (hi == 0)
	a->tmpvolume -= lo;
    else
	a->tmpvolume += hi;

    if (a->tmpvolume < 0)
	a->tmpvolume = 0;
    else if (a->tmpvolume > 64)
	a->tmpvolume = 64;
}



void DoXMGlobalSlide(UBYTE inf)
{
    UBYTE lo, hi;

    if (inf) {
	globalslide = inf;
    }
    inf = globalslide;

    if (!vbtick)
	return;

    lo = inf & 0xf;
    hi = inf >> 4;

    if (hi == 0)
	globalvolume -= lo;
    else
	globalvolume += hi;

    if (globalvolume < 0)
	globalvolume = 0;
    else if (globalvolume > 64)
	globalvolume = 64;
}



void DoXMPanSlide(UBYTE inf)
{
    UBYTE lo, hi;
    SWORD pan;


    if (inf != 0)
	a->pansspd = inf;
    else
	inf = a->pansspd;

    if (!vbtick)
	return;

    lo = inf & 0xf;
    hi = inf >> 4;

    /* slide right has absolute priority: */

    if (hi)
	lo = 0;

    pan = a->panning;

    pan -= lo;
    pan += hi;

    if (pan < 0)
	pan = 0;
    if (pan > 255)
	pan = 255;

    a->panning = pan;
}



void DoS3MSlideDn(UBYTE inf)
{
    UBYTE hi, lo;

    if (inf != 0)
	a->slidespeed = inf;
    else
	inf = a->slidespeed;

    hi = inf >> 4;
    lo = inf & 0xf;

    if (hi == 0xf) {
	if (!vbtick)
	    a->tmpperiod += (UWORD) lo << 2;
    } else if (hi == 0xe) {
	if (!vbtick)
	    a->tmpperiod += lo;
    } else {
	if (vbtick)
	    a->tmpperiod += (UWORD) inf << 2;
    }
}



void DoS3MSlideUp(UBYTE inf)
{
    UBYTE hi, lo;

    if (inf != 0)
	a->slidespeed = inf;
    else
	inf = a->slidespeed;

    hi = inf >> 4;
    lo = inf & 0xf;

    if (hi == 0xf) {
	if (!vbtick)
	    a->tmpperiod -= (UWORD) lo << 2;
    } else if (hi == 0xe) {
	if (!vbtick)
	    a->tmpperiod -= lo;
    } else {
	if (vbtick)
	    a->tmpperiod -= (UWORD) inf << 2;
    }
}



void DoS3MTremor(UBYTE inf)
{
    UBYTE on, off;

    if (inf != 0)
	a->s3mtronof = inf;
    else
	inf = a->s3mtronof;

    if (!vbtick)
	return;

    on = (inf >> 4) + 1;
    off = (inf & 0xf) + 1;

    a->s3mtremor %= (on + off);
    a->volume = (a->s3mtremor < on) ? a->tmpvolume : 0;
    a->s3mtremor++;
}



void DoS3MRetrig(UBYTE inf)
{
    UBYTE hi, lo;

    hi = inf >> 4;
    lo = inf & 0xf;

    if (lo) {
	a->s3mrtgslide = hi;
	a->s3mrtgspeed = lo;
    }
    if (hi) {
	a->s3mrtgslide = hi;
    }
    /* only retrigger if
       lo nibble > 0 */

    if (a->s3mrtgspeed > 0) {
	if (a->retrig == 0) {

	    /* when retrig counter reaches 0,
	       reset counter and restart the sample */

	    a->kick = 1;
	    a->retrig = a->s3mrtgspeed;

	    if (vbtick) {	/* don't slide on first retrig */
		switch (a->s3mrtgslide) {

		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		    a->tmpvolume -= (1 << (a->s3mrtgslide - 1));
		    break;

		case 6:
		    a->tmpvolume = (2 * a->tmpvolume) / 3;
		    break;

		case 7:
		    a->tmpvolume = a->tmpvolume >> 1;
		    break;

		case 9:
		case 0xa:
		case 0xb:
		case 0xc:
		case 0xd:
		    a->tmpvolume += (1 << (a->s3mrtgslide - 9));
		    break;

		case 0xe:
		    a->tmpvolume = (3 * a->tmpvolume) / 2;
		    break;

		case 0xf:
		    a->tmpvolume = a->tmpvolume << 1;
		    break;
		}
		if (a->tmpvolume < 0)
		    a->tmpvolume = 0;
		if (a->tmpvolume > 64)
		    a->tmpvolume = 64;
	    }
	}
	a->retrig--;		/* countdown */
    }
}


void DoS3MSpeed(UBYTE speed)
{
    if (vbtick || patdly2)
	return;

    if (speed) {		/* <- v0.44 bugfix */
	mp_sngspd = speed;
	vbtick = 0;
    }
}


void DoS3MTempo(UBYTE tempo)
{
    if (vbtick || patdly2)
	return;
    old_bpm = tempo;
    mp_bpm = (int) rint(old_bpm * speed_constant);
}


void DoToneSlide(void)
{
    int dist, t;

    if (!vbtick) {
	a->tmpperiod = a->period;
	return;
    }
    /* We have to slide a->period towards a->wantedperiod, so
       compute the difference between those two values */

    dist = a->period - a->wantedperiod;

    if (dist == 0 ||		/* if they are equal */
	a->portspeed > abs(dist)) {	/* or if portamentospeed is too big */

	a->period = a->wantedperiod;	/* make tmpperiod equal tperiod */
    } else if (dist > 0) {	/* dist>0 ? */
	a->period -= a->portspeed;	/* then slide up */
    } else
	a->period += a->portspeed;	/* dist<0 -> slide down */

/*      if(a->glissando){

   If glissando is on, find the nearest
   halfnote to a->tmpperiod

   for(t=0;t<60;t++){
   if(a->tmpperiod>=npertab[a->finetune][t]) break;
   }

   a->period=npertab[a->finetune][t];
   }
   else
 */
    a->tmpperiod = a->period;
}


void DoPTEffect0(UBYTE dat)
{
    UBYTE note;

    note = a->note;

    if (dat != 0) {
	switch (vbtick % 3) {
	case 1:
	    note += (dat >> 4);
	    break;
	case 2:
	    note += (dat & 0xf);
	    break;
	}
	a->period = GetPeriod(note + a->transpose, a->c2spd);
	a->ownper = 1;
    }
}


void PlayNote(void)
{
    INSTRUMENT *i;
    SAMPLE *s;
    UWORD period;
    UBYTE inst, c;
    UBYTE note;

    if (a->row == NULL)
	return;

    UniSetRow(a->row);

    while (c = UniGetByte()) {

	switch (c) {

	case UNI_NOTE:
	    note = UniGetByte();

	    if (note == 96) {	/* key off ? */
		a->keyon = 0;
		if (a->i && !(a->i->volflg & EF_ON)) {
		    a->tmpvolume = 0;
		}
	    } else {
		a->note = note;

		period = GetPeriod(note + a->transpose, a->c2spd);

		a->wantedperiod = period;
		a->tmpperiod = period;

		a->kick = 1;
		a->start = 0;

		/* retrig tremolo and vibrato waves ? */

		if (!(a->wavecontrol & 0x80))
		    a->trmpos = 0;
		if (!(a->wavecontrol & 0x08))
		    a->vibpos = 0;
	    }
	    break;

	case UNI_INSTRUMENT:
	    inst = UniGetByte();
	    if (inst >= pf->numins)
		break;		/* <- safety valve */

	    a->sample = inst;

	    i = &pf->instruments[inst];
	    a->i = i;

	    if (i->samplenumber[a->note] >= i->numsmp)
		break;

	    s = &i->samples[i->samplenumber[a->note]];
	    a->s = s;

	    /* channel or instrument determined panning ? */

	    if (s->flags & SF_OWNPAN) {
		a->panning = s->panning;
	    } else {
		a->panning = pf->panning[mp_channel];
	    }

	    a->transpose = s->transpose;
	    a->handle = s->handle;
	    a->tmpvolume = s->volume;
	    a->volume = s->volume;
	    a->c2spd = s->c2spd;
	    a->retrig = 0;
	    a->s3mtremor = 0;

	    period = GetPeriod(a->note + a->transpose, a->c2spd);

	    a->wantedperiod = period;
	    a->tmpperiod = period;
	    break;

	default:
	    UniSkipOpcode(c);
	    break;
	}
    }
}




void PlayEffects(void)
{
    UBYTE c, dat;

    if (a->row == NULL)
	return;

    UniSetRow(a->row);

    a->ownper = 0;
    a->ownvol = 0;

    while (c = UniGetByte()) {

	switch (c) {

	case UNI_NOTE:
	case UNI_INSTRUMENT:
	    UniSkipOpcode(c);
	    break;

	case UNI_PTEFFECT0:
	    DoPTEffect0(UniGetByte());
	    break;

	case UNI_PTEFFECT1:
	    dat = UniGetByte();
	    if (dat != 0)
		a->slidespeed = (UWORD) dat << 2;
	    if (vbtick)
		a->tmpperiod -= a->slidespeed;
	    break;

	case UNI_PTEFFECT2:
	    dat = UniGetByte();
	    if (dat != 0)
		a->slidespeed = (UWORD) dat << 2;
	    if (vbtick)
		a->tmpperiod += a->slidespeed;
	    break;

	case UNI_PTEFFECT3:
	    dat = UniGetByte();
	    a->kick = 0;	/* temp XM fix */
	    if (dat != 0) {
		a->portspeed = dat;
		a->portspeed <<= 2;
	    }
	    DoToneSlide();
	    a->ownper = 1;
	    break;

	case UNI_PTEFFECT4:
	    dat = UniGetByte();
	    if (dat & 0x0f)
		a->vibdepth = dat & 0xf;
	    if (dat & 0xf0)
		a->vibspd = (dat & 0xf0) >> 2;
	    DoVibrato();
	    a->ownper = 1;
	    break;

	case UNI_PTEFFECT5:
	    dat = UniGetByte();
	    a->kick = 0;
	    DoToneSlide();
	    DoVolSlide(dat);
	    a->ownper = 1;
	    break;

	case UNI_PTEFFECT6:
	    dat = UniGetByte();
	    DoVibrato();
	    DoVolSlide(dat);
	    a->ownper = 1;
	    break;

	case UNI_PTEFFECT7:
	    dat = UniGetByte();
	    if (dat & 0x0f)
		a->trmdepth = dat & 0xf;
	    if (dat & 0xf0)
		a->trmspd = (dat & 0xf0) >> 2;
	    DoTremolo();
	    a->ownvol = 1;
	    break;

	case UNI_PTEFFECT8:
	    dat = UniGetByte();
	    if (mp_panning) {
		a->panning = dat;
		pf->panning[mp_channel] = dat;
	    }
	    break;

	case UNI_PTEFFECT9:
	    dat = UniGetByte();
	    if (dat)
		a->soffset = (UWORD) dat << 8;	/* <- 0.43 fix.. */
	    a->start = a->soffset;
	    if (a->start > a->s->length)
		a->start = a->s->length;
	    break;

	case UNI_PTEFFECTA:
	    DoVolSlide(UniGetByte());
	    break;

	case UNI_PTEFFECTB:
	    dat = UniGetByte();
	    if (patdly2)
		break;
	    if (dat < mp_sngpos)
		break;		/* avoid eternal looping */
	    patbrk = 0;
	    mp_sngpos = dat - 1;
	    posjmp = 3;
	    break;

	case UNI_PTEFFECTC:
	    dat = UniGetByte();
	    if (vbtick)
		break;
	    if (dat > 64)
		dat = 64;
	    a->tmpvolume = dat;
	    break;

	case UNI_PTEFFECTD:
	    dat = UniGetByte();
	    if (patdly2)
		break;
	    {
		int hi = (dat & 0xf0) >> 4;
		int lo = (dat & 0xf);
		patbrk = (hi * 10) + lo;
	    }
	    if (patbrk > 64)
		patbrk = 64;	/* <- v0.42 fix */
	    posjmp = 3;
	    break;

	case UNI_PTEFFECTE:
	    DoEEffects(UniGetByte());
	    break;

	case UNI_PTEFFECTF:
	    dat = UniGetByte();

	    if (vbtick || patdly2)
		break;

	    if (mp_extspd && dat >= 0x20) {
		old_bpm = dat;
		mp_bpm = (int) rint(old_bpm * speed_constant);
	    } else {
		if (dat) {	/* <- v0.44 bugfix */
		    mp_sngspd = dat;
		    vbtick = 0;
		}
	    }
	    break;

	case UNI_S3MEFFECTD:
	    DoS3MVolSlide(UniGetByte());
	    break;

	case UNI_S3MEFFECTE:
	    DoS3MSlideDn(UniGetByte());
	    break;

	case UNI_S3MEFFECTF:
	    DoS3MSlideUp(UniGetByte());
	    break;

	case UNI_S3MEFFECTI:
	    DoS3MTremor(UniGetByte());
	    a->ownvol = 1;
	    break;

	case UNI_S3MEFFECTQ:
	    DoS3MRetrig(UniGetByte());
	    break;

	case UNI_S3MEFFECTA:
	    DoS3MSpeed(UniGetByte());
	    break;

	case UNI_S3MEFFECTT:
	    DoS3MTempo(UniGetByte());
	    break;

	case UNI_XMEFFECTA:
	    DoXMVolSlide(UniGetByte());
	    break;

	case UNI_XMEFFECTG:
	    globalvolume = UniGetByte();
	    break;

	case UNI_XMEFFECTH:
	    DoXMGlobalSlide(UniGetByte());
	    break;

	case UNI_XMEFFECTP:
	    DoXMPanSlide(UniGetByte());
	    break;

	default:
	    UniSkipOpcode(c);
	    break;
	}
    }

    if (!a->ownper) {
	a->period = a->tmpperiod;
    }
    if (!a->ownvol) {
	a->volume = a->tmpvolume;
    }
}




SWORD InterpolateEnv(SWORD p, ENVPT * a, ENVPT * b)
{
    return (Interpolate(p, a->pos, b->pos, a->val, b->val));
}


SWORD DoPan(SWORD envpan, SWORD pan)
{
    return (pan + (((envpan - 128) * (128 - abs(pan - 128))) / 128));
}

SWORD DoVol(ULONG a, SWORD b, SWORD c)
{
    a *= b;
    a *= c;

    return (a >> 23);
}


void StartEnvelope(ENVPR * t, UBYTE flg, UBYTE pts, UBYTE sus, UBYTE beg, UBYTE end, ENVPT * p)
{
    t->flg = flg;
    t->pts = pts;
    t->sus = sus;
    t->beg = beg;
    t->end = end;
    t->env = p;
    t->p = 0;
    t->a = 0;
    t->b = 1;
}



SWORD ProcessEnvelope(ENVPR * t, SWORD v, UBYTE keyon)
{
    if (t->flg & EF_ON) {

	/* panning active? -> copy variables */

	UBYTE a, b;
	UWORD p;

	a = t->a;
	b = t->b;
	p = t->p;

	/* compute the envelope value between points a and b */

	v = InterpolateEnv(p, &t->env[a], &t->env[b]);

	/* Should we sustain? (sustain flag on, key-on, point a is the sustain
	   point, and the pointer is exactly on point a) */

	if ((t->flg & EF_SUSTAIN) && keyon && a == t->sus && p == t->env[a].pos) {
	    /* do nothing */
	} else {
	    /* don't sustain, so increase pointer. */

	    p++;

	    /* pointer reached point b? */

	    if (p >= t->env[b].pos) {

		/* shift points a and b */

		a = b;
		b++;

		if (t->flg & EF_LOOP) {
		    if (b > t->end) {
			a = t->beg;
			b = a + 1;
			p = t->env[a].pos;
		    }
		} else {
		    if (b >= t->pts) {
			b--;
			p--;
		    }
		}
	    }
	}
	t->a = a;
	t->b = b;
	t->p = p;
    }
    return v;
}

/*
   long GetFreq2(long period)
   {
   float frequency;

   frequency=8363.0*pow(2,((6*12*16*4.0)-period)/(12*16*4.0));
   return(floor(frequency));
   }
 */

long GetFreq2(long period)
{
    int okt;
    long frequency;
    period = 7680 - period;
    okt = period / 768;
    frequency = lintab[period % 768];
    frequency <<= 2;
    return (frequency >> (7 - okt));
}

void MP_HandleTick(void)
{
    ULONG tmpvol;
    int z, t, tr, ui_result;
    BOOL reinit_audio = 0;

    pause_flag = -128;
    if (isfirst) {
	/* don't handle the very first ticks, this allows the
	   other hardware to settle down so we don't lose any 
	   starting notes
	 */
	isfirst--;
	return;
    }
    if (forbid)
	return;			/* don't go any further when forbid is true */

    if (MP_Ready())
	return;

    if (++vbtick >= mp_sngspd) {

	mp_patpos++;
	vbtick = 0;

	if (patdly) {
	    patdly2 = patdly;
	    patdly = 0;
	}
	if (patdly2) {

	    /* patterndelay active */

	    if (--patdly2) {
		mp_patpos--;	/* so turn back mp_patpos by 1 */
	    }
	}
	/* Do we have to get a new patternpointer ?
	   (when mp_patpos reaches 64 or when
	   a patternbreak is active). Also check for 256 - if mod 
	   is broken it will continue forever otherwise */

	if (mp_patpos == numrow || mp_patpos > 255)
	    posjmp = 3;


	if (posjmp) {
	    mp_patpos = patbrk;
	    mp_sngpos += (posjmp - 2);
	    patbrk = posjmp = 0;
	    if (mp_sngpos >= pf->numpos) {
/*                              if(1) return; */
		if (!mp_loop)
		    return;
		mp_sngpos = pf->reppos;
	    }
	    if (mp_sngpos < 0)
		mp_sngpos = pf->numpos - 1;
	}
	if (!patdly2) {

	    for (t = 0; t < pf->numchn; t++) {

		tr = pf->patterns[(pf->positions[mp_sngpos] * pf->numchn) + t];
		numrow = pf->pattrows[pf->positions[mp_sngpos]];

		mp_channel = t;
		a = &mp_audio[t];
		a->row = (tr < pf->numtrk) ? UniFindRow(pf->tracks[tr], mp_patpos) : NULL;

		PlayNote();
	    }
	}
    }
    /* Update effects */

    for (t = 0; t < pf->numchn; t++) {
	mp_channel = t;
	a = &mp_audio[t];
	PlayEffects();
    }

    for (t = 0; t < pf->numchn; t++) {
	INSTRUMENT *i;
	SAMPLE *s;
	SWORD envpan, envvol;

	a = &mp_audio[t];
	i = a->i;
	s = a->s;

	if (i == NULL || s == NULL)
	    continue;

	if (a->period < 40)
	    a->period = 40;
	if (a->period > 8000)
	    a->period = 8000;

	if (a->kick) {
	    MD_VoicePlay(t, a->handle, a->start, s->length, s->loopstart, s->loopend, s->flags);
	    a->kick = 0;
	    a->keyon = 1;

	    a->fadevol = 32768;

	    StartEnvelope(&a->venv, i->volflg, i->volpts, i->volsus, i->volbeg, i->volend, i->volenv);
	    StartEnvelope(&a->penv, i->panflg, i->panpts, i->pansus, i->panbeg, i->panend, i->panenv);
	}
	envvol = ProcessEnvelope(&a->venv, 256, a->keyon);
	envpan = ProcessEnvelope(&a->penv, 128, a->keyon);

	tmpvol = a->fadevol;	/* max 32768 */
	tmpvol *= envvol;	/* * max 256 */
	tmpvol *= a->volume;	/* * max 64 */
	tmpvol /= 16384;	/* tmpvol/(256*64) => tmpvol is max 32768 */

	tmpvol *= globalvolume;	/* * max 64 */
	tmpvol *= mp_volume;	/* * max 100 */
	tmpvol /= 3276800UL;	/* tmpvol/(64*100*512) => tmpvol is max 64 */

	MD_VoiceSetVolume(t, tmpvol);

	if (s->flags & SF_OWNPAN) {
	    MD_VoiceSetPanning(t, DoPan(envpan, a->panning));
	} else {
	    MD_VoiceSetPanning(t, a->panning);
	}

	if (pf->flags & UF_LINEAR)
	    MD_VoiceSetFrequency(t, GetFreq2(a->period));
	else
	    MD_VoiceSetFrequency(t, (3579546UL << 2) / a->period);

	/*  if key-off, start substracting
	   fadeoutspeed from fadevol: */

	if (!a->keyon) {
	    if (a->fadevol >= i->volfade)
		a->fadevol -= i->volfade;
	    else
		a->fadevol = 0;
	}
    }
}



void MP_Init(UNIMOD * m)
{
    int t;

    pf = m;
    reppos = 0;
    repcnt = 0;
    mp_sngpos = 0;
    mp_sngspd = m->initspeed;

    vbtick = mp_sngspd;
    patdly = 0;
    patdly2 = 0;
    mp_bpm = m->inittempo;
    old_bpm = mp_bpm;

    forbid = 0;
    mp_patpos = 0;
    posjmp = 2;			/* <- make sure the player fetches the first note */
    patbrk = 0;

    isfirst = 2;		/* delay start by 2 ticks */

    globalvolume = 64;		/* reset global volume */

    /* Make sure the player doesn't start with garbage: */

    for (t = 0; t < pf->numchn; t++) {
	mp_audio[t].kick = 0;
	mp_audio[t].tmpvolume = 0;
	mp_audio[t].retrig = 0;
	mp_audio[t].wavecontrol = 0;
	mp_audio[t].glissando = 0;
	mp_audio[t].soffset = 0;
    }
}



int MP_Ready(void)
{
    return (mp_sngpos >= pf->numpos);
}


void MP_NextPosition(void)
{
    forbid = 1;
    posjmp = 3;
    patbrk = 0;
    vbtick = mp_sngspd;
    forbid = 0;
}


void MP_PrevPosition(void)
{
    forbid = 1;
    posjmp = 1;
    patbrk = 0;
    vbtick = mp_sngspd;
    forbid = 0;
}

void MP_RestartPosition(void)
{
    forbid = 1;
    posjmp = 2;
    patbrk = 0;
    vbtick = mp_sngspd;
    forbid = 0;
}


void MP_SetPosition(UWORD pos)
{				/* avoid infinitely-looping mods */

/*      if(pos>=pf->numpos) pos=pf->numpos;
   forbid=1;
   posjmp=2;
   patbrk=0;
   mp_sngpos=pos; 
   vbtick=mp_sngspd;
   forbid=0; */
}
