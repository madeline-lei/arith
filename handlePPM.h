#ifndef HANDLE_PPM_H
#define HANDLE_PPM_H

#include <stdio.h>
#include "pnm.h"

Pnm_ppm readInPPM(FILE *input);
Pnm_ppm trimImage(Pnm_ppm original);

void printCompressedImage(A2Methods_UArray2 image,
                          const struct A2Methods_T *methods);

void printWordApply(int col, int row, A2Methods_UArray2 array2,
                    A2Methods_Object *elem, void *cl);

A2Methods_UArray2 readInCompressed(FILE *input,
                                   const struct A2Methods_T *methods);
#endif
