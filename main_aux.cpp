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
		return NULL;
	}

	ImageProc ip(config);
	char path[STR_MAX_LENGTH+1] = {'\0'};
	FILE* featsFile=NULL;

	//extracting of sift features from images or from files
	if (spConfigIsExtractionMode(config, msg)) { //extracting from images and saving to feats files
		for (int i=0; i<numOfImgs; i++) {
			//get current image path
			*msg = spConfigGetImagePath(path, config ,i);
			//if unsuccessful print error and return
			if(*msg!=SP_CONFIG_SUCCESS) {
				spLoggerPrintError(IMAGE_PATH_ERROR,__FILE__,__func__,__LINE__);
				return -1;
			}
			//get current image features
			siftDB[i]=ip.getImageFeatures(path,i,numOfFeaturesPerImage+i);
			//if unsuccessful free resources and return
			if(siftDB[i]==NULL) {
				return -1;
			}
			//get current image output file path
			*msg = spConfigGetFeatsPath(path, config, i);
			//if unsuccessful print error and return
			if(*msg!=SP_CONFIG_SUCCESS) {
				spLoggerPrintError(IMAGE_PATH_ERROR,__FILE__,__func__,__LINE__);
				return -1;
			}

			//create output file
			featsFile = fopen(path,"w");
			//if unsuccessful print error and return
			if(featsFile==NULL) {
				spLoggerPrintError(FEAT_CANNOT_OPEN_FILE,__FILE__,__func__,__LINE__);
				return -1;
			}
			//saving extracted features to feats files (one file per image)
			//the index and number of features for the current image is written in the first line of the feat file
			//if unsuccessful print error and return
			if(fprintf (featsFile,"%d\n",numOfFeaturesPerImage[i])<0) {
				spLoggerPrintError(FEAT_CANNOT_OPEN_FILE,__FILE__,__func__,__LINE__);
				fclose(featsFile);
				return -1;
			}

			//writing features to output file
			for (int j=0; j<numOfFeaturesPerImage[i]; j++) {
				for (int k=0; k<spConfigGetPCADim(config, msg); k++) {
					if(fprintf(featsFile, "%lf ",spPointGetAxisCoor(siftDB[i][j],k))<0) {
						spLoggerPrintError(FEAT_WRITE_ERROR,__FILE__,__func__,__LINE__);
						fclose(featsFile);
						return -1;
					}
				}
				if(fprintf(featsFile, "\n")<0) {
					spLoggerPrintError(FEAT_WRITE_ERROR,__FILE__,__func__,__LINE__);
					fclose(featsFile);
					return -1;
				}
			}
			//updating num of all features
			*numOfAllFeatures += numOfFeaturesPerImage[i];
		}
		fclose(featsFile);
	}

	else //extracting from feats files
	{
		spLoggerPrintInfo(EXTRACTS_FEATURES);
		int pcaNumComp = spConfigGetPCADim(config, msg);

		//getting features from files
		for (int i=0; i<numOfImgs; i++) {
			//get current image path
			*msg = spConfigGetFeatsPath(path, config ,i);

			//insert image features from file to DB
			siftDB[i] = readsFaturesFromFile(i, numOfFeaturesPerImage+i, config, path, pcaNumComp);
			*numOfAllFeatures += numOfFeaturesPerImage[i];
		}
	}

	return 1;
}

