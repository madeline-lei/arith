/**************************************************************
 *                     packword.c
 *
 *     Assignment: arith
 *     Authors:  Diana Calderon and Madeline Lei
 *     Usernames: dcalde02, mlei03
 *     Date:     10/21/2025
 *
 *     summary:
 *
 *     This file contains the function declarations for packWord.c. 
 *     These functions will deal with packing the fields of a quantized 2x2
 *     block into a 32-bit word (for compression) and unpacking that word
 *     back into  a Quantized_block (for decompression).
 * 
 **************************************************************/
#ifndef PACK_WORD_H
#define PACK_WORD_H

#include "a2methods.h"
#include "helpers.h"
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