#include "handlePPM.h"
#include "a2plain.h"
#include "a2methods.h"
#include "assert.h"
#include <stdio.h>
#include <stdlib.h>
#include "bitpack.h"

const int BYTES_PER_WORD = 4;

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

Pnm_ppm trimImage(Pnm_ppm original)
{
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

void printCompressedImage(A2Methods_UArray2 image,
                          const struct A2Methods_T *methods)
{
        printf("COMP40 Compressed image format 2\n%u %u\n",
               methods->width(image) * 2, methods->height(image) * 2);
        methods->map_row_major(image, printWordApply, NULL);
}

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