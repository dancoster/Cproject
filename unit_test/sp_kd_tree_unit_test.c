/*
 * sp_kd_tree_unit_test.c
 *
 *  Created on: 28 באפר 2017
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

static SPPoint** spKD3DArrayPoints(){
	SPPoint** pointsArray = (SPPoint**) malloc (sizeof(SPPoint*)*5);
	double point_a[3] = {1,2,1};
	double point_b[3] = {123,70,2};
	double point_c[3] = {2,7,3};
	double point_d[3] = {9,11,4};
	double point_e[3] = {3,4,5};


	pointsArray[0] = spPointCreate(point_a,3,1);
	pointsArray[1] = spPointCreate(point_b,3,2);
	pointsArray[2] = spPointCreate(point_c,3,3);
	pointsArray[3] = spPointCreate(point_d,3,4);
	pointsArray[4] = spPointCreate(point_e,3,5);

	return pointsArray;
}

static bool tester(){
	SPPoint** pointsArray =spKD2DArrayPoints();
	SPKDArray* testArray = spKDArrayInit(pointsArray, 5, 2);
	SP_KD_TREE_SPLIT_METHOD splitMethod = MAX_SPREAD;
	SPKDTreeNode* tree = spKDTreeBuild(pointsArray, 5, 2, MAX_SPREAD);
}

//int main(){
//	SPPoint** pointsArray =spKD2DArrayPoints();
//	SPKDArray* testArray = spKDArrayInit(pointsArray, 5, 2);
//	SP_KD_TREE_SPLIT_METHOD splitMethod = MAX_SPREAD;
//	SPKDTreeNode* tree = spKDTreeBuild(pointsArray, 5, 2, MAX_SPREAD);
//
//	SPPoint* LLL = spKDTreeGetNodePoint(spKDTreeGetLeftNode(spKDTreeGetLeftNode(spKDTreeGetLeftNode(tree))));
//	SPPoint* LLR = spKDTreeGetNodePoint(spKDTreeGetRightNode(spKDTreeGetLeftNode(spKDTreeGetLeftNode(tree))));
//	SPPoint* LR = spKDTreeGetNodePoint(spKDTreeGetRightNode(spKDTreeGetLeftNode(tree)));
//	SPPoint* RR = spKDTreeGetNodePoint(spKDTreeGetRightNode(spKDTreeGetRightNode(tree)));
//	SPPoint* RL = spKDTreeGetNodePoint(spKDTreeGetLeftNode(spKDTreeGetRightNode(tree)));
//
//	int LLR0 = spPointGetAxisCoor(LLR,0);
//	int LLR1 = spPointGetAxisCoor(LLR,1);
//
//	int LLL0 = spPointGetAxisCoor(LLL,0);
//	int LLL1 = spPointGetAxisCoor(LLL,1);
//
//	int LR0 = spPointGetAxisCoor(LR,0);
//	int LR1 = spPointGetAxisCoor(LR,1);
//
//	int RR0 = spPointGetAxisCoor(RR,0);
//	int RR1 = spPointGetAxisCoor(RR,1);
//
//	int RL0 = spPointGetAxisCoor(RL,0);
//	int RL1 = spPointGetAxisCoor(RL,1);
//
//	printf("LLR0 %d \n", LLR0);
//	printf("LLR1 %d \n", LLR1);
//	printf("LLL0 %d \n", LLL0);
//	printf("LLL1 %d \n", LLL1);
//	printf("LR0 %d \n", LR0);
//	printf("LR1 %d \n", LR1);
//	printf("RR0 %d \n", RR0);
//	printf("RR1 %d \n", RR1);
//	printf("RL0 %d \n", RL0);
//	printf("RL1 %d \n", RL1);
//
//}
