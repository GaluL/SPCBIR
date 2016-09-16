/*
 * SPConfig.c
 *
 *  Created on: Aug 20, 2016
 *      Author: galkl
 */
#include "SPConfig.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "SPCommonDefs.h"
#include "SPMainAux.h"


struct sp_config_t
{
	char* spImagesDirectory;
	char* spImagesPrefix;
	char* spImagesSuffix;
	int spNumOfImages;
	int spPCADimension;
	char* spPCAFilename;
	int spNumOfFeatures;
	bool spExtractionMode;
	int spNumOfSimilarImages;
	SP_KDTREE_SPLIT_METHOD spKDTreeSplitMethod;
	int spKNN;
	bool spMinimalGUI;
	int spLoggerLevel;
	char* spLoggerFilename;

} ;

// function that sets the default values of the structure.

SPConfig spConfigInit(SP_CONFIG_MSG* msg)
{
	SPConfig config = NULL;
	config = (SPConfig)malloc(sizeof(*config));
	// the error will be printed from the calller.
	if (!config)
	{
		return NULL;
	}
	config->spImagesDirectory = NULL;
	config->spImagesPrefix = NULL;
	config->spImagesSuffix = NULL;
	config->spNumOfImages = 0;
	config->spPCADimension = PCA_DIMENSION_DEFULT;
	config->spPCAFilename = (char*)malloc((SIZE_OF_PCAYML + 1) * sizeof(char));
	if (!config->spPCAFilename)
	{
		*msg = SP_CONFIG_ALLOC_FAIL;
		free(config);
		config = NULL;
		return NULL;
	}
	strcpy(config->spPCAFilename, PCA_FILE_NAME_DEFULT);

	config->spNumOfFeatures = NUM_OF_FEATURES_DEFULT;
	config->spExtractionMode = true;
	config->spNumOfSimilarImages = NUM_OF_SIMILAR_IMAGES_DEFULT;
	config->spKDTreeSplitMethod = MAX_SPREAD;
	config->spKNN = KNN_DEFULT;
	config->spMinimalGUI = false;
	config->spLoggerLevel = LOGGER_LEVEL_DEFULT;
	config->spLoggerFilename  = (char*)malloc((SIZE_OF_STDOUT +1) * sizeof(char));
	if (!config->spLoggerFilename)
	{
		*msg = SP_CONFIG_ALLOC_FAIL;
		free(config->spPCAFilename);
		config->spPCAFilename = NULL;
		free(config);
		config = NULL;
		return NULL;
	}

	strcpy(config->spLoggerFilename, LOGGER_FILE_NAME_DEFULT);
	*msg = SP_CONFIG_SUCCESS;
	return config;
}

/*
 * functions that prints config errors
 */
void spPrintInvalidLineError(const char* filename, int line)
{
	printf("%s%s\n%s%d\n%s%s\n",ERROR_FILE, filename, ERROR_LINE, line, ERROR_MASSAGE,
			ERROR_INVALID_CONFIGURATION_LINE);
	fflush(NULL);
}

void spPrintConstraintNotMetError(const char* filename, int line)
{
	printf("%s%s\n%s%d\n%s%s\n", ERROR_FILE, filename, ERROR_LINE, line, ERROR_MASSAGE,
			ERROR_INVALID_VALUE);
	fflush(NULL);
}

void spPrintParameterNotSetError(const char* filename, int line, char* parameterName)
{
	printf("%s%s\n%s%d\n%s%s%s%s\n", ERROR_FILE, filename, ERROR_LINE, line,
			 ERROR_MASSAGE, ERROR_PARAMTER, parameterName, ERROR_IS_NOT_SET);
	fflush(NULL);
}

