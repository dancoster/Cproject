/*
 * sp_kd_tree_unit_test.c
 *
 *  Created on: 28 ���� 2017
 *      Author: Dan Coster
 */


#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "unit_test_util.h" //SUPPORTING MACROS ASSERT_TRUE/ASSERT_FALSE etc..
#include "../SPLogger.h"
#include "../SPKDTreeNode.h"


// This is a helper function which checks if two files are identical
static bool identicalFiles(const char* fname1, const char* fname2) {
	FILE *fp1, *fp2;
	fp1 = fopen(fname1, "r");
	fp2 = fopen(fname2, "r");
	char ch1 = EOF, ch2 = EOF;

	if (fp1 == NULL) {
		return false;
	} else if (fp2 == NULL) {
		fclose(fp1);
		return false;
	} else {
		ch1 = getc(fp1);
		ch2 = getc(fp2);

		while ((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2)) {
			ch1 = getc(fp1);
			ch2 = getc(fp2);
		}
		fclose(fp1);
		fclose(fp2);
	}
	if (ch1 == ch2) {
		return true;
	} else {
		return false;
	}
}

static SPPoint** spKD2DArrayPoints(){
	SPPoint** pointsArray = (SPPoint**) malloc (sizeof(SPPoint*)*5);
	double point_a[2] = {1,2};
	double point_b[2] = {123,70};
	double point_c[2] = {2,7};
	double point_d[2] = {9,11};
	double point_e[2] = {3,4};


	pointsArray[0] = spPointCreate(point_a,2,1);
	pointsArray[1] = spPointCreate(point_b,2,2);
	pointsArray[2] = spPointCreate(point_c,2,3);
	pointsArray[3] = spPointCreate(point_d,2,4);
	pointsArray[4] = spPointCreate(point_e,2,5);

	return pointsArray;
}

static bool maxSpreadTreeTest(){
	SPPoint** pointsArray =spKD2DArrayPoints();
	SPKDArray* testArray = spKDArrayInit(pointsArray, 5, 2);
	SP_KD_TREE_SPLIT_METHOD splitMethod = MAX_SPREAD;
	SPKDTreeNode* tree = spKDTreeBuild(pointsArray, 5, 2, splitMethod);

	SPPoint* LLL = spKDTreeGetNodePoint(spKDTreeGetLeftNode(spKDTreeGetLeftNode(spKDTreeGetLeftNode(tree))));
	SPPoint* LLR = spKDTreeGetNodePoint(spKDTreeGetRightNode(spKDTreeGetLeftNode(spKDTreeGetLeftNode(tree))));
	SPPoint* LR = spKDTreeGetNodePoint(spKDTreeGetRightNode(spKDTreeGetLeftNode(tree)));
	SPPoint* RR = spKDTreeGetNodePoint(spKDTreeGetRightNode(spKDTreeGetRightNode(tree)));
	SPPoint* RL = spKDTreeGetNodePoint(spKDTreeGetLeftNode(spKDTreeGetRightNode(tree)));

	ASSERT_TRUE(spPointGetAxisCoor(LLR,0) == 3);
	ASSERT_TRUE(spPointGetAxisCoor(LLR,1) == 4);
	ASSERT_TRUE(spPointGetAxisCoor(LLL,0) == 1);
	ASSERT_TRUE(spPointGetAxisCoor(LLL,1) == 2);
	ASSERT_TRUE(spPointGetAxisCoor(LR,0) == 2);
	ASSERT_TRUE(spPointGetAxisCoor(LR,1) == 7);
	ASSERT_TRUE(spPointGetAxisCoor(RR,0) == 123);
	ASSERT_TRUE(spPointGetAxisCoor(RR,1) == 70);
	ASSERT_TRUE(spPointGetAxisCoor(RL,0) == 9);
	ASSERT_TRUE(spPointGetAxisCoor(RL,1) == 11);

	spKDArrayDestroy(testArray);
	spPoint1DDestroy(pointsArray, 5);
	spKDTreeNodeDestroy(tree);
	spPointDestroy(LLL);
	spPointDestroy(LLR);
	spPointDestroy(LR);
	spPointDestroy(RR);
	spPointDestroy(RL);

	return true;
}


//int main(){
//	RUN_TEST(maxSpreadTreeTest);
//	printf("*********************************************\n");
//}
