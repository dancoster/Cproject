#include "SPKDTreeNode.h"
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <math.h>

struct sp_kdtree_node_t {
	int dim;
	double val;
	SPKDTreeNode* left;
	SPKDTreeNode* right;
	SPPoint* point;
};

SPKDTreeNode* spKDTreeBuild(SPPoint** points, int size, int dim, SP_KD_TREE_SPLIT_METHOD splitMethod) {
	if (points == NULL || size <= 0) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}
	SPKDArray* arr = spKDArrayInit(points, size, dim);
	if (arr == NULL) { // spLogger msg inside
		return NULL;
	}
//	spLoggerPrintInfo("SPKDArray INIT of tree SUCCESS");
//	printf("size of array: %d\n", spKDArrayGetSize(arr));
//	spKDArrayPrintMatrix(arr);
//	fflush(NULL);

	int splitDim = spKDTreeNodeSplitByDim(arr, 0, splitMethod);
//	printf("SPLIT DIM: %d\n", splitDim);
//	fflush(NULL);

	SPKDTreeNode* result = spKDTreeNodeCreate(arr, splitDim, splitMethod); // spLogger msg inside
	spKDArrayDestroy(arr);
//	spLoggerPrintInfo("spKDTreeBuild SUCCESS\n");
	return result;
}

SPKDTreeNode* spKDTreeNodeCreate(SPKDArray* arr, int dim, SP_KD_TREE_SPLIT_METHOD splitMethod) {
	if (arr==NULL || dim<0) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}

	SPKDTreeNode* node;
	SPKDArray** splittedArray;
	int sizeLeft, medianIndex;

//	printf("size of ARR: %d\n",spKDArrayGetSize(arr));
//	fflush(NULL);
	if (spKDArrayGetSize(arr) == 1) { // if the node is a leaf
		return spKDTreeNodeCreateLeaf(arr); // spLogger msg inside
	}

	node = (SPKDTreeNode*) malloc(sizeof(SPKDTreeNode));
	if (node == NULL) { //Allocation failure
		spLoggerPrintError(ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		spKDArrayDestroy(arr);
		return NULL;
	}

	splittedArray = spKDArraySplit(arr, dim-1);
//	spLoggerPrintInfo("spKDArraySplit SUCCESS\n");
//	fflush(NULL);
	if (splittedArray == NULL) { //split function failure, spLogger msg inside
		spKDArrayDestroy(arr);
		free(node);
		return NULL;
	}

	sizeLeft = spKDArrayGetSize(splittedArray[LEFT]);
	medianIndex = spKDArrayGetSortedMatrix(splittedArray[LEFT])[dim-1][sizeLeft-1];
//	printf("median: %d\n", medianIndex);
//	fflush(NULL);
	node->dim = dim;
//	printf("node dim: %d\n", node->dim);
//	fflush(NULL);
	node->val = spPointGetAxisCoor((spKDArrayGetPoints(splittedArray[LEFT]))[medianIndex], dim-1);
//	printf("node meidan val: %f\n", node->val);
//	fflush(NULL);
	node->left = spKDTreeNodeCreate(splittedArray[LEFT],
			spKDTreeNodeSplitByDim(splittedArray[LEFT], dim, splitMethod), splitMethod);
	node->right = spKDTreeNodeCreate(splittedArray[RIGHT],
			spKDTreeNodeSplitByDim(splittedArray[RIGHT], dim, splitMethod), splitMethod);
	node->point = NULL;

//	fflush(NULL);
//	printf("success note return 1\n");
//	fflush(NULL);

	// free LEFT RIGHT arrays?
	spKDArrayDestroy(splittedArray[LEFT]);
	spKDArrayDestroy(splittedArray[RIGHT]);
	free(splittedArray);
	fflush(NULL);

//	fflush(NULL);
//	printf("success note return 2\n");
//	fflush(NULL);

	if (node->left == NULL || node->right == NULL) { //if create failed, spLogger msg inside
		spKDTreeNodeDestroy(node);
		return NULL;
	}

	return node;
}

SPKDTreeNode* spKDTreeNodeCreateLeaf(SPKDArray* arr) {
	if (arr == NULL) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}

