/**************************************************************
 *                     a2plain.c
 *
 *     Assignment: locality
 *     Authors:  Diana Calderon and Madeline Lei
 *     Usernames: dcalde, mlei03
 *     Date:     10/07/2025
 *
 *     summary:
 *
 *     This file is an method suite that contains function pointers
 *     that can be applied to a UArray2. It defines a private version
 *     of each function in A2Methods_T that we implement.
 *
 **************************************************************/

#include <string.h>

#include <a2plain.h>
#include "uarray2.h"
#include "assert.h"

typedef A2Methods_UArray2 A2; // private abbreviation

/*
 * Name:       new
 * Purpose:    Allocated and initializes a new instance of a UArray2 with the
 *             specified width, height, and element size.
 * Parameters: int width: the width of the UArray2
 *             int height: the height of the UArray2
 *             int size: the size of each of the elements in the UArray2
 * Returns:    A pointer to the newly created UArray2 struct, stored as a type
 *             A2Methods_UArray2
 * Expects:    height and width to be greater or equal to 0, and size to be
 *             positive.
 * Notes:      Will raise a CRE through UArray2_new() if something goes wrong
 *             Caller assumes ownership of the returned UArray2 and is
 *             responsible for freeing it by calling a2free.
*/
static A2Methods_UArray2 new(int width, int height, int size)
{
        return UArray2_new(width, height, size);
}

/*
 * Name:       new_with_blocksize
 * Purpose:    Allocated and initializes a new instance of a UArray2 with the
 *             specified width, height, and element size. Blocksize is not
 *             used
 * Parameters: int width: the width of the UArray2
 *             int height: the height of the UArray2
 *             int size: the size of each of the elements in the UArray2
 *             int blocksize: not used in this function
 * Returns:    A pointer to the newly created UArray2 struct, stored as a type
 *             A2Methods_UArray2
 * Expects:    height and width to be greater or equal to 0, and size to be
 *             positive.
 * Notes:      Will raise a CRE through new() if something goes wrong
 *             Caller assumes ownership of the returned UArray2 and is
 *             responsible for freeing it by calling a2free.
*/
static A2Methods_UArray2 new_with_blocksize(int width, int height, int size,
                                            int blocksize)
{
        (void) blocksize;
        return new(width, height, size);
}

/*
 * Name:       a2free
 * Purpose:    Clears the A2Methods_UArray2 and frees the memory used in it
 * Parameters: A2 *array2p: a pointer to a pointer of A2Methods_UArray2.
 * Returns:    None.
 * Expects:    The provided UArray2 shouldn't be or be pointing to NULL
 * Notes:      If the provided UArray2 is NULL or pointing to NULL, a checked
 *             runtime error is raised.
 *             Caller relinquishes ownership of the memory for UArray2 and 
 *             its internal array.
 */
static void a2free(A2 *array2p)
{
        UArray2_free((UArray2_T *) array2p);
}

/*
 * Name:       width
 * Purpose:    Returns an int representing the width of the UArray2, which
 *             is the total number of columns in UArray2.
 * Parameters: A2 array2: a pointer to a struct A2Methods_UArray2.
 * Returns:    An int representing the number of columns in UArray2.
 * Expects:    The UArray2 passed in has an initialized, non-negative width.
 *             And the UArray2 is not NULL.
 * Notes:      Raises a CRE through UArray2_width if expectations are not met
 */
static int width(A2 array2)
{
        return UArray2_width(array2);
}

/*
 * Name:       height
 * Purpose:    Returns an integer representing the height of the UArray2, which
 *             is the total number of rows in UArray2.
 * Parameters: A2 array2: a pointer to a struct A2Methods_UArray2.
 * Returns:    An int representing the number of rows in UArray2.
 * Expects:    The UArray2 passed in has an initialized, non-negative height.
 *             And the UArray2 is not NULL.
 * Notes:      Raises a CRE through UArray2_height if expectations are not met
 */
static int height(A2 array2)
{
        return UArray2_height(array2);
}

