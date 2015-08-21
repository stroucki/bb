/*

   Name:
   DRV_PCSP.C

   Description:
   Mikmod driver for output on ps speaker
   Copyright (C) Jan Hubicka 1997

   Portability:

   MSDOS:       BC(n)   Watcom(?)       DJGPP(y)
   Win95:       n
   Os2: n
   Linux:       n

   (y) - yes
   (n) - no (not possible or not useful)
   (?) - may be possible, but not tested

 */
#include <stdio.h>
#include <sys/nearptr.h>
#include <stdlib.h>
#include <dos.h>
#include <malloc.h>
#include <conio.h>
#include <time.h>
#include <dpmi.h>
#include <go32.h>
#ifndef __DJGPP__
#include <mem.h>
#endif

#include "mikmod.h"
#include "mdma.h"
#include "mirq.h"

/***************************************************************************
>>>>>>>>>>>>>>>>>>>>>>>>> The actual PCSP driver <<<<<<<<<<<<<<<<<<<<<<<<<<<<<
***************************************************************************/


unsigned char sp_tab[] =
{
    64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 62, 62,
    62, 62, 62, 62, 62, 62, 62, 62,
    61, 61, 61, 61, 61, 61, 61, 61,
    61, 60, 60, 60, 60, 60, 60, 60,
    60, 60, 60, 59, 59, 59, 59, 59,
    59, 59, 59, 59, 59, 58, 58, 58,
    58, 58, 58, 58, 58, 58, 58, 57,
    57, 57, 57, 57, 57, 57, 57, 57,
    57, 56, 56, 56, 56, 56, 56, 56,
    56, 55, 55, 55, 55, 55, 54, 54,
    54, 54, 53, 53, 53, 53, 52, 52,
    52, 51, 51, 50, 50, 49, 49, 48,
    48, 47, 46, 45, 44, 43, 42, 41,
    40, 39, 38, 37, 36, 35, 34, 33,
    32, 31, 30, 29, 28, 27, 26, 25,
    24, 23, 22, 21, 20, 19, 18, 17,
    17, 16, 16, 15, 15, 14, 14, 13,
    13, 13, 12, 12, 12, 12, 11, 11,
    11, 11, 10, 10, 10, 10, 10, 9,
    9, 9, 9, 9, 9, 9, 9, 9,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 7, 7, 7, 7,
    7, 7, 7, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
};
char *_ptr, *_ptr_start, *_ptr_end;
__dpmi_paddr _oldirq_handler;
char __pcspe;			/*stuff exported to pcsp.o */
static char *_data;
int _counter = -1;
uclock_t udist, ustart;
int _delay, _bios = 0x10000;
static int irq_virgin = 1;
extern void _irq_wrapper(), _irq_wrapper_end();

#define DISABLE() asm volatile ("cli")
#define ENABLE() asm volatile ("sti")
#define LOCK_VARIABLE(x)      _go32_dpmi_lock_data((void *)&x, sizeof(x))
#define LOCK_FUNCTION(x)      _go32_dpmi_lock_code(x, (long)x##_end - (long)x)
#define TIMERS_PER_SECOND     1193181L
#define BPS_TO_TIMER(x)       (TIMERS_PER_SECOND / (long)(x))

/* set_timer:
 *  Sets the delay time for PIT channel 1 in one-shot mode.
 */
static inline void set_timer(long time)
{
    outportb(0x43, 0x30);
    outportb(0x40, time & 0xff);
    outportb(0x40, time >> 8);
}



/* set_timer_rate:
 *  Sets the delay time for PIT channel 1 in cycle mode.
 */
static inline void set_timer_rate(long time)
{
    outportb(0x43, 0x34);
    outportb(0x40, time & 0xff);
    outportb(0x40, time >> 8);
}



/* read_timer:
 *  Reads the elapsed time from PIT channel 1.
 */
static inline long read_timer()
{
    long x;

    outportb(0x43, 0x00);
    x = inportb(0x40);
    x += inportb(0x40) << 8;

    return 0xFFFF - x;
}


uclock_t myuclock()
{
    if (_counter==-1) {
	return (uclock() + udist);
    } else {
	return (ustart + (((uclock_t)_counter)*(uclock_t)md_dmabufsize+(uclock_t)(_ptr-_ptr_start)) * (-_delay));
    }
}

/* _install_irq:
 *  Installs a hardware interrupt handler for the specified irq, allocating
 *  an asm wrapper function which will save registers and handle the stack
 *  switching. The C function should return zero to exit the interrupt with 
 *  an iret instruction, and non-zero to chain to the old handler.
 */
