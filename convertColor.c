#include "convertColor.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"
#include "assert.h"
#include <stdlib.h>
#include <math.h>

// void print(int col, int row, UArray2b_T array2b, void *elem, void *cl);

// void print(int col, int row, UArray2b_T array2b, void *elem, void *cl)
// {
//         struct YPbPr_pixel *pixel = elem;
//         printf("Y: %f\n", pixel->Y);
//         (void) col;
//         (void) row;
//         (void) array2b;
//         (void) cl;
// }

struct ConvertClosure {
        A2Methods_UArray2 original;
        unsigned denominator;
        const struct A2Methods_T *methods;
};

A2Methods_UArray2 rgbToYPbPr(A2Methods_UArray2 original, unsigned denominator,
                             const struct A2Methods_T *methods)
{
        assert(original != NULL && methods != NULL);
        A2Methods_UArray2 destination = methods->new(
                methods->width(original), methods->height(original),
                sizeof(struct YPbPr_pixel));

        struct ConvertClosure *cl = malloc(sizeof(*cl));
        assert(cl != NULL);
        cl->original = original;
        cl->denominator = denominator;
        cl->methods = methods;

        methods->map_default(destination, convertYbPbPrApply, cl);
        free(cl);

        return destination;
}

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

void convertYbPbPrApply(int col, int row, A2Methods_UArray2 array2,
                        A2Methods_Object *elem, void *cl)
{
        assert(elem != NULL && cl != NULL);

        struct ConvertClosure *convertcl = cl;
        A2Methods_UArray2 original = convertcl->original;

        Pnm_rgb currPixel = convertcl->methods->at(original, col, row);
        assert(currPixel != NULL);

        struct YPbPr_pixel *newPixel = elem;

        *newPixel = pixelToYPbPr(currPixel, convertcl->denominator);

        (void) array2;
}

/* -------------------------------------------------------------------------------------*/

A2Methods_UArray2 YPbPrToRGB(A2Methods_UArray2 original, unsigned denominator,
                             const struct A2Methods_T *methods)
{
        assert(original != NULL && methods != NULL);

        A2Methods_UArray2 destination = methods->new(methods->width(original),
                                                     methods->height(original),
                                                     sizeof(struct Pnm_rgb));

        struct ConvertClosure *cl = malloc(sizeof(*cl));
        assert(cl != NULL);
        cl->original = original;
        cl->denominator = denominator;
        cl->methods = methods;

        methods->map_default(destination, convertRgbApply, cl);
        free(cl);

        return destination;
}

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

void convertRgbApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                     void *cl)
{
        assert(elem != NULL && cl != NULL);

        struct ConvertClosure *convertcl = cl;
        A2Methods_UArray2 original = convertcl->original;

        struct YPbPr_pixel *oldPixel =
                convertcl->methods->at(original, col, row);
        assert(oldPixel != NULL);

        Pnm_rgb newPixel = elem;

        *newPixel = pixelToRGB(oldPixel, convertcl->denominator);

        (void) array2;
}

/* -------------------------------------------------------------------------------------*/

float clamp(float value, float min, float max)
{
        if (value < min) {
                return min;
        }
        if (value > max) {
                return max;
        }

        return value;
}
