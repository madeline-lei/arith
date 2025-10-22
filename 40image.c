/**************************************************************
 *                     40image.c
 *
 *     Assignment: arith
 *     Authors:  Diana Calderon and Madeline Lei
 *     Usernames: dcalde02, mlei03
 *     Date:     10/21/2025
 *
 *     summary:
 *
 *     This file contains the main driver function for compressing
 *     and decompressing an image represented as a PPM. It handles
 *     reading user input from the terminal.
 *
 **************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "compress40.h"

static void (*compress_or_decompress)(FILE *input) = compress40;

/************************ main ******************************
 *
 * Reads in user input from the terminal and compresses/decompresses
 * an image based on it
 *
 * Parameters:
 *         int argc representing the number of arguments on the command line
 *         char *argv[] representing the arguments on the command line.
 *
 * Return: EXIT_SUCCESS if nothing went wrong, EXIT_FAILURE there is an
 *         invalid argument or too many arguments
 *
 * Expects
 *         argc not to be greater than 3. Any flags provided are valid
 *         (-c or -d).
 * Notes:
 *         May open and close a file provided, may read from stdin
 *
 ************************************************************/
int main(int argc, char *argv[])
{
        int i;

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-c") == 0) {
                        compress_or_decompress = compress40;
                } else if (strcmp(argv[i], "-d") == 0) {
                        compress_or_decompress = decompress40;
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        exit(1);
                } else if (argc - i > 2) {
                        fprintf(stderr,
                                "Usage: %s -d [filename]\n"
                                "       %s -c [filename]\n",
                                argv[0], argv[0]);
                        exit(1);
                } else {
                        break;
                }
        }
        assert(argc - i <= 1); /* at most one file on command line */
        if (i < argc) {
                FILE *fp = fopen(argv[i], "r");
                assert(fp != NULL);
                compress_or_decompress(fp);
                fclose(fp);
        } else {
                compress_or_decompress(stdin);
        }

        return EXIT_SUCCESS;
}
