/*
 * sp_kd_array_unit_test.c
 *
 *  Created on: 28 באפר 2017
 *      Author: Dan Coster
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "unit_test_util.h" //SUPPORTING MACROS ASSERT_TRUE/ASSERT_FALSE etc..
#include "../SPLogger.h"
#include "../SPKDArray.h"


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

static SPPoint** spKDArrayPoints(){
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
bool IntializationTest(){
	SPPoint** pointsArray =spKDArrayPoints();
	SPKDArray* testArray = spKDArrayInit(pointsArray, 5, 2);
	ASSERT_TRUE(spKDArrayGetDim(testArray) == 2);
	ASSERT_TRUE(spKDArrayGetSize(testArray) == 5);

	spKDArrayDestroy(testArray);
	spPoint1DDestroy(pointsArray, 5);
	return true;
}


bool SortingTest(){
	SPPoint** pointsArray =spKDArrayPoints();
	SPKDArray* testArray = spKDArrayInit(pointsArray, 5, 2);

	//Axis 0, Correct indexes Order: 0 2 4 3 1
	ASSERT_TRUE((spKDArrayGetSortedMatrix(testArray)[0][0]) == 0);
	ASSERT_TRUE((spKDArrayGetSortedMatrix(testArray)[0][1]) == 2);
	ASSERT_TRUE((spKDArrayGetSortedMatrix(testArray)[0][2]) == 4);
	ASSERT_TRUE((spKDArrayGetSortedMatrix(testArray)[0][3]) == 3);
	ASSERT_TRUE((spKDArrayGetSortedMatrix(testArray)[0][4]) == 1);

	//Axis 1, Correct indexes Order: 0 4 2 3 1
	ASSERT_TRUE((spKDArrayGetSortedMatrix(testArray)[1][0]) == 0);
	ASSERT_TRUE((spKDArrayGetSortedMatrix(testArray)[1][1]) == 4);
	ASSERT_TRUE((spKDArrayGetSortedMatrix(testArray)[1][2]) == 2);
	ASSERT_TRUE((spKDArrayGetSortedMatrix(testArray)[1][3]) == 3);
	ASSERT_TRUE((spKDArrayGetSortedMatrix(testArray)[1][4]) == 1);

	spKDArrayDestroy(testArray);
	spPoint1DDestroy(pointsArray, 5);

	return true;

}

bool SplittingTest(){
	SPPoint** pointsArray =spKDArrayPoints();
	SPKDArray* testArray = spKDArrayInit(pointsArray, 5, 2);

	SPKDArray** splittedArrays = spKDArraySplit(testArray, 0);

	SPKDArray* leftArray = splittedArrays[LEFT];
	SPKDArray* rightArray = splittedArrays[RIGHT];

	//Axis 1, LEFT, Correct indexes Order: 0 2 1
	ASSERT_TRUE((spKDArrayGetSortedMatrix(leftArray)[1][0]) == 0);
	ASSERT_TRUE((spKDArrayGetSortedMatrix(leftArray)[1][1]) == 2);
	ASSERT_TRUE((spKDArrayGetSortedMatrix(leftArray)[1][2]) == 1);

	//Axis 1, RIGHT, Correct indexes Order: 1 0
	ASSERT_TRUE((spKDArrayGetSortedMatrix(rightArray)[1][0]) == 1);
	ASSERT_TRUE((spKDArrayGetSortedMatrix(rightArray)[1][1]) == 0);

	spKDArrayDestroy(leftArray);
	spKDArrayDestroy(rightArray);
	spKDArrayDestroy(testArray);
	spPoint1DDestroy(pointsArray, 5);
	free(splittedArrays);

	return true;
}

//int main() {
//	RUN_TEST(IntializationTest);
//	printf("*********************************************\n");
//	RUN_TEST(SortingTest);
//	printf("*********************************************\n");
//	RUN_TEST(SplittingTest);
//	printf("*********************************************\n");
//	printf("test ok!");
//	return 0;
//}

