/***************************************************************************
 *                     uarray2b.c
 *
 *     Assignment: locality
 *     Authors:  Diana Calderon and Madeline Lei
 *     Usernames: dcalde02, mlei03
 *     Date:     10/07/2025
 *
 *     summary:
 *
 *     This file contains the implementation for a UArray2b. It
 *     implements the functions declared in uarray2b.h that define
 *     an instance of a UArray2b.
 * 
 *     implementation:
 *     
 *     A single UArray2b is represented as a UArray2_T. Its width
 *     is the amount of blocks width-wise, its height is the
 *     amount of blocks height-wise, and its element size is the
 *     size of UArray_T. 
 * 
 *     Each element of the UArray2_T is a UArray_T that represents a single 
 *     block of memory. Its length is blocksize * blocksize, and its size is 
 *     the given element size. This ensures that all of the elements of the
 *     block will be near each other in memory.
 * 
 *     invariants:
 *     
 *     - Each cell of the UArray2b_T block (i.e. the elements in the UArray_T) 
 *       has the same size as what was specified by the user. This ensures that 
 *       the array can store exactly the amount of elements specified, with no 
 *       leftover space
 * 
 *     - Each block stores blocksize * blocksize elements. This ensures that 
 *       the blocks are the expected size for the user
 * 
 *     - The total size of a block is blocksize * blocksize * size. This 
 *       ensures that the block has enough memory to hold every element the 
 *       user inserts
 * 
 *     - The unused (leftover) cells at the edges of the UArray2b are not 
 *       able to be accessed
 *     
 *
 **************************************************************************/
#include "uarray2b.h"
#include "uarray.h"
#include "uarray2.h"
#include <math.h>
#include <assert.h>
#include <stdlib.h>

/************************** UArray2b_T *************************
 * 
 * A two dimensional unboxed blocked array. It is implemented as a 2D
 * unboxed array, where each element is a 1D unboxed array. These 1D unboxed
 * arrays represent blocks
 *
 * Components:
 *      int width: number of columns in the array
 *      int height: number of rows in the array
 *      int size: size of each element (in bytes)
 *      int blocksize: number of cells on one side of a block
 *      UArray2_T uarray2: a 2D UArray of 1D UArrays where each
 *      element represents a block
 *
 * Notes:
 *      Memory for uarray2 and inner arrays is allocated at
 *      UArray2_new and freed at UArray2_free.
 **************************************************************/
struct UArray2b_T {
        int width;
        int height;
        int size;
        int blocksize;
        UArray2_T uarray2;
};

/*
 * Name:       UArray2b_new
 * Purpose:    Allocated and initializes a new instance of a UArray2b with the
 *             specified width, height, element size, and blocksize.
 * Parameters: int width: the width of the UArray2
 *             int height: the height of the UArray2
 *             int size: the size of each of the elements in the UArray2
 *             int blocksize: number of cells on one side of a block
 * Returns:    A pointer to the newly created UArray2b struct
 * Expects:    height and width to be greater or equal to 0, and size and
 *             blocksize to be positive.
 * Notes:      If the allocation of memory fails or the height, width, size,
 *             or blocksize are  not valid, a checked runtime error is raised.
 *             Caller assumes ownership of the returned UArray2b and is
 *             responsible for freeing it by calling UArray2b_free.
*/
UArray2b_T UArray2b_new(int width, int height, int size, int blocksize)
{
        assert(width >= 0 && height >= 0 && size > 0 && blocksize > 0);
        UArray2b_T uarray2b = malloc(sizeof(*uarray2b));
        assert(uarray2b != NULL);
        uarray2b->width = width;
        uarray2b->height = height;
        uarray2b->size = size;
        uarray2b->blocksize = blocksize;

        int blockWidth = (width + blocksize - 1) / blocksize;
        int blockHeight = (height + blocksize - 1) / blocksize;

        uarray2b->uarray2 =
                UArray2_new(blockWidth, blockHeight, sizeof(UArray_T));

        for (int row = 0; row < blockHeight; row++) {
                for (int col = 0; col < blockWidth; col++) {
                        UArray_T block =
                                UArray_new(blocksize * blocksize, size);
                        *(UArray_T *) UArray2_at(uarray2b->uarray2, col, row) =
                                block;
                }
        }
        return uarray2b;
}

