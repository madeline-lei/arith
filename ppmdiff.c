#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "a2methods.h"
#include "a2plain.h"
#include "assert.h"
#include "pnm.h"
#include <math.h>

static FILE *openOrAbort(char *fname, char *mode);

int main(int argc, char *argv[])
{
        assert(argc == 3);

        A2Methods_T methods = uarray2_methods_plain;
        FILE *fp1 = openOrAbort(argv[1], "r");
        FILE *fp2 = openOrAbort(argv[2], "r");

        Pnm_ppm image1 = Pnm_ppmread(fp1, methods);
        Pnm_ppm image2 = Pnm_ppmread(fp2, methods);

        if (abs((int) (image1->height - image2->height)) < 1) {
                fprintf(stderr, "image heights/widths differ by more than 1\n");
                printf("1\n");
                return 0;
        }

        float numerator;
        int h = fmin(image1->height, image2->height);
        int w = fmin(image1->width, image2->width);

        A2Methods_UArray2 pixels1 = image1->pixels;
        A2Methods_UArray2 pixels2 = image2->pixels;

        for (int i = 0; i < w; i++) {
                for (int j = 0; j < h; j++) {
                        Pnm_rgb p1 = methods->at(pixels1, i, j);
                        Pnm_rgb p2 = methods->at(pixels2, i, j);
                        float denom1 = image1->denominator;
                        float denom2 = image2->denominator;

                        numerator +=
                                pow(((p1->red) / denom1) - ((p2->red) / denom2),
                                    2) +
                                pow(((p1->green) / denom1) -
                                            ((p2->green) / denom2),
                                    2) +
                                pow(((p1->blue) / denom1) -
                                            ((p2->blue) / denom2),
                                    2);
                }
        }

        float bottom = 3 * w * h;
        float E = sqrt(numerator / bottom);

        printf("%.4f\n", E);
        return 0;
}

/*
 * Name:       openOrAbort
 * Purpose:    opens a file, or aborts if it cannot be opened.
 * Parameters: char *fname which is the name of the file
 *             char *mode which is the mode that the file should be opened in
 * Return:     FILE fp which is the opened file
 * Expects:    *fname to be a valid filename, and *mode to be a valid mode that
 *             file can be opened in
 * Notes:      Will CRE if the filename cannot be opened
 */
static FILE *openOrAbort(char *fname, char *mode)
{
        FILE *fp = fopen(fname, mode);
        if (fp == NULL) {
                fprintf(stderr, "filename %s cannot be opened\n", fname);
                exit(EXIT_FAILURE);
        }
        return fp;
}
