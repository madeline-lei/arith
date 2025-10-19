#ifndef PACK_WORD_H
#define PACK_WORD_H

#include "a2methods.h"
#include <stdint.h>

A2Methods_UArray2 packWord(A2Methods_UArray2 original,
                           const struct A2Methods_T *methods);

void packWordApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                   void *cl);

uint32_t bitpackWord(unsigned a, int b, int c, int d, unsigned avgPb,
                     unsigned avgPr);

A2Methods_UArray2 unpackWord(A2Methods_UArray2 original,
                           const struct A2Methods_T *methods);

void unpackWordApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                   void *cl);

void unbitpackWord(uint32_t word, unsigned *a, int *b, int *c, int *d,
                   unsigned *avgPb, unsigned *avgPr);

#endif