// setter for spImagesDirectory
bool setspImagesDirectory (SPConfig config, char* variable_value, SP_CONFIG_MSG* msg)
{
	if (config->spImagesDirectory)
	{
		free(config->spImagesDirectory);
		config->spImagesDirectory = NULL;
	}

	config->spImagesDirectory = (char*)malloc((strlen(variable_value) +1) * sizeof(char));
	if (!config->spImagesDirectory)
	{
		*msg = SP_CONFIG_ALLOC_FAIL;
		return false;
	}
	strcpy(config->spImagesDirectory,variable_value);
	return true;
}
// setter for spImagesPrefix
bool setspImagesPrefix (SPConfig config, char* variable_value, SP_CONFIG_MSG* msg)
{
	if (config->spImagesPrefix)
	{
		free(config->spImagesPrefix);
		config->spImagesPrefix = NULL;
	}

	config->spImagesPrefix = (char*)malloc((strlen(variable_value) +1) * sizeof(char));
	if (!config->spImagesPrefix)
	{
		*msg = SP_CONFIG_ALLOC_FAIL;
		return false;
	}

	strcpy(config->spImagesPrefix,variable_value);
	return true;
}
// setter for spImagesSuffix
bool setspImagesSuffix (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
		const char* filename)
{
	if (config->spImagesSuffix)
	{
		free(config->spImagesSuffix);
		config->spImagesSuffix = NULL;
	}

	// check we got one of the right suffix
	if ((strcmp(variable_value,SUFFIX_JPEG)== 0) || (strcmp(variable_value,SUFFIX_PNG)== 0)||
			(strcmp(variable_value,SUFFIX_BMP)== 0) || (strcmp(variable_value,SUFFIX_GIF)== 0))
	{
		config->spImagesSuffix = (char*)malloc((strlen(variable_value) +1) * sizeof(char));
		if (!config->spImagesSuffix)
		{
			*msg = SP_CONFIG_ALLOC_FAIL;
			return false;
		}
		strcpy(config->spImagesSuffix,variable_value);
		return true;
	}
	// if we got  other suffix return invalid string
	else
	{
		*msg = SP_CONFIG_INVALID_STRING;
		spPrintConstraintNotMetError(filename, line);
		return false;
	}
}
// setter for spNumOfImages
bool setspNumOfImages (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
		const char* filename)
{
	// check for the constraint
	if (atoi(variable_value) > 0)
	{
		config->spNumOfImages = atoi(variable_value);
		return true;
	}
	else
	{
		*msg = SP_CONFIG_INVALID_INTEGER;
		spPrintConstraintNotMetError(filename, line);
		return false;
	}
}
// setter for spPCADimension
bool setspPCADimension (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
		const char* filename)
{
	// check for the constraint
	if ((atoi(variable_value) > SP_PCA_DIMENSION_CONSTRAINT_LOW )&&
					(atoi(variable_value) < SP_PCA_DIMENSION_CONSTRAINT_HIGH))
	{
		config->spPCADimension = atoi(variable_value);
		return true;
	}
	else
	{
		*msg = SP_CONFIG_INVALID_INTEGER;
		spPrintConstraintNotMetError(filename, line);
		return false;
	}
}
// setter for spPCAFilename
bool setspPCAFilename (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg)
{
	// releasing the default value assigned before
	if (config->spPCAFilename)
	{
		free(config->spPCAFilename);
		config->spPCAFilename = NULL;
	}

	config->spPCAFilename = (char*)malloc((strlen(variable_value) +1) * sizeof(char));
	if (!config->spPCAFilename)
	{
		*msg = SP_CONFIG_ALLOC_FAIL;
		return false;
	}
	strcpy(config->spPCAFilename, variable_value);
	return true;
}
// setter for spNumOfFeatures
bool setspNumOfFeatures (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
		const char* filename)
{
	// check for the constraint
	if (atoi(variable_value) > 0)
	{
		config->spNumOfFeatures = atoi(variable_value);
		return true;
	}
	else
	{
		*msg = SP_CONFIG_INVALID_INTEGER;
		spPrintConstraintNotMetError(filename, line);
		return false;
	}
}
// setter for spExtractionMode
bool setspExtractionMode (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
		const char* filename)
{
	// check for the constraint
	if (strcmp(variable_value,MODE_TRUE)== 0)
	{
		config->spExtractionMode = true;

		return true;
	}
	else if (strcmp(variable_value,MODE_FALSE)== 0)
	{
		config->spExtractionMode = false;

		return true;
	}
	else
	{
		*msg = SP_CONFIG_INVALID_STRING;
		spPrintConstraintNotMetError(filename, line);
		return false;
	}
}
// setter for spNumOfSimilarImages
bool setspNumOfSimilarImages (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
			const char* filename)
{
	// check for the constraint
	if(atoi(variable_value) > 0)
	{
		config->spNumOfSimilarImages  = atoi(variable_value);
		return true;
	}
	else
	{
		*msg = SP_CONFIG_INVALID_INTEGER;
		spPrintConstraintNotMetError(filename, line);
		return false;
	}
}
// setter for spKDTreeSplitMethod
bool setspKDTreeSplitMethod (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
			const char* filename)
{
	// check for the constraint
	if (! strcmp(variable_value, SPLIT_METHOD_RANDOM))
	{
		config->spKDTreeSplitMethod  = RANDOM;
	}
	else if (! strcmp(variable_value, SPLIT_METHOD_MAX_SPREAD))
	{
		config->spKDTreeSplitMethod  = MAX_SPREAD;
	}
	else if (! strcmp(variable_value, SPLIT_METHOD_MAX_INCREMENTAL))
	{
		config->spKDTreeSplitMethod  = INCREMENTAL;
	}
	else
	{
		*msg = SP_CONFIG_INVALID_STRING;
		spPrintConstraintNotMetError(filename, line);
		return false;
	}
	return true;
}
// setter for spKNN
bool setspKNN (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
			const char* filename)
{
	// check for the constraint
	if(atoi(variable_value) > 0)
	{
		config->spKNN = atoi(variable_value);
		return true;
	}
	else
	{
		*msg = SP_CONFIG_INVALID_INTEGER;
		spPrintConstraintNotMetError(filename, line);
		return false;
	}
}
// setter for spMinimalGUI
bool setspMinimalGUI (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
			const char* filename)
{
	// check for the constraint
	if (strcmp(variable_value,MODE_TRUE)== 0)
	{
		config->spMinimalGUI = true;
	}
	else if (strcmp(variable_value,MODE_FALSE)== 0)
	{
		config->spMinimalGUI = false;
	}
	else
	{
		*msg = SP_CONFIG_INVALID_STRING;
		spPrintConstraintNotMetError(filename, line);
		return false;
	}
	return true;
}
// setter for spLoggerLevel
bool setspLoggerLevel (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
			const char* filename)
{
	// check for the constraint
	if ((atoi(variable_value) > LOGGER_LEVEL_MIN) && (atoi(variable_value) < LOGGER_LEVEL_MAX))
	{
		config->spLoggerLevel = atoi(variable_value);
		return true;
	}
	else
	{
		*msg = SP_CONFIG_INVALID_INTEGER;
		spPrintConstraintNotMetError(filename, line);
		return false;
	}
}
// setter for spLoggerFilename
bool setspLoggerFilename (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg)
{
	if (config->spLoggerFilename)
	{
		free(config->spLoggerFilename);
		config->spLoggerFilename = NULL;
	}

	config->spLoggerFilename = (char*)malloc((strlen(variable_value) +1) * sizeof(char));
	if (!config->spLoggerFilename)
	{
		*msg = SP_CONFIG_ALLOC_FAIL;
		return false;
	}
	strcpy(config->spLoggerFilename,variable_value);
	return true;
}

