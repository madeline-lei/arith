#include "bitpack.h"
#include "assert.h"
#include <math.h>

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
        assert(width <= 64);
        assert(width + lsb <= 64);

        uint64_t mask = ~0;
        mask = mask >> (64 - width) << lsb;

        word &= mask;

        return word >> lsb;
}

// look at this with TA
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width + lsb <= 64);
        assert(width <= 64);

        if (width == 0)
                return 0;

        uint64_t shifted = word << (64 - width - lsb);

        int64_t result = ((int64_t) shifted) >> (64 - width);

        return result;
}

uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                      uint64_t value)
{
        assert(width <= 64);
        assert(width + lsb <= 64);
        if (!Bitpack_fitsu(value, width)) {
                RAISE(Bitpack_Overflow);
        }

        uint64_t mask = ~0;
        mask = mask >> (64 - width) << lsb;
        mask = ~mask;

        return ((word & mask) | (value << lsb));
}

//ask ta abt this one
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
                      int64_t value)
{
        assert(width <= 64);
        assert(Bitpack_fitss(value, width));

        uint64_t uvalue = (uint64_t) value & ((1ULL << width) - 1);
        uint64_t mask = ((1ULL << width) - 1) << lsb;

        word &= ~mask;
        word |= (uvalue << lsb) & mask;
        return word;
}