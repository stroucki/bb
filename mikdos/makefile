##############################################################################
# Makefile to compile mikmod with djgpp                                      #
# more DJGPP stuff for games & gfx at:                                       #
#      http://www.informatik.tu-muenchen.de/~breitlip/djgpp.html             #
# Peter Breitling (breitlip@informatik.tu-muenchen.de) 11/96                 #
##############################################################################

CC      = gcc
CFLAGS  = -O2 -m486 -ffast-math -funroll-loops -fstrength-reduce -I./include

LOADERS = load_s3m
DRIVERS = drv_gus drv_sb drv_ss drv_pcsp
MMOBJ   = mirq.o mdma.o virtch.o mloader.o mdriver.o mplayer.o \
          munitrk.o mmio.o pcsp.o 

LDOBJ   = $(LOADERS:%=%.o)
DROBJ   = $(DRIVERS:%=%.o)
OBJ     = $(MMOBJ) $(MCOBJ) $(DROBJ) $(LDOBJ)

all:	libmik.a

libmik.a: $(MMOBJ) $(LDOBJ) $(DROBJ)
	$(AR) rcs libmik.a $(MMOBJ) $(LDOBJ) $(DROBJ) 
clean:
	del *.o