// the function find the argument in the config needed to be assigned and assign the variable value
bool spAssignArgument(SPConfig config, char* variable_name, char* variable_value, SP_CONFIG_MSG* msg, int line,
		const char* filename)
{
	if (! strcmp(variable_name,SP_IMAGES_DIRECTORY))
	{
		return setspImagesDirectory(config, variable_value, msg);
	}
	else if (! strcmp(variable_name,SP_IMAGES_PREFIX))
	{
		return setspImagesPrefix(config, variable_value, msg);
	}
	else if (! strcmp(variable_name,SP_IMAGES_SUFFIX))
	{
		return setspImagesSuffix(config, variable_value, msg, line, filename);
	}
	else if (! strcmp(variable_name,SP_NUM_OF_IMAGES))
	{
		return setspNumOfImages(config, variable_value, msg, line, filename);
	}
	else if (! strcmp(variable_name,SP_PCA_DIMENSION))
	{
		return setspPCADimension(config, variable_value, msg, line, filename);
	}
	else if (! strcmp(variable_name,SP_PCA_FILE_NAME))
	{
		return setspPCAFilename(config, variable_value, msg);
	}
	else if (!strcmp(variable_name, SP_NUM_OF_FEATURES))
	{
		return setspNumOfFeatures(config, variable_value, msg, line, filename);
	}
	else if (! strcmp(variable_name,SP_EXTRACTION_MODE))
	{
		return setspExtractionMode(config, variable_value, msg, line, filename);
	}
	else if (! strcmp(variable_name,SP_NUM_OF_SIMILAR_IMAGES))
	{
		return setspNumOfSimilarImages(config, variable_value, msg, line, filename);
	}
	else if (!strcmp(variable_name,SP_KD_TREE_SPLIT_METHOD))
	{
		return setspKDTreeSplitMethod(config, variable_value, msg, line, filename);
	}
	else if (! strcmp(variable_name,SP_KNN))
	{
		return setspKNN(config, variable_value, msg, line, filename);
	}
	else if (! strcmp(variable_name,SP_MINMIMAL_GUI))
	{
		return setspMinimalGUI(config, variable_value, msg, line, filename);
	}
	else if (! strcmp(variable_name,SP_MY_LOGGER_LEVEL))
	{
		return setspLoggerLevel(config, variable_value, msg, line, filename);
	}
	else if (! strcmp(variable_name,SP_LOGGER_FILE_NAME))
	{
		return setspLoggerFilename(config, variable_value, msg);
	}
	// if we got here means the there is no such field in the config structure
	else
	{
		*msg = SP_CONFIG_INVALID_STRING;
		spPrintInvalidLineError(filename, line);
		return false;
	}
}
// cut the white spaces of the received string from the beginning and the end
char* trimWhitespace(char *str)
{
	char *end;

	// Trim leading space
	while(isspace(*str))
	{
		str++;
	}

	// if all whitespaces
	if(*str == 0)
	{
		return str;
	}

	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && isspace(*end))
	{
		end--;
	}

	// Write new null terminator
	*(end+1) = 0;

	return str;
}
// given a string the function will check if he is valid
//and clean the white spaces from the beginning and the end
char* cleanAssignmentOperand(char* str, SP_CONFIG_MSG* msg, const char* filename, int line,
		bool isParameterValue)
{
	char* result = NULL;
	int i = 0;
	char* trimmedStr = NULL;
	int length = 0;

	trimmedStr = trimWhitespace(str);
	length = strlen(trimmedStr);

	for (i = 0; i < length; i++)
	{
		//	checking if the letter is #   or has  spaces is the middle
		if( (trimmedStr[i] == COMMENT_MARK) || isspace(trimmedStr[i]))
		{
			*msg = SP_CONFIG_INVALID_STRING;

			if (!isParameterValue || trimmedStr[i] == COMMENT_MARK)
			{
				spPrintInvalidLineError(filename, line);
			}
			else
			{
				spPrintConstraintNotMetError(filename, line);
			}

			return NULL;
		}
	}

	result = (char*)malloc((strlen(trimmedStr) + 1) * sizeof(char));
	if (!result)
	{
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}

	strcpy(result, trimmedStr);

	return result;
}

