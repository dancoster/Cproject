#ifndef SPKDTREENODE_H_
#define SPKDTREENODE_H_

#include "SPKDArray.h"
#include "SPBPriorityQueue.h"
#include "SPConfig.h"

#define INVALID -1

/** A data-structure which is used for storing image features **/
typedef struct sp_kdtree_node_t SPKDTreeNode;

/**
 * Allocates a new KDTree in the memory.
 * Given points array, size of the array and split method.
 * Creating a new KDArray from the points array and using spKDTreeNodeCreate to build the KDTree
 *
 * @param points		- array of points to build the KDArray from
 * @param size			- the size of points array
 * @param dim 			- spPCADimension from the config used for init the KDArray
 * @param splitMethod 	- KDTree split method used to split the array
 *
 * @return
 * NULL in case of allocation failure, or points==NULL or size<=0
 * Otherwise, the new KDTree is returned
 */
SPKDTreeNode* spKDTreeBuild(SPPoint** points, int size, int dim, SP_KD_TREE_SPLIT_METHOD splitMethod);

/**
 * Creates a new KDTreeNode from the KDArray,
 * and recursively builds the rest of the tree by splitting the array by <dim>.
 * if the size of the KDArray is 1, creating a leaf by calling spKDTreeNodeCreateLeaf function.
 *
 * @param arr 			- the KDArray used to build the tree from
 * @param dim 			- the dimension used to split the array
 * @param splitMethod 	- KDTree split method used to split the array
 *
 * @return
 * NULL if an error occurred, or arr==NULL or dim<0 (first call to this function with dim = 0)
 * Otherwise, a new KDTreeNode is returned:
 * dim = <dim>
 * value = the value of median point according to <dim>
 * left = the left size of the KDArray
 * right = the right side of the KDArray
 * point = NULL
 *
 * if the KDTreeNode is a leaf, then a new KDTreeNode with leaf values is returned
 * (check spKDTreeNodeCreateLeaf for more info)
 */
SPKDTreeNode* spKDTreeNodeCreate(SPKDArray* arr, int dim, SP_KD_TREE_SPLIT_METHOD splitMethod);

/**
 * Creates a new KDTreeNode leaf.
 *
 * @param arr - the KDArray of size 1 used to build the leaf from
 *
 * @return
 * NULL in case of allocation failure, or arr==NULL
 * Otherwise, a new KDTreeNode is returned:
 * dim = -1
 * value = -1
 * left = NULL
 * right = NULL
 * point = the only point in the KDArray <arr>
 */
SPKDTreeNode* spKDTreeNodeCreateLeaf(SPKDArray* arr);

/**
 * Returns the dimension used to split the KDArray
 * according to the splitting method criteria:
 * MAX_SPREAD 	- the i-th dimension with the highest spread (= the difference between the maximum and the minimum
 * 				  of the i-th coordinate of all points)
 * RANDOM		- choose a random dimension
 * INCREMENTAL	- the dimension such that (<dim>+1)%d (= d is the max dimension)
 *
 * @param arr 			- the KDArray to split
 * @param dim 			- the previous dimension used to split by (needed for INCREMENTAL)
 * @param splitMethod 	- KDTree split method used to split the array
 *
 * @return
 * NULL if arr==NULL, or dim<0 (first call to this function with dim = 0)
 * Otherwise, the next dimension to split the KDArray by
 */
int spKDTreeNodeSplitByDim(SPKDArray* arr, int dim, SP_KD_TREE_SPLIT_METHOD splitMethod);

/**
 * A getter for max spread dimension of KDArray.
 *
 * @param arr - The source KDArray
 *
 * @return
 * -1 if arr==NULL
 * Otherwise, the max spread dimension is returned
 */
int spKDTreeNodeGetMaxSpreadDim(SPKDArray* arr);

/**
 * Frees all memory allocation associated with KDTree.
 *
 * @param root 	- the KDTree root to destroy
 *
 * if node is NULL nothing happens.
 */
void spKDTreeNodeDestroy(SPKDTreeNode* root);

/**
 * Initializing procedure for K-Nearest Neighbors Search.
 * creating a BPQueue, with maximum size of spKNN, which used to store
 * the K-Nearest Neighbors candidates.
 *
 * @param root 	- the KDTree root to search in
 * @param point - the point used to search the K-Nearest Neighbors for
 * @param spKNN - the maximum size of the BPQueue
 *
 * @return
 * NULL if the search failed
 * Otherwise, the BPQueue which stores the KNN is returned
 */
int spKDTreeNodeGetKNN(SPKDTreeNode* root, SPBPQueue* bpq, SPPoint* point);

/**
 * Searches for K-Nearest Neighbors of a given point in the given KDTree
 * and stores them in the given BPQueue.
 *
 * @param bpq	- the BPQueue used to store the K-Nearest Neighbors in
 * @param curr	- the current KDTreeNode used for the recursive search (starting from the root)
 * @param point 		- the point used to search the K-Nearest Neighbors for
 *
 * @return
 * True if the search succeeded, False if an error occurred.
 */
bool spKDTreeNodeSearchKNN(SPBPQueue* bpq, SPKDTreeNode* curr, SPPoint* point);

//simple getter of Left
SPKDTreeNode* spKDTreeGetLeftNode(SPKDTreeNode* tree);

//simple getter of Right
SPKDTreeNode* spKDTreeGetRightNode(SPKDTreeNode* tree);

//simple getter of point in a node
SPPoint* spKDTreeGetNodePoint(SPKDTreeNode* node);
#endif /* SPKDTREENODE_H_ */
