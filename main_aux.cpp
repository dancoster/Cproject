#include "main_aux.h"
#include "unistd.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>

/**
 * A comparator which compares two SPPoints by i-th coordinate.
 *
 * @param a - an element to compare (casted to BPQueueElement*)
 * @param b - an element to compare (casted to BPQueueElement*)
 *
 * @return
 * -1 if a.value > b.value, 1 if a.value < b.value
 * if a.value == b.value, returns -1 if a.index < b.index, 1 if a.index > b.index
 */
int cmpfunc(const void *a, const void *b);

int extractFeatures(SPPoint*** siftDB, int numOfImgs, int* numOfFeaturesPerImage, int* numOfAllFeatures,
		SPConfig config, SP_CONFIG_MSG* msg) {
	if (siftDB==NULL || numOfImgs<1 || numOfFeaturesPerImage==NULL || numOfAllFeatures==NULL || config==NULL || msg==NULL) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return -1;
	}

	ImageProc ip(config);
	char path[STR_MAX_LENGTH+1] = {'\0'};
	FILE* featsFile=NULL;

	//extracting of sift features from images or from files
	bool isExtractMode = spConfigIsExtractionMode(config, msg);
	if (*msg != SP_CONFIG_SUCCESS) {	// if unsuccessful
		spLoggerPrintError(FUNCTION_ERROR,__FILE__,__func__,__LINE__);
		return -1;
	}
	if (isExtractMode) { //extracting from images and saving to feats files
		for (int i=0; i<numOfImgs; i++) {
			//get current image path
			if(spConfigGetImagePath(path, config ,i) != SP_CONFIG_SUCCESS) {		// if unsuccessful
				spLoggerPrintError(IMG_PATH_ERROR,__FILE__,__func__,__LINE__);
				return -1;
			}
			//get current image features
			siftDB[i]=ip.getImageFeatures(path,i,numOfFeaturesPerImage+i);
			if (siftDB[i] == NULL) {	// if unsuccessful
				spLoggerPrintError(FUNCTION_ERROR, __FILE__, __func__, __LINE__);
				return -1;
			}
			//get current image output file path
			if (spConfigGetFeatsPath(path, config, i) != SP_CONFIG_SUCCESS) {	// if unsuccessful
				spLoggerPrintError(IMG_PATH_ERROR,__FILE__,__func__,__LINE__);
				return -1;
			}

			//create output file
			featsFile = fopen(path,	"w");
			if (featsFile == NULL) { 	// if unsuccessful
				spLoggerPrintError(FEAT_CANNOT_OPEN_FILE,__FILE__,__func__,__LINE__);
				return -1;
			}
			//saving extracted features to feats files (one file per image)
			//the index and number of features for the current image is written in the first line of the feat file
			//if unsuccessful print error and return
			if (fprintf(featsFile, "%d\n", numOfFeaturesPerImage[i]) < 0) {
				spLoggerPrintError(FEAT_CANNOT_OPEN_FILE,__FILE__,__func__,__LINE__);
				fclose(featsFile);
				return -1;
			}

			//writing features to output file
			for (int j=0; j<numOfFeaturesPerImage[i]; j++) {
				for (int k=0; k<spConfigGetPCADim(config, msg); k++) {
					if (fprintf(featsFile, "%lf ", spPointGetAxisCoor(siftDB[i][j],k)) < 0) {
						spLoggerPrintError(FEAT_WRITE_ERROR,__FILE__,__func__,__LINE__);
						fclose(featsFile);
						return -1;
					}
				}
				if (fprintf(featsFile, "\n") < 0) {
					spLoggerPrintError(FEAT_WRITE_ERROR,__FILE__,__func__,__LINE__);
					fclose(featsFile);
					return -1;
				}
			}
			//updating num of all features
			*numOfAllFeatures += numOfFeaturesPerImage[i];
		}
		// closing the file
		fclose(featsFile);
	}

	else //extracting from feats files
	{
		spLoggerPrintInfo(EXTRACTS_FEATURES);
		int pcaNumComp = spConfigGetPCADim(config, msg);

		//getting features from files
		for (int i=0; i<numOfImgs; i++) {
			//get current image path
			if(spConfigGetFeatsPath(path, config ,i) != SP_CONFIG_SUCCESS) {		// if unsuccessful
				spLoggerPrintError(IMG_PATH_ERROR,__FILE__,__func__,__LINE__);
				return -1;
			}
			//insert image features from file to DB
			siftDB[i] = readFeaturesFromFile(i, numOfFeaturesPerImage+i, config, path, pcaNumComp);
			if (siftDB[i] == NULL) {	// if unsuccessful
				spLoggerPrintError(FUNCTION_ERROR, __FILE__, __func__, __LINE__);
				return -1;
			}
			//updating num of all features
			*numOfAllFeatures += numOfFeaturesPerImage[i];
		}
	}

	return 1;
}

