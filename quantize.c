/**************************************************************
 *                     quantize.c
 *
 *     Assignment: arith
 *     Authors:  Diana Calderon and Madeline Lei
 *     Usernames: dcalde02, mlei03
 *     Date:     10/21/2025
 *
 *     summary:
 *
 *     This file contains the quantizeData() and dequantizeData() functions,
 *     which quantize the image data (for compression) and dequantize
 *     that quantized image data (for decompression)
 *
 **************************************************************/
#include "quantize.h"
#include "assert.h"
#include <stdlib.h>
#include <math.h>
#include "arith40.h"

static const unsigned A_WIDTH = 9;
static const unsigned B_WIDTH = 5;
static const unsigned C_WIDTH = 5;
static const unsigned D_WIDTH = 5;

static float clamp(float value, float min, float max);

/************************ quantizeData ******************************
 *
 * Quantizes the DCT coefficients and average chroma values of each YPbPr_block
 * of the image.
 *
 * Parameters:
 *        A2Methods_UArray2 original: a pointer to a UArray2 storing 
 *        YPbPr_block structs, which represent the pixels of a PPM packed 
 *        into blocks.
 *        const struct A2Methods_T *methods: A pointer to a A2Methods_T struct
 *        that contains pointers to functions on can use on a UArray2
 *
 * Return: a pointer to a newly allocated A2Methods_UArray2 struct storing
 *         Quantized_Block structs representing the pixels from the original
 *         image stored in quantized, packed blocks.
 *
 * Expects
 *         original and methods to not be NULL
 *         the pixels stored in original to be YPbPr_block structs
 * Notes:
 *         Allocates memory for a A2Methods_UArray2 struct (destination) that
 *         the caller is responsible for freeing using methods->free()
 *         Will raise a CRE if original or methods is NULL or if
 *         methods->new() fails.
 *
 ************************************************************/
A2Methods_UArray2 quantizeData(A2Methods_UArray2 original,
                               const struct A2Methods_T *methods)
{
        assert(original != NULL && methods != NULL);
        A2Methods_UArray2 destination = methods->new(
                methods->width(original), methods->height(original),
                sizeof(struct Quantized_Block));

        struct Closure cl = { .array = destination, .methods = methods };

        methods->map_default(original, quantizeApply, &cl);

        return destination;
}

/*
 * Name:       quantizeApply
 * Purpose:    Quantize each 2x2 block of the image by converting its cosine 
 *             coefficients and averaged chroma values into integers
 * Parameters: int col: an integer representing the distance between the
 *             current element and the left edge of the array
 *             int row: an integer representing the distance between the
 *             current element and the top of the array
 *             A2Methods_UArray2 array2: required by the A2Methods_Object
 *             interface; ignored here
 *             A2Methods_Object *elem: a pointer to a YPbPr_block representing 
 *             the current 2x2 block of the image
 *             void *cl: a pointer to a Closure struct containing a pointer
 *             to a A2Methods_UArray2 (the destination image in YPbPr_pixel
 *             format) and a pointer to an A2Methods_T struct with function
 *             pointers for operating on the array.
 * Return:     None
 * Expects:    elem to not be NULL and cl points to a Closure struct that
 *             stores a non-NULL A2Methods_UArray2 and A2Methods_T struct 
 *             and is not NULL
 *             col and row are within the bounds of the array
 * Notes:      will CRE if cl is NULL or its array or methods field is NULL
 *             will CRE if elem is NULL
 *             If col or row are out of bounds or not used by the array,
 *             the methods->at function will throw a CRE
 */
void quantizeApply(int col, int row, A2Methods_UArray2 array2,
                   A2Methods_Object *elem, void *cl)
{
        assert(elem != NULL && cl != NULL);
        struct Closure *closure = cl;
        assert(closure != NULL && closure->methods != NULL &&
               closure->array != NULL);
        struct YPbPr_block *currBlock = elem;

        struct Quantized_Block *quantized =
                closure->methods->at(closure->array, col, row);

        quantized->a = linearQuantizeValue(currBlock->a, A_WIDTH, 1);
        quantized->b = linearQuantizeValue(clamp(currBlock->b, -0.3, 0.3),
                                           B_WIDTH - 1, 0.3);
        quantized->c = linearQuantizeValue(clamp(currBlock->c, -0.3, 0.3),
                                           C_WIDTH - 1, 0.3);
        quantized->d = linearQuantizeValue(clamp(currBlock->d, -0.3, 0.3),
                                           D_WIDTH - 1, 0.3);
        quantized->avgPb = Arith40_index_of_chroma(currBlock->avgPb);
        quantized->avgPr = Arith40_index_of_chroma(currBlock->avgPr);

        (void) array2;
}

/*
 * Name:       linearQuantizeValue
 * Purpose:    Linearly scales and converts a floating-point value into an 
 *             integer representation for quantization
 * Parameters: float value: the floating-point value to quantize
 *             int width: the bit width of the field used to store this value
 *             float maxFloat: the maximum magnitude of the input range
 * Return:     an integer value representing the quantized input
 * Expects:    None
 * Notes:      None
 */
