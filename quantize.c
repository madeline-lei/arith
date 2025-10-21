#include "quantize.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"
#include "assert.h"
#include "2x2pack.h"
#include <stdlib.h>
#include <math.h>
#include "arith40.h"

static const unsigned A_WIDTH = 9;
static const unsigned B_WIDTH = 5;
static const unsigned C_WIDTH = 5;
static const unsigned D_WIDTH = 5;

static float clamp(float value, float min, float max);

struct Closure {
        A2Methods_UArray2 array;
        const struct A2Methods_T *methods;
};

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

void quantizeApply(int col, int row, A2Methods_UArray2 array2,
                   A2Methods_Object *elem, void *cl)
{
        assert(elem != NULL && cl != NULL);
        struct Closure *closure = cl;
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

int linearQuantizeValue(float value, int width, float maxFloat)
{
        float scale = (pow(width, 2) - 1) / maxFloat;
        return round(value * scale);
}

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

void dequantizeApply(int col, int row, A2Methods_UArray2 array2,
                     A2Methods_Object *elem, void *cl)
{
        assert(elem != NULL && cl != NULL);
        struct Closure *closure = cl;

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

float linearDequantizeValue(float value, int width, float maxFloat)
{
        float scale = maxFloat / (pow(width, 2) - 1);
        return value * scale;
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
