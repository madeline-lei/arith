/**************************************************************
 *                     2x2pack.c
 *
 *     Assignment: arith
 *     Authors:  Diana Calderon and Madeline Lei
 *     Usernames: dcalde02, mlei03
 *     Date:     10/21/2025
 *
 *     summary:
 *
 *     This file contains the packBlock() and unpackBlock() functions,
 *     which pack the pixels of a PPM image in 2x2 regions into block (for 
 *     compression) or unpack those blocks into 4 pixels (for 
 *     decompression), respectively. Also implements helper functions
 *     to achieve this.
 *
 **************************************************************/
#include <stdlib.h>
#include "a2methods.h"
#include "assert.h"
#include "2x2pack.h"

const int BLOCK_SIZE = 2;

/************************ packBlock ******************************
 *
 * Packs the given image PPM (in component video color space) into
 * blocks of pixels, using averages and the discrete cosine transform (DCT)
 * function. Stores these blocks in a A2Methods_UArray2 that will be 
 * returned.
 * 
 * Parameters:
 *        A2Methods_UArray2 original: a pointer to a UArray2 storing 
 *        YPbPr_pixel structs, which represents an image in the PPM format 
 *        with the pixels in the component video space.
 *        const struct A2Methods_T *methods: A pointer to a A2Methods_T struct
 *        that contains pointers to functions on can use on a UArray
 * 
 * Return: 
 *         A pointer to a newly allocated and initialzed A2Methods_UArray2 of
 *         YPbPr_block structs, which represent the pixels of a PPM packed 
 *         into blocks
 * 
 * Expects
 *         original and methods to not be NULL
 *         the pixels stored in original to be in the component video space
 *         and its dimensions to be multiples of BLOCK_SIZE
 *
 * Notes:
 *         Allocates memory for a A2Methods_UArray2 struct (destination) that
 *         the caller is responsible for freeing using methods->free()
 *         Will raise a CRE if original or methods is NULL or if
 *         methods->new() fails.
 *
 ************************************************************/
A2Methods_UArray2 packBlock(A2Methods_UArray2 original,
                            const struct A2Methods_T *methods)
{
        assert(original != NULL && methods != NULL);
        A2Methods_UArray2 destination =
                methods->new(methods->width(original) / BLOCK_SIZE,
                             methods->height(original) / BLOCK_SIZE,
                             sizeof(struct YPbPr_block));

        struct Closure cl = { .array = original, .methods = methods };

        methods->map_default(destination, packBlockApply, &cl);

        return destination;
}

/*
 * Name:       packBlockApply
 * Purpose:    Packs a 2x2 region of pixels (in component video color space)
 *             from the original image into a single YPbPr_block. Computes
 *             the average chroma (Pb, Pr) values and the discrete cosine
 *             transform (DCT) coefficients for the luminance (Y) values.
 * Parameters: int col: an integer representing the distance between the
 *             current element and the left edge of the array
 *             int row: an integer representing the distance between the
 *             current element and the top of the array
 *             A2Methods_UArray2 array2: required by the A2Methods_Object
 *             interface; ignored here
 *             A2Methods_Object *elem: a pointer to a YPbPr_block representing 
 *             a 2x2 block of pixels of the image
 *             void *cl: a pointer to a Closure struct containing a pointer
 *             to a A2Methods_UArray2 (the original image in YPbPr_pixel
 *             format) and a pointer to an A2Methods_T struct with function
 *             pointers for operating on the array
 * Returns:    None
 * Expects:    elem to not be NULL and cl points to a Closure struct that
 *             stores a non-NULL A2Methods_UArray2 and A2Methods_T struct 
 *             and is not NULL
 *             col and row are within the bounds of the array
 * Notes:      will CRE if cl is NULL or its array or methods field is NULL
 *             will CRE if elem is NULL
 *             If col or row are not valid, the methods->at function will 
 *             throw a CRE
 */
void packBlockApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                    void *cl)
{
        assert(elem != NULL && cl != NULL);

        struct Closure *closure = cl;
        assert(closure->array != NULL && closure != NULL &&
               closure->methods != NULL);

        A2Methods_UArray2 original = closure->array;
        const struct A2Methods_T *methods = closure->methods;

        col *= BLOCK_SIZE;
        row *= BLOCK_SIZE;

        struct YPbPr_block *currBlock = elem;

        struct YPbPr_pixel *pixel1 = methods->at(original, col, row);
        struct YPbPr_pixel *pixel2 = methods->at(original, col + 1, row);
        struct YPbPr_pixel *pixel3 = methods->at(original, col, row + 1);
        struct YPbPr_pixel *pixel4 = methods->at(original, col + 1, row + 1);

        averageChroma(pixel1, pixel2, pixel3, pixel4, currBlock);
        pixelToDCT(pixel1->Y, pixel2->Y, pixel3->Y, pixel4->Y, currBlock);

        (void) array2;
}