bool processAssignmentLine(SPConfig config, const char* line, const char* filename, int lineNumber, SP_CONFIG_MSG* configMsg)
{
	char* equalMarkIndex = NULL;
	char* tmpVariableName = NULL;
	char* tmpVariableValue = NULL;
	int variableLen = 0;
	int valueLen = 0;
	char* variableName = NULL;
	char* variableValue = NULL;

	// check for '=' mark if none exist we have an error
	equalMarkIndex = strchr(line, EQUAL_MARK);
	if (!equalMarkIndex)
	{
		// DONT KNOW IF INVALID STRING IS THE RIGHT CHOISE
		*configMsg = SP_CONFIG_INVALID_STRING;
		spPrintInvalidLineError(filename, lineNumber);
		return NULL;
	}
	//getting the length of the value and the variable
	variableLen = (int)(equalMarkIndex - line);
	valueLen = (int)(line + strlen(line) - equalMarkIndex);

	tmpVariableName = (char*)malloc((variableLen + 1) * sizeof(char));
	tmpVariableValue = (char*)malloc((valueLen + 1) * sizeof(char));

	strncpy(tmpVariableName, line, variableLen);
	tmpVariableName[variableLen] = NULL_TERMINATE;
	strncpy(tmpVariableValue, line + variableLen + 1, valueLen);
	tmpVariableValue[valueLen] = NULL_TERMINATE;

	// cleaning the variables from white spaces in the beginning and the end
	variableName = cleanAssignmentOperand(tmpVariableName, configMsg, filename, lineNumber, false);
	if (!variableName || strlen(variableName) == 0)
	{
		// Checking if variable name is empty
		if (variableName)
		{
			spPrintInvalidLineError(filename, lineNumber);
		}

		free(tmpVariableName);
		free(tmpVariableValue);
		return false;
	}

	variableValue = cleanAssignmentOperand(tmpVariableValue, configMsg, filename, lineNumber, true);
	if (!variableValue || strlen(variableValue) == 0)
	{
		// Checking if value is empty
		if (variableValue)
		{
			spPrintInvalidLineError(filename, lineNumber);
		}

		free(variableName);
		free(tmpVariableName);
		free(tmpVariableValue);
		return false;
	}

	free(tmpVariableName);
	free(tmpVariableValue);
	// making the assignment
	if (!spAssignArgument(config, variableName, variableValue,
			configMsg, lineNumber, filename))
	{
		free(variableName);
		free(variableValue);
		return false;
	}

	free(variableName);
	free(variableValue);

	return true;
}

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg)
{
	int line_counter = 1;
	char* input = NULL;
	char* tmpInput = NULL;
	FILE *file;
	char* newLinePointer = NULL;
	// checking the file name
	if (!filename)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		printf("%s %s %s\n", ERROR_THE_CONFIGURATION_FILE, filename, ERROR_COULD_NOT_OPEN);
		fflush(NULL);
		return NULL;
	}
	// open the config file
	file = fopen(filename, "r");
	if (!file)
	{
		// check whether it is the deafaul file or user file
		if (strcmp(filename,DEFAULT_CONFIG_FILE) == 0)
		{
			flushed_printf(ERROR_SPCBIR_NOT_OPEN);
		}
		else
		{
			printf("%s %s %s\n", ERROR_THE_CONFIGURATION_FILE, filename, ERROR_COULD_NOT_OPEN);
			fflush(NULL);
		}

		*msg = SP_CONFIG_CANNOT_OPEN_FILE;

		return NULL;
	}
	// initiate the config file fill it with the default values
	SPConfig config = spConfigInit(msg);
	if (!config)
	{
		fclose(file);
		return NULL;
	}

	input = (char*)malloc((MAX_LINE_LENGTH + 1)* sizeof(char));
	if (!input)
	{
		*msg = SP_CONFIG_ALLOC_FAIL;
		fclose(file);
		return NULL;
	}

	while (fgets(input, MAX_LINE_LENGTH + 1, file) != NULL)
	{
		tmpInput = input;
		// replacing the newline sign with null terminator
		if ((newLinePointer = strchr(tmpInput, '\n')) != NULL)
		{
		    *newLinePointer = NULL_TERMINATE;
		}
		// trim the white space if exist
		tmpInput = trimWhitespace(tmpInput);
		// if it was an comment line or an empty line, we continue else keep process
		if ((*tmpInput != COMMENT_MARK) && (*tmpInput != NULL_TERMINATE))
		{
			if (!processAssignmentLine(config, tmpInput, filename, line_counter, msg))
			{
				spConfigDestroy(config);
				free(input);
				fclose(file);
				return NULL;
			}
		}

		line_counter++;
	}

	free(input);
	fclose(file);
	// check all must initiate variables were initiate - if not initiated printing error with line
	// number of line in file which is the line counter "decreased" to be previous line number
	if (!spNecessaryVarsInitiated(filename, msg, config, line_counter - 1))
	{
		spConfigDestroy(config);
		return NULL;
	}

	// Check that number of similar images resquested isn't greater the total
	// number of images
	if(config->spNumOfSimilarImages > config->spNumOfImages)
	{
		flushed_printf(ERROR_SIMILAR_GREATER_THAN_IMAGES);
		spConfigDestroy(config);
		return NULL;
	}

	*msg = SP_CONFIG_SUCCESS;

	return config;
}

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
bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
	if (!config)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spExtractionMode;
}

