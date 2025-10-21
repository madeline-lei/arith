#include "convertColor.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"
#include "assert.h"
#include <stdlib.h>
#include <math.h>

static float clamp(float value, float min, float max);

struct Closure {
        A2Methods_UArray2 array;
        unsigned denominator;
        const struct A2Methods_T *methods;
};

/*
 * Name:       rgbToYPbPr
 * Purpose:    Creates and initializes a new array representing an image converted 
 *             from RGB color space to component video color space
 * Parameters: A2Methods_UArray2 original: the UArray2 holding the pixels of 
 *             the original image in RGB color space unsigned denominator: an 
 *             integer representing the maximum color value const struct 
 *             A2Methods_T *methods: an A2Methods_T struct that contains 
 *             pointers to functions one can use on the array containing the 
*              pixels
 * Return:     a pointer to the newly allocated A2Methods_UArray2 representing
 *             the image converted from RGB to component video color space
 * Expects:    original to not be NULL
 *             methods to not be NULL
 * Notes:      will CRE if it fails to malloc cl 
 *             will CRE if original is NULL
 *             The new() function in the methods suite will throw a CRE if
 *             height or width are not nonnegative            
 */
A2Methods_UArray2 rgbToYPbPr(A2Methods_UArray2 original, unsigned denominator,
                             const struct A2Methods_T *methods)
{
        assert(original != NULL && methods != NULL);
        A2Methods_UArray2 destination = methods->new(
                methods->width(original), methods->height(original),
                sizeof(struct YPbPr_pixel));

        struct Closure cl = { .array = original,
                              .methods = methods,
                              .denominator = denominator };

        methods->map_default(destination, convertYbPbPrApply, &cl);

        return destination;
}

/*
 * Name:       pixelToYPbPr
 * Purpose:    Change a single pixel in RGB color space to its floating-point 
 *             representation in component video color space
 * Parameters: Pnm_rgb pixel: the pixel of an image in RGB color space
 *             unsigned denominator: an integer representing the maximum color 
 *             value
 * Return:     a struct representing the new pixel in component video color 
 *             space
 * Expects:    pixel to not be NULL
 * Notes:      will CRE if pixel is NULL
 *             If the floating point representation of pixels in RGB color 
 *             space are out of bounds, they are clamped to their assigned 
 *             range when converting to Y, Pb, and Pr.
 */
struct YPbPr_pixel pixelToYPbPr(Pnm_rgb pixel, unsigned denominator)
{
        assert(pixel != NULL);
        struct YPbPr_pixel newPixel;

        float r = (float) pixel->red / denominator;
        float g = (float) pixel->green / denominator;
        float b = (float) pixel->blue / denominator;

        float Y = 0.299 * r + 0.587 * g + 0.114 * b;
        newPixel.Y = clamp(Y, 0, 1);

        float Pb = -0.168736 * r - 0.331264 * g + 0.5 * b;
        newPixel.Pb = clamp(Pb, -0.5, 0.5);

        float Pr = 0.5 * r - 0.418688 * g - 0.081312 * b;
        newPixel.Pr = clamp(Pr, -0.5, 0.5);

        return newPixel;
}

/*
 * Name:       convertYbPbPrApply
 * Purpose:    Transform each pixel from RGB color space into component video 
 *             color space
 * Parameters: int col: an integer representing the distance between the
 *             current element and the left edge of the array
 *             int row: an integer representing the distance between the
 *             current element and the top of the array
 *             A2Methods_UArray2 array2: required by the A2Methods_Object
 *             interface; ignored here
 *             A2Methods_Object *elem: a pointer to a YPbPr_pixel representing 
 *             the current pixel of the image transformed into component video 
 *             color spacevoid *cl: a pointer to a closure containing the 
 *             original image data
 * Return:     None
 * Expects:    elem to not be NULL and cl to not be NULL
 *             col and row are within the bounds of the array
 * Notes:      will CRE if cl is NULL
 *             will CRE if elem is NULL
 *             If col or row are out of bounds or not used by the array,
 *             the methods->at function will throw a CRE
 */
void convertYbPbPrApply(int col, int row, A2Methods_UArray2 array2,
                        A2Methods_Object *elem, void *cl)
{
        assert(elem != NULL && cl != NULL);

        struct Closure *closure = cl;
        A2Methods_UArray2 original = closure->array;

        Pnm_rgb currPixel = closure->methods->at(original, col, row);
        assert(currPixel != NULL);

        struct YPbPr_pixel *newPixel = elem;

        *newPixel = pixelToYPbPr(currPixel, closure->denominator);

        (void) array2;
}

