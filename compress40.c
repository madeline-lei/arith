#include "compress40.h"
#include "handlePPM.h"
#include "convertColor.h"
#include "2x2pack.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"
#include "pnm.h"
#include "assert.h"

void compress40(FILE *input)
{
        assert(input != NULL);
        Pnm_ppm original = readInPPM(input);

        A2Methods_UArray2 YPbPr_pixels = rgbToYPbPr(
                original->pixels, original->denominator, original->methods);

        A2Methods_UArray2 blockedPixels =
                packBlock(YPbPr_pixels, original->methods);

        A2Methods_UArray2 unblockedPixels =
                unpackBlock(blockedPixels, original->methods);

        struct Pnm_ppm pixmap = { .width = original->width,
                                  .height = original->height,
                                  .denominator = original->denominator,
                                  .pixels = unblockedPixels,
                                  .methods = original->methods };

        Pnm_ppmwrite(stdout, &pixmap);

        Pnm_ppmfree(&original);
        // (void) unblockedPixels;
}

void decompress40(FILE *input)
{
        (void) input;
}