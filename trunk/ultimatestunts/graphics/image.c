/* Lifted from libtk/image.c */

/* This code lets your read in SGI .rgb files. */

/* Modifications were made by CJP to add RGBA support */
/* Modifications were made by CJP to add SDL_image support */

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include "image.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SDL_IMAGE
#include "SDL.h"
#include "SDL_image.h"
#endif

#ifndef SEEK_SET
#  define SEEK_SET 0
#endif


/******************************************************************************/

typedef unsigned int GLuint;  /* Should be 32-bit value. */
typedef int GLint;  /* Should be 32-bit value. */

typedef struct _rawImageRec {
    unsigned short imagic;
    unsigned short type;
    unsigned short dim;
    unsigned short sizeX, sizeY, sizeZ;
    unsigned long min, max;
    unsigned long wasteBytes;
    char name[80];
    unsigned long colorMap;
    FILE *file;
    unsigned char *tmp, *tmpR, *tmpG, *tmpB, *tmpA;
    unsigned long rleEnd;
    GLuint *rowStart;
    GLint *rowSize;
} rawImageRec;

/******************************************************************************/

static void ConvertShort(unsigned short *array, unsigned int length)
{
    unsigned short b1, b2;
    unsigned char *ptr;

    ptr = (unsigned char *)array;
    while (length--) {
	b1 = *ptr++;
	b2 = *ptr++;
	*array++ = (b1 << 8) | (b2);
    }
}

static void ConvertUint(GLuint *array, unsigned int length)
{
    unsigned int b1, b2, b3, b4;
    unsigned char *ptr;

    ptr = (unsigned char *)array;
    while (length--) {
	b1 = *ptr++;
	b2 = *ptr++;
	b3 = *ptr++;
	b4 = *ptr++;
	*array++ = (b1 << 24) | (b2 << 16) | (b3 << 8) | (b4);
    }
}

static rawImageRec *RawImageOpen(const char *fileName)
{
    union {
	int testWord;
	char testByte[4];
    } endianTest;
    rawImageRec *raw;
    int swapFlag;
    int x;

    endianTest.testWord = 1;
    if (endianTest.testByte[0] == 1) {
	swapFlag = 1;
    } else {
	swapFlag = 0;
    }

    raw = (rawImageRec *)malloc(sizeof(rawImageRec));
    if (raw == NULL) {
	fprintf(stderr, "Out of memory!\n");
	exit(1);
    }
    if ((raw->file = fopen(fileName, "rb")) == NULL) {
	perror(fileName);
	exit(1);
    }

    fread(raw, 1, 12, raw->file);

    if (swapFlag) {
	ConvertShort(&raw->imagic, 6);
    }

    raw->tmp = (unsigned char *)malloc(raw->sizeX*256);
    raw->tmpR = (unsigned char *)malloc(raw->sizeX*256);
    raw->tmpG = (unsigned char *)malloc(raw->sizeX*256);
    raw->tmpB = (unsigned char *)malloc(raw->sizeX*256);
    raw->tmpA = (unsigned char *)malloc(raw->sizeX*256);
    if (raw->tmp == NULL || raw->tmpR == NULL || raw->tmpG == NULL ||
	raw->tmpB == NULL || raw->tmpA == NULL) {
	fprintf(stderr, "Out of memory!\n");
	exit(1);
    }

    if ((raw->type & 0xFF00) == 0x0100) {
	x = raw->sizeY * raw->sizeZ * (int) sizeof(GLuint);
	raw->rowStart = (GLuint *)malloc(x);
	raw->rowSize = (GLint *)malloc(x);
	if (raw->rowStart == NULL || raw->rowSize == NULL) {
	    fprintf(stderr, "Out of memory!\n");
	    exit(1);
	}
	raw->rleEnd = 512 + (2 * x);
	fseek(raw->file, 512, SEEK_SET);
	fread(raw->rowStart, 1, x, raw->file);
	fread(raw->rowSize, 1, x, raw->file);
	if (swapFlag) {
	    ConvertUint(raw->rowStart,
	        (unsigned int) (x/sizeof(GLuint)));
	    ConvertUint((GLuint *)raw->rowSize,
	        (unsigned int) (x/sizeof(GLint)));
	}
    }

    return raw;
}

