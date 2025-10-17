#ifndef CONVERT_COLOR_INCLUDED
#define CONVERT_COLOR_INCLUDED

#include "pnm.h"
#include "uarray2b.h"

#include <stdio.h>

UArray2b_T rgbToYPbPr(A2Methods_UArray2 original, unsigned denominator,
                      const A2Methods_T methods);
struct YPbPr_pixel pixelToYPbPr(Pnm_rgb pixel, unsigned denominator);
void convertYbPbPrApply(int col, int row, UArray2b_T array2b, void *elem,
                        void *cl);

A2Methods_UArray2 YPbPrToRGB(A2Methods_UArray2 original, unsigned denominator,
                             const A2Methods_T methods);
struct Pnm_rgb pixelToRGB(struct YPbPr_pixel *pixel, unsigned denominator);
void convertRgbApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                     void *cl);

struct YPbPr_pixel {
        float Y;
        float Pb;
        float Pr;
};

#endif
