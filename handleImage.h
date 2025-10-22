/**************************************************************
 *                     handleImage.h
 *
 *     Assignment: arith
 *     Authors:  Diana Calderon and Madeline Lei
 *     Usernames: dcalde02, mlei03
 *     Date:     10/21/2025
 *
 *     summary:
 *
 *     This file contains function declarations for handleImage.c.
 *     These functions will deal with reading in images stored as compressed
 *     and decompressed PPMs and printing them out.
 *
 **************************************************************/
#ifndef HANDLE_IMAGE_H
#define HANDLE_IMAGE_H

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
