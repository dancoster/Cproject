#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_

#include "SPImageProc.h"
extern "C" {
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "SPKDTreeNode.h"
}
using namespace sp;

#define DEFAULT_CONFIG_FILENAME "spcbir.config"
#define INVALID_COMMAND_LINE_ERROR "Invalid command line : use -c %s\n"
#define CONFIG_CANNOT_OPEN_FILE "The %s configuration file %s couldn't be opened\n"
#define CONFIG_CREATED "Config CREATED\n"
#define CONFIG_DESTROY "Config DESTROYED\n"
#define CONFIG_ERROR "Config couldn't be created\n"
#define LOGGER_CREATED "Logger CREATED\n"
#define LOGGER_DESTROY "Logger DESTROYED\n"
#define LOGGER_ERROR "Logger couldn't be created\n"
#define LOGGER_FILENAME "Logger Filename"
#define LOGGER_LEVEL "Logger Level"
#define IMAGE_PATH_ERROR "Image Path couldn't be resolved\n"
#define FEAT_CANNOT_OPEN_FILE "The feat file couldn't be opened\n"
#define FEAT_WRITE_ERROR "Write to feat file failed\n"
#define ENTER_QUERY_PATH "Please enter a query image path:\n"
#define BEST_CANDIDATES "Best candidates for - %s - are:\n"
#define COULDNT_BE_RESOLVED "couldn't be resolved\n"
#define EXTRACTING_FEATS_ERROR "Extracting features from images failed\n"
#define SIFT_DB_CREATED "Sift DB CREATED\n"
#define SIFT_DB_DESTROY "Sift DB DESTROYED\n"
#define ALL_FEATURES_ARRAY_CREATED "All Features Array CREATED\n"
#define ALL_FEATURES_ARRAY_DESTROY "All Features Array DESTROYED\n"
#define ALL_FEATURES_ARRAY_ERROR "All Features Array couldn't be created\n"
#define KD_TREE_CREATED "KD Tree CREATED\n"
#define KD_TREE_DESTROY "KD Tree DESTROYED\n"
#define KD_TREE_ERROR "KD Tree couldn't be created\n"
#define KNN_ERROR "Couldn't find K nearest neighbors\n"
#define TERMINATE "<>"
#define EXITING "Exiting…\n"
#define COUNT_K_CLOSEST_ERROR "the function countKClosestPerFeature couldn't be completed\n"

int extractFeatures(SPPoint*** siftDB, int numOfImgs, int* numOfFeaturesPerImage, int* numOfAllFeatures,
		SPConfig config, SP_CONFIG_MSG* msg);

int createAllFeaturesArray(SPPoint** allFeaturesArr, SPPoint*** siftDB, int numOfImgs, int* numOfFeaturesPerImage);

SPKDTreeNode* buildFeaturesKDTree(SPPoint** allFeaturesArr, int numOfAllFeatures,SPConfig conf,SP_CONFIG_MSG* msg);

int* countKClosestPerFeature(SPKDTreeNode* featuresTree, int numOfImgs, char* queryPath,
		SPConfig config, SP_CONFIG_MSG* msg);

BPQueueElement* sortFeaturesCount(int* counter, int numOfImgs);

int getQueryPath(char* path);

void showResults(char* queryPath, BPQueueElement* queryClosestImages, SPConfig config, SP_CONFIG_MSG* msg);

void terminate(SPConfig config, SPPoint*** siftDB, int numOfImgs, int* numOfFeaturesPerImage,
		SPPoint** allFeaturesArr, int numOfAllFeatures, SPKDTreeNode* featuresTree);

#endif /* MAIN_AUX_H_ */