/*
 * Returns true if spMinimalGUI = true, false otherwise.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return true if spExtractionMode = true, false otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
	if (!config)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spMinimalGUI;
}
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
int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg)
{

		assert(msg != NULL);
		if (!config)
		{
			*msg = SP_CONFIG_INVALID_ARGUMENT;
			return -1;
		}
		*msg = SP_CONFIG_SUCCESS;

		return config->spNumOfImages;
}
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
int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
	if (!config)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfFeatures;
}

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
int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
	if (!config)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spPCADimension;
}

// given an integer n the function return the number of digits in the number
int spConfigHowManyDigits(int n)
{
	int count = 0;

	if (n == 0)
	{
		return 1;
	}

	while(n != 0)
	{
		n /= 10;             // n = n/10
		++count;
	}

	return count;
}
/**
* Given an index 'index' the function stores in imagePath the full path of the
* ith image
*/
SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index)
{
	return spConfigGetImageCombPath(imagePath, config, index, config->spImagesSuffix);
}
/*
* Given an index 'index' the function stores in imageFeatsPath the full path of the
* ith image with suffix of feats.
*/
SP_CONFIG_MSG spConfigGetImageFeatsPath(char* imageFeatsPath, const SPConfig config,
		int index)
{
	return spConfigGetImageCombPath(imageFeatsPath, config, index, FEATS_SUFFIX);
}
/*
 * Given an index 'index' the function stores in imagePath the full path of the
 * ith image with suffix of .feats.
 */
