#include <string.h>
#include <stdlib.h>
#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"
#include "assert.h"
#include "2x2pack.h"

const int BLOCK_SIZE = 2;

struct ConvertClosure {
        A2Methods_UArray2 array;
        const struct A2Methods_T *methods;
};

A2Methods_UArray2 packBlock(A2Methods_UArray2 original,
                            const struct A2Methods_T *methods)
{
        A2Methods_UArray2 destination =
                methods->new(methods->width(original) / BLOCK_SIZE,
                             methods->height(original) / BLOCK_SIZE,
                             sizeof(struct YPbPr_block));

        struct ConvertClosure *cl = malloc(sizeof(*cl));
        assert(cl != NULL);
        cl->array = original;
        cl->methods = methods;

        methods->map_default(destination, packBlockApply, cl);

        free(cl);
        return destination;
}

void packBlockApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                    void *cl)
{
        assert(elem != NULL && cl != NULL);

        struct ConvertClosure *convertcl = cl;
        A2Methods_UArray2 original = convertcl->array;
        const struct A2Methods_T *methods = convertcl->methods;

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

A2Methods_UArray2 unpackBlock(A2Methods_UArray2 original,
                              const struct A2Methods_T *methods)
{
        A2Methods_UArray2 destination =
                methods->new(methods->width(original) * BLOCK_SIZE,
                             methods->height(original) * BLOCK_SIZE,
                             sizeof(struct YPbPr_pixel));

        struct ConvertClosure *cl = malloc(sizeof(*cl));
        assert(cl != NULL);
        cl->array = destination;
        cl->methods = methods;

        methods->map_default(original, unpackBlockApply, cl);

        struct ConvertClosure cl2 = { .array = original, .methods = methods };

        methods->map_default(destination, setChromaApply, &cl2);

        free(cl);
        return destination;
}

void unpackBlockApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                      void *cl)
{
        assert(elem != NULL && cl != NULL);

        struct ConvertClosure *convertcl = cl;
        A2Methods_UArray2 destination = convertcl->array;
        const struct A2Methods_T *methods = convertcl->methods;

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

void DCTtoPixel(float a, float b, float c, float d, float *Y1, float *Y2,
                float *Y3, float *Y4)
{
        *Y1 = a - b - c + d;
        *Y2 = a - b + c - d;
        *Y3 = a + b - c - d;
        *Y4 = a + b + c + d;
}

void setChromaApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                    void *cl)
{
        assert(elem != NULL && cl != NULL);

        struct ConvertClosure *convertcl = cl;
        A2Methods_UArray2 original = convertcl->array;
        const struct A2Methods_T *methods = convertcl->methods;

        struct YPbPr_pixel *currPix = elem;
        struct YPbPr_block *currBlock =
                methods->at(original, col / BLOCK_SIZE, row / BLOCK_SIZE);

        currPix->Pb = currBlock->avgPb;
        currPix->Pr = currBlock->avgPr;

        (void) array2;
}