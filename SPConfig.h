#ifndef SPCONFIG_H_
#define SPCONFIG_H_

#include <stdbool.h>
#include <stdio.h>
#include "SPLogger.h"

//default values
#define DEFAULT_PCA_DIM 20
#define DEFAULT_PCA_FILENAME "pca.yml"
#define DEFAULT_NUM_OF_FEATURES 100
#define DEFAULT_EXTRACT_MODE true
#define DEFAULT_MINIMAL_GUI false
#define DEFAULT_NUM_OF_SIMILAR_IMGS 1
#define DEFAULT_KNN 1
#define DEFAULT_KDT_SPLIT_METHOD MAX_SPREAD //check struct def here
#define DEFAULT_LOGGER_LVL 3
#define DEFAULT_LOGGER_FILENAME "stdout"
#define DEFAULT_INT 0
#define DEFAULT_STR ""
#define DEFAULT_STR2 '\0'
#define DEFAULT_CONFIG_FILE "spcbir.config"

/**
 * A data-structure which is used for configuring the system.
 */

typedef enum sp_config_msg_t {
	SP_CONFIG_MISSING_DIR,
	SP_CONFIG_MISSING_PREFIX,
	SP_CONFIG_MISSING_SUFFIX,
	SP_CONFIG_MISSING_NUM_IMAGES,
	SP_CONFIG_CANNOT_OPEN_FILE,
	SP_CONFIG_ALLOC_FAIL,
	SP_CONFIG_INVALID_INTEGER,
	SP_CONFIG_INVALID_STRING,
	SP_CONFIG_INVALID_ARGUMENT,
	SP_CONFIG_INDEX_OUT_OF_RANGE,
	SP_CONFIG_SUCCESS
} SP_CONFIG_MSG;

//added
/** A type used to decide the KDTree split method**/
typedef enum sp_kdtree_split_method {
	MAX_SPREAD,
	RANDOM,
	INCREMENTAL
} SP_KD_TREE_SPLIT_METHOD;

typedef struct sp_config_t* SPConfig;

/**
 * Creates a new system configuration struct. The configuration struct
 * is initialized based on the configuration file given by 'filename'.
 * 
 * @param filename - the name of the configuration file
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return NULL in case an error occurs. Otherwise, a pointer to a struct which
 * 		   contains all system configuration.
 * 
 * The resulting value stored in msg is as follow:
 * - SP_CONFIG_INVALID_ARGUMENT - if filename == NULL
 * - SP_CONFIG_CANNOT_OPEN_FILE - if the configuration file given by filename cannot be open
 * - SP_CONFIG_ALLOC_FAIL - if an allocation failure occurred
 * - SP_CONFIG_INVALID_INTEGER - if a line in the config file contains invalid integer
 * - SP_CONFIG_INVALID_STRING - if a line in the config file contains invalid string
 * - SP_CONFIG_MISSING_DIR - if spImagesDirectory is missing
 * - SP_CONFIG_MISSING_PREFIX - if spImagesPrefix is missing
 * - SP_CONFIG_MISSING_SUFFIX - if spImagesSuffix is missing 
 * - SP_CONFIG_MISSING_NUM_IMAGES - if spNumOfImages is missing
 * - SP_CONFIG_SUCCESS - in case of success
 */
SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg);

/*
 * Returns true if spExtractionMode = true, false otherwise.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return true if spExtractionMode = true, false otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns true if spMinimalGUI = true, false otherwise.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return true if spMinimalGUI = true, false otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns the number of images set in the configuration file, i.e the value
 * of spNumOfImages.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns the number of features to be extracted. i.e the value
 * of spNumOfFeatures.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg);

/**
 * Returns the dimension of the PCA. i.e the value of spPCADimension.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg);

/**
 * Returns the the value of spKNN.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetKNN(const SPConfig config, SP_CONFIG_MSG* msg);

/**
 * Returns the number of the similar images. i.e the value of spNumOfSimilarImages.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetNumOfSimilarImages(const SPConfig config, SP_CONFIG_MSG* msg);

/**
 * Given an index 'index' the function stores in imagePath the full path of the
 * ith image.
 *
 * For example:
 * Given that the value of:
 *  spImagesDirectory = "./images/"
 *  spImagesPrefix = "img"
 *  spImagesSuffix = ".png"
 *  spNumOfImages = 17
 *  index = 10
 *
 * The functions stores "./images/img10.png" to the address given by imagePath.
 * Thus the address given by imagePath must contain enough space to
 * store the resulting string.
 *
 * @param imagePath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @param index - the index of the image.
 *
 * @return
 * - SP_CONFIG_INVALID_ARGUMENT - if imagePath == NULL or config == NULL
 * - SP_CONFIG_INDEX_OUT_OF_RANGE - if index >= spNumOfImages
 * - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index);

/**
 * The function stores in pcaPath the full path of the pca file.
 * For example given the values of:
 *  spImagesDirectory = "./images/"
 *  spPcaFilename = "pca.yml"
 *
 * The functions stores "./images/pca.yml" to the address given by pcaPath.
 * Thus the address given by pcaPath must contain enough space to
 * store the resulting string.
 *
 * @param imagePath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @return
 *  - SP_CONFIG_INVALID_ARGUMENT - if imagePath == NULL or config == NULL
 *  - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config);

/*
 * Returns the Logger Filename. i.e the value of spLoggerFileName.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return a pointer to the string represents the logger filename
 *
 *	 The resulting value stored in msg is as follow:
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
char* spConfigGetLoggerFilename(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns the logger level. i.e the value of spLoggerLevel.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return enum of type SP_LOGGER_LEVEL which indicates the logger level
 *
 *	 The resulting value stored in msg is as follow:
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
SP_LOGGER_LEVEL spConfigGetLoggerLevel(const SPConfig config,
		SP_CONFIG_MSG* msg);

/*
 * Returns the split method. i.e the value of spKDTreeSplitMethod.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return enum of type SP_KD_TREE_SPLIT_METHOD which indicates the split method
 *
 *	 The resulting value stored in msg is as follow:
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
SP_KD_TREE_SPLIT_METHOD spConfigGetKDTreeSplitMethod(const SPConfig config,
		SP_CONFIG_MSG* msg);

/**
 * Given an index 'index' the function stores in imagePath the full path of the
 * ith image features file.
 *
 * For example:
 * Given that the value of:
 *  spImagesDirectory = "./images/"
 *  spImagesPrefix = "img"
 *  spImagesSuffix = ".png"
 *  spNumOfImages = 17
 *  index = 10
 *
 * The functions stores "./images/img10.feats" to the address given by imagePath.
 * Thus the address given by imagePath must contain enough space to
 * store the resulting string.
 *
 * @param imagePath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @param index - the index of the image.
 *
 * @return
 * - SP_CONFIG_INVALID_ARGUMENT - if imagePath == NULL or config == NULL
 * - SP_CONFIG_INDEX_OUT_OF_RANGE - if index >= spNumOfImages
 * - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigGetFeatsPath(char* imagePath, SPConfig config, int index);

/**
 * Frees all memory resources associate with config. 
 * If config == NULL nothig is done.
 */
