#ifndef MTYPES_H
#define MTYPES_H

/*
   MikMod atomic types:
   ====================
 */


#ifdef __OS2__

typedef signed char SBYTE;	/* has to be 1 byte signed */
typedef unsigned char UBYTE;	/* has to be 1 byte unsigned */
typedef short SWORD;		/* has to be 2 bytes signed */
typedef unsigned short UWORD;	/* has to be 2 bytes unsigned */
typedef long SLONG;		/* has to be 4 bytes signed */
/* ULONG and BOOL are already defined in OS2.H */

#elif defined(__alpha)

typedef char SBYTE;		/* has to be 1 byte signed */
typedef unsigned char UBYTE;	/* has to be 1 byte unsigned */
typedef short SWORD;		/* has to be 2 bytes signed */
typedef unsigned short UWORD;	/* has to be 2 bytes unsigned */
/* long is 8 bytes on dec alpha - RCA */
typedef int SLONG;		/* has to be 4 bytes signed */
typedef unsigned int ULONG;	/* has to be 4 bytes unsigned */
typedef int BOOL;		/* doesn't matter.. 0=FALSE, <>0 true */

#elif __STDC_VERSION__ >= 199901L

#include <stdint.h>

typedef int8_t SBYTE;		/* has to be 1 byte signed */
typedef uint8_t UBYTE;		/* has to be 1 byte unsigned */
typedef int16_t SWORD;		/* has to be 2 bytes signed */
typedef uint16_t UWORD;		/* has to be 2 bytes unsigned */
typedef int32_t SLONG;		/* has to be 4 bytes signed */
typedef uint32_t ULONG;		/* has to be 4 bytes unsigned */
typedef uint8_t BOOL;		/* doesn't matter.. 0=FALSE, <>0 true */

#else

typedef signed char SBYTE;	/* has to be 1 byte signed */
typedef unsigned char UBYTE;	/* has to be 1 byte unsigned */
typedef signed short SWORD;	/* has to be 2 bytes signed */
typedef unsigned short UWORD;	/* has to be 2 bytes unsigned */
typedef signed int SLONG;	/* has to be 4 bytes signed */
typedef unsigned int ULONG;	/* has to be 4 bytes unsigned */
typedef int BOOL;		/* doesn't matter.. 0=FALSE, <>0 true */

#endif


#ifdef __OS2__
#define INCL_DOS
#define INCL_MCIOS2
#define INCL_MMIOOS2
#include <os2.h>
#include <os2me.h>
#include <mmio.h>
#endif


#ifdef __WATCOMC__
#define inportb(x) inp(x)
#define outportb(x,y) outp(x,y)
#define inport(x) inpw(x)
#define outport(x,y) outpw(x,y)
#define disable() _disable()
#define enable() _enable()
#endif


#ifdef __DJGPP__
#include <dpmi.h>
#include <go32.h>
#include <pc.h>
#define inp inportw
#define outport outportw
#define inport inportw
#define interrupt
#endif

#endif
