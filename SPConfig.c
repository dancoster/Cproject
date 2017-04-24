#include "SPConfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

//File open mode
#define SP_CONFIG_OPEN_MODE "r"

struct sp_config_t {
	char spImagesDirectory[STR_MAX_LENGTH+1];	//path of images directory
	char spImagesPrefix[STR_MAX_LENGTH+1];		//the images prefix
	char spImagesSuffix[STR_MAX_LENGTH+1];		//the images file format
	int spNumOfImages;							//the number of images in images directory
	int spPCADimension;							//the images file format
	char spPCAFilename[STR_MAX_LENGTH+1];		//the filename of the PCA file
	int spNumOfFeatures;						//the number of features
	bool spExtractionMode;
	int spNumOfSimilarImages;
	SP_KD_TREE_SPLIT_METHOD spKDTreeSplitMethod;
	int spKNN;
	bool spMinimalGUI;
	int spLoggerLevel;							//indicates the active level of the logger {1,2,3,4}
	char spLoggerFilename[STR_MAX_LENGTH+1];	//the log file name
};

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg) {
	assert(msg!=NULL);
	if (filename==NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}

	FILE* fp = fopen(filename, SP_CONFIG_OPEN_MODE);
	if (fp == NULL) { //Open failed
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}

	SPConfig config = (SPConfig) malloc(sizeof(*config));
	if (config == NULL) { //Allocation failure
		fclose(fp);
		printf(ALLOCATION_ERROR);
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}

	// setting the default values before iterating the file
	spConfigSetDefaultValues(config);

	// getting system variables by iterating lines
	int lineNumber=1;	//counting the lines
	// variable assignment failed:
	if (!spConfigGetVariables(config, fp, msg, filename, &lineNumber)) { // assigning msg inside
		return NULL;
	}

	// check if all parameters initialized:
	if (!spConfigCheckVariablesInitialized(config, fp, msg, filename, lineNumber)) { // assigning msg inside
		return NULL;
	}

	// check if spNumOfSimilarImages > spNumOfImages
	if (config->spNumOfSimilarImages > config->spNumOfImages) {
		printf("spNumOfSimilarImages(%d) is bigger than spNumOfImages(%d).\n"
				"Setting spNumOfSimilarImages=%d\n", config->spNumOfSimilarImages, config->spNumOfImages,
				config->spNumOfImages);
		config->spNumOfSimilarImages = config->spNumOfImages;
	}

	// all checks passed
	*msg = SP_CONFIG_SUCCESS;
	fclose(fp); // closing the config file
	return config;
}

bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg!=NULL);
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}
	if (config->spExtractionMode == true) {
		*msg = SP_CONFIG_SUCCESS;
		return true;
	}
	else {
		*msg = SP_CONFIG_SUCCESS;
		return false;
	}
}

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg!=NULL);
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}
	if (config->spMinimalGUI == true) {
		*msg = SP_CONFIG_SUCCESS;
		return true;
	}
	else {
		*msg = SP_CONFIG_SUCCESS;
		return false;
	}
}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg!=NULL);
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfImages;
}

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg!=NULL);
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfFeatures;
}

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg!=NULL);
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spPCADimension;
}

int spConfigGetKNN(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg!=NULL);
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spKNN;
}

int spConfigGetNumOfSimilarImages(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg!=NULL);
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfSimilarImages;
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config, int index) {
	if (imagePath == NULL || config == NULL)
		return SP_CONFIG_INVALID_ARGUMENT;
	if (index >= config->spNumOfImages)
		return SP_CONFIG_INDEX_OUT_OF_RANGE;

	if (sprintf(imagePath, "%s%s%d%s", config->spImagesDirectory,
			config->spImagesPrefix, index, config->spImagesSuffix) < 0) {
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}
	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config) {
	if (pcaPath == NULL || config == NULL)
		return SP_CONFIG_INVALID_ARGUMENT;

    if (sprintf(pcaPath, "%s%s", config->spImagesDirectory, config->spPCAFilename) < 0) {
    	return SP_CONFIG_INDEX_OUT_OF_RANGE;
    }
    return SP_CONFIG_SUCCESS;
}

char* spConfigGetLoggerFilename(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spLoggerFilename;
}

SP_LOGGER_LEVEL spConfigGetLoggerLevel(const SPConfig config,
		SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return SP_LOGGER_ERROR_LEVEL;
	}
	*msg = SP_CONFIG_SUCCESS;
	switch (config->spLoggerLevel) {
	case 1:
		return SP_LOGGER_ERROR_LEVEL;
	case 2:
		return SP_LOGGER_WARNING_ERROR_LEVEL;
	case 3:
		return SP_LOGGER_INFO_WARNING_ERROR_LEVEL;
	default:
		return SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL;
	}
}

