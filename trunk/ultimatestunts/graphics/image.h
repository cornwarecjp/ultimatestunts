/* Lifted from include/gltk.h */

#ifndef IMAGE_H
#define IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
** RGB Image Structure
*/

typedef struct _RGBImageRec {
    int sizeX, sizeY;
    unsigned char *data;
} RGBImageRec;

extern RGBImageRec *RGBImageLoad(char *);

#ifdef __cplusplus
}
#endif

#endif