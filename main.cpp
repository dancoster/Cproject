#include "main_aux.h"

int main(int argc, char* argv[]) {
	char config_filename[STR_MAX_LENGTH+1] = DEFAULT_CONFIG_FILENAME; // default config file
	SPConfig config;
	SP_CONFIG_MSG msg;

	// checking command line arguments
	if (argc > 1) { // checks if config file entered?
		if (argc == 3 && strcmp(argv[1], "-c") == 0) { // correct num of args && -c flag
			strcpy(config_filename, argv[2]);
		} else { //invalid args
			printf(INVALID_COMMAND_LINE_ERROR, config_filename);
			return -1;
		}
	}

	// creating config
	config = spConfigCreate(config_filename, &msg);
	if (msg == SP_CONFIG_CANNOT_OPEN_FILE) {
		if (argc > 1) { // used config filename from command line
			printf(CONFIG_CANNOT_OPEN_FILE, "", config_filename);
		}
		else {  // used default config filename
			printf(CONFIG_CANNOT_OPEN_FILE, "default", config_filename);
		}
	}

	if (msg != SP_CONFIG_SUCCESS) { // create fail, spLogger msg inside
		printf(CONFIG_ERROR);
//		spConfigDestroy(config);
		terminate(config,NULL,0,NULL,NULL,0,NULL);
		return -1;
	}
	else {
		printf(CONFIG_CREATED); // create success
		spConfigPrintInfo(config);
	}
	//-------------------

	// creating logger
	char* logger_filename = spConfigGetLoggerFilename(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		printf("%s %s\n", LOGGER_FILENAME, COULDNT_BE_RESOLVED);
//		spConfigDestroy(config);
		terminate(config,NULL,0,NULL,NULL,0,NULL);
		return -1;
	}

	SP_LOGGER_LEVEL logger_level = spConfigGetLoggerLevel(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		printf("%s %s\n", LOGGER_LEVEL, COULDNT_BE_RESOLVED);
//		spConfigDestroy(config);
		terminate(config,NULL,0,NULL,NULL,0,NULL);
		return -1;
	}

	if (strcmp(logger_filename, "stdout") == 0) { // logger file name is stdout
		logger_filename = NULL;
	}
	if (spLoggerCreate(logger_filename, logger_level) != SP_LOGGER_SUCCESS) {
		printf("%s\n", LOGGER_ERROR);
//		spConfigDestroy(config);
		terminate(config,NULL,0,NULL,NULL,0,NULL);
		return -1;
	}
	spLoggerPrintInfo(LOGGER_CREATED);
	//-------------------

	int numOfImgs = spConfigGetNumOfImages(config ,&msg);
//	try
//	{
		int* numOfFeaturesPerImage = (int*) malloc(numOfImgs*sizeof(int));
		SPPoint*** siftDB = (SPPoint***) malloc(numOfImgs*sizeof(SPPoint**));
		//check allocation error
		if(numOfFeaturesPerImage==NULL || siftDB==NULL) {
			spLoggerPrintError(ALLOCATION_ERROR,__FILE__,__func__,__LINE__);
			terminate(config,siftDB,numOfImgs,numOfFeaturesPerImage,NULL,0,NULL);
			return -1;
		}
		int numOfAllFeatures=0;
		//creating the sift Database

		//if unsuccessful terminate program
		if (extractFeatures(siftDB, numOfImgs, numOfFeaturesPerImage, &numOfAllFeatures, config, &msg) == -1) {
			terminate(config,siftDB,numOfImgs,numOfFeaturesPerImage,NULL,0,NULL);
			return -1;
		}
		spLoggerPrintInfo(SIFT_DB_CREATED);
		fflush(NULL);
		SPPoint** allFeaturesArr = (SPPoint**) malloc(numOfAllFeatures*sizeof(SPPoint*));
		//check allocation error
		if(allFeaturesArr==NULL) {
			spLoggerPrintError(ALLOCATION_ERROR,__FILE__,__func__,__LINE__);
			terminate(config,siftDB,numOfImgs,numOfFeaturesPerImage,NULL,0,NULL);
			return -1;
		}
		// conversion of siftDB into one big sift array for all imgs
		if(createAllFeaturesArray(allFeaturesArr, siftDB, numOfImgs, numOfFeaturesPerImage) == numOfAllFeatures) {
			spLoggerPrintInfo(ALL_FEATURES_ARRAY_CREATED);
		}
		else {
			spLoggerPrintError(ALLOCATION_ERROR,__FILE__,__func__,__LINE__);
			terminate(config,siftDB,numOfImgs,numOfFeaturesPerImage,NULL,0,NULL);
			return -1;
		}
		fflush(NULL);

		//build KDtree from all features
		SPKDTreeNode* featuresTree = buildFeaturesKDTree(allFeaturesArr, numOfAllFeatures, config, &msg);
//		return 0;
		//if unsuccessfull terminate program
		if(featuresTree==NULL) {
			spLoggerPrintError(KD_TREE_ERROR,__FILE__,__func__,__LINE__);
			terminate(config,siftDB,numOfImgs,numOfFeaturesPerImage,allFeaturesArr,numOfAllFeatures,featuresTree);
//			spLoggerDestroy();
			return -1;
		}
		spLoggerPrintInfo(KD_TREE_CREATED);
		fflush(NULL);

		char queryPath[STR_MAX_LENGTH+1] = {'\0'};
		if (getQueryPath(queryPath) < 0) {
			spLoggerPrintError(COULDNT_BE_RESOLVED,__FILE__,__func__,__LINE__);
			terminate(config,siftDB,numOfImgs,numOfFeaturesPerImage,allFeaturesArr,numOfAllFeatures,featuresTree);
//			spLoggerDestroy();
			return -1;
		}

		int* counter = countKClosestPerFeature(config, &msg, featuresTree, numOfImgs, queryPath);
		if (counter == NULL) {
			spLoggerPrintError(COUNT_K_CLOSEST_ERROR,__FILE__,__func__,__LINE__);
			terminate(config,siftDB,numOfImgs,numOfFeaturesPerImage,allFeaturesArr,numOfAllFeatures,featuresTree);
			return -1;
			}

		BPQueueElement* queryClosestImages = sortFeaturesCount(counter, numOfImgs);
		printf(BEST_CAND, "query");
		for(int i=0; i<spConfigGetNumOfSimilarImages(config, &msg); i++)
		{
			printf("%d ",queryClosestImages[i].index);
		}
		printf("\n");
		fflush(NULL);
		free(queryClosestImages);
		free(counter);

//	}
//	catch(std::exception & ex )
//	{
//		spLoggerPrintError("EXCEPTION_IN_IMAGE_PROC",__FILE__,__func__,__LINE__);
//		free(&msg);
//		free(conf);
//		return 1;
//	}
	terminate(config,siftDB,numOfImgs,numOfFeaturesPerImage,allFeaturesArr,numOfAllFeatures,featuresTree);
	fflush(NULL);
	printf("===========================END===========================\n");
	fflush(NULL);
	return 0;
}