SP_CONFIG_MSG spConfigGetImageCombPath(char* imagePath, const SPConfig config,
		int index, char* suffix)
{
	SP_CONFIG_MSG msg;
	char* intBuffer = NULL;
	int index_len = spConfigHowManyDigits(index);

	if ((!config)||(!imagePath))
	{
		msg = SP_CONFIG_INVALID_ARGUMENT;
		return msg;
	}
	if (index >= config->spNumOfImages)
	{
		msg = SP_CONFIG_INDEX_OUT_OF_RANGE;
		return msg;
	}
	// getting the index number as a string
	intBuffer = (char*)malloc((index_len + 1) * sizeof(char));
	if (!intBuffer)
	{
		msg = SP_CONFIG_ALLOC_FAIL;
		return msg;
	}

	sprintf(intBuffer, "%d", index);

	if (!sprintf(imagePath, "%s%s%s%s", config->spImagesDirectory, config->spImagesPrefix,
						intBuffer, suffix))
	{
		msg = SP_CONFIG_ALLOC_FAIL;
		return msg;
	}

	free(intBuffer);
	msg = SP_CONFIG_SUCCESS;

	return msg;
}


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
SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config)
{
	SP_CONFIG_MSG msg;
	if ((!config)||(!pcaPath))
	{
		msg = SP_CONFIG_INVALID_ARGUMENT;
		return msg;
	}

	if (!sprintf(pcaPath, "%s%s", config->spImagesDirectory, config->spPCAFilename))
	{
		// TODO: CHECK IF ITS THE RIGHT HANDLE
		msg = SP_CONFIG_ALLOC_FAIL;
		return msg;
	}

	msg = SP_CONFIG_SUCCESS;
	return msg;
}

