#include "quantize.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"
#include "assert.h"
#include "2x2pack.h"
#include <stdlib.h>
#include <math.h>
#include "arith40.h"

struct ConvertClosure {
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

        struct ConvertClosure *cl = malloc(sizeof(*cl));
        assert(cl != NULL);
        cl->array = destination;
        cl->methods = methods;

        methods->map_default(original, quantizeApply, cl);

        free(cl);
        return destination;
}

void quantizeApply(int col, int row, A2Methods_UArray2 array2,
                   A2Methods_Object *elem, void *cl)
{
        assert(elem != NULL && cl != NULL);
        struct ConvertClosure *closure = cl;
        struct YPbPr_block *currBlock = elem;

        struct Quantized_Block *quantized =
                closure->methods->at(closure->array, col, row);

        quantized->a = linearQuantizeValue(currBlock->a, 9, 1);
        quantized->b = linearQuantizeValue(clampBCD(currBlock->b), 4, 0.3);
        quantized->c = linearQuantizeValue(clampBCD(currBlock->c), 4, 0.3);
        quantized->d = linearQuantizeValue(clampBCD(currBlock->d), 4, 0.3);
        quantized->avgPb = Arith40_index_of_chroma(currBlock->avgPb);
        quantized->avgPr = Arith40_index_of_chroma(currBlock->avgPr);

        (void) array2;
}

int linearQuantizeValue(float value, int width, float maxFloat)
{
        float scale = ((width << 1) - 1) / maxFloat;
        return round(value * scale);
}

A2Methods_UArray2 dequantizeData(A2Methods_UArray2 original,
                                 const struct A2Methods_T *methods)
{
        assert(original != NULL && methods != NULL);
        A2Methods_UArray2 destination = methods->new(
                methods->width(original), methods->height(original),
                sizeof(struct YPbPr_block));

        struct ConvertClosure *cl = malloc(sizeof(*cl));
        assert(cl != NULL);
        cl->array = destination;
        cl->methods = methods;

        methods->map_default(original, dequantizeApply, cl);

        free(cl);
        return destination;
}

void dequantizeApply(int col, int row, A2Methods_UArray2 array2,
                     A2Methods_Object *elem, void *cl)
{
        assert(elem != NULL && cl != NULL);
        struct ConvertClosure *closure = cl;

        struct Quantized_Block *quantized = elem;
        struct YPbPr_block *currBlock =
                closure->methods->at(closure->array, col, row);

        currBlock->a = linearDequantizeValue(quantized->a, 9, 1);
        currBlock->b = linearDequantizeValue(quantized->b, 4, 0.3);
        currBlock->c = linearDequantizeValue(quantized->c, 4, 0.3);
        currBlock->d = linearDequantizeValue(quantized->d, 4, 0.3);
        currBlock->avgPb = Arith40_chroma_of_index(quantized->avgPb);
        currBlock->avgPr = Arith40_chroma_of_index(quantized->avgPr);

        (void) array2;
}

float linearDequantizeValue(float value, int width, float maxFloat)
{
        float scale = maxFloat / ((width << 1) - 1);
        return value * scale;
}

float clampBCD(float value)
{
        if (value > 0.3) {
                return 0.3;
        } else if (value < -0.3) {
                return -0.3;
        }

        return value;
}