//	printf("ENTERED CREATE LEAF\n");
//	fflush(NULL);

	SPKDTreeNode* node = (SPKDTreeNode*) malloc(sizeof(SPKDTreeNode));
	if (node == NULL) { //Allocation failure
		spLoggerPrintError(ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		spKDArrayDestroy(arr);
		return NULL;
	}

	node->dim = INVALID;
	node->val = INVALID;
	node->left = NULL;
	node->right = NULL;
	node->point = spPointCopy(spKDArrayGetPoints(arr)[0]);

	if (node->point == NULL) { //Allocation failure
		spLoggerPrintError(ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		free(node);
		return NULL;
	}

	return node;
}

int spKDTreeNodeSplitByDim(SPKDArray* arr, int dim, SP_KD_TREE_SPLIT_METHOD splitMethod) {
	if (arr==NULL || dim<0) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return INVALID;
	}

	if (splitMethod == MAX_SPREAD) {
//		printf("ENTERED MAX SPREAD\n");
		return spKDTreeNodeGetMaxSpreadDim(arr); // spLogger msg inside
	}
	if (splitMethod == RANDOM) {
//		printf("ENTERED RANDOM\n");
		return (rand() % spKDArrayGetDim(arr)); // spLogger msg inside
	}
	if (splitMethod == INCREMENTAL) {
//		printf("ENTERED INCREMENTAL\n");
		return ((dim+1) % spKDArrayGetDim(arr)); // spLogger msg inside
	}
	return INVALID;
}

int spKDTreeNodeGetMaxSpreadDim(SPKDArray* arr) {
	if (arr == NULL) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return -1;
	}

	int maxSpreadDim = 0;
	double maxSpread = 0;
	double currSpread = 0;
	int firstIndex, lastIndex; // first and last index of the point in i'th row of sortedMatrix
	int n = spKDArrayGetSize(arr);

	for (int i=0; i<spKDArrayGetDim(arr); i++) {
		firstIndex = (spKDArrayGetSortedMatrix(arr))[i][0];
		lastIndex = (spKDArrayGetSortedMatrix(arr))[i][n-1];
		//calculating the i'th dim spread
		currSpread = (spPointGetAxisCoor((spKDArrayGetPoints(arr))[lastIndex], i)
				- spPointGetAxisCoor((spKDArrayGetPoints(arr))[firstIndex], i));
//		printf("row %d spread: %f\n", i, currSpread);
//		fflush(NULL);
		if (currSpread > maxSpread) { //checking if new max spread is found
			maxSpread = currSpread;
			maxSpreadDim = i;
		}
	}
//	printf("MAX SPREAD DIM: %d, SPREAD: %f\n", maxSpreadDim+1, maxSpread);
//	fflush(NULL);
	return maxSpreadDim+1;
}

void spKDTreeNodeDestroy(SPKDTreeNode* root) {
	if (root == NULL) {
		return;
	}

	spKDTreeNodeDestroy(root->left);
	spKDTreeNodeDestroy(root->right);

	spPointDestroy(root->point);
	free(root);
	root = NULL;
}

int spKDTreeNodeGetKNN(SPKDTreeNode* root, SPBPQueue* bpq, SPPoint* point) {
	if (root==NULL || bpq==NULL || point==NULL) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return -1;
	}

	if(!spKDTreeNodeSearchKNN(bpq, root, point)) { // search failed, spLogger msg inside
		return -1;
	}
	else
		return 1;
}

bool spKDTreeNodeSearchKNN(SPBPQueue* bpq, SPKDTreeNode* curr, SPPoint* point) {
	if (bpq==NULL || curr==NULL || point==NULL) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return false;
	}

//	printf("PASS 11\n");
//	fflush(NULL);

	if (curr->dim == INVALID) { // if curr is a leaf
//		printf("PASS LEAF\n");
//		printf("\n");
//		fflush(NULL);
		if (spBPQueueEnqueue(bpq, spPointGetIndex(curr->point),   		//!!IMPORTANT!! check what to do if malloc fails
				spPointL2SquaredDistance(curr->point, point))			//at bpq element inside the func spBPQueueEnqueue
				== SP_BPQUEUE_OUT_OF_MEMORY) {							//OR the enqueue failed due to full queue or invalid args
			spLoggerPrintError(ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
			return false;
		}
		else
			return true;
	}

//	printf("PASS 12\n");
//	fflush(NULL);

	bool searchedSide; // side searched
	if (spPointGetAxisCoor(point, curr->dim-1) <= curr->val) { // search left subtree
		if (!spKDTreeNodeSearchKNN(bpq, curr->left, point)) {
			return false;
			}
		searchedSide = LEFT;
		}
	else { 													// search right subtree
		if (!spKDTreeNodeSearchKNN(bpq, curr->right, point)) {
			return false;
		}
		searchedSide = RIGHT;
	}

//	printf("PASS 13\n");
//	fflush(NULL);
//	printf("CURR DIM: %d\n",curr->dim);
//	fflush(NULL);

	if (!spBPQueueIsFull(bpq)
			|| pow((curr->val - spPointGetAxisCoor(point, curr->dim-1)), 2)
					< spBPQueueMaxValue(bpq)) {
		if (searchedSide == LEFT) { // searching right side
			return spKDTreeNodeSearchKNN(bpq, curr->right, point);
		}
		else { 					// searching left side
			return spKDTreeNodeSearchKNN(bpq, curr->left, point);
		}
	}
//	printf("PASS 14\n");
//	fflush(NULL);
	//spLoggerPrintInfo(DONE_S_KNN);
	return true;
}