SP_KD_TREE_SPLIT_METHOD spConfigGetKDTreeSplitMethod(const SPConfig config,
		SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return MAX_SPREAD;
	}
	*msg = SP_CONFIG_SUCCESS;
	switch (config->spKDTreeSplitMethod) {
	case 0:
		return MAX_SPREAD;
	case 1:
		return RANDOM;
	case 2:
		return INCREMENTAL;
	default:
		return MAX_SPREAD;
	}
}

SP_CONFIG_MSG spConfigGetFeatsPath(char* imagePath, SPConfig config, int index) {
	if (imagePath == NULL || config == NULL)
		return SP_CONFIG_INVALID_ARGUMENT;
	if (index >= config->spNumOfImages)
		return SP_CONFIG_INDEX_OUT_OF_RANGE;

	if (sprintf(imagePath, "%s%s%d%s", config->spImagesDirectory,
			config->spImagesPrefix, index, ".feats") < 0) {
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}
	return SP_CONFIG_SUCCESS;
}

void spConfigDestroy(SPConfig config) {
	if (!config) {
		return;
	}
	free(config);	//free allocation
	config = NULL;
}

void spConfigPrintError(const char* filename, int line, int error_type, char* param_name) {
	if (filename == NULL || line < 0 || error_type < 0 || error_type > 3 || (error_type == 3 && param_name == NULL)) {
		printf("Invalid arguments\n");
		return;
	}
	switch (error_type) {
	case 1:
		printf("File: %s\nLine: %d\nMessage: Invalid configuration line\n", filename, line);
		break;
	case 2:
		printf("File: %s\nLine: %d\nMessage: Invalid value - constraint not met\n", filename, line);
		break;
	case 3:
		printf("File: %s\nLine: %d\nMessage: Parameter %s is not set\n", filename, line, param_name);
		break;
	}
}

void spConfigTerminate(SPConfig config, FILE* fp, SP_CONFIG_MSG* msg, SP_CONFIG_MSG config_msg,
		const char* filename, int lineNumber, int error_type, char* param_name) {
	if (config == NULL || fp == NULL || msg == NULL || filename == NULL)
		return;

	fclose(fp);
	spConfigPrintError(filename, lineNumber, error_type, param_name);
	spConfigDestroy(config);
	*msg = config_msg;
}

