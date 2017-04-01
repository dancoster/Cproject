#include "SPKDArray.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Global variable for sorting
int sortByDim;

struct sp_kd_array_t {
	SPPoint** points;
	int** sortedMatrix;
	int dim;				// set to be the #rows of sortedMatrix
	int size;				// set to be the #cols of sortedMatrix
};

SPKDArray* spKDArrayInit(SPPoint** points, int n, int dim) {
	if (points==NULL || n<=0 || dim<0) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}

//	spKDArrayPrintPoints(points, n);
//	fflush(NULL);

	SPKDArray* arr = spKDArrayAlloc(points, n, dim); // allocating memory for all array fields

	if (arr == NULL) { //Allocation failure
		spLoggerPrintError(ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}

	// using BPQueueElement to sort the points by i-th coor value
	BPQueueElement* elements = (BPQueueElement*) malloc(n*sizeof(BPQueueElement));
	if (elements == NULL) { //Allocation failure
		spLoggerPrintError(ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		spKDArrayDestroy(arr);
		return NULL;
	}

	for (int i=0; i<dim; i++) {
		for (int j=0; j<n; j++) {
			BPQueueElement element = {j, spPointGetAxisCoor(points[j],i)}; // setting each element with the i-th coor value
			elements[j] = element;
		}
		qsort(elements, n, sizeof(BPQueueElement), spKDArrayCompareValuesByDim); // sorting the elements by i-th coor value
//		printf("AFTER SORTING\n");
//		for (int l=0; l<n; l++) {
//			printf("element[%d]=(%d,%f)\n", l, elements[l].index,elements[l].value);
//			fflush(NULL);
//			}
		for (int k=0; k<n; k++) { // setting i-th row of sortedMatrix with the sorted indexes
//			printf("%d ", elements[k].index);
//			fflush(NULL);
			arr->sortedMatrix[i][k] = elements[k].index;
		}

	}
	free(elements);
//	printf("\n");
//	printf("FINISH ARR INIT\n");
	return arr;
}

SPKDArray** spKDArraySplit(SPKDArray* arr, int coor) {
	if (arr==NULL || coor<0 || coor>=arr->dim) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}

	// setting and allocating variables:
	int n = arr->size;						// number of points
	int dim = arr->dim;						// #rows of sortedMatrix
	int nLeft, nRight;						// left & right arrays sizes
	if (n%2 == 0)
		nLeft = n/2;
	else nLeft = n/2+1;
	nRight =  n - nLeft;
//	printf("size left: %d, size right: %d\n", nLeft, nRight);
//	fflush(NULL);
	SPPoint** arrLeft = (SPPoint**) (malloc(nLeft*sizeof(SPPoint*)));
	SPPoint** arrRight = (SPPoint**) (malloc(nRight*sizeof(SPPoint*)));
	SPKDArray** splittedArrays = (SPKDArray**) malloc(2*sizeof(SPKDArray*));
	int cLeft=0, cRight=0; // counters for the indexes in left and right array splits
	int* X = (int*) calloc(n, sizeof(int));
	int* map = (int*) (calloc(n, sizeof(int)));

	if (arrLeft==NULL || arrRight==NULL || splittedArrays==NULL || X==NULL || map==NULL) { //Allocation failure
		spLoggerPrintError(ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		free(arrLeft);
		free(arrRight);
		free(splittedArrays);
		free(X);
		free(map);
		return NULL;
	}

//	printf("SPLIT MSG #1\n");
//	fflush(NULL);

	// editing X's indexes - set '0' for left array, '1' for right array
	int k;
	for (int i=0; i<n; i++) {
		k = arr->sortedMatrix[coor][i];
//		printf("%d ", k);
		if (i < nLeft)
			X[k] = LEFT;
		else
			X[k] = RIGHT;
	}
//	printf("\n");
//	printf("X of row %d is:\n", coor);
//	for (int i=0; i<n; i++)
//		printf("%d ", X[i]);
//
//
//	printf("\n");
//	printf("SPLIT MSG #2\n");
//	fflush(NULL);

	// creating the two arrays of points
	for (int i=0; i<n; i++) {
		if (X[i] == LEFT) { // point belongs to left array
			arrLeft[cLeft] = spPointCopy(arr->points[i]);
			if (arrLeft[cLeft] == NULL) { // Allocation failure
				break;
			}
			map[i] = cLeft; // map the cLeft-th order point of the left side
			cLeft++;
		}
		else { // point belongs to right array
			arrRight[cRight] = spPointCopy(arr->points[i]);
			if (arrRight[cRight] == NULL) { // Allocation failure
				break;
			}
			map[i] = cRight; // map the cRight-th order point of the left side
			cRight++;
		}
	}
//	printf("cLeft, cRight = %d,%d\n", cLeft, cRight);
//	printf("map:\n");
//	for (int i=0; i<n; i++)
//		printf("%d ", map[i]);
//	printf("\n");
//	printf("SPLIT MSG #2.5\n");
//	fflush(NULL);
	// Allocation failure occurred in a copy point from the loop above, releasing all memory allocations
	if ((cLeft != nLeft) || (cRight != nRight)) {
		spLoggerPrintError(ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		spPoint1DDestroy(arrLeft, cLeft);
		spPoint1DDestroy(arrRight, cRight);
		spKDArrayDestroy(splittedArrays[LEFT]);
		spKDArrayDestroy(splittedArrays[RIGHT]);
		free(splittedArrays);
		free(X);
		free(map);
		return NULL;
	}

//	printf("SPLIT MSG #3\n");
//	fflush(NULL);

	// allocating the KDArrays of left and right arrays
	splittedArrays[LEFT] = spKDArrayAlloc(arrLeft, nLeft, dim);
	splittedArrays[RIGHT] = spKDArrayAlloc(arrRight, nRight, dim);
	if (splittedArrays[LEFT]==NULL || splittedArrays[RIGHT]==NULL) { //Allocation failure
		spLoggerPrintError(ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		spPoint1DDestroy(arrLeft, nLeft);
		spPoint1DDestroy(arrRight, nRight);
		spKDArrayDestroy(splittedArrays[LEFT]);
		spKDArrayDestroy(splittedArrays[RIGHT]);
		free(splittedArrays);
		free(X);
		free(map);
		return NULL;
	}

	// setting the left and right arrays sortedMatrix
	for (int i=0; i<dim; i++) {
		cLeft = 0;
		cRight = 0;
		for (int j=0; j<n; j++) {
			k = arr->sortedMatrix[i][j];
			if (X[k] == LEFT) { // the point belongs to left array
				splittedArrays[LEFT]->sortedMatrix[i][cLeft] = map[k];
				cLeft++;
			}
			else { // the point belongs to right array
				splittedArrays[RIGHT]->sortedMatrix[i][cRight] = map[k];
				cRight++;
			}
		}
	}
//	printf("\n");
//	printf("LEFT MATRIX:\n");
//	spKDArrayPrintMatrix(splittedArrays[LEFT]);
//	printf("\n");
//	printf("RIGHT MATRIX:\n");
//	spKDArrayPrintMatrix(splittedArrays[RIGHT]);
//	fflush(NULL);
//	printf("SPLIT MSG #4\n");
//	fflush(NULL);

	// free all memory allocations used
	spPoint1DDestroy(arrLeft, nLeft);
	spPoint1DDestroy(arrRight, nRight);
	free(X);
	free(map);

	return splittedArrays;
}

SPKDArray* spKDArrayAlloc(SPPoint** points, int n, int dim) {
	if (points == NULL || n <= 0 || dim < 0) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}

	SPKDArray* arr = (SPKDArray*) malloc(sizeof(SPKDArray));
	if (arr == NULL) { //Allocation failure
		spLoggerPrintError(ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}
	arr->size = n;
	arr->dim = dim;
	arr->points = (SPPoint**) malloc(n*sizeof(SPPoint*));

	if (arr->points == NULL) { //Allocation failure
		spLoggerPrintError(ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		free(arr);
		return NULL;
	}

	// coping the points to arr->points
	int successCopies = spDKArrayPointsCopy(arr->points, points, n);
	if (successCopies != n) {
		spPoint1DDestroy(arr->points, successCopies); //destroying all the points until index i=successCopies
		free(arr);
		return NULL;
	}

	arr->sortedMatrix = spKDArrayMatrixAlloc(dim, n); // matrix memory allocation
	if (arr->sortedMatrix == NULL) { //Allocation failure
		spLoggerPrintError(ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		spPoint1DDestroy(arr->points, n);
		free(arr);
		return NULL;
	}
	return arr;
}

int spDKArrayPointsCopy(SPPoint** dest, SPPoint** src, int n) {
	if (dest == NULL || src == NULL || n <= 0) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return 0;
	}

	for (int i=0; i<n; i++) {
		dest[i] = spPointCopy(src[i]);
		if (dest[i] == NULL) { //Allocation failure
			spLoggerPrintError(ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
			return i;
		}
	}
	return n;
}

int spKDArrayCompareValuesByDim(const void *a, const void *b) {
	BPQueueElement* e1 = (BPQueueElement*)a; // casting pointer types
	BPQueueElement* e2 = (BPQueueElement*)b; // casting pointer types

	  if (e1->value > e2->value)
		  return 1;
	  else if (e1->value < e2->value)
		  return -1;
	  else {
		 if (e1->index > e2->index)
			 return 1;
		 return -1;
	  }
}

void spPoint1DDestroy(SPPoint** arr, int n) {
	if (arr == NULL || n<0) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return;
	}
	for (int i=0; i<n; i++) {
		spPointDestroy(arr[i]);
	}
	free(arr);
}

int** spKDArrayMatrixAlloc(int n, int m) {
	if (n <= 0 || m <= 0) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}

	int** arr = (int**) malloc(n*(sizeof(int*)));
	if (arr == NULL) {	//Allocation failure
		spLoggerPrintError(ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}
	for (int i=0; i<n; i++) {
		arr[i] = (int*) malloc(m*sizeof(int));
		if (arr[i] == NULL) {
			spLoggerPrintError(ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
			for (int j=0; j<i; j++) { //free previous point allocations
				free(arr[j]);
			}
			free(arr);
			return NULL;
		}
	}
	return arr;
}

void spKDArrayMatrixDestroy(int** mat, int n, int m) {
	if (mat==NULL || n<=0 || m<=0) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return;
	}

	for (int i=0; i<n; i++) {
		free(mat[i]);
	}
	free(mat);
}

void spKDArrayDestroy(SPKDArray* arr) {
	if (arr==NULL) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return;
	}

	spKDArrayMatrixDestroy(arr->sortedMatrix, arr->dim, arr->size);
	spPoint1DDestroy(arr->points, arr->size);

	free(arr);
}

SPPoint** spKDArrayGetPoints(SPKDArray* arr) {
	if (arr == NULL) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}
	return (arr->points);
}

int** spKDArrayGetSortedMatrix(SPKDArray* arr) {
	if (arr == NULL) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}
	return (arr->sortedMatrix);
}

int spKDArrayGetDim(SPKDArray* arr) {
	if (arr== NULL) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return -1;
	}
	return arr->dim;
}

int spKDArrayGetSize(SPKDArray* arr) {
	if (arr== NULL) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return -1;
	}
	return arr->size;
}

void spKDArrayPrintMatrix(SPKDArray* arr) {
	if (arr== NULL) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return;
	}

	for (int i=0; i < arr->dim; i++) {
        for (int j = 0; j < arr->size; j++) {
            printf("%d ", arr->sortedMatrix[i][j]);
        }
        printf("\n");
    }
}

void spKDArrayPrintPoints(SPPoint** points , int n) {
	if (points== NULL) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return;
	}

	for (int i=0; i < n; i++) {
            printf("%d ", spPointGetIndex(points[i]));
		}
    printf("\n");
}