/**
 * Frees all memory resources associate with config.
 * If config == NULL nothig is done.
 */
void spConfigDestroy(SPConfig config)
{
	if (config)
	{
		if (config->spImagesDirectory)
		{
			free(config->spImagesDirectory);
			config->spImagesDirectory = NULL;
		}
		if (config->spImagesPrefix)
		{
			free(config->spImagesPrefix);
			config->spImagesPrefix = NULL;
		}
		if (config->spImagesSuffix)
		{
			free(config->spImagesSuffix);
			config->spImagesSuffix = NULL;
		}
		if (config->spPCAFilename)
		{
			free(config->spPCAFilename);
			config->spPCAFilename = NULL;
		}
		if (config->spLoggerFilename)
		{
			free(config->spLoggerFilename);
			config->spLoggerFilename = NULL;
		}
		free (config);
		config = NULL;
	}
}

// check if all the default variables were initiated
bool spNecessaryVarsInitiated(const char* filename, SP_CONFIG_MSG* msg, SPConfig config, int line)
{
	if (!config->spImagesDirectory)
	{
		*msg = SP_CONFIG_MISSING_DIR;
		spPrintParameterNotSetError(filename, line, SP_IMAGES_DIRECTORY);
		return false;
	}

	if (!config->spImagesPrefix)
	{
		*msg = SP_CONFIG_MISSING_PREFIX;
		spPrintParameterNotSetError(filename, line, SP_IMAGES_PREFIX);
		return false;
	}

	if (!config->spImagesSuffix)
	{
		*msg = SP_CONFIG_MISSING_SUFFIX;
		spPrintParameterNotSetError(filename, line, SP_IMAGES_SUFFIX);
		return false;
	}

	if (config->spNumOfImages < 1)
	{
		*msg = SP_CONFIG_MISSING_NUM_IMAGES;
		spPrintParameterNotSetError(filename, line, SP_NUM_OF_IMAGES);
		return false;
	}

	return true;
}
// getter for num of images
int spConfigGetNumOfSimilarImage (const SPConfig config, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
		if (!config)
		{
			*msg = SP_CONFIG_INVALID_ARGUMENT;
			return -1;
		}
		*msg = SP_CONFIG_SUCCESS;
		return config->spNumOfSimilarImages;
}
// getter for spKDTreeSplitMethod
SP_KDTREE_SPLIT_METHOD spConfigGetspKDTreeSplitMethod (const SPConfig config, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);

		if (!config)
		{
			*msg = SP_CONFIG_INVALID_ARGUMENT;
			return -1;
		}
		*msg = SP_CONFIG_SUCCESS;
		return config->spKDTreeSplitMethod;
}
// getter for spKNN
int spConfigGetKNN (const SPConfig config, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
		if (!config)
		{
			*msg = SP_CONFIG_INVALID_ARGUMENT;
			return -1;
		}
		*msg = SP_CONFIG_SUCCESS;
		return config->spKNN;

}
// getter for spLoggerLevel
int spConfigLoggerLevel (const SPConfig config,SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
		if (!config)
		{
			*msg = SP_CONFIG_INVALID_ARGUMENT;
			return -1;
		}
		*msg = SP_CONFIG_SUCCESS;
		return config->spLoggerLevel;

}

// getter for spLoggerFilename
SP_CONFIG_MSG spConfigGetLoggerFilename(char* loggerFileName, const SPConfig config)
{
	if ((!config)||(!loggerFileName))
	{
		return SP_CONFIG_INVALID_ARGUMENT;
	}

	strcpy(loggerFileName, config->spLoggerFilename);

	return SP_CONFIG_SUCCESS;
}