int _install_irq()
{
    int c;
    __dpmi_paddr addr;

    if (irq_virgin) {		/* first time we've been called? */
	LOCK_VARIABLE(_oldirq_handler);
	LOCK_VARIABLE(sp_tab);
	LOCK_VARIABLE(_counter);
	LOCK_VARIABLE(_bios);
	LOCK_VARIABLE(__pcspe);
	LOCK_VARIABLE(_ptr);
	LOCK_VARIABLE(_ptr_start);
	LOCK_VARIABLE(_data);
	LOCK_VARIABLE(_ptr_end);
	LOCK_VARIABLE(_delay);
	LOCK_FUNCTION(_irq_wrapper);
	irq_virgin = 0;
    }
    addr.selector = _my_cs();

    addr.offset32 = (long) _irq_wrapper;

    __dpmi_get_protected_mode_interrupt_vector(8,
					       &_oldirq_handler);
    __pcspe = inportb(0x61) | 0x03;
    _bios=0x10000;
    outportb(0x43, 0x92);
    _delay = -_delay;
    __dpmi_set_protected_mode_interrupt_vector(8, &addr);
    for (c = 0; c < 8; c++)
	set_timer_rate(-_delay);

    return 0;
}



/* _remove_irq:
 *  Removes a hardware interrupt handler, restoring the old vector.
 */
void _remove_irq()
{int i;
    for(i=0;i<8;i++)
    set_timer_rate(0x10000);
    __dpmi_set_protected_mode_interrupt_vector(8,
					       &_oldirq_handler);
}

static BOOL PC_IsThere(void)
{
    return 1;
}


static BOOL PC_Init(void)
{
    ULONG t;
    int d;
    md_mode &= ~DMODE_16BITS;
    md_mode &= ~DMODE_STEREO;
    if (md_mixfreq > 18356)
	md_mixfreq = 18356;
    d = BPS_TO_TIMER(md_mixfreq);
    md_mixfreq = TIMERS_PER_SECOND / d;
    /*md_dmabufsize = 2 * md_mixfreq;*/
    _ptr_start = malloc(md_dmabufsize);
    _data = _ptr = _ptr_start = _ptr_start;
    _go32_dpmi_lock_data(_ptr_start, md_dmabufsize);
    _ptr_end = _ptr_start + md_dmabufsize;

    if (!VC_Init())
	return 0;

    return 1;
}



static void PC_Exit(void)
{
    int i;
    free(_ptr_start);
    VC_Exit();
    outportb(0x61,__pcspe);
}
static int MyWrite(unsigned char *start,int n)
{int i;
	n= VC_WriteBytes(start,n);
        for(i=0;i<n;i++)
          start[i]=sp_tab[start[i]];
        return(i);
}

static void PC_Update(void)
{
    UWORD todo, index;
    int last = _data - _ptr_start, curr = _ptr - _ptr_start;
    /*printf("%i %i %i %i\n",last,curr,md_dmabufsize,_ptr_end-buff); */

    if (curr == last)
	return;

    if (curr > last) {
	todo = curr - last;
	index = last;
	_data += MyWrite(&_ptr_start[index], todo);
	if (_data >= _ptr_end)
	    _data = _ptr_start;
    } else {
	todo = md_dmabufsize - last;
	MyWrite(&_ptr_start[last], todo);
	_data = _ptr_start + MyWrite(_ptr_start, curr);
    }
}




static void PC_PlayStart(void)
{
    ustart = myuclock();
    VC_PlayStart();
    _ptr = _data = _ptr_start;
    _delay = BPS_TO_TIMER(md_mixfreq);
    _counter=0;
    memset(_ptr_start, 0, md_dmabufsize);
    _install_irq();
}


static void PC_PlayStop(void)
{
    int i;
    uclock_t u1 = myuclock();
    DISABLE();
    for (i = 0; i < 8; i++)
	set_timer_rate(0x10000);
    _remove_irq();
    ENABLE();
    VC_PlayStop();
    udist = -uclock() + u1;
    _counter = -1;
}


DRIVER drv_pcsp =
{
    NULL,
    "pc buzzer",
    "MikMod pc speaker driver",
    PC_IsThere,
    VC_SampleLoad,
    VC_SampleUnload,
    PC_Init,
    PC_Exit,
    PC_PlayStart,
    PC_PlayStop,
    PC_Update,
    VC_VoiceSetVolume,
    VC_VoiceSetFrequency,
    VC_VoiceSetPanning,
    VC_VoicePlay
};
