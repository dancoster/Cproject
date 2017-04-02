#ifndef SPLOGGER_H_
#define SPLOGGER_H_
/**
 * SP Logger summary:
 * SP Logger is defined at compilation time and it must be initialized
 * by calling the create function prior to the usage. 
 *
 * The logger has 4 print levels:
 * 	- Error level: In this level only error messages are printed
 * 	- Warning level: In this level only warning and error messages are printed
 * 	- Info level: in this level info, warning and error messages are printed
 * 	- Debug level: in this level all message are printed
 * 	
 * The logger supports another printing function which can be called at any level
 * The user must destroy the logger at end of usage
 *	
 * The following functions are supported:
 * spLoggerCreate 		- Creates and initializes the logger
 * spLoggerDestroy		- Closes are frees all resources of the logger
 * spLoggerPrintError   - Prints error messages at leves {Error, Warning, Info, Debug}
 * spLoggerPrintWarning - Prints warnning messages at levels {Warning, Info, Debug}
 * spLoggerPrintInfo    - Prints info messages at levels {Info, Debug}
 * spLoggerPrintDebug   - Prints debug messages at level {Debug}
 * spLoggerPrintMsg     - Prints the exact message at any level (Without formatting)
 */

#define STR_MAX_LENGTH 1024
#define ALLOCATION_ERROR "Allocation error occurred\n"
#define INVALID_ARGUMENTS_ERROR "one or more of function arguments is not valid\n"
#define FUNCTION_ERROR "the function couldn't be complete\n"

/** A type used to decide the level of the logger**/
typedef enum sp_logger_level_t {
	SP_LOGGER_ERROR_LEVEL, //Error level
	SP_LOGGER_WARNING_ERROR_LEVEL, //Warning level
	SP_LOGGER_INFO_WARNING_ERROR_LEVEL, //Info level
	SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL //Debug level
} SP_LOGGER_LEVEL;

/** A type used to indicate errors in function calls **/
typedef enum sp_logger_msg_t {
	SP_LOGGER_CANNOT_OPEN_FILE,
	SP_LOGGER_INVAlID_ARGUMENT,
	SP_LOGGER_OUT_OF_MEMORY,
	SP_LOGGER_UNDIFINED,
	SP_LOGGER_DEFINED,
	SP_LOGGER_WRITE_FAIL,
	SP_LOGGER_SUCCESS
} SP_LOGGER_MSG;

/** A type used for defining the logger**/
typedef struct sp_logger_t* SPLogger;

/**
 * Creates a logger. This function should be called once, prior
 * to the usage of any SP Logger print functions. It is the responsibility
 * of the user to create the logger prior to usage, and the logger
 * must be destroyed at the end of usage.
 *
 * @param filename - The name of the log file, if not specified stdout is used
 * 					 as default.
 * @param level - The level of the logger prints
 * @return
 * SP_LOGGER_DEFINED 			- The logger has been defined
 * SP_LOGGER_OUT_OF_MEMORY 		- In case of memory allocation failure
 * SP_LOGGER_CANNOT_OPEN_FILE 	- If the file given by filename cannot be opened
 * SP_LOGGER_SUCCESS 			- In case the logger has been successfully opened
 */
SP_LOGGER_MSG spLoggerCreate(const char* filename, SP_LOGGER_LEVEL level);

/**
 * Frees all memory allocated for the logger. If the logger is not defined
 * then nothing happens.
 */
void spLoggerDestroy();

/**
 * 	Prints error message. The error message format is given below:
 * 	---ERROR---
 * 	- file: <file>
 *  - function: <function>
 *  - line: <line>
 *  - message: <msg>
 *
 * 	<file> 	   - is the string given by file, it represents the file in which
 * 		   		 the error print call occurred.
 * 	<function> - is the string given by function, it represents the function in which
 * 			   	 the error print call occurred.
 * 	<line> 	   - is the string given by line, it represents the line in which
 * 		   		 the error print call occurred
 * 	<msg> 	   - is the string given by msg, it contains the msg given by the user
 *
 * 	Error messages will be printed at levels:
 *
 * 	SP_LOGGER_ERROR_LEVEL,
 *	SP_LOGGER_WARNING_ERROR_LEVEL,
 *	SP_LOGGER_INFO_WARNING_ERROR_LEVEL,
 *	SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL
 *
 * 	A new line will be printed after the print call.
 *
 * @param msg     	- The message to printed
 * @param file    	- A string representing the filename in which spLoggerPrintError call occurred
 * @param function 	- A string representing the function name in which spLoggerPrintError call ocurred
 * @param line		- A string representing the line in which the function call occurred
 * @return
 * SP_LOGGER_UNDIFINED 			- If the logger is undefined
 * SP_LOGGER_INVAlID_ARGUMENT	- If any of msg or file or function are null or line is negative
 * SP_LOGGER_WRITE_FAIL			- If Write failure occurred
 * SP_LOGGER_SUCCESS			- otherwise
 */
