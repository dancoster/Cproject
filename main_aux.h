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
#define CONFIG_CANNOT_OPEN_FILE "The%s configuration file %s couldn't be opened\n"
#define CONFIG_CREATED "Config CREATED\n"
#define CONFIG_DESTROY "Config DESTROYED\n"
#define CONFIG_ERROR "Config couldn't be created\n"
#define LOGGER_CREATED "Logger CREATED\n"
#define LOGGER_DESTROY "Logger DESTROYED\n"
#define LOGGER_ERROR "Logger couldn't be created\n"
#define LOGGER_FILENAME "Logger Filename"
#define LOGGER_LEVEL "Logger Level"
#define IMG_PATH_ERROR "Image path couldn't be resolved\n"
#define FEATS_PATH_ERROR "Feats path couldn't be resolved\n"
#define FEAT_CANNOT_OPEN_FILE "The feat file couldn't be opened\n"
#define FEAT_WRITE_ERROR "Write to feat file failed\n"
#define FEAT_READ_ERROR "Read features file is failed\n"
#define ENTER_QUERY_PATH "Please enter a query image path:\n"
#define QUERY_PATH_ERROR "Query path couldn't be resolved\n"
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
#define EXITING "Exiting...\n"
#define COUNT_K_CLOSEST_ERROR "the function countKClosestPerFeature couldn't be complete\n"
#define SORT_FEATURES_COUNT_ERROR "the function sortFeaturesCount couldn't be complete\n"
#define FEATS_ERROR "There is no feats for these image"
#define FEATS_READING_ERROR "Can't read features from file"
#define NUM_FEATS_READING_ERROR "Can't read number of features per image"
#define EXTRACTS_FEATURES "Extracting the features from the feats files"
#define SHOW_RESULTS_ERROR "the function showResults couldn't be complete\n"


/**
 * Extracting all the features of the images, and stores it to a SPPoint array database.
 * Supports two modes:
 * EXTRACTION		- extracts the features of each image and stores each of these features to a
 * 					  ".feat" file.
 * NON-EXTRACTION	- extracts the features of each image from the ".feat" files.
 *
 * @param siftDB 		 	 - the SPPoint array in which the function stores the extracted features to
 * @param numOfImgs			 - the number of images to extract the features from
 * @param numOfAllFeatures 	 - the number of all features which were successfully extracted
 * @param config 			 - the configuration structure
 * @param msg 				 - pointer in which the msg returned by any functions of the config is stored
 *
 * @return
 * -1 in case of invalid arguments, or failure
 * 1 if successfully extracted all the features and stored it to <siftDB>
 */
int extractFeatures(SPPoint*** siftDB, int numOfImgs, int* numOfFeaturesPerImage, int* numOfAllFeatures,
		SPConfig config, SP_CONFIG_MSG* msg, ImageProc* imageProc);

/**
 * Converting 2D SPPoint array to 1D SPPoint array.
 *
 * @param allFeaturesArr 	 	- the SPPoint array in which the function stores the features to
 * @param siftDB			 	- the sift features which stores all the features of the files
 * @param numOfImgs 	 	 	- the number of images
 * @param numOfFeaturesPerImage - array of ints which stores the number of features of each image
 *
 * @return the number of points which were copied to <allFeaturesArr>
 */
int createAllFeaturesArray(SPPoint** allFeaturesArr, SPPoint*** siftDB, int numOfImgs, int* numOfFeaturesPerImage);

/**
 * Builds the features KDTree database.
 *
 * @param allFeaturesArr 	 - the SPPoint array in which the function uses to build the KDArray
 * @param numOfAllFeatures 	 - the number of all features which successfully extracted
 * @param config 			 - the configuration structure
 * @param msg 				 - pointer in which the msg returned by any functions of the config is stored
 *
 * @return
 * NULL in case of invalid arguments, or failure
 * Otherwise, the root of the KDTree is returned
 */
SPKDTreeNode* buildFeaturesKDTree(SPPoint** allFeaturesArr, int numOfAllFeatures, SPConfig config ,SP_CONFIG_MSG* msg);

/**
 * Getting the querySift DB, finding KNN for each feature, and counting the feature hits for each image.
 *
 * @param featuresTree 	 	 - the root to the features KDArray
 * @param numOfImgs 	 	 - the number of images
 * @param queryPath 		 - the query path
 * @param config 			 - the configuration structure
 * @param msg 				 - pointer in which the msg returned by any functions of the config is stored
 *
 * @return
 * NULL in case of invalid arguments, or failure
 * Otherwise, the pointer to the counter array which stores the feature hits for each image is returned
 */
int* countKClosestPerFeature(SPKDTreeNode* featuresTree, int numOfImgs, char* queryPath,
		SPConfig config, SP_CONFIG_MSG* msg, ImageProc* imageProc);

/**
 * Sorting the images indexes by the number of feature hits.
 * Using BPQueueElement which holds <i ,counter[i]>, such that:
 * <i> = the index of the image, counter[i] = the number of feature hits of the image.
 * The best images appear first, i.e the images which have the most feature hits
 *
 * @param counter 	 		 - the root to the features KDArray
 * @param numOfImgs 	 	 - the number of images
 *
 * @return
 * NULL in case of invalid arguments, or failure
 * Otherwise, the sorted BPQueueElement array of the images indexes and their # of feature hits is returned
 */
BPQueueElement* sortFeaturesCount(int* counter, int numOfImgs);

/**
 * Reads the features of an image from the ".feat" file, and stores it to a SPPoint array.
 *
 * @param imgIndex 		 	 - the index of the image
 * @param numFeatures		 - the array which used to stored the number of extracted features
 * @param path				 - the path to the image
 * @param pcaNumComp		 - the PCA dimension
 *
 * @return
 * NULL in case of invalid arguments, or failure
 * Otherwise, the SPPoint array which stores the extracted features from the file is returned
 */
SPPoint** readFeaturesFromFile(int imgIndex, int* numFeatures, char* path, int pcaNumComp);

/**
 * Getting the image query path from user.
 *
 * @param path - pointer to the string which the function stores the path to
 *
 * @return
 * -1 in case of failure
 * 1 if successfully stored the entered path into <path>
 */
int getQueryPath(char* path);

/**
 * Showing the results, i.e the numOfSimilarImages closest images to the query image by feature hits.
 * Supports two modes:
 * MINIMAL GUI 		- displays the images one after the other. If the user presses any key, the next
 * 				 	  image displayed.
 * NON-MINIMAL GUI	- prints the results in a format. for example:
 * 					  Best candidates for - ./query.png - are:
 * 					  ./images/img9.png
 * 					  ./images/img4.png
 *
 * @param queryPath 		 - the query path
 * @param queryClosestImages - the queue elements which holds the best image indexes
 * @param config 			 - the configuration structure
 * @param msg 				 - pointer in which the msg returned by any functions of the config is stored
 *
 * @return
 * false in case of invalid arguments, or failure
 * Otherwise, true
 */
bool showResults(char* queryPath, BPQueueElement* queryClosestImages, SPConfig config, SP_CONFIG_MSG* msg);

/**
 * Frees all memory resources associate with the program, and terminates it.
 */
void terminate(SPConfig config, SPPoint*** siftDB, int numOfImgs, int* numOfFeaturesPerImage,
		SPPoint** allFeaturesArr, int numOfAllFeatures, SPKDTreeNode* featuresTree);

#endif /* MAIN_AUX_H_ */