/*
 * Name:       size
 * Purpose:    Returns the size of one element in the given UArray2.
 * Parameters: A2 array2: a pointer to a struct A2Methods_UArray2.
 * Returns:    An int representing the size of one element, that is the
 *             number of bytes the element uses (the amount of memory allocated
 *             for the data type of the element.
 * Expects:    The UArray2 passed in has an initialized, non-negative size. And
 *             the UArray2 is not NULL.
 * Notes:      Raises a CRE through UArray2_size if expectations are not met
 */
static int size(A2 array2)
{
        return UArray2_size(array2);
}

/*
 * Name:       blocksize
 * Purpose:    Returns the size of a block in the given UArray2. This is
 *             always 1
 * Parameters: A2 array2: a pointer to a struct A2Methods_UArray2.
 * Returns:    An int representing the size of a block in the given UArray2.
 *             This will always be 1
 * Expects:    The UArray2 passed in is not NULL
 * Notes:      Raises a CRE is the UArray2 passed in is NULL
 */
static int blocksize(A2 array2)
{
        assert(array2 != NULL);
        (void) array2;
        return 1;
}

/*
 * Name:       at
 * Purpose:    Returns a pointer to the element at the given column and row
 *             indices in the given UArray2.
 * Parameters: A2 array2: a pointer to a struct A2Methods_UArray2.
 *             int i: the distance between the element and left edge of
 *             the UArray2
 *             int j: the distance between the element and the top row of
 *             the UArray2
 * Returns:    A A2Methods_Object pointer to the element at the given row
 *             and column indices.
 * Expects:    array2 not to be NULL, and the column and row to be within
 *             the bounds of its height and width.
 * Notes:      Raises a CRE through UArray2_at if expectations are not met
 */
static A2Methods_Object *at(A2 array2, int i, int j)
{
        return UArray2_at(array2, i, j);
}

typedef void applyfun(int i, int j, UArray2_T array2, void *elem, void *cl);

/*
 * Name:       map_row_major
 * Purpose:    Calls the given apply function for each element of the array.
 *             Column indices vary more rapidly than row indices.
 * Parameters: A2Methods_UArray2 uarray2: A pointer to a struct 
 *             A2Methods_UArray2
 *             A2Methods_applyfun apply: A function pointer to be applied to
 *             each element of the array
 *             void *cl: a void pointer to a closure argument
 * Returns:    Nothing
 * Expects:    The UArray2 passed in is not NULL and each parameter in the
 *             apply function is not NULL and valid.
 * Notes:      Column indices vary more rapidly than row indices
 *             Raises a CRE through UArray2_map_col_major if expectations are
 *             not met
 */
static void map_row_major(A2Methods_UArray2 uarray2, A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_row_major(uarray2, (applyfun *) apply, cl);
}

/*
 * Name:       map_col_major
 * Purpose:    Calls the given apply function for each element of the array.
 *             Row indices vary more rapidly than column indices.
 * Parameters: A2Methods_UArray2 uarray2: A pointer to a struct 
 *             A2Methods_UArray2
 *             A2Methods_applyfun apply: A function pointer to be applied to
 *             each element of the array
 *             void *cl: a void pointer to a closure argument
 * Returns:    Nothing
 * Expects:    The UArray2 passed in is not NULL and each parameter in the
 *             apply function is not NULL and valid.
 * Notes:      Row indices vary more rapidly than column indices.
 *             Raises a CRE through UArray2_map_col_major if expectations are
 *             not met
 */
static void map_col_major(A2Methods_UArray2 uarray2, A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_col_major(uarray2, (applyfun *) apply, cl);
}

/*
 * Name:       small_closure
 * Components: A2Methods_smallapplyfun *apply: a pointer to a function that
 *             will be applied to every element of a UArray2
 *             void *cl: a void pointer to a closure argument
 * 
 * Notes:      expects apply and cl to not be NULL
 */
struct small_closure {
        A2Methods_smallapplyfun *apply;
        void *cl;
};

