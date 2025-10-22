/**************************************************************
 *                     convertColor.h
 *
 *     Assignment: arith
 *     Authors:  Diana Calderon and Madeline Lei
 *     Usernames: dcalde02, mlei03
 *     Date:     10/21/2025
 *
 *     summary:
 *
 *     This file contains the function declarations for convertColor.c.
 *     These functions will deal with transforming pixels from the
 *     RGB color space to the component video color space (and back).
 *
 **************************************************************/

#ifndef CONVERT_COLOR_H
#define CONVERT_COLOR_H

#include "pnm.h"
#include "helpers.h"

A2Methods_UArray2 rgbToYPbPr(A2Methods_UArray2 original, unsigned denominator,
                             const struct A2Methods_T *methods);
struct YPbPr_pixel pixelToYPbPr(Pnm_rgb pixel, unsigned denominator);
void convertYbPbPrApply(int col, int row, A2Methods_UArray2 array2,
                        A2Methods_Object *elem, void *cl);

A2Methods_UArray2 YPbPrToRGB(A2Methods_UArray2 original, unsigned denominator,
                             const struct A2Methods_T *methods);
struct Pnm_rgb pixelToRGB(struct YPbPr_pixel *pixel, unsigned denominator);
void convertRgbApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                     void *cl);

#endif
