/**************************************************************
 *                     bitpack.c
 *
 *     Assignment: arith
 *     Authors:  Diana Calderon and Madeline Lei
 *     Usernames: dcalde02, mlei03
 *     Date:     10/21/2025
 *
 *     summary:
 *
 *     This file contains the implementation for the functions
 *     declared in bitpack.h. These functions deal with packing
 *     fields into a 64-bit word and getting those fields from
 *     those words.
 *
 **************************************************************/
#include "bitpack.h"
#include "assert.h"
#include <math.h>
#include <stdio.h>

const unsigned INT_SIZE = 64;

Except_T Bitpack_Overflow = { "Overflow packing bits" };

/*
 * Name:       Bitpack_fitsu
 * Purpose:    Determines if an unsigned integer can be represented in a given
 *             amount of bits.
 * Parameters: uint64_t n: a 64-bit unsigned integer to check
 *             unsigned width: an unsigned integer representing the amount
 *             of bits n will be fit into.
 * Returns:    True if n can fit in width bits, false if not
 * Expects:    Width to be greater than 0 and less than or equal to 64.
 * Notes:      None
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        return n <= (pow(2, width) - 1);
}

/*
 * Name:       Bitpack_fitss
 * Purpose:    Determines if a signed integer can be represented in a given
 *             amount of bits.
 * Parameters: uint64_t n: a 64-bit signed integer (in two's complement
 *             representation) to check
 *             unsigned width: an unsigned integer representing the amount
 *             of bits n will be fit into.
 * Returns:    True if n can fit in width bits, false if not
 * Expects:    Width to be greater than 0 and less than or equal to 64.
 *             n to be in two's complement representation
 * Notes:      None
 */
bool Bitpack_fitss(int64_t n, unsigned width)
{
        return n <= (pow(2, width - 1) - 1);
}

/*
 * Name:       Bitpack_getu
 * Purpose:    Extracts an unsigned integer with given width and least 
 *             significant bit from a word.
 * Parameters: uint64_t word: a 64-bit unsigned integer to extract from
 *             unsigned width: an unsigned integer representing the amount
 *             of bits to extract
 *             unsigned lsb: an unsigned integer representing the least
 *             significant bit the extracted field will have
 * Returns:    An unsigned integer representing the field that was extracted
 * Expects:    width to be greater than 0 and less than or equal to 64.
 *             width + lsb to be less than or equal to 64
 * Notes:      Raises a CRE if either of the above expectations are not met
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= INT_SIZE);
        assert(width + lsb <= INT_SIZE);

        if (width == 0) {
                return 0;
        }

        uint64_t mask = ~0;
        mask = mask >> (INT_SIZE - width) << lsb;

        word &= mask;

        return word >> lsb;
}

/*
 * Name:       Bitpack_gets
 * Purpose:    Extracts an signed integer (in two's complement representation)
 *             with given width and least significant bit from a word.
 * Parameters: uint64_t word: a 64-bit unsigned integer to extract from
 *             unsigned width: an unsigned integer representing the amount
 *             of bits to extract
 *             unsigned lsb: an unsigned integer representing the least
 *             significant bit the extracted field will have
 * Returns:    A signed integer (in two's complement representation) 
 *             representing the field that was extracted
 * Expects:    width to be greater than 0 and less than or equal to 64.
 *             width + lsb to be less than or equal to 64
 * Notes:      Raises a CRE if either of the above expectations are not met
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width + lsb <= INT_SIZE);
        assert(width <= INT_SIZE);

        if (width == 0) {
                return 0;
        }

        uint64_t mask = ~0;
        mask = mask << (width - 1);

        uint64_t field = Bitpack_getu(word, width, lsb);

        if ((field & mask) == 0) {
                return field;
        }

        int64_t signed_field = field | mask;

        return signed_field;
}

/*
 * Name:       Bitpack_newu
 * Purpose:    Returns a new 64-bit word with a field replaced with the given
 *             unsigned value
 * Parameters: uint64_t word: a 64-bit unsigned integer that represents the
 *             original
 *             unsigned width: an unsigned integer representing the amount
 *             of bits to replace
 *             unsigned lsb: an unsigned integer representing the least
 *             significant bit the replaced field will have
 *             uint64_t value: an unsigned integer representing the value
 *             that will be inserted into word
 * Returns:    An unsigned integer representing the new word with the
 *             editted field
 * Expects:    width to be greater than 0 and less than or equal to 64.
 *             width + lsb to be less than or equal to 64
 *             value is able to fit into width unsigned bits
 * Notes:      Raises a CRE if width < 0 or width + lsb > 64. Rais
 *             Raises a Bitpack_Overflow exception if value does not fit into
 *             width unsigned bits
 */
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

/*
 * Name:       Bitpack_news
 * Purpose:    Returns a new 64-bit word with a field replaced with the given
 *             signed value
 * Parameters: uint64_t word: a 64-bit unsigned integer that represents the
 *             original
 *             unsigned width: an unsigned integer representing the amount
 *             of bits replace
 *             unsigned lsb: an unsigned integer representing the least
 *             significant bit the replaced field will have
 *             uint64_t value: a signed integer (in two's complement)
 *             representing the value that will be inserted into word
 * Returns:    An unsigned integer representing the new word with the
 *             editted field
 * Expects:    width to be greater than 0 and less than or equal to 64.
 *             width + lsb to be less than or equal to 64
 *             value is able to fit into width signed bits
 * Notes:      Raises a CRE if width < 0 or width + lsb > 64. Rais
 *             Raises a Bitpack_Overflow exception if value does not fit into
 *             width signed bits
 */
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