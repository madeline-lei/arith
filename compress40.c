#include "compress40.h"
#include "handlePPM.h"
#include "convertColor.h"
#include "quantize.h"
#include "packWord.h"
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

        A2Methods_UArray2 quantizedPix =
                quantizeData(blockedPixels, original->methods);

        A2Methods_UArray2 packedPix = packWord(quantizedPix, original->methods);

        A2Methods_UArray2 depackedPix =
                unpackWord(packedPix, original->methods);

        A2Methods_UArray2 dequantizedPix =
                dequantizeData(depackedPix, original->methods);

        A2Methods_UArray2 unblockedPixels =
                unpackBlock(dequantizedPix, original->methods);

        A2Methods_UArray2 decompressed = YPbPrToRGB(
                unblockedPixels, original->denominator, original->methods);

        struct Pnm_ppm pixmap = {
                .width = uarray2_methods_plain->width(unblockedPixels),
                .height = uarray2_methods_plain->height(unblockedPixels),
                .denominator = original->denominator,
                .pixels = decompressed,
                .methods = original->methods
        };

        // Pnm_ppmwrite(stdout, original);
        Pnm_ppmwrite(stdout, &pixmap);

        original->methods->free(&unblockedPixels);
        original->methods->free(&blockedPixels);
        original->methods->free(&YPbPr_pixels);

        Pnm_ppmfree(&original);
        // Pnm_ppmfree(&pixmap);
}

void decompress40(FILE *input)
{
        (void) input;
}