int linearQuantizeValue(float value, int width, float maxFloat)
{
        float scale = (pow(width, 2) - 1) / maxFloat;
        return round(value * scale);
}

/************************ dequantizeData ******************************
 *
 * Dequantizes each block of an image into its DCT coefficients and 
 * average chroma values
 *
 * Parameters:
 *        A2Methods_UArray2 original: a pointer to a UArray2 storing 
 *        Quantized_Block structs, which represent the pixels of a PPM packed 
 *        into blocks and quantized.
 *        const struct A2Methods_T *methods: A pointer to a A2Methods_T struct
 *        that contains pointers to functions on can use on a UArray2
 *
 * Return: a pointer to a newly allocated A2Methods_UArray2 struct storing
 *         YPbPr_block structs representing the dequantized DCT coefficients
 *         and average chroma values
 *
 * Expects
 *         original and methods to not be NULL
 *         the pixels stored in original to be Quantized_Block structs
 * Notes:
 *         Allocates memory for a A2Methods_UArray2 struct (destination) that
 *         the caller is responsible for freeing using methods->free()
 *         Will raise a CRE if original or methods is NULL or if
 *         methods->new() fails.
 *
 ************************************************************/
A2Methods_UArray2 dequantizeData(A2Methods_UArray2 original,
                                 const struct A2Methods_T *methods)
{
        assert(original != NULL && methods != NULL);
        A2Methods_UArray2 destination = methods->new(
                methods->width(original), methods->height(original),
                sizeof(struct YPbPr_block));

        struct Closure cl = { .array = destination, .methods = methods };

        methods->map_default(original, dequantizeApply, &cl);

        return destination;
}

/*
 * Name:       dequantizeApply
 * Purpose:    Dequantize each 2x2 block of the image by converting its integer 
 *             quantized coefficients and chromas back into floating-point 
 *             values in component video color space.
 * Parameters: int col: an integer representing the distance between the
 *             current element and the left edge of the array
 *             int row: an integer representing the distance between the
 *             current element and the top of the array
 *             A2Methods_UArray2 array2: required by the A2Methods_Object
 *             interface; ignored here
 *             A2Methods_Object *elem: a pointer to a Quantized_Block 
 *             representing the current 2x2 block of the image
 *             void *cl: a pointer to a Closure struct containing a pointer
 *             to a A2Methods_UArray2 (the destination image in YPbPr_pixel
 *             format) and a pointer to an A2Methods_T struct with function
 *             pointers for operating on the array.
 * Return:     None
 * Expects:    elem to not be NULL and cl points to a Closure struct that
 *             stores a non-NULL A2Methods_UArray2 and A2Methods_T struct 
 *             and is not NULL
 *             col and row are within the bounds of the array
 * Notes:      will CRE if cl is NULL or its array or methods field is NULL
 *             will CRE if elem is NULL
 *             If col or row are out of bounds or not used by the array,
 *             the methods->at function will throw a CRE
 */
void dequantizeApply(int col, int row, A2Methods_UArray2 array2,
                     A2Methods_Object *elem, void *cl)
{
        assert(elem != NULL && cl != NULL);
        struct Closure *closure = cl;
        assert(closure != NULL && closure->methods != NULL &&
               closure->array != NULL);

        struct Quantized_Block *quantized = elem;
        struct YPbPr_block *currBlock =
                closure->methods->at(closure->array, col, row);

        currBlock->a = linearDequantizeValue(quantized->a, A_WIDTH, 1);
        currBlock->b = linearDequantizeValue(quantized->b, B_WIDTH - 1, 0.3);
        currBlock->c = linearDequantizeValue(quantized->c, C_WIDTH - 1, 0.3);
        currBlock->d = linearDequantizeValue(quantized->d, D_WIDTH - 1, 0.3);
        currBlock->avgPb = Arith40_chroma_of_index(quantized->avgPb);
        currBlock->avgPr = Arith40_chroma_of_index(quantized->avgPr);

        (void) array2;
}

/*
 * Name:       linearDequantizeValue
 * Purpose:    Converts an integer quantized value back into its floating-point
 *             representation using the given bit width and maximum float 
 *             magnitude.
 * Parameters: float value: the integer value to dequantize
 *             int width: the bit width of the field used during quantization
 *             float maxFloat: the maximum magnitude of the original 
 *             floating-point range
 * Return:     the dequantized floating-point value
 * Expects:    None
 * Notes:      None
 */
float linearDequantizeValue(float value, int width, float maxFloat)
{
        float scale = maxFloat / (pow(width, 2) - 1);
        return value * scale;
}

/*
 * Name:       clamp
 * Purpose:    a private function that fits a floating-point value within a
 *             given range, returning  either the min or max if it exceeds
 *             those limits.
 * Parameters: float value: a float that needs to be clamped
 *             float min: the minimum value in the range
 *             float max: the maximum value in the range
 * Return:     a float clamped to the given range
 * Expects:    min > max
 * Notes:      None
 */
static float clamp(float value, float min, float max)
{
        if (value < min) {
                return min;
        }
        if (value > max) {
                return max;
        }

        return value;
}
