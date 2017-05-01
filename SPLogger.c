#include "SPLogger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//File open mode
#define SP_LOGGER_OPEN_MODE "w"

// Global variable holding the logger
SPLogger logger = NULL;

struct sp_logger_t {
	FILE* outputChannel; //The logger file
	bool isStdOut; //Indicates if the logger is stdout
	SP_LOGGER_LEVEL level; //Indicates the level
};

SP_LOGGER_MSG spLoggerCreate(const char* filename, SP_LOGGER_LEVEL level) {
	if (logger != NULL) { //Already defined
		return SP_LOGGER_DEFINED;
	}
	logger = (SPLogger) malloc(sizeof(*logger));
	if (logger == NULL) { //Allocation failure
		return SP_LOGGER_OUT_OF_MEMORY;
	}
	logger->level = level; //Set the level of the logger
	if (filename == NULL) { //In case the filename is not set use stdout
		logger->outputChannel = stdout;
		logger->isStdOut = true;
	} else { //Otherwise open the file in write mode
		logger->outputChannel = fopen(filename, SP_LOGGER_OPEN_MODE);
		if (logger->outputChannel == NULL) { //Open failed
			free(logger);
			logger = NULL;
			return SP_LOGGER_CANNOT_OPEN_FILE;
		}
		logger->isStdOut = false;
	}
	return SP_LOGGER_SUCCESS;
}

void spLoggerDestroy() {
	if (!logger) {
		return;
	}
	if (!logger->isStdOut) {//Close file only if not stdout
		fclose(logger->outputChannel);
	}
	free(logger);//free allocation
	logger = NULL;
}

SP_LOGGER_MSG spLoggerPrintError(const char* msg, const char* file,
		const char* function, const int line) {
	if (msg==NULL || file==NULL || function==NULL || line<0) {
		return SP_LOGGER_INVAlID_ARGUMENT;
	}
	if (logger == NULL) {
		return SP_LOGGER_UNDIFINED;
	}

	if (logger->level == SP_LOGGER_ERROR_LEVEL
			|| logger->level == SP_LOGGER_WARNING_ERROR_LEVEL
			|| logger->level == SP_LOGGER_INFO_WARNING_ERROR_LEVEL
			|| logger->level == SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL) {
		char output[STR_MAX_LENGTH+1];
		if (sprintf(output,
				"---ERROR---\n- file: %s\n- function: %s\n- line: %d\n- message: %s\n", file, function, line, msg) < 0) {
			return SP_LOGGER_WRITE_FAIL;
		}
		if (fprintf(logger->outputChannel, "%s", output) < 0) {
			return SP_LOGGER_WRITE_FAIL;
		}
		else {
			return SP_LOGGER_SUCCESS;
		}
	}
	else {
		return SP_LOGGER_SUCCESS;
	}
}

SP_LOGGER_MSG spLoggerPrintWarning(const char* msg, const char* file,
		const char* function, const int line) {
	if (msg==NULL || file==NULL || function==NULL || line<0) {
		return SP_LOGGER_INVAlID_ARGUMENT;
	}
	if (logger == NULL) {
		return SP_LOGGER_UNDIFINED;
	}

	if (logger->level == SP_LOGGER_WARNING_ERROR_LEVEL
			|| logger->level == SP_LOGGER_INFO_WARNING_ERROR_LEVEL
			|| logger->level == SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL) {
		char output[STR_MAX_LENGTH+1];
		if (sprintf(output,
				"---WARNING---\n- file: %s\n- function: %s\n- line: %d\n- message: %s\n", file, function, line, msg) < 0) {
			return SP_LOGGER_WRITE_FAIL;
		}
		if (fprintf(logger->outputChannel, "%s", output) < 0) {
			return SP_LOGGER_WRITE_FAIL;
		}
		else {
			return SP_LOGGER_SUCCESS;
		}
	}
	else {
		return SP_LOGGER_SUCCESS;
	}
}

SP_LOGGER_MSG spLoggerPrintInfo(const char* msg) {
	if (msg==NULL) {
		return SP_LOGGER_INVAlID_ARGUMENT;
	}
	if (logger == NULL) {
		return SP_LOGGER_UNDIFINED;
	}

	if (logger->level == SP_LOGGER_INFO_WARNING_ERROR_LEVEL
			|| logger->level == SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL) {
		char output[STR_MAX_LENGTH+1];
		if (sprintf(output, "---INFO---\n- message: %s\n", msg) < 0) {
			return SP_LOGGER_WRITE_FAIL;
		}
		if (fprintf(logger->outputChannel, "%s", output) < 0) {
			return SP_LOGGER_WRITE_FAIL;
		}
		else {
			return SP_LOGGER_SUCCESS;
		}
	}
	else {
		return SP_LOGGER_SUCCESS;
	}
}

SP_LOGGER_MSG spLoggerPrintDebug(const char* msg, const char* file,
		const char* function, const int line) {
	if (msg==NULL || file==NULL || function==NULL || line<0) {
		return SP_LOGGER_INVAlID_ARGUMENT;
	}
	if (logger == NULL) {
		return SP_LOGGER_UNDIFINED;
	}

	if (logger->level == SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL) {
		char output[STR_MAX_LENGTH+1];
		if (sprintf(output,
				"---DEBUG---\n- file: %s\n- function: %s\n- line: %d\n- message: %s\n", file, function, line, msg) < 0) {
			return SP_LOGGER_WRITE_FAIL;
		}
		if (fprintf(logger->outputChannel, "%s", output) < 0) {
			return SP_LOGGER_WRITE_FAIL;
		}
		else {
			return SP_LOGGER_SUCCESS;
		}
	}
	else {
		return SP_LOGGER_SUCCESS;
	}
}

SP_LOGGER_MSG spLoggerPrintMsg(const char* msg) {
	if (msg==NULL) {
		return SP_LOGGER_INVAlID_ARGUMENT;
	}
	if (logger == NULL) {
		return SP_LOGGER_UNDIFINED;
	}

	if (logger->level == SP_LOGGER_ERROR_LEVEL
			|| logger->level == SP_LOGGER_WARNING_ERROR_LEVEL
			|| logger->level == SP_LOGGER_INFO_WARNING_ERROR_LEVEL
			|| logger->level == SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL) {
		if (fprintf(logger->outputChannel, "%s", msg) < 0) {
			return SP_LOGGER_WRITE_FAIL;
		}
		else {
			return SP_LOGGER_SUCCESS;
		}
	}
	else {
		return SP_LOGGER_SUCCESS;
	}
}