int createAllFeaturesArray(SPPoint** allFeaturesArr, SPPoint*** siftDB, int numOfImgs, int* numOfFeaturesPerImage) {
	int k=0;
	for(int i=0; i<numOfImgs; i++){
		for(int j=0; j<numOfFeaturesPerImage[i]; j++) {
			allFeaturesArr[k] = spPointCopy(siftDB[i][j]);
			if (allFeaturesArr[k] == NULL) { 	//Allocation failure
				spLoggerPrintError(ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
				return k;
			}
			k++;
		}
	}
	return k;
}


SPKDTreeNode* buildFeaturesKDTree(SPPoint** allFeaturesArr, int numOfAllFeatures, SPConfig config, SP_CONFIG_MSG* msg) {
	if (allFeaturesArr==NULL || numOfAllFeatures<1 || config==NULL || msg==NULL) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}

	SP_KD_TREE_SPLIT_METHOD splitMethod = spConfigGetKDTreeSplitMethod(config ,msg);
//	printf("SPLIT METHOD: %d\n",splitMethod );
//	fflush(NULL);
	int dim = spConfigGetPCADim(config, msg);
	//if kdArr==NULL something
	SPKDTreeNode* featuresTree = spKDTreeBuild(allFeaturesArr, numOfAllFeatures, dim, splitMethod);
//	spLoggerPrintInfo("BUILT FEATS TREE SUCCESS");
//	fflush(NULL);
	if (featuresTree == NULL) {
		spLoggerPrintError(FUNCTION_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}
	return featuresTree;
}


int getQueryPath(char* path) {
	printf(ENTER_QUERY_PATH);
	fflush(NULL);
	int res = scanf("%s", path);
	if(res == EOF || res==0)
		return -1;
	return 1;
}

int* countKClosestPerFeature(SPKDTreeNode* featuresTree, int numOfImgs, char* queryPath,
		SPConfig config, SP_CONFIG_MSG* msg) {
	if (featuresTree==NULL || numOfImgs<1 || queryPath==NULL || config==NULL || msg==NULL) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}

	int spKNN = spConfigGetKNN(config, msg);
	int* counter = (int*) calloc(numOfImgs, sizeof(int));
	SPBPQueue* bpq = spBPQueueCreate(spKNN);
	BPQueueElement* element = (BPQueueElement*) malloc(sizeof(BPQueueElement));
	ImageProc ip(config);
	int nFeaturesQuery = 0;
	SPPoint** querySift = ip.getImageFeatures(queryPath, 0, &nFeaturesQuery);
	if (counter==NULL || querySift==NULL || bpq==NULL || element==NULL || spKNN==-1) {
		free(counter);
		spPoint1DDestroy(querySift, nFeaturesQuery);
		spBPQueueDestroy(bpq);
		free(element);
		spLoggerPrintError(ALLOCATION_ERROR,__FILE__,__func__,__LINE__);
		return NULL;
	}

//	printf("PASS 1\n");
//	printf("%d ",nFeaturesQuery);
//	fflush(NULL);

	// searching for KNN points for each query feature
	for(int i=0; i<nFeaturesQuery; i++) {
//		printf("QUERY[%d] point=(%d)\n",i,spPointGetIndex(querySift[i]));
//		fflush(NULL);
		// getting the KNN into the bpq
		if (spKDTreeNodeGetKNN(featuresTree, bpq, querySift[i]) == -1) { // search failed
			free(counter);
			spPoint1DDestroy(querySift, nFeaturesQuery);
			spBPQueueDestroy(bpq);
			free(element);
			spLoggerPrintError(KNN_ERROR,__FILE__,__func__,__LINE__);
			return NULL;
		}
//		printf("PASS 2\n");
//		fflush(NULL);
		// counting which images the KNN points belong to
		for(int j=0; j<spKNN; j++) {
			spBPQueuePeek(bpq, element);
			spBPQueueDequeue(bpq);
			counter[element->index]++;
		}
	}
//	printf("PASS 3\n");
//	fflush(NULL);
	// free allocations
	spPoint1DDestroy(querySift, nFeaturesQuery);
	spBPQueueDestroy(bpq);
	free(element);
	for(int i=0; i<numOfImgs; i++)
		printf("img%d: %d\n", i, counter[i]);
	printf("\n");
	fflush(NULL);
	return counter;
}

