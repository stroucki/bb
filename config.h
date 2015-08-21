#ifndef CONFIG_H
#define CONFIG_H
			     /*#define I_WAS_HERE *//*uncoment this to disable changing of parameters by atoconf */

#define DEFAULT_MAX_ITER 170
				      /*default number of iterations should be set
				         lower for slow computers */
#define FPOINT_TYPE  double
				       /*floating point math type on computers
				          with medium speed floating point math should   
				          use float instead */
#define USE_LONG		/*for autoconf..undefine if
				   fpoint_type is set to other than
				   long_double */
/*#define MITSHM */
				       /*undefine this if you system does not
				          support shared memory */
#define MAXSTEP		0.008
				       /*zooming step. For slow computer should
				          be set lower. Longer steps takes more time
				          and invoke yet longer ones etc.. */
#define XSIZE		320
				       /*default sizes of window */
#define YSIZE		200
#define ROTATIONSPEED   -7

#ifdef __DJGPP__
#include "aconfig.dos"
#else
#include "aconfig.h"
#endif
#if !defined(HAVE_LONG_DOUBLE)&&!defined(I_WAS_HERE)
#undef FPOINT_TYPE
#define FPOINT_TYPE double
#endif
#define CONST const
#define INLINE inline
#ifdef __GNUC__
#define myfabs(x) fabs(x)
#ifndef __STRICT_ANSI__
#define CONSTF __attribute__ ((const))
#define REGISTERS(n) __attribute__ ((regparm(n)))
#else
#define CONSTF
#define REGISTERS(n)
#endif
#else
#define myfabs(x) ((x)>0?(x):-(x))
#define CONSTF
#define REGISTERS(n)
#endif
#ifdef HAVE_MOUSEMASK
#define NCURSESMOUSE
#endif
#ifndef HAVE_LIMITS_H
#define INT_MAX 2127423647
#endif


		/*some constatnts used by various parts of XaoS */
#undef DEBUG
#ifndef DEBUG
#define NDEBUG
#endif
#define FRAMERATE	20
#define LOOKSIZE 2
				  /*size explored by autopilot */
#define MAXTIME 50
				  /*maximum zooming time to one direction */
#define NGUESSES1 40
				/*maximum number of guesses using first
				   method */
#define NGUESSES2 1000
				/*maximum number of guesses using second
				   method */
#define STEP		0.0006
				/*speedup */
#define SPEEDUP		1.05
				/*speedup of speedup */
/*Don't change following lines */

#undef SCROLLING		/* color scrolling is broken for now */
#define SIFT		(26)	/*fixed point parameters */
#define MAXCOLORS	256	/*number of colors */
#define MUL		(1<<SIFT)
extern int MAXITER;

typedef FPOINT_TYPE number_t;

#ifdef STATISTICS
#define STAT(x) x
#else
#define STAT(x)
#endif
#endif				/*CONFIG_H */