SP_LOGGER_MSG spLoggerPrintError(const char* msg, const char* file,
		const char* function, const int line);

/**
 * 	Prints warning message. The warning message format is given below:
 * 	---WARNING---
 * 	- file: <file>
 *  - function: <function>
 *  - line: <line>
 *  - message: <msg>
 *
 * 	<file> 	   - is the string given by file, it represents the file in which
 * 		   		 the warning print call occurred.
 * 	<function> - is the string given by function, it represents the function in which
 * 			   	 the warning print call occurred.
 * 	<line> 	   - is the string given by line, it represents the line in which
 * 		   		 the warning print call occurred
 * 	<msg> 	   - is the string given by msg, it contains the msg given by the user
 *
 * 	Warning messages will be printed at levels:
 *
 *	SP_LOGGER_WARNING_ERROR_LEVEL,
 *	SP_LOGGER_INFO_WARNING_ERROR_LEVEL,
 *	SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL
 *  
 *	A new line will be printed after the print call.
 *
 * @param msg     	- The message to printed
 * @param file    	- A string representing the filename in which spLoggerPrintWarning call occurred
 * @param function 	- A string representing the function name in which spLoggerPrintWarning call ocurred
 * @param line		- A string representing the line in which the spLoggerPrintWarning call occurred
 * @return
 * SP_LOGGER_UNDIFINED 			- If the logger is undefined
 * SP_LOGGER_INVAlID_ARGUMENT	- If any of msg or file or function are null or line is negative
 * SP_LOGGER_WRITE_FAIL			- If write failure occurred
 * SP_LOGGER_SUCCESS			- otherwise
 */
SP_LOGGER_MSG spLoggerPrintWarning(const char* msg, const char* file,
		const char* function, const int line);
/**
 * 	Prints Info message. The info message format is given below:
 * 	---INFO---
 *  - message: <msg>
 *
 * 	<msg> 	   - is the string given by msg, it contains the msg given by the user
 *
 * 	Info messages will be printed at levels:
 *
 *	SP_LOGGER_INFO_WARNING_ERROR_LEVEL,
 *	SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL
 *
 * 	A new line will be printed after the print call.
 *
 * @param msg     	- The message to printed
 * @return
 * SP_LOGGER_UNDIFINED 			- If the logger is undefined
 * SP_LOGGER_INVAlID_ARGUMENT	- If msg is null
 * SP_LOGGER_WRITE_FAIL			- If Write failure occurred
 * SP_LOGGER_SUCCESS			- otherwise
 */
SP_LOGGER_MSG spLoggerPrintInfo(const char* msg);

/**
 * 	Prints the debug message. The debug message format is given below:
 * 	---DEBUG---
 * 	- file: <file>
 *  - function: <function>
 *  - line: <line>
 *  - message: <msg>
 *
 * 	<file> 	   - is the string given by file, it represents the file in which
 * 		   		 the debug print call occurred.
 * 	<function> - is the string given by function, it represents the function in which
 * 			   	 the debug print call occurred.
 * 	<line> 	   - is the string given by line, it represents the line in which
 * 		   		 the debug print call occurred
 * 	<msg> 	   - is the string given by msg, it contains the msg given by the user
 *
 * 	Debug messages will be printed at level:
 *
 *	SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL
 *
 * 	A new line will be printed after the print call.
 *
 * @param msg     	- The message to printed
 * @param file    	- A string representing the filename in which spLoggerPrintWarning call occurred
 * @param function 	- A string representing the function name in which spLoggerPrintWarning call ocurred
 * @param line		- A string representing the line in which the function call occurred
 * @return
 * SP_LOGGER_UNDIFINED 			- If the logger is undefined
 * SP_LOGGER_INVAlID_ARGUMENT	- If any of msg or file or function are null or line is negative
 * SP_LOGGER_WRITE_FAIL			- If Write failure occurred
 * SP_LOGGER_SUCCESS			- otherwise
 */
SP_LOGGER_MSG spLoggerPrintDebug(const char* msg, const char* file,
		const char* function, const int line);

/**
 * The given message is printed. A new line is printed at the end of msg
 * The message will be printed in all levels.
 *
 * @param msg - The message to be printed
 * @return
 * SP_LOGGER_UNDIFINED 			- If the logger is undefined
 * SP_LOGGER_INVAlID_ARGUMENT	- If msg is null
 * SP_LOGGER_WRITE_FAIL			- If Write failure occurred
 * SP_LOGGER_SUCCESS			- otherwise
 */
SP_LOGGER_MSG spLoggerPrintMsg(const char* msg);

#endif