BPQueueElement* sortFeaturesCount(int* counter, int numOfImgs) {
	if (counter==NULL || numOfImgs<1) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}

	BPQueueElement* queryClosestImages = (BPQueueElement*) malloc (numOfImgs*sizeof(BPQueueElement));
	if (queryClosestImages == NULL) { //Allocation failure
		spLoggerPrintError(ALLOCATION_ERROR,__FILE__,__func__,__LINE__);
		return NULL;
	}

	for (int i=0; i<numOfImgs; i++) {
		BPQueueElement node = {i ,(double) counter[i]};
		queryClosestImages[i]=node;
	}
	qsort(queryClosestImages, numOfImgs, sizeof(BPQueueElement), cmpfunc);

	return queryClosestImages;
}

SPPoint** readFeaturesFromFile(int imgIndex, int* numFeatures, SPConfig config, char* path, int pcaNumComp) {
	//checks if the feats file is available
	if (access( path, F_OK ) == -1 ) {
		// file doesn't exist or wrong permission
		spLoggerPrintError(FEATS_ERROR,__FILE__,__func__,__LINE__);
		return NULL;
	}
	//read values from file
	FILE* featuresFile;
	featuresFile = fopen(path,"r");
	//checks if open failed
	if (featuresFile == NULL) {
		spLoggerPrintError(FEAT_READ_ERROR,__FILE__,__func__,__LINE__);
		fclose(featuresFile);
		return NULL;
	}

	//detect the number of feature of image imgIndex
	if (fscanf(featuresFile, " %d\n", numFeatures) <= 0) {
		spLoggerPrintError(NUM_FEATS_READING_ERROR,__FILE__,__func__,__LINE__);
		fclose(featuresFile);
		return NULL;
	}
//			printf("number of features: %d\n", *numFeatures);
//			fflush(NULL);

	//read features
	SPPoint** featuresArray = (SPPoint**) malloc((*numFeatures)*sizeof(SPPoint*));
	double* tempArray = (double*) malloc(pcaNumComp*sizeof(double));
	if (tempArray==NULL || featuresArray==NULL) {
		spLoggerPrintError(ALLOCATION_ERROR,__FILE__,__func__,__LINE__);
		free(tempArray);
		free(featuresArray);
		fclose(featuresFile);
		return NULL;
	}

//			printf("MSG 1\n");
//			fflush(NULL);

	for (int i=0; i<(*numFeatures); i++) {
		// reading double values to tempArray
		for (int j=0; j<pcaNumComp; j++) {
			if(fscanf(featuresFile, " %lf", tempArray+j) <= 0) {
				spLoggerPrintError(FEATS_READING_ERROR,__FILE__,__func__,__LINE__);
				free(featuresArray);
				free(tempArray);
				fclose(featuresFile);
				return NULL;
			}
		} // end reading double values to tempArray

//				printf("MSG 2\n");
//				fflush(NULL);
//				for (int r=0; r<pcaNumComp; r++) {
//					printf("%lf ", tempArray[r]);
//				}
//				fflush(NULL);

		// creating the i'th point from tempArray
		featuresArray[i] = spPointCreate(tempArray, pcaNumComp, imgIndex);
		if(featuresArray[i] == NULL) { // create point fail
			spLoggerPrintError(FUNCTION_ERROR,__FILE__,__func__,__LINE__);
			free(featuresArray);
			free(tempArray);
			fclose(featuresFile);
			return NULL;
		}
	}

	// free all allocations
	free(tempArray);
	fclose(featuresFile);

	return featuresArray;
}