/*
 * Name:       apply_small
 * Purpose:    Applies a function to a single element of a UArray2
 * Parameters: int i: required by the map function interface; unused here
 *             int j: required by the map function interface; unused here
 *             UArray2_T uarray2: required by the map function interface; 
 *             unused here
 *             void *element: a void pointer to the current element
 *             void *vcl: a void pointer to a small_closure struct storing
 *             an apply function and a closure argument
 * Returns:    Nothing
 * Expects:    vcl is not NULL.
 * Notes:      Raises a CRE if vcl is NULL
 */
static void apply_small(int i, int j, UArray2_T uarray2, void *elem, void *vcl)
{
        assert(vcl != NULL);
        struct small_closure *cl = vcl;
        (void) i;
        (void) j;
        (void) uarray2;
        cl->apply(elem, cl->cl);
}

/*
 * Name:       small_map_row_major
 * Purpose:    Calls the given apply function for each element of the array.
 *             Row indices vary more rapidly than column indices.
 * Parameters: A2Methods_UArray2 a2: represents a struct A2Methods_UArray2 
 *             to map over
 *             A2Methods_smallapplyfun apply: A function pointer to be applied
 *             to each element of the array
 *             void *cl: a void pointer to a closure argument
 * Returns:    Nothing
 * Expects:    apply is not NULL
 * Notes:      Row indices vary more rapidly than column indices.
 *             Raises a CRE if apply is NULL
 */
static void small_map_row_major(A2Methods_UArray2 a2,
                                A2Methods_smallapplyfun apply, void *cl)
{
        assert(apply != NULL);
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}

/*
 * Name:       small_map_row_major
 * Purpose:    Calls the given apply function for each element of the array.
 *             Column indices vary more rapidly than row indices.
 * Parameters: A2Methods_UArray2 a2: represents a struct A2Methods_UArray2 
 *             to map over
 *             A2Methods_smallapplyfun apply: A function pointer to be applied
 *             to each element of the array
 *             void *cl: a void pointer to a closure argument
 * Returns:    Nothing
 * Expects:    apply is not NULL
 * Notes:      Column indices vary more rapidly than row indices.
 *             Raises a CRE if apply is NULL
 */
static void small_map_col_major(A2Methods_UArray2 a2,
                                A2Methods_smallapplyfun apply, void *cl)
{
        assert(apply != NULL);
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}

/*
 * Name:       uarray2_methods_plain_struct
 * Components:
 *             new: a function pointer that allocates a new UArray2 with given
 *             width, height, element size
 *             new_with_blocksize: a function pointer that allocates a new
 *             UArray2 with given width, height, element size (blocksize is 
 *             ignored)
 *             a2free: a function pointer that frees a UArray2
 *             width: a function pointer that returns the width of a UArray2
 *             height: a function pointer that returns the height of a UArray2
 *             size: a function pointer that returns the size of each element
 *             in a UArray2
 *             blocksize: a function pointer that returns the blocksize of
 *             a UArray2 (which is always 1)
 *             at: a function pointer that returns a pointer to the element at
 *             the given indices
 *             map_row_major: a function pointer that maps a function over 
 *             elements in row-major order.
 *             map_col_major: a function pointer that maps a function over 
 *             elements in col-major order.
 *             map_block_major: NULL (not supported)
 *             map_default: the default map function, map_row_major()
 *             small_map_row_major: a function pointer that maps a small
 *             function over  elements in row-major order.
 *             small_map_col_major: a function pointer that maps a small
 *             function over  elements in row-major order.
 *             small_map_block_major: NULL (not supported)
 *             small_map_default: the default small map function, 
 *             small_map_row_major()
 * 
 */
static struct A2Methods_T uarray2_methods_plain_struct = {
        new,
        new_with_blocksize,
        a2free,
        width,
        height,
        size,
        blocksize,
        at,
        map_row_major,
        map_col_major,
        NULL, // map_block_major
        map_row_major, // map_default
        small_map_row_major,
        small_map_col_major,
        NULL, // small_map_block_major
        small_map_row_major, // small_map_block_major
};

A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
