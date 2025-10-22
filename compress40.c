/**************************************************************
 *                     compress40.c
 *
 *     Assignment: arith
 *     Authors:  Diana Calderon and Madeline Lei
 *     Usernames: dcalde02, mlei03
 *     Date:     10/21/2025
 *
 *     summary:
 *
 *     This file contains the implementations for the functions
 *     compress40() and decompress40(), which deal with compressing
 *     and decompressing an image. 
 *
 **************************************************************/
#include "compress40.h"
#include "handleImage.h"
#include "convertColor.h"
#include "quantize.h"
#include "packWord.h"
#include "2x2pack.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"
#include "pnm.h"
#include "assert.h"

/************************ compress40 ******************************
 *
 * Compresses an image stored as a PPM and prints it out to stdout in
 * big-endian order.
 *
 * Parameters:
 *         FILE *input: a pointer to an open file stream beginning at the
 *         start of a valid PPM readable by Pnm_ppmread. Represents an
 *         image to be compressed.
 *
 * Return: None
 *
 * Expects
 *         input to not be NULL.
 *         the file stored in input is a valid PPM with nonzero dimensions
 * Notes:
 *         Prints the compressed PPM to stdout in big-endian order
 *         Frees memory allocated for a A2Methods_UArray2 allocated in
 *         rgbToYPbPr(), packBlock(), quantizeData(), and packWord().
 *         Frees memory allocated for a PPM allocated in readInPPM()
 *         Will raise a CRE if input is NULL.
 *
 ************************************************************/
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

/************************ decompress40 ******************************
 *
 * Decompresses an image and prints it out as a PPM to stdout
 *
 * Parameters:
 *         FILE *input: a pointer to an open file stream beginning at the
 *         start of a compressed PPM in big-endian order.
 *
 * Return: None
 *
 * Expects
 *         input to not be NULL.
 *         the file stored in input is a valid compressed PPM in big-endian
 *         order
 * Notes:
 *         Prints the decompressed PPM to stdout
 *         Frees memory allocated for a A2Methods_UArray2 allocated in
 *         readInCompressed(), unpackWord(), dequantizedPix(), unblockedPixels(),
 *         and decompressedImage().
 *         Will raise a CRE if input is NULL.
 *
 ************************************************************/
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