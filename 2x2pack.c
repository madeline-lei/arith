#include <string.h>
#include <stdlib.h>
#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"
#include "assert.h"
#include "2x2pack.h"

const int BLOCK_SIZE = 2;

struct Closure {
        A2Methods_UArray2 array;
        const struct A2Methods_T *methods;
};

/*
 * Name:       packBlock
 * Purpose:    Creates and initializes a new array representing an image in 
 *             component video color space as 2-by-2 blocks.
 * Parameters: A2Methods_UArray2 original: the UArray2 holding the pixels of the 
 *             original image in component video color space
 *             const struct A2Methods_T *methods: an A2Methods_T struct that contains 
 *             pointers to functions one can use on the array containing the pixels
 * Return:     a pointer to the newly allocated A2Methods_UArray2 representing
 *             the image in component video color space as 2-by-2 blocks
 * Expects:    original to not be NULL
 *             methods to not be NULL
 * Notes:      will CRE if it fails to malloc cl 
 *             will CRE if original is NULL
 *             The new() function in the methods suite will CRE if
 *             height or width are negative            
 */
A2Methods_UArray2 packBlock(A2Methods_UArray2 original,
                            const struct A2Methods_T *methods)
{
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
 * Purpose:    Find the average of the Pb and Pr values of the four pixels, 
 *             then compute the cosine transform coefficients from the four Y values in the block.
 * Parameters: int col: an integer representing the distance between the
 *             current element and the left edge of the array
 *             int row: an integer representing the distance between the
 *             current element and the top of the array
 *             A2Methods_UArray2 array2: required by the A2Methods_Object
 *             interface; ignored here
 *             A2Methods_Object *elem: a pointer to a YPbPr_block representing 
 *             the current block of the image transformed into component video color space
 *             void *cl: a pointer to a closure containing the original image data
 * Return:     None
 * Expects:    elem to not be NULL and cl to not be NULL
 *             col and row are within the bounds of the array
 * Notes:      will CRE if cl is NULL
 *             will CRE if elem is NULL
 *             If col or row are out of bounds or not used by the array,
 *             the methods->at function will throw a CRE
 */
void packBlockApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                    void *cl)
{
        assert(elem != NULL && cl != NULL);

        struct Closure *closure = cl;
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
 * Parameters: struct YPbPr_pixel *p1: struct of a pixel in the first position 
 *             of the 2x2 block
 *             struct YPbPr_pixel *p2: struct of a pixel in the second position 
 *             of the 2x2 block
 *             struct YPbPr_pixel *p3: struct of a pixel in the third position 
 *             of the 2x2 block
 *             struct YPbPr_pixel *p4: struct of a pixel in the fourth position 
 *             of the 2x2 block
 *             struct YPbPr_block *block: struct of a YPbPr_block where the 
 *             averaged chroma values will be stored
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
 * Purpose:    Transform the four Y-values of the pixels into cosine coefficients 
 *             a, b, c, and d.
 * Parameters: float Y1: Y-value of the first pixel in a 2x2 block
 *             float Y2: Y-value of the second pixel in a 2x2 block
 *             float Y3: Y-value of the third pixel in a 2x2 block
 *             float Y4: Y-value of the fourth pixel in a 2x2 block
 *             struct YPbPr_block *block: struct of a YPbPr_block where the 
 *             coefficients will be stored
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

/* ---------------------------------------------------------------------------- */

/*
 * Name:       unpackBlock
 * Purpose:    Creates and initializes a new array representing an image in 
 *             component video color space as 2-by-2 blocks.
 * Parameters: A2Methods_UArray2 original: the UArray2 holding the pixels of the 
 *             original image in component video color space
 *             const struct A2Methods_T *methods: an A2Methods_T struct that contains 
 *             pointers to functions one can use on the array containing the pixels
 * Return:     a pointer to the newly allocated A2Methods_UArray2 representing
 *             the image in component video color space as 2-by-2 blocks
 * Expects:    original to not be NULL
 *             methods to not be NULL
 * Notes:      will CRE if it fails to malloc cl 
 *             will CRE if original is NULL
 *             The new() function in the methods suite will CRE if
 *             height or width are negative            
 */
A2Methods_UArray2 unpackBlock(A2Methods_UArray2 original,
                              const struct A2Methods_T *methods)
{
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
 * Purpose:    Use the inverse of the discrete cosine transform to compute Y1, Y2, Y3, and Y4 from a, b, c, and d. 

 * Parameters: int col: an integer representing the distance between the
 *             current element and the left edge of the array
 *             int row: an integer representing the distance between the
 *             current element and the top of the array
 *             A2Methods_UArray2 array2: required by the A2Methods_Object
 *             interface; ignored here
 *             A2Methods_Object *elem: a pointer to a YPbPr_block representing 
 *             the current block of the image transformed into component video color space
 *             void *cl: a pointer to a closure containing the original image data
 * Return:     None
 * Expects:    elem to not be NULL and cl to not be NULL
 *             col and row are within the bounds of the array
 * Notes:      will CRE if cl is NULL
 *             will CRE if elem is NULL
 *             If col or row are out of bounds or not used by the array,
 *             the methods->at function will throw a CRE
 */
void unpackBlockApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                      void *cl)
{
        assert(elem != NULL && cl != NULL);

        struct Closure *closure = cl;
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
 * Purpose:    Transform the four Y-values of the pixels into cosine coefficients 
 *             a, b, c, and d.
 * Parameters: float Y1: Y-value of the first pixel in a 2x2 block
 *             float Y2: Y-value of the second pixel in a 2x2 block
 *             float Y3: Y-value of the third pixel in a 2x2 block
 *             float Y4: Y-value of the fourth pixel in a 2x2 block
 *             struct YPbPr_block *block: struct of a YPbPr_block where the 
 *             coefficients will be stored
 * Return:     None
 * Expects:    block to not be NULL
 * Notes:      will CRE if block is NULL
 */
void DCTtoPixel(float a, float b, float c, float d, float *Y1, float *Y2,
                float *Y3, float *Y4)
{
        *Y1 = a - b - c + d;
        *Y2 = a - b + c - d;
        *Y3 = a + b - c - d;
        *Y4 = a + b + c + d;
}

/*
 * Name:       setChromaApply
 * Purpose:    Transform each pixel from RGB color space into component video color space
 * Parameters: int col: an integer representing the distance between the
 *             current element and the left edge of the array
 *             int row: an integer representing the distance between the
 *             current element and the top of the array
 *             A2Methods_UArray2 array2: required by the A2Methods_Object
 *             interface; ignored here
 *             A2Methods_Object *elem: a pointer to a YPbPr_pixel representing the current
 *             pixel of the image transformed into component video color space
 *             void *cl: a pointer to a closure containing the original image data
 * Return:     None
 * Expects:    elem to not be NULL and cl to not be NULL
 *             col and row are within the bounds of the array
 * Notes:      will CRE if cl is NULL
 *             will CRE if elem is NULL
 *             If col or row are out of bounds or not used by the array,
 *             the methods->at function will throw a CRE
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