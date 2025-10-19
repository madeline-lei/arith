#include "bitpack.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"

int main()
{
        uint64_t word = 0x00; // start with empty word

        // Test 1: Insert small positive number
        word = Bitpack_news(word, 3, 0, 3); // 3 fits in 3 bits
        int64_t got = Bitpack_gets(word, 3, 0);
        printf("Test 1: expected 3, got %ld\n", got);
        assert(got == 3);

        // Test 2: Insert negative number
        word = Bitpack_news(word, 3, 3, -2); // -2 in 3 bits
        got = Bitpack_gets(word, 3, 3);
        printf("Test 2: expected -2, got %ld\n", got);
        assert(got == -2);

        // Test 3: Insert maximum positive number
        word = Bitpack_news(word, 4, 6, 7); // 7 fits in 4-bit signed
        got = Bitpack_gets(word, 4, 6);
        printf("Test 3: expected 7, got %ld\n", got);
        assert(got == 7);

        // Test 4: Insert maximum negative number
        word = Bitpack_news(word, 4, 10, -8); // -8 fits in 4-bit signed
        got = Bitpack_gets(word, 4, 10);
        printf("Test 4: expected -8, got %ld\n", got);
        assert(got == -8);

        // Test 5: Overwriting previous bits
        word = Bitpack_news(word, 3, 0, -4); // overwrite first 3 bits
        got = Bitpack_gets(word, 3, 0);
        printf("Test 5: expected -4, got %ld\n", got);
        assert(got == -4);

        printf("All signed packing tests passed!\n");
        return 0;
}