/*
 * Name:       averageChroma
 * Purpose:    Find the average of the Pb and Pr values of the four pixels
 * Parameters: struct YPbPr_pixel *p1: a pointer to a YPbPr_pixel struct 
 *             representing the pixel in the first position of the 2x2 block
 *             struct YPbPr_pixel *p2: a pointer to a YPbPr_pixel struct 
 *             representing the pixel in the second position of the 2x2 block
 *             struct YPbPr_pixel *p3: a pointer to a YPbPr_pixel struct 
 *             representing the pixel in the third position of the 2x2 block
 *             struct YPbPr_pixel *p4: a pointer to a YPbPr_pixel struct 
 *             representing the pixel in the fourth position of the 2x2 block
 *             struct YPbPr_block *block: a pointer to a YPbPr_block struct
 *             where the averaged chroma values will be stored
 * Return:     None
 * Expects:    block to not be NULL
 * Notes:      will CRE if block is NULL
 */
void averageChroma(struct YPbPr_pixel *p1, struct YPbPr_pixel *p2,
                   struct YPbPr_pixel *p3, struct YPbPr_pixel *p4,
                   struct YPbPr_block *block)
{
        assert(block != NULL);

        float PbAvg = (p1->Pb + p2->Pb + p3->Pb + p4->Pb) / 4.0;
        float PrAvg = (p1->Pr + p2->Pr + p3->Pr + p4->Pr) / 4.0;

        block->avgPb = PbAvg;
        block->avgPr = PrAvg;
}

/*
 * Name:       pixelToDCT
 * Purpose:    Transform the four Y-values of the pixels into cosine 
 *             coefficients a, b, c, and d using the DCT.
 * Parameters: float Y1: Y-value of the first pixel in a 2x2 block
 *             float Y2: Y-value of the second pixel in a 2x2 block
 *             float Y3: Y-value of the third pixel in a 2x2 block
 *             float Y4: Y-value of the fourth pixel in a 2x2 block
 *             struct YPbPr_block *block: a pointer to a YPbPr_block struct
 *             where the  coefficients will be stored
 * Return:     None
 * Expects:    block to not be NULL
 * Notes:      will CRE if block is NULL
 */
void pixelToDCT(float Y1, float Y2, float Y3, float Y4,
                struct YPbPr_block *block)
{
        assert(block != NULL);
        block->a = (Y4 + Y3 + Y2 + Y1) / 4.0;
        block->b = (Y4 + Y3 - Y2 - Y1) / 4.0;
        block->c = (Y4 - Y3 + Y2 - Y1) / 4.0;
        block->d = (Y4 - Y3 - Y2 + Y1) / 4.0;
}

/************************ unpackBlock ******************************
 * 
 * Unpacks a UArray2 of YPbPr_block structs (representing 2x2 blocks of pixels 
 * in a compressed PPM image) into a UArray2 of YPbPr_pixel structs, 
 * representing each pixel in component video color space (Y, Pb, Pr).
 * Stores these pixels in a A2Methods_UArray2 that will be returned.
 * 
 * Parameters:
 *        A2Methods_UArray2 original: a pointer to a UArray2 storing 
 *        YPbPr_block structs, which represent 2x2 blocks of pixels that
 *        make up a compressed PPM image
 *        const struct A2Methods_T *methods: A pointer to a A2Methods_T struct
 *        that contains pointers to functions on can use on a UArray
 * 
 * Return: 
 *         A pointer to a newly allocated and initialzed A2Methods_UArray2 of
 *         YPbPr_pixel structs, which represent the pixels of a PPM in
 *         component video color space
 * 
 * Expects
 *         original and methods to not be NULL
 *         the pixels stored in original are YPbPr_block structs
 *
 * Notes:
 *         Allocates memory for a A2Methods_UArray2 struct (destination) that
 *         the caller is responsible for freeing using methods->free()
 *         Will raise a CRE if original or methods is NULL or if
 *         methods->new() fails.
 *
 ************************************************************/
A2Methods_UArray2 unpackBlock(A2Methods_UArray2 original,
                              const struct A2Methods_T *methods)
{
        assert(original != NULL && methods != NULL);
        A2Methods_UArray2 destination =
                methods->new(methods->width(original) * BLOCK_SIZE,
                             methods->height(original) * BLOCK_SIZE,
                             sizeof(struct YPbPr_pixel));

        struct Closure cl = { .array = destination, .methods = methods };

        methods->map_default(original, unpackBlockApply, &cl);

        struct Closure cl2 = { .array = original, .methods = methods };

        methods->map_default(destination, setChromaApply, &cl2);

        return destination;
}

/*
 * Name:       unpackBlockApply
 * Purpose:    Unpacks a YPbPr_block from the compressed image into a 2x2
 *             region of pixels (in component video color space). Uses the 
 *             inverse of the discrete cosine transform to compute Y1, 
 *             Y2, Y3, and Y4 from a, b, c, and d.
 * Parameters: int col: an integer representing the distance between the
 *             current element and the left edge of the array
 *             int row: an integer representing the distance between the
 *             current element and the top of the array
 *             A2Methods_UArray2 array2: required by the A2Methods_Object
 *             interface; ignored here
 *             A2Methods_Object *elem: a pointer to a YPbPr_block representing 
 *             the current block of the image transformed into component video 
 *             color space
 *             void *cl: a pointer to a Closure struct containing a pointer
 *             to a A2Methods_UArray2 (the destination image in YPbPr_pixel
 *             format) and a pointer to an A2Methods_T struct with function
 *             pointers for operating on the array
 * Returns:    None
 * Expects:    elem to not be NULL and cl points to a Closure struct that
 *             stores a non-NULL A2Methods_UArray2 and A2Methods_T struct 
 *             and is not NULL
 *             col and row are within the bounds of the array
 * Notes:      will CRE if cl is NULL or its array or methods field is NULL
 *             will CRE if elem is NULL
 *             If col or row are not valid, the methods->at function will 
 *             throw a CRE
 */
void unpackBlockApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                      void *cl)
{
        assert(elem != NULL && cl != NULL);

        struct Closure *closure = cl;
        assert(closure->array != NULL && closure != NULL &&
               closure->methods != NULL);

        A2Methods_UArray2 destination = closure->array;
        const struct A2Methods_T *methods = closure->methods;

        struct YPbPr_block *currBlock = elem;

        col *= BLOCK_SIZE;
        row *= BLOCK_SIZE;

        float Y1;
        float Y2;
        float Y3;
        float Y4;

        DCTtoPixel(currBlock->a, currBlock->b, currBlock->c, currBlock->d, &Y1,
                   &Y2, &Y3, &Y4);

        struct YPbPr_pixel *pixel1 = methods->at(destination, col, row);
        struct YPbPr_pixel *pixel2 = methods->at(destination, col + 1, row);
        struct YPbPr_pixel *pixel3 = methods->at(destination, col, row + 1);
        struct YPbPr_pixel *pixel4 = methods->at(destination, col + 1, row + 1);

        pixel1->Y = Y1;
        pixel2->Y = Y2;
        pixel3->Y = Y3;
        pixel4->Y = Y4;

        (void) array2;
}

/*
 * Name:       DCTtoPixel
 * Purpose:    Transform the cosine coefficients a, b, c, and d into four 
 *             Y-values of the pixel
 * Parameters: float a: cosign coefficient a in pixel
 *             float b: cosign coefficient b in pixel
 *             float c: cosign coefficient c in pixel
 *             float d: cosign coefficient d in pixel
 *             float *Y1: a pointer to the Y-value of the first pixel 
 *             in a 2x2 block
 *             float *Y2: a pointer to the Y-value of the second pixel 
 *             in a 2x2 block
 *             float *Y3: a pointer to the Y-value of the third pixel 
 *             in a 2x2 block
 *             float *Y4: a pointer to the Y-value of the fourth pixel 
 *             in a 2x2 block
 *             struct YPbPr_block *block: a pointer to a YPbPr_block struct
 *             where the coefficients will be stored
 * Return:     None
 * Expects:    all Y pointers to not be NULL
 * Notes:      will CRE if Y pointer is NULL
 */
void DCTtoPixel(float a, float b, float c, float d, float *Y1, float *Y2,
                float *Y3, float *Y4)
{
        assert(Y1 != NULL && Y2 != NULL && Y3 != NULL && Y4 != NULL);

        *Y1 = a - b - c + d;
        *Y2 = a - b + c - d;
        *Y3 = a + b - c - d;
        *Y4 = a + b + c + d;
}

/*
 * Name:       setChromaApply
 * Purpose:    Sets the Pb and Pr (chroma) values for each YPbPr_pixel
 *             using the average Pb and Pr values stored in the corresponding
 *             2x2 YPbPr_block from the original compressed image.
 * Parameters: int col: an integer representing the distance between the
 *             current element and the left edge of the array
 *             int row: an integer representing the distance between the
 *             current element and the top of the array
 *             A2Methods_UArray2 array2: required by the A2Methods_Object
 *             interface; ignored here
 *             A2Methods_Object *elem: a pointer to a YPbPr_pixel representing 
 *             the current pixel of the image transformed into component video 
 *             color space
 *             void *cl: a pointer to a Closure struct containing a pointer
 *             to a A2Methods_UArray2 (the original image in YPbPr_pixel
 *             format) and a pointer to an A2Methods_T struct with function
 *             pointers for operating on the array
 * Return:     None
 * Expects:    elem to not be NULL and cl points to a Closure struct that
 *             stores a non-NULL A2Methods_UArray2 and A2Methods_T struct 
 *             and is not NULL
 *             col and row are within the bounds of the array
 * Notes:      will CRE if cl is NULL
 *             will CRE if elem is NULL
 *             If col or row are not valid, the methods->at function will 
 *             throw a CRE
 */
void setChromaApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                    void *cl)
{
        assert(elem != NULL && cl != NULL);

        struct Closure *closure = cl;
        A2Methods_UArray2 original = closure->array;
        const struct A2Methods_T *methods = closure->methods;

        struct YPbPr_pixel *currPix = elem;
        struct YPbPr_block *currBlock =
                methods->at(original, col / BLOCK_SIZE, row / BLOCK_SIZE);

        currPix->Pb = currBlock->avgPb;
        currPix->Pr = currBlock->avgPr;

        (void) array2;
}