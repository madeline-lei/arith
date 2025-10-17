#include "compress40.h"
#include "handlePPM.h"
#include "convertColor.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"
#include "pnm.h"
#include "assert.h"

void compress40(FILE *input)
{
        assert(input != NULL);
        Pnm_ppm original = readInPPM(input);

        UArray2b_T newArray = rgbToYPbPr(original->pixels,
                                         original->denominator,
                                         (const A2Methods_T) original->methods);

        A2Methods_UArray2 newArray2 = YPbPrToRGB(
                newArray, original->denominator, uarray2_methods_blocked);

        struct Pnm_ppm pixmap2 = { .width = original->width,
                                   .height = original->height,
                                   .denominator = original->denominator,
                                   .pixels = newArray2,
                                   .methods = uarray2_methods_blocked };

        Pnm_ppmwrite(stdout, &pixmap2);

        Pnm_ppmfree(&original);
}

void decompress40(FILE *input)
{
        (void) input;
}