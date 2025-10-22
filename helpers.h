/**************************************************************
 *                     helpers.h
 *
 *     Assignment: arith
 *     Authors:  Diana Calderon and Madeline Lei
 *     Usernames: dcalde02, mlei03
 *     Date:     10/21/2025
 *
 *     summary:
 *
 *     This file contains struct declarations for a closures and
 *     image data representations used for compressing and
 *     decompressing and image.
 *
 **************************************************************/
#ifndef HELPERS_H
#define HELPERS_H

/*
 * Name:       Closure
 * Purpose:    Stores information for a UArray2 to be used as a
 *             closure argument in a mapping function
 * Components: 
 *             A2Methods_UArray2 array: a 2D UArray storing image data
 *             unsigned denominator: an unsigned integer representing the
 *             maximum color value of the PPM
 *             const struct A2Methods_T *methods: A pointer to a A2Methods_T 
 *             struct that contains pointers to functions on can use on a
 *             UArray2
 */
struct Closure {
        A2Methods_UArray2 array;
        unsigned denominator;
        const struct A2Methods_T *methods;
};

/*
 * Name:       YPbPr_block
 * Purpose:    Represents a 2x2 block of pixels in component video color space.
 *             Stores DCT coefficients for luminance and the average chroma
 *             values
 * Components: 
 *             float a: a float representing the average brightness of the
 *             image (DCT coefficient)
 *             float b: a float representing the degree to which the image
 *             gets brighter as we move from top to bottom (DCT coefficient)
 *             float c: a float representing the degree to which the image
 *             gets brighter as we move from left to right (DCT coefficient)
 *             float d: a float representing the degree to which the pixels
 *             on one diagonal are brighter than the pixels on the other
 *             diagonal (DCT coefficient)
 *             float avgPb: a float representing the average Pb value
 *             float avgPr: a float representing the average Pr value
 */
struct YPbPr_block {
        float a;
        float b;
        float c;
        float d;
        float avgPb;
        float avgPr;
};

/*
 * Name:       YPbPr_pixel
 * Purpose:    Represents a single pixel in component video color space
 * Components: 
 *             float Y: the brightness of a color
 *             float Pb: the blue-difference signal
 *             float Pr: the red-difference signal
 */
struct YPbPr_pixel {
        float Y;
        float Pb;
        float Pr;
};

/*
 * Name:       Quantized_Block
 * Purpose:    Represents a 2x2 block of pixels in component video color space.
 *             Stores DCT coefficients for luminance and the average chroma
 *             values
 * Components: 
 *             unsigned a, int b, int c, int d: quantized DCT coefficients
 *             unsigned avgPb, unsigned avgPr: quantized chroma averages
 */
struct Quantized_Block {
        unsigned a;
        int b;
        int c;
        int d;
        unsigned avgPb;
        unsigned avgPr;
};

#endif