/*
 * Name:       UArray2b_new_64K_block
 * Purpose:    Allocated and initializes a new instance of a UArray2b with the
 *             specified width, height, element size, and a blocksize as large
 *             as possible provided a block occupies at most 64KB.
 * Parameters: int width: the width of the UArray2
 *             int height: the height of the UArray2
 *             int size: the size of each of the elements in the UArray2
 * Returns:    A pointer to the newly created UArray2b struct
 * Expects:    height and width to be greater or equal to 0, and size to be 
 *             positive.
 * Notes:      If the allocation of memory fails or the height, width, size,
 *             or blocksize are  not valid, a checked runtime error is raised.
 *             Caller assumes ownership of the returned UArray2b and is
 *             responsible for freeing it by calling UArray2b_free.
*/
UArray2b_T UArray2b_new_64K_block(int width, int height, int size)
{
        assert(width >= 0 && height >= 0 && size > 0);

        int kb = 64 * 1024;
        int blocksize = sqrt((kb / size));

        return UArray2b_new(width, height, size, blocksize);
}

/*
 * Name:       UArray2b_free
 * Purpose:    Clears the UArray2b and frees the memory used to malloc the
 *             given UArray2b.
 * Parameters: UArray2b_T *array2b: a pointer to a pointer of UArray2b_T.
 * Returns:    None.
 * Expects:    The provided UArray2b shouldn't be or be pointing to NULL
 * Notes:      If the provided UArray2b is NULL or pointing to NULL, a checked
 *             runtime error is raised.
 *             Caller relinquishes ownership of the memory for UArray2b and 
 *             its internal array.
 */
void UArray2b_free(UArray2b_T *array2b)
{
        assert(array2b != NULL && *array2b != NULL);
        UArray2_T uarray2 = (*array2b)->uarray2;
        for (int row = 0; row < UArray2_height(uarray2); row++) {
                for (int col = 0; col < UArray2_width(uarray2); col++) {
                        UArray_T *uarray1 = UArray2_at(uarray2, col, row);
                        UArray_free(uarray1);
                }
        }

        UArray2_free(&(*array2b)->uarray2);
        free((*array2b));
        *array2b = NULL;
}

/*
 * Name:       UArray2b_width
 * Purpose:    Returns an int representing the width of the UArray2b, which
 *             is the total number of columns in UArray2b.
 * Parameters: UArray2b_T array2b: a pointer to a struct UArray2b_T.
 * Returns:    An int representing the number of columns in UArray2b.
 * Expects:    The UArray2b passed in has an initialized, non-negative width.
 *             And the UArray2b is not NULL.
 * Notes:      If the provided UArray2b is NULL, a checked runtime error is
 *             raised.
 */
int UArray2b_width(UArray2b_T array2b)
{
        assert(array2b != NULL);

        return array2b->width;
}

/*
 * Name:       UArray2b_height
 * Purpose:    Returns an integer representing the height of the UArray2b,
 *             which is the total number of rows in UArray2b.
 * Parameters: UArray2b_T array2b: a pointer to a struct Uarray2b_T.
 * Returns:    An int representing the number of rows in UArray2b.
 * Expects:    The UArray2b passed in has an initialized, non-negative height.
 *             And the UArray2b is not NULL.
 * Notes:      If the provided UArray2b is NULL, a checked runtime error is
 *             raised.
 */
int UArray2b_height(UArray2b_T array2b)
{
        assert(array2b != NULL);

        return array2b->height;
}

/*
 * Name:       UArray2b_size
 * Purpose:    Returns the size of one element in the given UArray2b.
 * Parameters: UArray2b_T array2b: a pointer to a struct Uarray2b_T.
 * Returns:    An int representing the size of one element, that is the
 *             number of bytes the element uses (the amount of memory allocated
 *             for the data type of the element.
 * Expects:    The UArray2b passed in has an initialized, non-negative size. And
 *             the UArray2b is not NULL.
 * Notes:      If the provided UArray2b is NULL, a checked runtime error is
 *             raised.
 */