// print the massage stored in msg to the logger file
void spConfigPrintConfigMsgToLogger (SP_CONFIG_MSG msg, const char* file, const char * func, int line)
{
	switch(msg)
		{
			case SP_CONFIG_MISSING_DIR:
			{
				spLoggerPrintError(SP_CONFIG_MSG_MISSING_DIR, file, func, line);
				break;
			}
			case SP_CONFIG_MISSING_PREFIX:
			{
				spLoggerPrintError(SP_CONFIG_MSG_MISSING_PREFIX,  file, func, line);
				break;
			}
			case SP_CONFIG_MISSING_SUFFIX:
			{
				spLoggerPrintError(SP_CONFIG_MSG_MISSING_SUFFIX, file, func, line);
				break;
			}
			case SP_CONFIG_MISSING_NUM_IMAGES:
			{
				spLoggerPrintError(SP_CONFIG_MSG_MISSING_NUM_IMAGES,  file, func, line);
				break;
			}
			case SP_CONFIG_CANNOT_OPEN_FILE:
			{
				spLoggerPrintError(SP_CONFIG_MSG_CANNOT_OPEN_FILE,  file, func, line);
				break;
			}
			case SP_CONFIG_ALLOC_FAIL:
			{
				spLoggerPrintError(SP_CONFIG_MSG_ALLOC_FAIL,  file, func, line);
				break;
			}
			case SP_CONFIG_INVALID_INTEGER:
			{
				spLoggerPrintError(SP_CONFIG_MSG_INVALID_INTEGER,  file, func, line);
				break;
			}
			case SP_CONFIG_INVALID_STRING:
			{
				spLoggerPrintError(SP_CONFIG_MSG_INVALID_STRING,  file, func, line);
				break;
			}
			case SP_CONFIG_INVALID_ARGUMENT:
			{
				spLoggerPrintError(SP_CONFIG_MSG_INVALID_ARGUMENT,  file, func, line);
				break;
			}
			case SP_CONFIG_INDEX_OUT_OF_RANGE:
			{
				spLoggerPrintError(SP_CONFIG_MSG_INDEX_OUT_OF_RANGE,   file, func, line);
				break;
			}
			case SP_CONFIG_SUCCESS:
				break;
		}
}
// print the massage stored in msg to the stdout
void spConfigPrintConfigMsgToStdout (SP_CONFIG_MSG msg)
{
	switch(msg)
		{
			case SP_CONFIG_MISSING_DIR:
			{
				printf("%s\n", SP_CONFIG_MSG_MISSING_DIR);
				fflush(NULL);
				break;
			}
			case SP_CONFIG_MISSING_PREFIX:
			{
				printf("%s\n", SP_CONFIG_MSG_MISSING_PREFIX);
				fflush(NULL);
				break;
			}
			case SP_CONFIG_MISSING_SUFFIX:
			{
				printf("%s\n", SP_CONFIG_MSG_MISSING_SUFFIX);
				fflush(NULL);
				break;
			}
			case SP_CONFIG_MISSING_NUM_IMAGES:
			{
				printf("%s\n", SP_CONFIG_MSG_MISSING_NUM_IMAGES);
				fflush(NULL);
				break;
			}
			case SP_CONFIG_CANNOT_OPEN_FILE:
			{
				printf("%s\n", SP_CONFIG_MSG_CANNOT_OPEN_FILE);
				fflush(NULL);
				break;
			}
			case SP_CONFIG_ALLOC_FAIL:
			{
				printf("%s\n", SP_CONFIG_MSG_ALLOC_FAIL);
				fflush(NULL);
				break;
			}
			case SP_CONFIG_INVALID_INTEGER:
			{
				printf("%s\n", SP_CONFIG_MSG_INVALID_INTEGER);
				fflush(NULL);
				break;
			}
			case SP_CONFIG_INVALID_STRING:
			{
				printf("%s\n", SP_CONFIG_MSG_INVALID_STRING);
				fflush(NULL);
				break;
			}
			case SP_CONFIG_INVALID_ARGUMENT:
			{
				printf("%s\n", SP_CONFIG_MSG_INVALID_ARGUMENT);
				fflush(NULL);
				break;
			}
			case SP_CONFIG_INDEX_OUT_OF_RANGE:
			{
				printf("%s\n", SP_CONFIG_MSG_INDEX_OUT_OF_RANGE);
				fflush(NULL);
				break;
			}
			case SP_CONFIG_SUCCESS:
				break;
		}
}
