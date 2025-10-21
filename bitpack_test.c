#include "bitpack.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"

int main()
{
        uint64_t word;
        unsigned width;
        unsigned lsb;
        int64_t val;

        // --------------------------
        // Unsigned insertion test
        // --------------------------
        word = 0x3f4;
        width = 6;
        lsb = 2;
        val = 7;

        assert(Bitpack_getu(Bitpack_newu(word, width, lsb, val), width, lsb) ==
               (uint64_t) val);
        printf("Unsigned test passed\n");

        // --------------------------
        // Signed positive test
        // --------------------------
        word = 0x0;
        width = 5;
        lsb = 0;
        val = 10; // fits in 5-bit signed

        assert(Bitpack_gets(Bitpack_news(word, width, lsb, val), width, lsb) ==
               val);
        printf("Signed positive test passed\n");

        // -- -- -- -- -- -- -- -- -- -- -- -- --Signed negative
        //                                             test-- -- -- -- -- -- -- -- -- -- -- -- --
        word = 0x0;
        width = 5;
        lsb = 0;
        val = -3; // fits in 5-bit signed

        assert(Bitpack_gets(Bitpack_news(word, width, lsb, val), width, lsb) ==
               val);
        printf("Signed negative test passed\n");

        // --------------------------
        // Overlapping insertion test
        // --------------------------
        word = 0x12345678;
        width = 4;
        lsb = 8;
        val = 0xA; // unsigned 4-bit

        assert(Bitpack_getu(Bitpack_newu(word, width, lsb, val), width, lsb) ==
               (uint64_t) val);
        printf("Overlapping unsigned test passed\n");

        // --------------------------
        // Full word signed test
        // --------------------------
        word = 0x0;
        width = 32;
        lsb = 0;
        val = -12345678;

        assert(Bitpack_gets(Bitpack_news(word, width, lsb, val), width, lsb) ==
               val);
        printf("Full 32-bit signed test passed\n");

        printf("All tests passed!\n");
        return 0;
}