bool spConfigGetVariables(SPConfig config, FILE* fp, SP_CONFIG_MSG* msg, const char* filename, int* lineNumber) {
	char line[STR_MAX_LENGTH+1], system_param[STR_MAX_LENGTH+1], val[STR_MAX_LENGTH+1], temp[STR_MAX_LENGTH+1];
	char* token;
	while (fgets(line, STR_MAX_LENGTH+1, fp) != NULL) {
		//check if the line is a comment or a blank line
		if (sscanf (line, " %s", temp) == 1) {
			if (temp[0] == '#' || strcmp(line, "\n") == 0 || strcmp(line, "\r\n") == 0) {
				(*lineNumber)++;;
				continue;
			}
		}
		else {
			spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_STRING ,filename, *lineNumber, 1, NULL); //check msg
			free(token);
			return false;
		}

		/* get the first token */
		token = strtok(line, "=");
		if (sscanf (token," %s %s", system_param, temp) > 1) {
			spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_STRING ,filename, *lineNumber, 1, NULL); //check msg
			free(token);
			return false;
		}

		/* get the second token */
		token = strtok(NULL, "=");
		if (token == NULL) {
			spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_STRING ,filename, *lineNumber, 1, NULL); //check msg
			free(token);
			return false;
		}
		if (sscanf (token," %s %s",val, temp) > 1) {
			spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_STRING ,filename, *lineNumber, 1, NULL); //check msg
			free(token);
			return false;
		}

		/* get the third token */
		token = strtok(NULL, "=");
		if (token != NULL) {
			spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_STRING ,filename, *lineNumber, 1, NULL); //check msg
			free(token);
			return false;
		}

		// searching which parameter is part1
		if (strcmp(system_param, "spImagesDirectory") == 0) {
			strcpy(config->spImagesDirectory, val);
			(*lineNumber)++;
			continue;
		}
		if (strcmp(system_param, "spImagesPrefix") == 0) {
			strcpy(config->spImagesPrefix, val);
			(*lineNumber)++;
			continue;
		}
		if (strcmp(system_param, "spImagesSuffix") == 0) {
			if ((strcmp(val, ".jpg") == 0) || (strcmp(val, ".png") == 0) || (strcmp(val, ".bmp") == 0)
					|| (strcmp(val, ".gif") == 0)) {
				strcpy(config->spImagesSuffix, val);
				(*lineNumber)++;
				continue;
			}
			else {
				spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_STRING ,filename, *lineNumber, 2, NULL);
				free(token);
				return false;
			}
		}
		if (strcmp(system_param, "spNumOfImages") == 0) {
			if (isNumber(val)) {
				int temp = atoi(val);
				if (temp > 0) {
					config->spNumOfImages = temp;
					(*lineNumber)++;
					continue;
				}
				else {
					spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_INTEGER ,filename, *lineNumber, 2, NULL);
					free(token);
					return false;
				}
			}
			else {
				spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_INTEGER ,filename, *lineNumber, 2, NULL);
				free(token);
				return false;
			}
		}
		if (strcmp(system_param, "spPCADimension") == 0) {
			if (isNumber(val)) {
				int temp = atoi(val);
				if (temp >= 10 && temp <= 28){
					config->spPCADimension = temp;
					(*lineNumber)++;
					continue;
				}
				else {
					spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_INTEGER ,filename, *lineNumber, 2, NULL);
					free(token);
					return false;
				}
			}
			else {
				spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_INTEGER ,filename, *lineNumber, 2, NULL);
				free(token);
				return false;
			}
		}
		if (strcmp(system_param, "spPCAFilename") == 0) {
			strcpy(config->spPCAFilename, val);
			(*lineNumber)++;
			continue;
		}
		if (strcmp(system_param, "spNumOfFeatures") == 0) {
			if (isNumber(val)) {
				int temp = atoi(val);
				if (temp > 0) {
				config->spNumOfFeatures = temp;
				(*lineNumber)++;
				continue;
				}
				else {
					spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_INTEGER ,filename, *lineNumber, 2, NULL);
					free(token);
					return false;
				}
			}
			else {
				spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_INTEGER ,filename, *lineNumber, 2, NULL);
				free(token);
				return false;
			}
		}
		if (strcmp(system_param, "spExtractionMode") == 0) {
			if (strcmp(val, "true") == 0) {
				config->spExtractionMode = true;
				(*lineNumber)++;
				continue;
			}
			else if (strcmp(val, "false") == 0) {
				config->spExtractionMode = false;
				(*lineNumber)++;
				continue;
			}
			else {
				spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_STRING ,filename, *lineNumber, 2, NULL);
				free(token);
				return false;
			}
		}
		if (strcmp(system_param, "spNumOfSimilarImages") == 0) {
			if (isNumber(val)) {
				int temp = atoi(val);
				if (temp > 0) {
					config->spNumOfSimilarImages = temp;
					(*lineNumber)++;
					continue;
				}
				else {
					spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_INTEGER ,filename, *lineNumber, 2, NULL);
					free(token);
					return false;
				}
			}
			else {
				spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_INTEGER ,filename, *lineNumber, 2, NULL);
				free(token);
				return false;
			}
		}
		if (strcmp(system_param, "spKDTreeSplitMethod") == 0) {
			if (strcmp(val, "RANDOM") == 0) {
				config->spKDTreeSplitMethod = RANDOM;
				(*lineNumber)++;
				continue;
			}
			else if (strcmp(val, "MAX_SPREAD") == 0) {
				config->spKDTreeSplitMethod = MAX_SPREAD;
				(*lineNumber)++;
				continue;
			}
			else if (strcmp(val, "INCREMENTAL") == 0) {
				config->spKDTreeSplitMethod = INCREMENTAL;
				(*lineNumber)++;
				continue;
			}
			else {
				spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_STRING ,filename, *lineNumber, 2, NULL);
				free(token);
				return false;
			}
		}
		if (strcmp(system_param, "spKNN") == 0) {
			if (isNumber(val)) {
				int temp = atoi(val);
				if (temp > 0) {
					config->spKNN = temp;
					(*lineNumber)++;
					continue;
				}
				else {
					spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_INTEGER ,filename, *lineNumber, 2, NULL);
					free(token);
					return false;
				}
			}
			else {
				spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_INTEGER ,filename, *lineNumber, 2, NULL);
				free(token);
				return false;
			}
		}
		if (strcmp(system_param, "spMinimalGUI") == 0) {
			if (strcmp(val, "true") == 0) {
				config->spMinimalGUI = true;
				(*lineNumber)++;
				continue;
			}
			else if (strcmp(val, "false") == 0) {
				config->spMinimalGUI = false;
				(*lineNumber)++;
				continue;
			}
			else {
				spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_STRING ,filename, *lineNumber, 2, NULL);
				free(token);
				return false;
			}
		}
		if (strcmp(system_param, "spLoggerLevel") == 0) {
			if (isNumber(val)) {
				int temp = atoi(val);
				if (temp >= 1 && temp <= 4){
					config->spLoggerLevel = temp;
					(*lineNumber)++;
					continue;
				}
				else {
					spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_INTEGER ,filename, *lineNumber, 2, NULL);
					free(token);
					return false;
				}
			}
			else {
				spConfigTerminate(config, fp, msg, SP_CONFIG_INVALID_INTEGER ,filename, *lineNumber, 2, NULL);
				free(token);
				return false;
			}
		}
		if (strcmp(system_param, "spLoggerFilename") == 0) {
			strcpy(config->spLoggerFilename, val);
			(*lineNumber)++;
			continue;
		}
	}

	//end of line scanning (while)
	free(token);
	return true;
}

