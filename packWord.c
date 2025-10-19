#include "packWord.h"
#include "assert.h"
#include "bitpack.h"
#include "quantize.h"
#include <stdlib.h>

struct ConvertClosure {
        A2Methods_UArray2 array;
        const struct A2Methods_T *methods;
};

A2Methods_UArray2 packWord(A2Methods_UArray2 original,
                           const struct A2Methods_T *methods)
{
        A2Methods_UArray2 destination = methods->new(methods->width(original),
                                                     methods->height(original),
                                                     sizeof(uint32_t));

        struct ConvertClosure *cl = malloc(sizeof(*cl));
        assert(cl != NULL);
        cl->array = original;
        cl->methods = methods;

        methods->map_default(destination, packWordApply, cl);

        free(cl);
        return destination;
}

void packWordApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                   void *cl)
{
        assert(elem != NULL && cl != NULL);
        struct ConvertClosure *closure = cl;
        struct Quantized_Block *currBlock =
                closure->methods->at(closure->array, col, row);
        uint32_t *currWord = elem;

        *currWord = bitpackWord(currBlock->a, currBlock->b, currBlock->c,
                                currBlock->d, currBlock->avgPb,
                                currBlock->avgPr);

        (void) array2;
}

uint32_t bitpackWord(unsigned a, int b, int c, int d, unsigned avgPb,
                     unsigned avgPr)
{
        uint32_t word = 0;
        word = Bitpack_newu(word, 9, 23, a);
        word = Bitpack_news(word, 5, 18, b);
        word = Bitpack_news(word, 5, 13, c);
        word = Bitpack_news(word, 5, 8, d);
        word = Bitpack_newu(word, 4, 4, avgPb);
        word = Bitpack_newu(word, 4, 0, avgPr);

        return word;
}

/* ----------------------------------------------------------------------------- */

A2Methods_UArray2 unpackWord(A2Methods_UArray2 original,
                             const struct A2Methods_T *methods)
{
        A2Methods_UArray2 destination = methods->new(
                methods->width(original), methods->height(original),
                sizeof(struct Quantized_Block));

        struct ConvertClosure *cl = malloc(sizeof(*cl));
        assert(cl != NULL);
        cl->array = original;
        cl->methods = methods;

        methods->map_default(destination, unpackWordApply, cl);

        free(cl);
        return destination;
}

void unpackWordApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                     void *cl)
{
        assert(elem != NULL && cl != NULL);
        struct ConvertClosure *closure = cl;
        struct Quantized_Block *currBlock = elem;
        uint32_t *word = closure->methods->at(closure->array, col, row);

        unbitpackWord(*word, &(currBlock->a), &(currBlock->b), &(currBlock->c),
                      &(currBlock->d), &(currBlock->avgPb),
                      &(currBlock->avgPr));

        (void) array2;
}

void unbitpackWord(uint32_t word, unsigned *a, int *b, int *c, int *d,
                   unsigned *avgPb, unsigned *avgPr)
{
        *a = Bitpack_getu(word, 9, 23);
        *b = Bitpack_gets(word, 5, 18);
        *c = Bitpack_gets(word, 5, 13);
        *d = Bitpack_gets(word, 5, 8);
        *avgPb = Bitpack_getu(word, 4, 4);
        *avgPr = Bitpack_getu(word, 4, 0);
}