static void RawImageClose(rawImageRec *raw)
{

    fclose(raw->file);
    free(raw->tmp);
    free(raw->tmpR);
    free(raw->tmpG);
    free(raw->tmpB);
    free(raw->tmpA);
    free(raw->rowStart); //Added by CJP
    free(raw->rowSize); //Added by CJP
    free(raw);
}

static void RawImageGetRow(rawImageRec *raw, unsigned char *buf, int y, int z)
{
    unsigned char *iPtr, *oPtr, pixel;
    int count;

    if ((raw->type & 0xFF00) == 0x0100) {
	fseek(raw->file, (long) raw->rowStart[y+z*raw->sizeY], SEEK_SET);
	fread(raw->tmp, 1, (unsigned int)raw->rowSize[y+z*raw->sizeY],
	      raw->file);

	iPtr = raw->tmp;
	oPtr = buf;
	for (;;) {
	    pixel = *iPtr++;
	    count = (int)(pixel & 0x7F);
	    if (!count) {
		return;
	    }
	    if (pixel & 0x80) {
		while (count--) {
		    *oPtr++ = *iPtr++;
		}
	    } else {
		pixel = *iPtr++;
		while (count--) {
		    *oPtr++ = pixel;
		}
	    }
	}
    } else {
	fseek(raw->file, 512+(y*raw->sizeX)+(z*raw->sizeX*raw->sizeY),
	      SEEK_SET);
	fread(buf, 1, raw->sizeX, raw->file);
    }
}

static void RawImageGetData(rawImageRec *raw, RGBImageRec *final)
{
    unsigned char *ptr;
    int i, j;

    final->data = (unsigned char *)malloc((raw->sizeX+1)*(raw->sizeY+1)*4); /* 4??? */
    if (final->data == NULL) {
	fprintf(stderr, "Out of memory!\n");
	exit(1);
    }

    ptr = final->data;
    for (i = 0; i < (int)(raw->sizeY); i++) {
	RawImageGetRow(raw, raw->tmpR, i, 0);
	RawImageGetRow(raw, raw->tmpG, i, 1);
	RawImageGetRow(raw, raw->tmpB, i, 2);
	for (j = 0; j < (int)(raw->sizeX); j++) {
	    *ptr++ = *(raw->tmpR + j);
	    *ptr++ = *(raw->tmpG + j);
	    *ptr++ = *(raw->tmpB + j);
	}
    }
}

/* RGBA support, by CJP */
static void RawImageGetData_RGBA(rawImageRec *raw, RGBImageRec *final)
{
    unsigned char *ptr;
    int i, j;

    final->data = (unsigned char *)malloc((raw->sizeX+1)*(raw->sizeY+1)*4);
    if (final->data == NULL) {
	fprintf(stderr, "Out of memory!\n");
	exit(1);
    }

    ptr = final->data;
    for (i = 0; i < (int)(raw->sizeY); i++) {
	RawImageGetRow(raw, raw->tmpR, i, 0);
	RawImageGetRow(raw, raw->tmpG, i, 1);
	RawImageGetRow(raw, raw->tmpB, i, 2);
	RawImageGetRow(raw, raw->tmpA, i, 3);
	for (j = 0; j < (int)(raw->sizeX); j++) {
	    *ptr++ = *(raw->tmpR + j);
	    *ptr++ = *(raw->tmpG + j);
	    *ptr++ = *(raw->tmpB + j);
	    *ptr++ = *(raw->tmpA + j);
	}
    }
}

