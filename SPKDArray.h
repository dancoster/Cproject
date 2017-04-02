#ifndef SPKDARRAY_H_
#define SPKDARRAY_H_

#include <stdbool.h>
#include "SPPoint.h"
#include "SPBPriorityQueue.h"
#include "SPLogger.h"

#define LEFT 0
#define RIGHT 1

/** A data-structure which is used for defining the KD Array **/
typedef struct sp_kd_array_t SPKDArray;

/**
 * Allocates a new KDArray in the memory.
 * Given points array and a size of the array.
 * such that the following holds:
 *
 * using spKDArrayAlloc function to:
 * - KDArray points - the points array is copied (in order) to the KDArray points array (allocation needed)
 * - KDArray sortedMatrix - only allocates 2d int array of (d x n) (d = PCA Dimension)
 * - KDArray size - set to be <n>
 * - KDArray dim - set to be <dim> (spPCADimension from the config)
 *
 * after allocations, initializing and sorting the sortedMatrix such that:
 * each i-th row is the indexes of the points in <points> sorted according to their i-th dimension, that is,
 * sortedMatrix[i][j] = the index of the j-th point with respect to the i-th coordinate
 *
 * @param points 	- array of spPoints to copy
 * @param n 		- size of <points>
 * @param dim 		- spPCADimension from the config
 *
 * @return
 * NULL in case of allocation failure occurred, or points==NULL, or n<=0, or dim<0
 * Otherwise, the new KDArray is returned
 */
SPKDArray* spKDArrayInit(SPPoint** points, int n, int dim);

/**
 * Splits the KDArray to two KDArrays (kdLeft, kdRight) such that:
 * the first ceiling(n/2) points with respect to <coor> are in kdLeft, and the rest
 * of the points are in kdRight
 *
 * @param arr - the KDArray to split
 * @param coor - the dimension to split by
 *
 * @return
 * NULL if arr==NULL or coor<0 or coor>=dimension of spPoints in array
 * Otherwise, an array of two KDArrays (index 0 is kdLeft, index 1 is kdRight)
 */
SPKDArray** spKDArraySplit(SPKDArray* arr, int coor);

/**
 * Allocates a new KDArray in the memory.
 * Given points array and a size of the array.
 * such that the following holds:
 *
 * - KDArray points - the points array is copied (in order) to the KDArray points array (allocation needed)
 * - KDArray size - set to be n
 * - KDArray sortedMatrix - only allocates 2d int array of (d x n) (d is the dim of a point)
 *
 * @param points 	- array of spPoints to copy
 * @param n 		- size of <points>
 * @param dim 		- spPCADimension from the config
 *
 * @return
 * NULL in case allocation failure occurred, or points==NULL, or n<=0, or dim<0
 * Otherwise, the new KDArray is returned
 */
SPKDArray* spKDArrayAlloc(SPPoint** points, int n, int dim);

/**
 * Copies the point array src to dest.
 *
 * @param dest - the destination array where the points are to be copied
 * @param src  - the points array to be copied
 *
 * @return
 * returns the number of points which were copied to dest
 * 0 - if (dest == NULL || src == NULL || n <= 0)
 * i - in case of allocation failure in the i-th point copy
 * n - all the points were copied
 */
int spDKArrayPointsCopy(SPPoint** dest, SPPoint** src, int n);

/**
 * Frees all memory allocation associated with a points array.
 *
 * @param arr 	- the points array to destroy
 * @param n 	- the size of <arr>
 *
 * if arr is NULL OR n<0 nothing happens.
 */
void spPoint1DDestroy(SPPoint** arr, int n);

/**
 * Allocates a 2d int array.
 *
 * @param n - rows
 * @param m - cols
 *
 * @return
 * NULL in case allocation failure occurred, or n<=0, or m<=0
 * Otherwise, the new int array is returned
 */
int** spKDArrayMatrixAlloc(int n, int m);

/**
 * Frees a 2d int array.
 *
 * @param mat 	- the 2d int array to destroy
 * @param n 	- rows
 * @param m 	- cols
 *
 * if mat==NULL, or n<=0, or m<=0, nothing happens.
 */
void spKDArrayMatrixDestroy(int** mat, int n, int m);

/**
 * Frees all memory allocation associated with KDArray.
 *
 * @param arr 	- the KDArray to destroy
 *
 * if arr is NULL nothing happens.
 */
void spKDArrayDestroy(SPKDArray* arr);

/**
 * A getter for the points array of KDArray.
 *
 * @param arr - The source KDArray
 *
 * @return
 * NULL if arr==NULL
 * Otherwise, the points array is returned
 */
SPPoint** spKDArrayGetPoints(SPKDArray* arr);

/**
 * A getter for the sortedMatrix of KDArray.
 *
 * @param arr - The source KDArray
 *
 * @return
 * NULL if arr==NULL
 * Otherwise, the sortedMatrix is returned
 */
int** spKDArrayGetSortedMatrix(SPKDArray* arr);

/**
 * A getter for dim of KDArray (#rows of sortedMatrix).
 *
 * @param arr - The source KDArray
 *
 * @return
 * -1 if arr==NULL
 * Otherwise, the dim is returned
 */
int spKDArrayGetDim(SPKDArray* arr);

/**
 * A getter for the number of points of KDArray.
 *
 * @param arr - The source KDArray
 *
 * @return
 * -1 if arr==NULL
 * Otherwise, the number of points is returned
 */
int spKDArrayGetSize(SPKDArray* arr);

void spKDArrayPrintMatrix(SPKDArray* arr);
void spKDArrayPrintPoints(SPPoint** points , int n);

#endif /* SPKDARRAY_H_ */