/*
 * Name:       YPbPrToRGB
 * Purpose:    Creates and initializes a new array representing an image 
 *             converted  from component video color space back to RGB color 
 *             space
 * Parameters: A2Methods_UArray2 original: the UArray2 holding the pixels of 
 *             the  image transformed to component video color space unsigned 
 *             denominator: an integer representing the maximum color value
 *             const struct A2Methods_T *methods: an A2Methods_T struct that 
 *             contains pointers to functions one can use on the array 
 *             containing the pixels
 * Return:     a pointer to the newly allocated A2Methods_UArray2 representing
 *             the image converted from component video color space to RGB 
 *             color space
 * Expects:    original to not be NULL
 *             methods to not be NULL
 * Notes:      will CRE if it fails to malloc cl 
 *             will CRE if original is NULL
 *             The new() function in the methods suite will throw a CRE if
 *             height or width are not nonnegative            
 */
A2Methods_UArray2 YPbPrToRGB(A2Methods_UArray2 original, unsigned denominator,
                             const struct A2Methods_T *methods)
{
        assert(original != NULL && methods != NULL);

        A2Methods_UArray2 destination = methods->new(methods->width(original),
                                                     methods->height(original),
                                                     sizeof(struct Pnm_rgb));

        struct Closure cl = { .array = original,
                              .methods = methods,
                              .denominator = denominator };

        methods->map_default(destination, convertRgbApply, &cl);

        return destination;
}

/*
 * Name:       pixelToRGB
 * Purpose:    Change a single pixel from component video color space back to 
 *             RGB color space 
 * Parameters: struct YPbPr_pixel *pixel: a pointer to a pixel in component 
 *             video color space
 *             unsigned denominator: an integer representing the maximum color 
 *             value
 * Return:     a struct representing the new pixel in RGB color space
 * Expects:    pixel to not be NULL
 * Notes:      will CRE if pixel is NULL
 *             If the floating point representation of pixels in component 
 *             video color space are out of bounds, they are clamped to their 
 *             assigned range when converting to red, green, and blue.
 */
struct Pnm_rgb pixelToRGB(struct YPbPr_pixel *pixel, unsigned denominator)
{
        assert(pixel != NULL);
        struct Pnm_rgb newPixel;

        float Y = pixel->Y;
        float Pb = pixel->Pb;
        float Pr = pixel->Pr;

        float r = 1 * Y + 0 * Pb + 1.402 * Pr;
        float g = 1 * Y - 0.344136 * Pb - 0.714136 * Pr;
        float b = 1.0 * Y + 1.772 * Pb + 0.0 * Pr;

        r = clamp(r, 0, 1);
        g = clamp(g, 0, 1);
        b = clamp(b, 0, 1);

        newPixel.red = (unsigned) round(r * denominator);
        newPixel.green = (unsigned) round(g * denominator);
        newPixel.blue = (unsigned) round(b * denominator);

        return newPixel;
}

/*
 * Name:       convertRgbApply
 * Purpose:    Transform each pixel from component video color space into RGB 
 *             color space
 * Parameters: int col: an integer representing the distance between the
 *             current element and the left edge of the array
 *             int row: an integer representing the distance between the
 *             current element and the top of the array
 *             A2Methods_UArray2 array2: required by the A2Methods_Object
 *             interface; ignored here
 *             void *elem: a pointer to a Pnm_rgb representing the current
 *             pixel of the image transformed into RGB color space
 *             void *cl: a pointer to a closure containing the original image 
 *             data
 * Return:     None
 * Expects:    elem to not be NULL and cl to not be NULL
 *             col and row are within the bounds of the array
 * Notes:      will CRE if cl is NULL
 *             will CRE if elem is NULL
 *             If col or row are out of bounds or not used by the array,
 *             the methods->at function will throw a CRE
 */
void convertRgbApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                     void *cl)
{
        assert(elem != NULL && cl != NULL);

        struct Closure *closure = cl;
        A2Methods_UArray2 original = closure->array;

        struct YPbPr_pixel *oldPixel = closure->methods->at(original, col, row);
        assert(oldPixel != NULL);

        Pnm_rgb newPixel = elem;

        *newPixel = pixelToRGB(oldPixel, closure->denominator);

        (void) array2;
}

/*
 * Name:       clamp
 * Purpose:    fit a floating-point value within a given range, returning 
 *             either the min or max if it exceeds those limits
 * Parameters: float value: a number that needs to be clamped
 *             float min: the minimum value in the range
 *             float max: the maximum value in the range
 * Return:     a value clamped to the given range
 * Expects:    None
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
