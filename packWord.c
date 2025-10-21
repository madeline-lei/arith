#include "packWord.h"
#include "assert.h"
#include "bitpack.h"
#include "quantize.h"
#include <stdlib.h>
#include <stdio.h>

static const unsigned A_WIDTH = 9;
static const unsigned B_WIDTH = 5;
static const unsigned C_WIDTH = 5;
static const unsigned D_WIDTH = 5;
static const unsigned CHORMA_WIDTH = 4;

struct Closure {
        A2Methods_UArray2 array;
        const struct A2Methods_T *methods;
};

A2Methods_UArray2 packWord(A2Methods_UArray2 original,
                           const struct A2Methods_T *methods)
{
        A2Methods_UArray2 destination = methods->new(methods->width(original),
                                                     methods->height(original),
                                                     sizeof(uint32_t));

        struct Closure cl = { .array = original, .methods = methods };

        methods->map_default(destination, packWordApply, &cl);

        return destination;
}

void packWordApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                   void *cl)
{
        assert(elem != NULL && cl != NULL);
        struct Closure *closure = cl;
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

        unsigned a_lsb = 32 - A_WIDTH;
        unsigned b_lsb = a_lsb - B_WIDTH;
        unsigned c_lsb = b_lsb - C_WIDTH;
        unsigned d_lsb = c_lsb - D_WIDTH;
        unsigned avgPb_lsb = d_lsb - CHORMA_WIDTH;
        unsigned avgPr_lsb = avgPb_lsb - CHORMA_WIDTH;

        word = Bitpack_newu(word, A_WIDTH, a_lsb, a);
        word = Bitpack_news(word, B_WIDTH, b_lsb, b);
        word = Bitpack_news(word, C_WIDTH, c_lsb, c);
        word = Bitpack_news(word, D_WIDTH, d_lsb, d);
        word = Bitpack_newu(word, CHORMA_WIDTH, avgPb_lsb, avgPb);
        word = Bitpack_newu(word, CHORMA_WIDTH, avgPr_lsb, avgPr);

        return word;
}

/* ----------------------------------------------------------------------------- */

A2Methods_UArray2 unpackWord(A2Methods_UArray2 original,
                             const struct A2Methods_T *methods)
{
        A2Methods_UArray2 destination = methods->new(
                methods->width(original), methods->height(original),
                sizeof(struct Quantized_Block));

        struct Closure cl = { .array = original, .methods = methods };

        methods->map_default(destination, unpackWordApply, &cl);

        return destination;
}

void unpackWordApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                     void *cl)
{
        assert(elem != NULL && cl != NULL);
        struct Closure *closure = cl;
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
        unsigned a_lsb = 32 - A_WIDTH;
        unsigned b_lsb = a_lsb - B_WIDTH;
        unsigned c_lsb = b_lsb - C_WIDTH;
        unsigned d_lsb = c_lsb - D_WIDTH;
        unsigned avgPb_lsb = d_lsb - CHORMA_WIDTH;
        unsigned avgPr_lsb = avgPb_lsb - CHORMA_WIDTH;

        *a = Bitpack_getu(word, A_WIDTH, a_lsb);
        *b = Bitpack_gets(word, B_WIDTH, b_lsb);
        *c = Bitpack_gets(word, C_WIDTH, c_lsb);
        *d = Bitpack_gets(word, D_WIDTH, d_lsb);
        *avgPb = Bitpack_getu(word, CHORMA_WIDTH, avgPb_lsb);
        *avgPr = Bitpack_getu(word, CHORMA_WIDTH, avgPr_lsb);
}