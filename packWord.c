/**************************************************************
 *                     packword.c
 *
 *     Assignment: arith
 *     Authors:  Diana Calderon and Madeline Lei
 *     Usernames: dcalde02, mlei03
 *     Date:     10/21/2025
 *
 *     summary:
 *
 *     This file contains the packWord() and unpackWord() functions,
 *     which pack the fields of a quantized 2x2 block into a 32-bit 
 *     word (for compression) and unpack that word back into 
 *     a Quantized_block (for decompression).
 * 
 **************************************************************/
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

/************************ packWord ******************************
 *
 * Packs the fields of a quantized 2x2 block into a 32-bit words
 *
 * Parameters:
 *        A2Methods_UArray2 original: a pointer to a UArray2 storing 
 *        Quantized_Block structs, which represents 2x2 blocks of pixels
 *        const struct A2Methods_T *methods: A pointer to a A2Methods_T struct
 *        that contains pointers to functions on can use on a UArray2
 *
 * Return: a pointer to a newly allocated A2Methods_UArray2 struct storing
 *         32-bit words that represent the compressed image data
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
A2Methods_UArray2 packWord(A2Methods_UArray2 original,
                           const struct A2Methods_T *methods)
{
        assert(original != NULL && methods != NULL);

        A2Methods_UArray2 destination = methods->new(methods->width(original),
                                                     methods->height(original),
                                                     sizeof(uint32_t));

        struct Closure cl = { .array = original, .methods = methods };

        methods->map_default(destination, packWordApply, &cl);

        return destination;
}

/*
 * Name:       packWordApply
 * Purpose:    Converts each Quantized_Block into a 32-bit packed word
 *             by bit-packing the quantized coefficients and chromas
 * Parameters: int col: an integer representing the distance between the
 *             current element and the left edge of the array
 *             int row: an integer representing the distance between the
 *             current element and the top of the array
 *             A2Methods_UArray2 array2: required by the A2Methods_Object
 *             interface; ignored here
 *             A2Methods_Object *elem: a pointer to a 32-bit word where packed
 *             data will be stored
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
void packWordApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                   void *cl)
{
        assert(elem != NULL && cl != NULL);
        struct Closure *closure = cl;
        assert(closure != NULL && closure->methods != NULL &&
               closure->array != NULL);
        struct Quantized_Block *currBlock =
                closure->methods->at(closure->array, col, row);
        uint32_t *currWord = elem;

        *currWord = bitpackWord(currBlock->a, currBlock->b, currBlock->c,
                                currBlock->d, currBlock->avgPb,
                                currBlock->avgPr);

        (void) array2;
}

/*
 * Name:       bitpackWord
 * Purpose:    Packs the quantized DCT coefficients and quantized average
 *             chroma values into a 32-bit word
 * Parameters: unsigned a: 9-bit unsigned value representing a quantized DCT
 *             coefficient
 *             int b: 5-bit signed value representing a quantized DCT
 *             coefficient
 *             int c: 5-bit signed value representing a quantized DCT
 *             coefficient
 *             int d: 5-bit signed value representing a quantized DCT
 *             coefficient
 *             unsigned avgPb: 4-bit unsigned chroma index representing a
 *             quantized average chroma value
 *             unsigned avgPr: 4-bit unsigned chroma index representing a
 *             quantized average chroma value
 * Return:     a 32-bit unsigned integer containing the packed data
 * Expects:    None
 * Notes:      None
 */
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

/************************ unpackWord ******************************
 *
 * Unpacks 32-bit words into quantized 2x2 blocks
 *
 * Parameters:
 *        A2Methods_UArray2 original: a pointer to a UArray2 storing 
 *        32-bit codewords representing the compressed image
 *        const struct A2Methods_T *methods: A pointer to a A2Methods_T struct
 *        that contains pointers to functions on can use on a UArray2
 *
 * Return: a pointer to a newly allocated A2Methods_UArray2 struct storing
 *         Quantized_Block structs made from the packed data
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
A2Methods_UArray2 unpackWord(A2Methods_UArray2 original,
                             const struct A2Methods_T *methods)
{
        assert(original != NULL && methods != NULL);

        A2Methods_UArray2 destination = methods->new(
                methods->width(original), methods->height(original),
                sizeof(struct Quantized_Block));

        struct Closure cl = { .array = original, .methods = methods };

        methods->map_default(destination, unpackWordApply, &cl);

        return destination;
}

/*
 * Name:       unpackWordApply
 * Purpose:    Converts 32-bit packed word back into its component
 *             fields, storing into a Quantized_Block.
 * Parameters: int col: an integer representing the distance between the
 *             current element and the left edge of the array
 *             int row: an integer representing the distance between the
 *             current element and the top of the array
 *             A2Methods_UArray2 array2: required by the A2Methods_Object
 *             interface; ignored here
 *             A2Methods_Object *elem: a pointer to Quantized_Block struct
 *             where the unpacked data will be stored
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
void unpackWordApply(int col, int row, A2Methods_UArray2 array2, void *elem,
                     void *cl)
{
        assert(elem != NULL && cl != NULL);
        struct Closure *closure = cl;

        assert(closure != NULL && closure->methods != NULL &&
               closure->array != NULL);
        struct Quantized_Block *currBlock = elem;
        uint32_t *word = closure->methods->at(closure->array, col, row);

        unbitpackWord(*word, &(currBlock->a), &(currBlock->b), &(currBlock->c),
                      &(currBlock->d), &(currBlock->avgPb),
                      &(currBlock->avgPr));

        (void) array2;
}

/*
 * Name:       unbitpackWord
 * Purpose:    Converts the packed fields from a 32-bit word into their
 *             quantized DCT coefficient and average chroma fields.
 * Parameters: uint32_t word: 32-bit unsigned word containing packed data
 *             unsigned *a: pointer to 9-bit unsigned a value representing 
 *             a quantized DCT coefficient
 *             int *b: pointer to 5-bit signed b value representing 
 *             a quantized DCT coefficient
 *             int *c: pointer to 5-bit signed c value representing 
 *             a quantized DCT coefficient
 *             int *d: pointer to 5-bit signed d value representing 
 *             a quantized DCT coefficient
 *             unsigned *avgPb: pointer to 4-bit unsigned avgPb index
 *             representing a quantized average chroma value
 *             unsigned *avgPr: pointer to 4-bit unsigned avgPr index
 *             representing a quantized average chroma value
 * Return:     None
 * Expects:    a, b, c, d to not be NULL
 *             avgPb and avgPr to not be NULL
 * Notes:      will CRE if a, b, c or d is NULL
 *             will CRE if avgPb or avgPr is NULL         
 */
void unbitpackWord(uint32_t word, unsigned *a, int *b, int *c, int *d,
                   unsigned *avgPb, unsigned *avgPr)
{
        assert(a != NULL && b != NULL && c != NULL && d != NULL);
        assert(avgPb != NULL && avgPr != NULL);

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