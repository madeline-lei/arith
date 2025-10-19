#ifndef QUANTIZE_H
#define QUANTIZE_H

#include "a2methods.h"

struct Quantized_Block {
        unsigned a;
        int b;
        int c;
        int d;
        unsigned avgPb;
        unsigned avgPr;
};

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

float clampBCD(float value);

#endif