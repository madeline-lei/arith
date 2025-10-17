#include "handlePPM.h"
#include "a2plain.h"
#include "a2methods.h"
#include "assert.h"
#include <stdlib.h>

Pnm_ppm readInPPM(FILE *input)
{
        assert(input != NULL);
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods != NULL);
        Pnm_ppm original = Pnm_ppmread(input, methods);
        assert(original != NULL);

        Pnm_ppm destination = trimImage(original);

        // printf("original width: %d, height: %d\n", original->width,
        //        original->height);
        // printf("new width: %d, height: %d\n", destination->width,
        //        destination->height);

        // Pnm_ppmwrite(stdout, destination);

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