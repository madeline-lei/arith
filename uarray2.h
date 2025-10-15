/**************************************************************
 *                     uarray2.h
 *
 *     Assignment: locality
 *     Authors:  Diana Calderon and Madeline Lei
 *     Usernames: dcalde02, mlei03
 *     Date:     10/07/2025
 *
 *     summary
 *
 *     This file contains an interface for a UArray2. It contains
 *     the functions that the client can use to create, edit, and
 *     delete an instance of a UArray2.
 *     
 *     NOTE: This file was provided by the course
 *
**************************************************************/

#ifndef ARRAY2_INCLUDED
#define ARRAY2_INCLUDED
#define T UArray2_T
typedef struct T *T;

typedef void UArray2_applyfun(int i, int j, T array2, void *elem, void *cl);
typedef void UArray2_mapfun(T array2, UArray2_applyfun apply, void *cl);

extern T UArray2_new(int width, int height, int size);
extern void UArray2_free(T *array2);
extern int UArray2_width(T array2);
extern int UArray2_height(T array2);
extern int UArray2_size(T array2);
extern void *UArray2_at(T array2, int i, int j);
extern void UArray2_map_row_major(T array2, UArray2_applyfun apply, void *cl);
extern void UArray2_map_col_major(T array2, UArray2_applyfun apply, void *cl);
#undef T
#endif