int UArray2b_size(UArray2b_T array2b)
{
        assert(array2b != NULL);

        return array2b->size;
}

/*
 * Name:       UArray2b_height
 * Purpose:    Returns an integer representing the blocksize of the UArray2b,
 *             which is the number of cells on one side of a block.
 * Parameters: UArray2b_T array2b: a pointer to a struct Uarray2b_T.
 * Returns:    An int representing the number of cells on one side of a block
 *             in UArray2b.
 * Expects:    The UArray2b passed in has an initialized, non-negative height.
 *             And the UArray2b is not NULL.
 * Notes:      If the provided UArray2b is NULL, a checked runtime error is
 *             raised.
 */
int UArray2b_blocksize(UArray2b_T array2b)
{
        assert(array2b != NULL);

        return array2b->blocksize;
}

/*
 * Name:       UArray2b_at
 * Purpose:    Returns a pointer to the element at the given column and row
 *             indices in the given UArray2b.
 * Parameters: UArray2b_T array2b: a pointer to a struct UArray2b_T.
 *             int column: the distance between the element and left edge of
 *             the UArray2b
 *             int row: the distance between the element and the top row of
 *             the UArray2b
 * Returns:    A void pointer to the element at the given row and column
 *             indices.
 * Expects:    array2b not to be NULL, and the column and row to be within
 *             the bounds of its height and width.
 * Notes:      Will raise a checked runtime error if the indices are not valid
 *             (out of bounds/negative) or the provided UArray2b is NULL
*/
void *UArray2b_at(UArray2b_T array2b, int column, int row)
{
        assert(array2b != NULL);
        assert(column < array2b->width && row < array2b->height);
        assert(column >= 0 && row >= 0);

        int blocksize = array2b->blocksize;

        int bRow = row / blocksize;
        int bCol = column / blocksize;

        int index = blocksize * (row % blocksize) + (column % blocksize);

        UArray_T *block = UArray2_at(array2b->uarray2, bCol, bRow);

        return UArray_at(*block, index);
}

/*
 * Name:       UArray2b_map
 * Purpose:    Calls the given apply function for each element of the array.
 *             Visits every cell in one block before moving to another
 * Parameters: UArray2b_T array2b: A pointer to a struct UArray2b
 *             void apply(): A function pointer to be applied to each element
 *             of the array
 *                  Parameters to apply() function:
 *                      int column: the distance between the element and the
 *                      left edge of the UArray2b
 *                      int row: the distance between the element and the top
 *                      row of the UArray2b
 *                      UArray2b_T array2b: A pointer to a struct UArray2b
 *                      void *element: a void pointer to the current element
 *                      void *cl: a void pointer to a closure argument
 *             void *cl: a void pointer to a closure argument
 * Returns:    Nothing
 * Expects:    The UArray2b passed in is not NULL and each parameter in the
 *             apply function is not NULL and valid.
 * Notes:      Visits every cell in one block before moving to another 
 *             Will raise a checked runtime error if uarray2b or apply() are
 *             NULL.
 *             apply() will be called for every element exactly once.
 */
void UArray2b_map(UArray2b_T array2b,
                  void apply(int col, int row, UArray2b_T array2b, void *elem,
                             void *cl),
                  void *cl)
{
        assert(array2b != NULL && apply != NULL);
        int blocksize = array2b->blocksize;
        int blockWidth = ceil((double) array2b->width / blocksize);
        int blockHeight = ceil((double) array2b->height / blocksize);

        for (int bRow = 0; bRow < blockHeight; bRow++) {
                for (int bCol = 0; bCol < blockWidth; bCol++) {
                        for (int index = 0; index < blocksize * blocksize;
                             index++) {
                                int col =
                                        bCol * blocksize + (index % blocksize);
                                int row =
                                        bRow * blocksize + (index / blocksize);
                                /* make sure you are not in unused cells */
                                if (col < array2b->width &&
                                    row < array2b->height) {
                                        apply(col, row, array2b,
                                              UArray2b_at(array2b, col, row),
                                              cl);
                                }
                        }
                }
        }
}