RGBImageRec *RGBImageLoad(const char *fileName)
{
	rawImageRec *raw;
	RGBImageRec *final;

	raw = RawImageOpen(fileName);
	final = (RGBImageRec *)malloc(sizeof(RGBImageRec));
	if (final == NULL) {
		fprintf(stderr, "Out of memory!\n");
		exit(1);
	}
	final->sizeX = raw->sizeX;
	final->sizeY = raw->sizeY;
	if(raw->sizeZ == 4) //with alpha layer
	{
		RawImageGetData_RGBA(raw, final);
		final->format = 1;
	}
	else
	{
		RawImageGetData(raw, final);
		final->format = 0;
	}
	RawImageClose(raw);

	return final;
}

#ifdef HAVE_SDL_IMAGE
Uint32 getSDLPixel(Uint8 *p, int Bpp)
{
	switch(Bpp)
	{
	case 1:
		return *p;
	case 2:
		return *(Uint16 *)p;
	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			{return p[0] << 16 | p[1] << 8 | p[2];}
		else
			{return p[0] | p[1] << 8 | p[2] << 16;}
	case 4:
		return *(Uint32 *)p;
	default:
		return 0;       /* shouldn't happen, but avoids warnings */
	}

	return 0;       /* shouldn't happen, but avoids warnings */
}

RGBImageRec *SDL2RGB(SDL_Surface *srcimage)
{
	Uint8 red, green, blue, alpha;
	Uint8 *SrcPtr, *DstPtr;
	Uint32 pixel;
	unsigned int x, y;
	Uint8 DstBytesPerPixel;
	int mustLock;
	RGBImageRec *ret = (RGBImageRec *)malloc(sizeof(RGBImageRec));
	SDL_Surface *image = srcimage;

	//Get image information
	ret->sizeX = image->w;
	ret->sizeY = image->h;
	ret->format = (image->format->Amask != 0); //Does it have alpha values?

	//Convert color keys to an alpha channel
	if(!(ret->format) && (image->flags & SDL_SRCCOLORKEY))
	{
		//printf("Color keying is used!\n");
		image = SDL_DisplayFormatAlpha(srcimage);

		if(image == NULL)
		{
			printf("Error: failed to convert color keying to alpha channel\n");
			image = srcimage;
		}
		else
		{
			SDL_FreeSurface(srcimage);
			ret->format = (image->format->Amask != 0); //Ask again: does it have alpha values?
		}
	}

	DstBytesPerPixel = 3;
	if(ret->format) DstBytesPerPixel = 4;
	ret->data = (unsigned char *)malloc(ret->sizeX*ret->sizeY*DstBytesPerPixel*sizeof(unsigned char));

	//Transfer pixel data
	mustLock = SDL_MUSTLOCK(image);
	if(mustLock) SDL_LockSurface(image);

	for(y=0; y < ret->sizeY; y++)
	{
		SrcPtr = (Uint8 *)(image->pixels) + y            * ret->sizeX * image->format->BytesPerPixel;
		DstPtr = (Uint8 *)(ret->data) + (ret->sizeY-y-1) * ret->sizeX * DstBytesPerPixel;

		for(x=0; x < ret->sizeX; x++)
		{
			pixel = getSDLPixel(SrcPtr, image->format->BytesPerPixel);
			SDL_GetRGBA(pixel, image->format, &red, &green, &blue, &alpha);

			DstPtr[0] = red;
			DstPtr[1] = green;
			DstPtr[2] = blue;
			if(ret->format) DstPtr[3] = alpha;

			SrcPtr += image->format->BytesPerPixel;
			DstPtr += DstBytesPerPixel;
		}
	}

	if(mustLock) SDL_UnlockSurface(image);

	SDL_FreeSurface(image);

	return ret;
}
#endif

RGBImageRec *GenericImageLoad(const char *fileName)
{
#ifdef HAVE_SDL_IMAGE
	SDL_Surface *image;
	image=IMG_Load(fileName);
	if(!image)
	{
		//If SDL_image can't load it, try our own RGB(A) loader
		return RGBImageLoad(fileName);
	}
	//Convert from SDL_Surface to own structure
	return SDL2RGB(image);

#else
	return RGBImageLoad(fileName);
#endif
}
