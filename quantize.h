/**************************************************************
 *                     quantize.h
 *
 *     Assignment: arith
 *     Authors:  Diana Calderon and Madeline Lei
 *     Usernames: dcalde02, mlei03
 *     Date:     10/21/2025
 *
 *     summary:
 *
 *     This file contains the function declarations for quantize.c.
 *     These functions deal with quantizing image data (for compression)
 *     and dequantizing quantized image data (for decompression)
 *
 **************************************************************/
#ifndef QUANTIZE_H
#define QUANTIZE_H

#include "a2methods.h"
#include "helpers.h"

A2Methods_UArray2 quantizeData(A2Methods_UArray2 original,
                               const struct A2Methods_T *methods);
void quantizeApply(int col, int row, A2Methods_UArray2 array2,
                   A2Methods_Object *elem, void *cl);
int linearQuantizeValue(float value, int width, float maxFloat);

A2Methods_UArray2 dequantizeData(A2Methods_UArray2 original,
                                 const struct A2Methods_T *methods);

void dequantizeApply(int col, int row, A2Methods_UArray2 array2,
                     A2Methods_Object *elem, void *cl);

float linearDequantizeValue(float value, int width, float maxFloat);

#endif