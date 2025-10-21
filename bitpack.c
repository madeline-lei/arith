#include "bitpack.h"
#include "assert.h"
#include <math.h>
#include <stdio.h>

const unsigned INT_SIZE = 64;

Except_T Bitpack_Overflow = { "Overflow packing bits" };

bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        return n <= (pow(2, width) - 1);
}

bool Bitpack_fitss(int64_t n, unsigned width)
{
        return n <= (pow(2, width - 1) - 1);
}

uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= INT_SIZE);
        assert(width + lsb <= INT_SIZE);

        uint64_t mask = ~0;
        mask = mask >> (INT_SIZE - width) << lsb;

        word &= mask;

        return word >> lsb;
}

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width + lsb <= INT_SIZE);
        assert(width <= INT_SIZE);

        uint64_t mask = ~0;
        mask = mask << (width - 1);

        uint64_t field = Bitpack_getu(word, width, lsb);

        if ((field & mask) == 0) {
                return field;
        }

        int64_t signed_field = field | mask;

        return signed_field;
}

uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                      uint64_t value)
{
        assert(width <= INT_SIZE);
        assert(width + lsb <= INT_SIZE);
        if (!Bitpack_fitsu(value, width)) {
                RAISE(Bitpack_Overflow);
        }

        uint64_t mask = ~0;
        mask = mask >> (INT_SIZE - width) << lsb;
        mask = ~mask;

        return ((word & mask) | (value << lsb));
}

uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
                      int64_t value)
{
        assert(width <= INT_SIZE);
        assert(width + lsb <= INT_SIZE);
        if (!Bitpack_fitss(value, width)) {
                RAISE(Bitpack_Overflow);
        }

        uint64_t mask = ~0;
        mask = mask >> (INT_SIZE - width);

        uint64_t unsigned_value = mask & value;

        return Bitpack_newu(word, width, lsb, unsigned_value);
}