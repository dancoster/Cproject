#include "main_aux.h"

int main(int argc, char* argv[]) {
	char config_filename[STR_MAX_LENGTH+1] = DEFAULT_CONFIG_FILENAME; // default config file
	SPConfig config;
	SP_CONFIG_MSG msg;
	ImageProc* imageProc;

	//-------checking command line arguments-------
	if (argc > 1) { // checks if config file entered?
		if (argc == 3 && strcmp(argv[1], "-c") == 0) { // correct num of args && -c flag
			strcpy(config_filename, argv[2]);
		} else { //invalid args
			printf(INVALID_COMMAND_LINE_ERROR, config_filename);
			return -1;
		}
	}
	//---------------------------------------------

	//-------------creating the config-------------
	config = spConfigCreate(config_filename, &msg);
	if (msg == SP_CONFIG_CANNOT_OPEN_FILE) {
		if (argc > 1) { // used config filename from command line
			printf(CONFIG_CANNOT_OPEN_FILE, "", config_filename);
		}
		else {  // used default config filename
			printf(CONFIG_CANNOT_OPEN_FILE, " default", config_filename);
		}
	}

	if (msg != SP_CONFIG_SUCCESS) { // create fail
		printf(CONFIG_ERROR);
		terminate(config,NULL,0,NULL,NULL,0,NULL);
		return -1;
	}
	else {
		printf(CONFIG_CREATED); // create success
	}
	//---------------------------------------------

	//-------------creating the logger-------------
	char* logger_filename = spConfigGetLoggerFilename(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		printf("%s %s\n", LOGGER_FILENAME, COULDNT_BE_RESOLVED);
		terminate(config,NULL,0,NULL,NULL,0,NULL);
		return -1;
	}

	SP_LOGGER_LEVEL logger_level = spConfigGetLoggerLevel(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		printf("%s %s\n", LOGGER_LEVEL, COULDNT_BE_RESOLVED);
		terminate(config,NULL,0,NULL,NULL,0,NULL);
		return -1;
	}

	if (strcmp(logger_filename, "stdout") == 0) { // logger file name is stdout
		logger_filename = NULL;
	}
	if (spLoggerCreate(logger_filename, logger_level) != SP_LOGGER_SUCCESS) {
		printf("%s\n", LOGGER_ERROR);
		terminate(config,NULL,0,NULL,NULL,0,NULL);
		return -1;
	}
	spLoggerPrintInfo(LOGGER_CREATED);
	//---------------------------------------------

	//-------------creating the ImageProc-------------
	try {
		imageProc = new ImageProc(config);
	}
	catch(std::exception & ex )
	{
		spLoggerPrintError(IMAGE_PROC_ERROR,__FILE__,__func__,__LINE__);
		terminate(config,NULL,0,NULL,NULL,0,NULL);
		return -1;
	}
	//------------------------------------------------

	//-----create the SIFT Database, build the KDTree-----
	int numOfImgs = spConfigGetNumOfImages(config ,&msg);
	if (numOfImgs == -1) { // fail in spConfigGetNumOfImages function
		spLoggerPrintError(FUNCTION_ERROR,__FILE__,__func__,__LINE__);
		delete imageProc;
		terminate(config,NULL,0,NULL,NULL,0,NULL);
		return -1;
	}

	// creating the SIFT Database
	int* numOfFeaturesPerImage = (int*) malloc(numOfImgs*sizeof(int));
	SPPoint*** siftDB = (SPPoint***) malloc(numOfImgs*sizeof(SPPoint**));
	if (numOfFeaturesPerImage==NULL || siftDB==NULL) { //Allocation failure
		spLoggerPrintError(ALLOCATION_ERROR,__FILE__,__func__,__LINE__);
		delete imageProc;
		terminate(config,siftDB,numOfImgs,numOfFeaturesPerImage,NULL,0,NULL);
		return -1;
	}
	int numOfAllFeatures=0;

	if (extractFeatures(siftDB, numOfImgs, numOfFeaturesPerImage, &numOfAllFeatures, config, &msg, imageProc) == -1) {
		spLoggerPrintError(EXTRACTING_FEATS_ERROR,__FILE__,__func__,__LINE__);
		delete imageProc;
		terminate(config,siftDB,numOfImgs,numOfFeaturesPerImage,NULL,0,NULL);
		return -1;
	}
	spLoggerPrintInfo(SIFT_DB_CREATED);

	// converting the SIFT Database to one array of all images features
	SPPoint** allFeaturesArr = (SPPoint**) malloc(numOfAllFeatures*sizeof(SPPoint*));
	if (allFeaturesArr == NULL) { //Allocation failure
		spLoggerPrintError(ALLOCATION_ERROR,__FILE__,__func__,__LINE__);
		delete imageProc;
		terminate(config,siftDB,numOfImgs,numOfFeaturesPerImage,NULL,0,NULL);
		return -1;
	}

	if (createAllFeaturesArray(allFeaturesArr, siftDB, numOfImgs, numOfFeaturesPerImage) == numOfAllFeatures) {
		spLoggerPrintInfo(ALL_FEATURES_ARRAY_CREATED);
	}
	else { 		// createAllFeaturesArray failed
		spLoggerPrintError(ALL_FEATURES_ARRAY_ERROR,__FILE__,__func__,__LINE__);
		delete imageProc;
		terminate(config,siftDB,numOfImgs,numOfFeaturesPerImage,allFeaturesArr,numOfAllFeatures,NULL);
		return -1;
	}

	// build KDtree from all features
	SPKDTreeNode* featuresTree = buildFeaturesKDTree(allFeaturesArr, numOfAllFeatures, config, &msg);
	if (featuresTree == NULL) { // buildFeaturesKDTree failed
		spLoggerPrintError(KD_TREE_ERROR,__FILE__,__func__,__LINE__);
		delete imageProc;
		terminate(config,siftDB,numOfImgs,numOfFeaturesPerImage,allFeaturesArr,numOfAllFeatures,featuresTree);
		return -1;
	}
	spLoggerPrintInfo(KD_TREE_CREATED);
	//-------------------------------------------------------

	//---------------------------------------------
	//-----------starting the query loop-----------
	//---------------------------------------------
	char queryPath[STR_MAX_LENGTH+1] = {'\0'};
	while (true) {
		// getting the query path from user
		if (getQueryPath(queryPath) < 0) {
			spLoggerPrintError(QUERY_PATH_ERROR,__FILE__,__func__,__LINE__);
			delete imageProc;
			terminate(config,siftDB,numOfImgs,numOfFeaturesPerImage,allFeaturesArr,numOfAllFeatures,featuresTree);
			return -1;
		}

		// if the user terminates the program
		if (strcmp(queryPath, TERMINATE) == 0) {
			delete imageProc;
			terminate(config,siftDB,numOfImgs,numOfFeaturesPerImage,allFeaturesArr,numOfAllFeatures,featuresTree);
			return 1;
		}

		// getting the querySift DB, finding KNN for each feature, and counting the feature hits for each image
		int* counter = countKClosestPerFeature(featuresTree, numOfImgs, queryPath, config, &msg, imageProc);
		if (counter == NULL) { // countKClosestPerFeature failed
			spLoggerPrintError(COUNT_K_CLOSEST_ERROR,__FILE__,__func__,__LINE__);
			delete imageProc;
			terminate(config,siftDB,numOfImgs,numOfFeaturesPerImage,allFeaturesArr,numOfAllFeatures,featuresTree);
			return -1;
			}

		// sorting the images indexes by the number of feature hits
		BPQueueElement* queryClosestImages = sortFeaturesCount(counter, numOfImgs);
		if (queryClosestImages == NULL) { // sortFeaturesCount failed
			spLoggerPrintError(SORT_FEATURES_COUNT_ERROR,__FILE__,__func__,__LINE__);
			delete imageProc;
			terminate(config,siftDB,numOfImgs,numOfFeaturesPerImage,allFeaturesArr,numOfAllFeatures,featuresTree);
			return -1;
			}

		// showing the results, i.e the numOfSimilarImages closest images to the query image by feature hits
		if (!showResults(queryPath, queryClosestImages, config, &msg, imageProc)) {
			spLoggerPrintError(SHOW_RESULTS_ERROR,__FILE__,__func__,__LINE__);
			delete imageProc;
			terminate(config,siftDB,numOfImgs,numOfFeaturesPerImage,allFeaturesArr,numOfAllFeatures,featuresTree);
			return -1;
			}

		// free allocations in this iteration
		free(queryClosestImages);
		free(counter);
	}
	// end of query loop
	//---------------------------------------------
}