int cmpfunc(const void *a, const void *b) {
	BPQueueElement* e1 = (BPQueueElement*)a; // casting pointer types
	BPQueueElement* e2 = (BPQueueElement*)b; // casting pointer types

	  if (e1->value > e2->value)
		  return -1;
	  else if (e1->value < e2->value)
		  return 1;
	  else {
		 if (e1->index > e2->index)
			 return 1;
		 return -1;
	  }
}

bool showResults(char* queryPath, BPQueueElement* queryClosestImages, SPConfig config, SP_CONFIG_MSG* msg) {
	if (queryPath==NULL || queryClosestImages==NULL || config==NULL || msg==NULL) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return false;
	}

	ImageProc ip(config);
	char imagePath[STR_MAX_LENGTH+1] = {'\0'};
	int numOfSimilarImages = spConfigGetNumOfSimilarImages(config, msg);

	if(spConfigMinimalGui(config, msg)) { // Minimal GUI
		for(int i=0; i<numOfSimilarImages; i++) {
			if (spConfigGetImagePath(imagePath, config, queryClosestImages[i].index) != SP_CONFIG_SUCCESS) {
				spLoggerPrintError(IMG_PATH_ERROR,__FILE__,__func__,__LINE__);
				return false;
			}
			ip.showImage(imagePath);
		}
	}
	else { // NON-Minimal GUI
		printf(BEST_CANDIDATES, queryPath);
		for(int i=0; i<numOfSimilarImages; i++) {
			if (spConfigGetImagePath(imagePath, config, queryClosestImages[i].index) != SP_CONFIG_SUCCESS) {
				spLoggerPrintError(IMG_PATH_ERROR,__FILE__,__func__,__LINE__);
				return false;
			}
			printf("%s\n",imagePath);
		}
	}
	return true;
}

void terminate(SPConfig config, SPPoint*** siftDB, int numOfImgs, int* numOfFeaturesPerImage,
		SPPoint** allFeaturesArr, int numOfAllFeatures, SPKDTreeNode* featuresTree) {
	printf(EXITING);
	bool onlyConfig = true;
	if (siftDB != NULL) {
		for (int i=0; i< numOfImgs; i++) {
			spPoint1DDestroy(siftDB[i], numOfFeaturesPerImage[i]);
		}
	free(siftDB);
	spLoggerPrintInfo(SIFT_DB_DESTROY);
	onlyConfig = false;
	}
	if (allFeaturesArr != NULL) {
		spPoint1DDestroy(allFeaturesArr, numOfAllFeatures);
		spLoggerPrintInfo(ALL_FEATURES_ARRAY_DESTROY);
		onlyConfig = false;
	}
	free(numOfFeaturesPerImage);
	if (featuresTree != NULL) {
			spKDTreeNodeDestroy(featuresTree);
		spLoggerPrintInfo(KD_TREE_DESTROY);
		onlyConfig = false;
	}
	if (onlyConfig) {  // before logger created
		spConfigDestroy(config);
		printf(CONFIG_DESTROY);
	}
	else { 				// after logger created
		spLoggerPrintInfo(CONFIG_DESTROY);
		spLoggerDestroy();
		printf(LOGGER_DESTROY);
	}
}