void spConfigDestroy(SPConfig config);

/**
 * 	Prints error messages. The error message format is given below:
 * 	Case 1: if a line is invalid
 * 	- File: <the configuration filename>
 *  - Line: <the number of the invalid line>
 *  - Message: Invalid configuration line
 *
 *  Case 2: if any of the constrains on the system parameters are not met
 * 	- File: <the configuration filename>
 *  - Line: <the number of the invalid value>
 *  - Message: Invalid value - constraint not met
 *
 *  Case 3: if a parameter with no default value is not set
 * 	- File: <the configuration filename>
 *  - Line: <the number of lines in the configuration file>
 *  - Message: Parameter <parameter name> is not set
 *
 * @param filename    	- A string representing the configuration filename
 * @param line			- The number of the invalid line in the configuration file
 * @param error_type	- Indicates the error message case={1,2,3}
 * @param param_name	- A string representing the parameter name in case 3, NULL in cases 1-2
 */
void spConfigPrintError(const char* filename, int line, int error_type, char* param_name);

/**
 * 	Frees all memory allocations of config, and closes fp:
 *
 * @param config 		- the configuration structure
 * @param fp 			- the pointer to the config file
 * @param msg 			- pointer in which the msg returned by the function is stored
 * @param config_msg	- which msg to store
 * @param filename    	- A string representing the configuration filename
 * @param lineNumber 	- The number of the invalid line in the configuration file
 * @param error_type	- Indicates the error message case={1,2,3}
 * @param param_name	- A string representing the parameter name in case 3, NULL in cases 1-2
 *
 * If (config==NULL || fp == NULL || msg == NULL || filename == NULL), then nothing happends.
 * sets *msg = config_msg
 */
void spConfigTerminate(SPConfig config, FILE* fp, SP_CONFIG_MSG* msg, SP_CONFIG_MSG config_msg,
		const char* filename, int lineNumber, int error_type, char* param_name);

/**
 * Reads through the configuration file and assign values of config if they are valid.
 * Skips all comments.
 *
 * @param config 	- the configuration structure
 * @param fp 		- the pointer to the config file
 * @param msg 		- pointer in which the msg returned by the function is stored
 * @param filename 	- the configuration file name
 * @param lineNumber 	- the line number which was read from the config file
 *
 * @return - True if all variables were set and valid
 *
 * 	The resulting value stored in msg is as follow:
 * - SP_CONFIG_INVALID_STRING - if an invalid parameter was entered in config
 * - SP_CONFIG_ALLOC_FAIL - if memory allocation failed
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigGetVariables(SPConfig config, FILE* fp, SP_CONFIG_MSG* msg, const char* filename, int* lineNumber);

/**
 *  Sets the default values to config:
 *
 *	@param config - the configuration structure
 *	does nothing if config == NULL
 */
void spConfigSetDefaultValues(SPConfig config);

/**
 * Checks if all variables were initialized:
 *
 * @param config 		- the configuration structure
 * @param fp	 		- the pointer to the config file
 * @param msg 			- pointer in which the msg returned by the function is stored
 * @param config_msg	- which msg to store
 * @param filename 		- the configuration file name
 * @param lineNumber 	- the line number which was read from the config file
 *
 * @return - True if all variables were initialized, False otherwise
 */
bool spConfigCheckVariablesInitialized(SPConfig config, FILE* fp, SP_CONFIG_MSG* msg, const char* filename,
		int lineNumber);

/**
 * Checks if the string is a positive integer:
 * for every i: ('0' <= num[i] <= '9'), using isdigit function
 *
 * @param num 		- the checked string
 *
 * @return - True if the string is a number, False otherwise
 */
bool isNumber(char* num);

void spConfigPrintInfo(const SPConfig config);

#endif /* SPCONFIG_H_ */
