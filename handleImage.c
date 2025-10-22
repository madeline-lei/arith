/**************************************************************
 *                     handleImage.c
 *
 *     Assignment: arith
 *     Authors:  Diana Calderon and Madeline Lei
 *     Usernames: dcalde02, mlei03
 *     Date:     10/21/2025
 *
 *     summary:
 *
 *     This file contains the implementations for functions declared
 *     in handleImage.h. These image will deal with reading in images stored as
 *     compressed and decompressed PPMs and printing them to stdout.
 *
 **************************************************************/
#include "handleImage.h"
#include "a2plain.h"
#include "a2methods.h"
#include "assert.h"
#include <stdio.h>
#include <stdlib.h>
#include "bitpack.h"

const int BYTES_PER_WORD = 4;

/*
 * Name:       readInPPM
 * Purpose:    Reads in a PPM and trims it to have an even width and height.
 * Parameters: FILE *input: A pointer to an open file stream beginning at the
 *             start of a valid PPM readable by Pnm_ppmread. Represents the
 *             PPM to be read.
 * Return:     a pointer to a Pnm_ppm struct containing the image data stored
 *             in the given file
 * Expects:    input to not be NULL
 * Notes:      will CRE if input is NULL, or if the read PPM is NULL
 *             Pnm_ppmread will raise an exception if the PPM is not able
 *             to be read
 *             Allocates memory for two Pnm_ppm structs (original, destination)
 *             Frees the memory for original, but the caller is responsible
 *             for freeing the memory allocated for destination with 
 *             Pnm_ppmfree()
 */
Pnm_ppm readInPPM(FILE *input)
{
        assert(input != NULL);
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods != NULL);
        Pnm_ppm original = Pnm_ppmread(input, methods);
        assert(original != NULL);

        Pnm_ppm destination = trimImage(original);

        Pnm_ppmfree(&original);

        return destination;
}

/*
 * Name:       trimImage
 * Purpose:    Trims the last row/column of a given PPM so it has an even
 *             width and height
 * Parameters: Pnm_ppm original: a pointer Pnm_ppm struct representing
 *             an image stored as a PPM
 * Return:     a pointer to a Pnm_ppm struct containing trimmed image data
 * Expects:    original to not be NULL
 * Notes:      will CRE if original is NULL
 *             will CRE if trimmed_image fails to malloc or methods->new() 
 *             fails.
 *             Allocates memory for a Pnm_ppm struct that the caller is
 *             responsible for freeing using Pnm_ppmfree().
 */
Pnm_ppm trimImage(Pnm_ppm original)
{
        assert(original != NULL);
        int trimmed_height = original->height - (original->height % 2);
        int trimmed_width = original->width - (original->width % 2);

        Pnm_ppm trimmed_image = malloc(sizeof(*trimmed_image));
        assert(trimmed_image != NULL);

        trimmed_image->height = trimmed_height;
        trimmed_image->width = trimmed_width;
        trimmed_image->denominator = original->denominator;
        trimmed_image->methods = original->methods;

        trimmed_image->pixels = trimmed_image->methods->new(
                trimmed_width, trimmed_height, sizeof(struct Pnm_rgb));

        for (int row = 0; row < trimmed_height; row++) {
                for (int col = 0; col < trimmed_width; col++) {
                        Pnm_rgb new_pixel = trimmed_image->methods->at(
                                trimmed_image->pixels, col, row);
                        Pnm_rgb old_pixel = original->methods->at(
                                original->pixels, col, row);

                        *new_pixel = *old_pixel;
                }
        }

        return trimmed_image;
}

/*
 * Name:       printCompressedImage
 * Purpose:    prints out a compressed image to stdout in big-endian order
 * Parameters: A2Methods_UArray2 image: a A2Methods_UArray2 storing 32-bit
 *             words representing compressed pixels of an image
 *             const struct A2Methods_T *methods: A pointer to a A2Methods_T 
 *             struct that contains pointers to functions on can use on a 
 *             UArray2
 * Return:     None
 * Expects:    None
 * Notes:      Prints out a compressed image to stdout
 */
void printCompressedImage(A2Methods_UArray2 image,
                          const struct A2Methods_T *methods)
{
        printf("COMP40 Compressed image format 2\n%u %u\n",
               methods->width(image) * 2, methods->height(image) * 2);
        methods->map_row_major(image, printWordApply, NULL);
}

/*
 * Name:       printWordApply
 * Purpose:    Prints out a single 32-bit code word to stdout in big-endian 
 *             order
 * Parameters: int col: required by the A2Methods_Object interface; ignored 
 *             here
 *             int row: required by the A2Methods_Object interface; ignored 
 *             here
 *             A2Methods_UArray2 array2: required by the A2Methods_Object
 *             interface; ignored here
 *             A2Methods_Object *elem: a 32-bit integer representing a 32-bit
 *             code word to be printed to stdout
 *             void *cl: required by the A2Methods_Object interface; ignored 
 *             here
 * Returns:    None
 * Expects:    elem to not be NULL and stores a 32-bit integer
 * Notes:      will CRE if elem is NULL
 */
void printWordApply(int col, int row, A2Methods_UArray2 array2,
                    A2Methods_Object *elem, void *cl)
{
        assert(elem != NULL);
        uint32_t *word = elem;
        putchar(Bitpack_getu(*word, 8, 24));
        putchar(Bitpack_getu(*word, 8, 16));
        putchar(Bitpack_getu(*word, 8, 8));
        putchar(Bitpack_getu(*word, 8, 0));

        (void) col;
        (void) row;
        (void) array2;
        (void) cl;
}

/*
 * Name:       readInCompressed
 * Purpose:    reads in a compressed image with code words stored in
 *             big-endian order
 * Parameters: FILE *input: A pointer to an open file stream beginning at the
 *             start of a compressed image.
 *             const struct A2Methods_T *methods: A pointer to a A2Methods_T 
 *             struct that contains pointers to functions on can use on a 
 *             UArray2
 * Return:     a pointer to a A2Methods_UArray2 struct storing 32-bit codewords
 * Expects:    input to not be NULL and the file to contain enough codewords
 *             for the given width and height
 * Notes:      will CRE if input is NULL or if methods-new() fails
 */
A2Methods_UArray2 readInCompressed(FILE *input,
                                   const struct A2Methods_T *methods)
{
        assert(input != NULL);
        unsigned height, width;
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u",
                          &width, &height);
        assert(read == 2);
        int c = getc(input);
        assert(c == '\n');

        unsigned wordsHeight = height / 2;
        unsigned wordsWidth = width / 2;
        A2Methods_UArray2 words =
                methods->new(wordsWidth, wordsHeight, sizeof(uint32_t));

        for (unsigned row = 0; row < wordsHeight; row++) {
                for (unsigned col = 0; col < wordsWidth; col++) {
                        uint32_t word = 0;
                        for (int byte = BYTES_PER_WORD - 1; byte >= 0; byte--) {
                                c = getc(input);
                                assert(c != EOF);
                                word = Bitpack_newu(word, 8, byte * 8, c);
                        }

                        uint32_t *currWordElement =
                                methods->at(words, col, row);

                        *currWordElement = word;
                }
        }

        return words;
}