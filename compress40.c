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

        printCompressedImage(packedPix, original->methods);

        original->methods->free(&YPbPr_pixels);
        original->methods->free(&blockedPixels);
        original->methods->free(&quantizedPix);
        original->methods->free(&packedPix);

        Pnm_ppmfree(&original);
}

void decompress40(FILE *input)
{
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods != NULL);

        A2Methods_UArray2 compressedImage = readInCompressed(input, methods);

        A2Methods_UArray2 depackedPix = unpackWord(compressedImage, methods);

        A2Methods_UArray2 dequantizedPix = dequantizeData(depackedPix, methods);

        A2Methods_UArray2 unblockedPixels =
                unpackBlock(dequantizedPix, methods);

        A2Methods_UArray2 decompressedImage =
                YPbPrToRGB(unblockedPixels, 255, methods);

        struct Pnm_ppm pixmap = { .width = methods->width(decompressedImage),
                                  .height = methods->height(decompressedImage),
                                  .denominator = 255,
                                  .pixels = decompressedImage,
                                  .methods = methods };

        Pnm_ppmwrite(stdout, &pixmap);

        methods->free(&compressedImage);
        methods->free(&depackedPix);
        methods->free(&dequantizedPix);
        methods->free(&unblockedPixels);
        methods->free(&decompressedImage);
}