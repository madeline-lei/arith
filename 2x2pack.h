#ifndef TWOBYTWO_PACK_H
#define TWOBYTWO_PACK_H

#include "convertColor.h"

struct YPbPr_block {
        float a;
        float b;
        float c;
        float d;
        float avgPb;
        float avgPr;
};

A2Methods_UArray2 packBlock(A2Methods_UArray2 original,
                            const struct A2Methods_T *methods);

void packBlockApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                    void *cl);
void averageChroma(struct YPbPr_pixel *p1, struct YPbPr_pixel *p2,
                   struct YPbPr_pixel *p3, struct YPbPr_pixel *p4,
                   struct YPbPr_block *block);
void pixelToDCT(float Y1, float Y2, float Y3, float Y4,
                struct YPbPr_block *block);

A2Methods_UArray2 unpackBlock(A2Methods_UArray2 original,
                              const struct A2Methods_T *methods);

void DCTtoPixel(float a, float b, float c, float d, float *Y1, float *Y2,
                float *Y3, float *Y4);

void unpackBlockApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                      void *cl);

void setChromaApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                    void *cl);

#endif
