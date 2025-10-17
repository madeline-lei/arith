#include "convertColor.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"
#include "assert.h"
#include <stdlib.h>

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
        A2Methods_T methods;
};

// UArray2b_T 
A2Methods_UArray2 rgbToYPbPr(A2Methods_UArray2 original, unsigned denominator,
                      const A2Methods_T methods)
{
        // UArray2b_T destination = UArray2b_new(methods->width(original),
        //                                       methods->height(original),
        //                                       sizeof(struct YPbPr_pixel), 2);

        A2Methods_UArray2 destination = methods->new_with_blocksize(
                                                methods->width(original),
                                                methods->height(original),
                                                sizeof(struct YPbPr_pixel), 2);

        struct ConvertClosure *cl = malloc(sizeof(*cl));
        assert(cl != NULL);
        cl->original = original;
        cl->denominator = denominator;
        cl->methods = methods;

        methods->map_block_major(destination, convertYbPbPrApply, cl);
        // UArray2b_map(destination, convertYbPbPrApply, cl);
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

        newPixel.Y = 0.299 * r + 0.587 * g + 0.114 * b;
        newPixel.Pb = -0.168736 * r - 0.331264 * g + 0.5 * b;
        newPixel.Pr = 0.5 * r - 0.418688 * g - 0.081312 * b;

        return newPixel;
}

// void convertYbPbPrApply(int col, int row, UArray2b_T array2b, void *elem,
//                         void *cl)
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
                             const A2Methods_T methods)
{
        // A2Methods_UArray2 destination = methods->new(methods->width(original),
        //                                              methods->height(original),
        //                                              sizeof(struct Pnm_rgb));

        A2Methods_UArray2 destination = methods->new_with_blocksize(
                                                methods->width(original),
                                                methods->height(original),
                                                sizeof(struct Pnm_rgb), 2);

        struct ConvertClosure *cl = malloc(sizeof(*cl));
        assert(cl != NULL);
        cl->original = original;
        cl->denominator = denominator;
        cl->methods = methods;

        methods->map_block_major(destination, convertRgbApply, cl);
        // methods->map_default(destination, convertRgbApply, cl);

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
        //a real number between 0 and 1
        // if (r < 0) {
        //         r = 0;
        // }
        // if (r > 1) {
        //         r = 1;
        // }
        // if (g < 0) {
        //         g = 0;
        // }
        // if (g > 1) {
        //         g = 1;
        // }
        // if (b < 0) {
        //         b = 0;
        // }
        // if (b > 1) {
        //         b = 1;
        // }

        newPixel.red = (unsigned) (r * denominator + 0.5);
        newPixel.green = (unsigned) (g * denominator + 0.5);
        newPixel.blue = (unsigned) (b * denominator + 0.5);

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

void averages(struct YPbPr_pixel *pixel) {
        float PbAvg = (p1->Pb + p2->Pb + p3->Pb + p4->Pb) / 4.0;
        float PrAvg = (p1->Pr + p2->Pr + p3->Pr + p4->Pr) / 4.0;

        AvgPb = Arith40_index_of_chroma(float PbAvg);
        AvgPr = Arith40_index_of_chroma(float PrAvg);
}
void pixelToDCT(float Y1, float Y2, float Y3, float Y4, float *a, float *b, 
                float *c, float *d) {
        assert(a && b && c && d);
        *a = (Y4 + Y3 + Y2 + Y1) / 4.0;
        *b = (Y4 + Y3 - Y2 - Y1) / 4.0;
        *c = (Y4 - Y3 + Y2 - Y1) / 4.0;
        *d = (Y4 - Y3 - Y2 + Y1) / 4.0;
}

void DCTtoPixel(float a, float b, float c, float d,
                float *Y1, float *Y2, float *Y3, float *Y4) {
        *Y1 = a - b - c + d;
        *Y2 = a - b + c - d;
        *Y3 = a + b - c - d;
        *Y4 = a + b + c + d;
}