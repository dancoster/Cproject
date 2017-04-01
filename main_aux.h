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
#define CONFIG_CANNOT_OPEN_FILE "The %s configuration file %s couldn't be open\n"
#define CONFIG_CREATED "Config CREATED\n"
#define CONFIG_DESTROY "Config DESTROYED\n"
#define CONFIG_ERROR "Config couldn't be created\n"
#define LOGGER_CREATED "Logger CREATED\n"
#define LOGGER_DESTROY "Logger DESTROYED\n"
#define LOGGER_ERROR "Logger couldn't be created\n"
#define LOGGER_FILENAME "Logger Filename"
#define LOGGER_LEVEL "Logger Level"
#define IMAGE_PATH_ERROR "Image Path couldn't be resolved\n"
#define FEAT_CANNOT_OPEN_FILE "The feat file couldn't be open\n"
#define FEAT_WRITE_ERROR "Write to feat file failed\n"
#define ENTER_QUERY_PATH "Please enter a query image path:\n"
#define BEST_CAND "Best candidates for - %s - are:\n"
#define IMAGE_PROC_CREATED "Image Proc object created!\n"
#define N_FEATURES_WARNING "Number of features doesn't match\n"
#define COULDNT_BE_RESOLVED "couldn't be resolved\n"
#define EXTRACTING_FEATS_IMG "Extracting features from images...\n"
#define EXTRACTING_FEATS_FILE "Extracting features from files...\n"
#define DONE_EXTRACTING "Done Extracting!\n"
#define SIFT_DB_CREATED "Sift DB CREATED\n"
#define SIFT_DB_DESTROY "Sift DB DESTROYED\n"
#define ALL_FEATURES_ARRAY_CREATED "All Features Array CREATED\n"
#define ALL_FEATURES_ARRAY_DESTROY "All Features Array DESTROYED\n"
#define KD_TREE_CREATED "KD Tree CREATED\n"
#define KD_TREE_DESTROY "KD Tree DESTROYED\n"
#define KD_TREE_ERROR "KD Tree couldn't be created\n"
#define N_IMGS_L "Number of images"
#define N_SIM "Number of similar images"
#define N_FEATS "Number of features"
#define FOUND_KNN "Found K nearest neighbors!"
#define KNN_ERROR "Couldn't find K nearest neighbors\n"
#define KNN_L "Number of K nearest neighbors"
#define EXT_MODE_L "Extraction Mode"
#define MINIMAL_GUI "Minimal GUI"
#define SPLIT_METHOD "Split Method"
#define Q_IMG_RECEIVED "Query Image received"
#define Q_FEATS_EXT "Query Image Features Extracted"
#define FOUND_CLOSEST_IMG "Found closest images"
#define QUERY_SUCCESS "Query loop ended successfully!"
#define TERMINATE "<>"
#define EXITING "Exiting…\n"
#define COUNT_K_CLOSEST_ERROR "the function countKClosestPerFeature couldn't be complete\n"

int extractFeatures (SPPoint*** siftDB, int numOfImgs, int* numOfFeaturesPerImage, int* numOfAllFeatures,
		SPConfig config, SP_CONFIG_MSG* msg);

int createAllFeaturesArray(SPPoint** allFeaturesArr, SPPoint*** siftDB, int numOfImgs, int* numOfFeaturesPerImage);

SPKDTreeNode* buildFeaturesKDTree(SPPoint** allFeaturesArr, int numOfAllFeatures,SPConfig conf,SP_CONFIG_MSG* msg);

int* countKClosestPerFeature(SPConfig config, SP_CONFIG_MSG* msg, SPKDTreeNode* featuresTree,
		int numOfImgs, char* queryPath);

BPQueueElement* sortFeaturesCount(int* counter, int numOfImgs);

void terminate (SPConfig config, SPPoint*** siftDB, int numOfImgs, int* numOfFeaturesPerImage,
		SPPoint** allFeaturesArr, int numOfAllFeatures, SPKDTreeNode* featuresTree);

int getQueryPath(char* path);

#endif /* MAIN_AUX_H_ */
