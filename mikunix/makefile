############################################################################
#									   #
# Instructions:								   #
# -------------								   #
# 1. Select the appropriate OS-specific options for your system		   #
# 2. Choose from the options section					   #
# 3. Type "make" and all should (hopefully) work...			   #
#									   #
############################################################################

############################################################################
############################################################################
##									  ##
##			OS-SPECIFIC DEFINITIONS				  ##
##									  ##
############################################################################
############################################################################

############################################################################
#
# Linux/FreeBSD/OSS definitions
#
# for output on linux and FreeBSD Open Sound System (OSS) (/dev/dsp) or 
# Ultrasound driver (/dev/gus)
#
############################################################################

OSDEF=-DOSS
CC = gcc

# if you want to use the Ultra driver uncomment the 3 lines below. 
# You need a Gravis Ultrasound and the library from the Linux Ultrasound 
# project to do this, from http://romeo.pf.jcu.cz/~perex/ultra/
# ULTRA=-DULTRA
# ULTRALIB=-lgus
# ULTRADRV=drv_ultr.o

#for an old ultrasound api
#ULTRA=-DULTRA
#ULTRALIB=-lgus
#ULTRADRV=drv_old.o

#OSFLAGS = -O3 $(OSDEF) $(ULTRA)
OSFLAGS = -O3 $(OSDEF) 
#debug options
#OSFLAGS = -g $(OSDEF) $(ULTRA)

#LIBS= -lm $(ULTRALIB)
LIBS= -lm 
#DRIVERS = drv_vox.o $(ULTRADRV)
DRIVERS = drv_vox.o

############################################################################
#
# SUN / Solaris definitions (including Solaris/i386)
#
# for output on the Sun audio device (/dev/audio)
#
############################################################################

# for SunOS 4.1.x, enable -DSUNOS, for Solaris 2.x, enable -DSOLARIS
#OSDEF=-DSUNOS -I/usr/5include
#OSDEF=-DSOLARIS

#CC = gcc

# defines
# For Solaris/i386, remove the -DMM_BIG_ENDIAN
#CDEFS=-DNEED_UNISTD -DMM_BIG_ENDIAN -DSUN $(OSDEF)

# Enable this for speed on v8 sparc (microSPARC and newer)
#OSFLAGS = -g -O2 -mv8 -msupersparc -I./include $(CDEFS)
# Enable this to work on v7 sparc
#OSFLAGS = -g -O2 -I./include $(CDEFS)
# SPARCompiler C 4.0 options
#OSFLAGS = -Iinclude -fast -s $(CDEFS)

#LIBS=-L/usr/5lib -lm
# Disable the above and use this on Solaris
#LIBS=-lm -lposix4

#DRIVERS = drv_sun.o

############################################################################
#
# DEC Alpha / AudioFile definitions
#
# systems using Digital AudioFile library, available from 
# ftp://crl.dec.com/pub/DEC/AF
#
############################################################################

#OSDEF=-DALPHA

# Choose between DEC cc
#CC = cc -g3

# and GNU gcc
#CC = gcc -g


# On my system I have to do
#INCDIR=-I/usr/local/vol/AudioFile/include
#LIBDIR=-L/usr/local/vol/AudioFile/lib

#OSFLAGS = -O2 $(OSDEF) $(INCDIR)
#DRIVERS = drv_af.o
#LIBS=$(LIBDIR) -lm -lAF

############################################################################
#
#   AIX definitions
#
############################################################################
 
#OSDEF=-DAIX
#CC = gcc

#OSFLAGS = -O3 -fsigned-char $(OSDEF) -DMM_BIG_ENDIAN

#LIBS= -lm
#DRIVERS = drv_aix.o 

############################################################################
#
#   HP 9000 series / HP-UX definitions
#
############################################################################

#OSDEF=-DHPUX

# To use gcc instead of HP's cc, uncomment the following two lines.
# HP's cc (NOT THE BUNDLED ONE) produces about 6% faster code here 
#CC = gcc
#OSFLAGS = -O3 -funroll-loops -mpa-risc-1-1 -mschedule=7100 $(OSDEF) -DMM_BIG_ENDIAN

# OR use HP's CC:

#CC = cc
#OSFLAGS = -Ae +O3 $(OSDEF) -DMM_BIG_ENDIAN

# Both need these:
#LIBS = -L/lib/pa1.1 -lm
#DRIVERS = drv_hp.o

############################################################################
#
#   SGI definitions
#
############################################################################

#OSDEF=-DSGI
#CC = gcc

#CFLAGS = -O2 $(OSDEF) $(ZIP) $(USLEEP) $(NCURSES) -DMM_BIG_ENDIAN
#
#LIBS= -laudio -lm 
#DRIVERS = drv_sgi.o

############################################################################
############################################################################
##									  ##
##			OPTIONS DEFINITIONS				  ##
##									  ##
############################################################################
############################################################################

# -s will renice the program to -19, -S will attempt to give it a
# realtime priority (ie. it'll get all the cpu it wants) using
# sched_setscheduler(). Default behaviour remains the same, i.e. to not use
# these
#SNAG=-DCPUTIME_SNAGGER

# Does your system have usleep()? If not, you'll need to uncomment this...
#USLEEP=-DNEEDS_USLEEP

CFLAGS = $(OSFLAGS) $(SNAG) $(USLEEP) 

############################################################################
############################################################################
##									  ##
##  Common / standard definitions - shouldn't need to touch below here!   ##
##									  ##
############################################################################
############################################################################

LOADERS = load_s3m.o 
OBJ = main_ux.o virtch.o mloader.o mdriver.o mplayer.o munitrk.o mmio.o 

all:	../bb_snd_server

../bb_snd_server: $(OBJ) $(LOADERS) $(DRIVERS) 
	$(CC) -Dunix $(CFLAGS) $(OBJ) $(LOADERS) $(DRIVERS) -o $@ $(LIBS)
	strip $@

clean:
	rm -f $(OBJ) $(OBJ2) $(DRIVERS) $(ARC) $(LOADERS) ../bb_snd_server

