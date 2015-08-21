
/*

   Name:
   MMIO.C

   Description:
   Miscellaneous I/O routines.. used to solve some portability issues
   (like big/little endian machines and word alignment in structures )
   Also includes mikmod's ingenious error handling variable + some much
   used error strings.

   Portability:
   All systems - all compilers

 */
#include <stdio.h>
#include "mikmod.h"

char *ERROR_ALLOC_STRUCT = "Error allocating structure";
char *ERROR_LOADING_PATTERN = "Error loading pattern";
char *ERROR_LOADING_TRACK = "Error loading track";
char *ERROR_LOADING_HEADER = "Error loading header";
char *ERROR_NOT_A_MODULE = "Unknown module format";
char *ERROR_LOADING_SAMPLEINFO = "Error loading sampleinfo";
char *ERROR_OUT_OF_HANDLES = "Out of sample-handles";
char *ERROR_SAMPLE_TOO_BIG = "Sample too big, out of memory";

char *myerr;

static long _mm_iobase = 0;

int _mm_fseek(FILE * stream, long offset, int whence)
{
    return fseek(stream,
		 (whence == SEEK_SET) ? offset + _mm_iobase : offset,
		 whence);
}

long _mm_ftell(FILE * stream)
{
    return ftell(stream) - _mm_iobase;
}

void _mm_setiobase(long iobase)
{
    _mm_iobase = iobase;
}

void _mm_write_SBYTE(SBYTE data, FILE * fp)
{
    fputc(data, fp);
}

void _mm_write_UBYTE(UBYTE data, FILE * fp)
{
    fputc(data, fp);
}

void _mm_write_M_UWORD(UWORD data, FILE * fp)
{
    _mm_write_UBYTE(data >> 8, fp);
    _mm_write_UBYTE(data & 0xff, fp);
}

void _mm_write_I_UWORD(UWORD data, FILE * fp)
{
    _mm_write_UBYTE(data & 0xff, fp);
    _mm_write_UBYTE(data >> 8, fp);
}

void _mm_write_M_SWORD(SWORD data, FILE * fp)
{
    _mm_write_M_UWORD((UWORD) data, fp);
}

void _mm_write_I_SWORD(SWORD data, FILE * fp)
{
    _mm_write_I_UWORD((UWORD) data, fp);
}

void _mm_write_M_ULONG(ULONG data, FILE * fp)
{
    _mm_write_M_UWORD(data >> 16, fp);
    _mm_write_M_UWORD(data & 0xffff, fp);
}

void _mm_write_I_ULONG(ULONG data, FILE * fp)
{
    _mm_write_I_UWORD(data & 0xffff, fp);
    _mm_write_I_UWORD(data >> 16, fp);
}

void _mm_write_M_SLONG(SLONG data, FILE * fp)
{
    _mm_write_M_ULONG((ULONG) data, fp);
}

void _mm_write_I_SLONG(SLONG data, FILE * fp)
{
    _mm_write_I_ULONG((ULONG) data, fp);
}


#define DEFINE_MULTIPLE_WRITE_FUNCTION(type_name, type)        \
void                                                           \
_mm_write_##type_name##S (type *buffer, int number, FILE *fp)  \
{                                                              \
	while(number>0){                                           \
		_mm_write_##type_name(*(buffer++),fp);	   			   \
		number--;											   \
	}														   \
}

DEFINE_MULTIPLE_WRITE_FUNCTION(SBYTE, SBYTE)
DEFINE_MULTIPLE_WRITE_FUNCTION(UBYTE, UBYTE)
DEFINE_MULTIPLE_WRITE_FUNCTION(M_SWORD, SWORD)
DEFINE_MULTIPLE_WRITE_FUNCTION(M_UWORD, UWORD)
DEFINE_MULTIPLE_WRITE_FUNCTION(I_SWORD, SWORD)
DEFINE_MULTIPLE_WRITE_FUNCTION(I_UWORD, UWORD)
DEFINE_MULTIPLE_WRITE_FUNCTION(M_SLONG, SLONG)
DEFINE_MULTIPLE_WRITE_FUNCTION(M_ULONG, ULONG)
DEFINE_MULTIPLE_WRITE_FUNCTION(I_SLONG, SLONG)
DEFINE_MULTIPLE_WRITE_FUNCTION(I_ULONG, ULONG)
SBYTE _mm_read_SBYTE(FILE * fp)
{
    return (fgetc(fp));
}

UBYTE _mm_read_UBYTE(FILE * fp)
{
    return (fgetc(fp));
}

UWORD _mm_read_M_UWORD(FILE * fp)
{
    UWORD result = ((UWORD) _mm_read_UBYTE(fp)) << 8;
    result |= _mm_read_UBYTE(fp);
    return result;
}

UWORD _mm_read_I_UWORD(FILE * fp)
{
    UWORD result = _mm_read_UBYTE(fp);
    result |= ((UWORD) _mm_read_UBYTE(fp)) << 8;
    return result;
}

SWORD _mm_read_M_SWORD(FILE * fp)
{
    return ((SWORD) _mm_read_M_UWORD(fp));
}

SWORD _mm_read_I_SWORD(FILE * fp)
{
    return ((SWORD) _mm_read_I_UWORD(fp));
}

ULONG _mm_read_M_ULONG(FILE * fp)
{
    ULONG result = ((ULONG) _mm_read_M_UWORD(fp)) << 16;
    result |= _mm_read_M_UWORD(fp);
    return result;
}

ULONG _mm_read_I_ULONG(FILE * fp)
{
    ULONG result = _mm_read_I_UWORD(fp);
    result |= ((ULONG) _mm_read_I_UWORD(fp)) << 16;
    return result;
}

SLONG _mm_read_M_SLONG(FILE * fp)
{
    return ((SLONG) _mm_read_M_ULONG(fp));
}

SLONG _mm_read_I_SLONG(FILE * fp)
{
    return ((SLONG) _mm_read_I_ULONG(fp));
}


int _mm_read_str(char *buffer, int number, FILE * fp)
{
    fread(buffer, 1, number, fp);
    return !feof(fp);
}


#define DEFINE_MULTIPLE_READ_FUNCTION(type_name, type)         \
int                                                            \
_mm_read_##type_name##S (type *buffer, int number, FILE *fp)   \
{                                                              \
	while(number>0){                                           \
		*(buffer++)=_mm_read_##type_name(fp);				   \
		number--;											   \
	}														   \
	return !feof(fp);										   \
}

DEFINE_MULTIPLE_READ_FUNCTION(SBYTE, SBYTE)
DEFINE_MULTIPLE_READ_FUNCTION(UBYTE, UBYTE)
DEFINE_MULTIPLE_READ_FUNCTION(M_SWORD, SWORD)
DEFINE_MULTIPLE_READ_FUNCTION(M_UWORD, UWORD)
DEFINE_MULTIPLE_READ_FUNCTION(I_SWORD, SWORD)
DEFINE_MULTIPLE_READ_FUNCTION(I_UWORD, UWORD)
DEFINE_MULTIPLE_READ_FUNCTION(M_SLONG, SLONG)
DEFINE_MULTIPLE_READ_FUNCTION(M_ULONG, ULONG)
DEFINE_MULTIPLE_READ_FUNCTION(I_SLONG, SLONG)
DEFINE_MULTIPLE_READ_FUNCTION(I_ULONG, ULONG)