void spConfigSetDefaultValues(SPConfig config) {
	config->spPCADimension = DEFAULT_PCA_DIM;
	strcpy(config->spPCAFilename, DEFAULT_PCA_FILENAME);
	config->spNumOfFeatures = DEFAULT_NUM_OF_FEATURES;
	config->spExtractionMode = DEFAULT_EXTRACT_MODE;
	config->spMinimalGUI = DEFAULT_MINIMAL_GUI;
	config->spNumOfSimilarImages = DEFAULT_NUM_OF_SIMILAR_IMGS;
	config->spKNN = DEFAULT_KNN;
	config->spKDTreeSplitMethod = DEFAULT_KDT_SPLIT_METHOD;
	config->spLoggerLevel = DEFAULT_LOGGER_LVL;
	strcpy(config->spLoggerFilename, DEFAULT_LOGGER_FILENAME);

	//str and int defaults:
	strcpy(config->spImagesDirectory, DEFAULT_STR);
	strcpy(config->spImagesPrefix, DEFAULT_STR);
	strcpy(config->spImagesSuffix, DEFAULT_STR);
	config->spNumOfImages = DEFAULT_INT;
}

bool spConfigCheckVariablesInitialized(SPConfig config, FILE* fp, SP_CONFIG_MSG* msg, const char* filename,
		int lineNumber) {
	if (strcmp(config->spImagesDirectory, DEFAULT_STR) == 0) {
		spConfigTerminate(config, fp, msg, SP_CONFIG_MISSING_DIR ,filename, lineNumber, 3, "spImagesDirectory");
		return false;
	}
	if (strcmp(config->spImagesPrefix, DEFAULT_STR) == 0) {
		spConfigTerminate(config, fp, msg, SP_CONFIG_MISSING_PREFIX ,filename, lineNumber, 3, "spImagesPrefix");
		return false;
	}
	if (strcmp(config->spImagesSuffix, DEFAULT_STR) == 0) {
		spConfigTerminate(config, fp, msg, SP_CONFIG_MISSING_SUFFIX ,filename, lineNumber, 3, "spImagesSuffix");
		return false;
	}
	if (config->spNumOfImages == DEFAULT_INT) {
		spConfigTerminate(config, fp, msg, SP_CONFIG_MISSING_NUM_IMAGES ,filename, lineNumber, 3, "spNumOfImages");
		return false;
	}
	return true;
}

bool isNumber(char* num) {
    for (int i=0; num[i] != 0; i++) {
        //if (number[i] > '9' || number[i] < '0')
        if (!isdigit(num[i]))
            return false;
    }
    return true;
}

void spConfigPrintInfo(const SPConfig config) {
	printf("/---------- CONFIG INFO ----------/\n");
	printf("spImagesDirectory: %s\n", config->spImagesDirectory);
	printf("spImagesPrefix: %s\n", config->spImagesPrefix);
	printf("spImagesSuffix: %s\n", config->spImagesSuffix);
	printf("spNumOfImages: %d\n", config->spNumOfImages);
	printf("spPCADimension: %d\n", config->spPCADimension);
	printf("spPCAFilename: %s\n", config->spPCAFilename);
	printf("spNumOfFeatures: %d\n", config->spNumOfFeatures);
	printf("spExtractionMode: %d\n", config->spExtractionMode);
	printf("spNumOfSimilarImages: %d\n", config->spNumOfSimilarImages);
	printf("spKDTreeSplitMethod: %d\n", config->spKDTreeSplitMethod);
	printf("spKNN: %d\n", config->spKNN);
	printf("spMinimalGUI: %d\n", config->spMinimalGUI);
	printf("spLoggerLevel: %d\n", config->spLoggerLevel);
	printf("spLoggerFilename: %s\n", config->spLoggerFilename);
	printf("/-------- CONFIG INFO END --------/\n");
}