int createAllFeaturesArray(SPPoint** allFeaturesArr, SPPoint*** siftDB, int numOfImgs, int* numOfFeaturesPerImage) {
	int k=0;
	for(int i=0; i<numOfImgs; i++){
		for(int j=0; j<numOfFeaturesPerImage[i];j++) {
			allFeaturesArr[k++] = spPointCopy(siftDB[i][j]);
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
	if (featuresTree==NULL) {
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
	return 0;
}

int* countKClosestPerFeature(SPKDTreeNode* featuresTree, int numOfImgs, char* queryPath,
		SPConfig config, SP_CONFIG_MSG* msg) {
	if (featuresTree==NULL || numOfImgs<1 || queryPath==NULL || config==NULL || msg==NULL) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}

	int spKNN = spConfigGetKNN(config, msg);
	int* counter = (int*)calloc(numOfImgs,sizeof(int));
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

	for(int i=0; i<nFeaturesQuery; i++) {
//		printf("QUERY[%d] point=(%d)\n",i,spPointGetIndex(querySift[i]));
//		fflush(NULL);
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
		for(int j=0; j<spKNN; j++) {
			spBPQueuePeek(bpq, element);
			spBPQueueDequeue(bpq);
			counter[element->index]++;
		}
	}
//	printf("PASS 3\n");
//	fflush(NULL);
	spPoint1DDestroy(querySift, nFeaturesQuery);
	spBPQueueDestroy(bpq);
	free(element);
	for(int i=0; i<numOfImgs; i++)
		printf("img%d: %d\n",i,counter[i]);
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
		BPQueueElement node = {i,(double) counter[i]};
		queryClosestImages[i]=node;
	}
	qsort(queryClosestImages, numOfImgs, sizeof(BPQueueElement), cmpfunc);

	return queryClosestImages;
}
SPPoint** readsFaturesFromFile(int imgIndex, int* numFeatures, SPConfig config, char* path, int pcaNumComp){
			//checks if the feats file is availble
			if( access( path, F_OK ) == -1 ) {
			    // file doesn't exist
				spLoggerPrintError(FEATS_ERROR,__FILE__,__func__,__LINE__);
				return NULL;
			}
			//read values from file
			FILE* featuresFile;
			featuresFile = fopen(path,"r");

			//checks if read failed
			if (featuresFile == NULL){
				spLoggerPrintError(FEAT_READ_ERROR,__FILE__,__func__,__LINE__);
				fclose(featuresFile);
			}

			//detect the number of feature of image imgIndex
			fscanf(featuresFile, " %d", &numFeatures[imgIndex]);
			printf("number of features %d\n", numFeatures[imgIndex]);

			//read features
			SPPoint** featuresArray;
			featuresArray = (SPPoint**) malloc((*numFeatures)*sizeof(SPPoint*));

			for (int j =0 ; j < (*numFeatures); j++ ){
				double *tempArray= (double *) malloc(sizeof(double)*pcaNumComp);
//				for (int t =0; t <pcaNumComp; t++){
//					tempArray[t] = t;
//					printf("pre readValues: %f", tempArray[t]);
//				}

				for (int i=0; i<pcaNumComp; i++){
					printf("post readValues: %lf\n");

					if(fscanf(featuresFile, " %lf", tempArray+i) <= 0){
						spLoggerPrintError(FEATS_READING_ERROR,__FILE__,__func__,__LINE__);
						free(featuresArray);
						free(tempArray);
						fclose(featuresFile);
						return NULL;
					}
					featuresArray[i] = spPointCreate(tempArray, pcaNumComp, imgIndex);
					free(tempArray);

					//malloc fail in featuresArray
					if(featuresArray[i] == NULL){
						spLoggerPrintError(ALLOCATION_ERROR,__FILE__,__func__,__LINE__);
						free(featuresArray);
						break;
					}
				}
//				int readValues = fread(tempArray, sizeof(double), pcaNumComp, featuresFile);
//					printf("ValuereadValues: %d\n", readValues);
//					printf("%s\n", path);

//				printf("%f", tempArray[0]);

				//malloc failure in read readValues
//				if (readValues != pcaNumComp){
//					spLoggerPrintError(FEATS_READING_ERROR,__FILE__,__func__,__LINE__);
//					free(tempArray);
//					break;

			fclose(featuresFile);
			return(featuresArray);
			//TODO: add malloc faliure

		}
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

void showResults(char* queryPath, BPQueueElement* queryClosestImages, SPConfig config, SP_CONFIG_MSG* msg) {
	if (queryPath==NULL || queryClosestImages==NULL || config==NULL || msg==NULL) {
		spLoggerPrintError(INVALID_ARGUMENTS_ERROR, __FILE__, __func__, __LINE__);
		return;
	}

	ImageProc ip(config);
	char imagePath[STR_MAX_LENGTH+1] = {'\0'};
	int numOfSimilarImages = spConfigGetNumOfSimilarImages(config, msg);

	if(spConfigMinimalGui(config, msg)) { // Minimal GUI
		for(int i=0; i<numOfSimilarImages; i++) {
			spConfigGetImagePath(imagePath, config, queryClosestImages[i].index);
			ip.showImage(imagePath);
		}
	}
	else { // NON-Minimal GUI
		printf(BEST_CANDIDATES, queryPath);
		for(int i=0; i<numOfSimilarImages; i++) {
			spConfigGetImagePath(imagePath, config, queryClosestImages[i].index);
			printf("%s\n",imagePath);
		}
	}
}

void terminate(SPConfig config, SPPoint*** siftDB, int numOfImgs, int* numOfFeaturesPerImage,
		SPPoint** allFeaturesArr, int numOfAllFeatures, SPKDTreeNode* featuresTree) {
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

	printf(EXITING);
}
