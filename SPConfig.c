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

// function that sets the default values of the struct.

SPConfig spConfigInit()
{
	SPConfig config = NULL;
	config = (SPConfig)malloc(sizeof(*config));
	if (!config)
	{
		// add error memory allocation failure
		return NULL;
	}
	config->spImagesDirectory = NULL;
	config->spImagesPrefix = NULL;
	config->spImagesSuffix = NULL;
	config->spNumOfImages = 0;
	config->spPCADimension = PCA_DIMENSION_DEFULT;
	config->spPCAFilename = (char*)malloc((SIZE_OF_PCAYML +1) * sizeof(char));
	if (!config->spPCAFilename)
	{
		// add error memory allocation failure
		free(config);
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
			// add error memory allocation failure
			free(config->spPCAFilename);
			free(config);
			return NULL;
		}
	strcpy(config->spLoggerFilename, LOGGER_FILE_NAME_DEFULT);
	return config;
}
/*
 * function that print regular errors
 * error type num = 0 ->  If a line is invalid
 * 					1 ->  If any of the constraints on the system parameters are not met
 * 					2 ->  If a parameter with no default value is not set
 */
void spRegularErrorPrinter(const char* filename, int line,int ErrorTypeNum, char* paramterName)
{
	// An array the hold the key word for the error array[0]
	char * errorArray[] = {ERROR_INVALID_CONFIGURATION_LINE,ERROR_INVALID_VALUE,
			 ERROR_PARAMTER,ERROR_IS_NOT_SET};

	if (ErrorTypeNum != ERROR_ARRAY_TYPE_PARAMTER_NOT_SET)
	{
		printf("%s%s\n%s%d\n%s%s\n",ERROR_FILE, filename, ERROR_LINE, line, ERROR_MASSAGE,
				 errorArray[ErrorTypeNum]);
		fflush(NULL);
	}
	else
	{
		printf("%s%s\n%s%d\n%s%s%s%s\n", ERROR_FILE, filename, ERROR_LINE, line,
				 ERROR_MASSAGE,errorArray[ErrorTypeNum],paramterName,errorArray[ErrorTypeNum+1]);
		fflush(NULL);
	}

}
bool setspImagesDirectory (SPConfig config, char* variable_value, SP_CONFIG_MSG* msg)
{
	config->spImagesDirectory = (char*)malloc((strlen(variable_value) +1) * sizeof(char));
	if (!config->spImagesDirectory)
	{
		*msg = SP_CONFIG_ALLOC_FAIL;
		return false;
	}

	strcpy(config->spImagesDirectory,variable_value);
	return true;
}
bool setspImagesPrefix (SPConfig config, char* variable_value, SP_CONFIG_MSG* msg)
{
	config->spImagesPrefix = (char*)malloc((strlen(variable_value) +1) * sizeof(char));
	if (!config->spImagesPrefix)
	{
		*msg = SP_CONFIG_ALLOC_FAIL;
		return false;
	}

	strcpy(config->spImagesPrefix,variable_value);
	return true;
}
bool setspImagesSuffix (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
		const char* filename)
{
	if ((strcmp(variable_value,SUFFIX_JPEG)== 0) || (strcmp(variable_value,SUFFIX_PNG)== 0)||
			(strcmp(variable_value,SUFFIX_BMP)== 0) || (strcmp(variable_value,SUFFIX_GIF)== 0))
	{
		config->spImagesSuffix = (char*)malloc((strlen(variable_value) +1) * sizeof(char));
		if (!config->spImagesSuffix)
		{
			//TODO THINK WHAT TO DO HERE
			*msg = SP_CONFIG_ALLOC_FAIL;
			return false;
		}
		strcpy(config->spImagesSuffix,variable_value);
		return true;
	}
	else
	{
		*msg = SP_CONFIG_INVALID_STRING;
		spRegularErrorPrinter(filename,line,ERROR_ARRAY_TYPE_CONSTEINT_NOT_MET,DUMMY);
		return false;
	}
}
bool setspNumOfImages (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
		const char* filename)
{
	if (atoi(variable_value) > 0)
	{
		config->spNumOfImages = atoi(variable_value);
		return true;
	}
	else
	{
		*msg = SP_CONFIG_INVALID_INTEGER;
		spRegularErrorPrinter(filename,line,ERROR_ARRAY_TYPE_CONSTEINT_NOT_MET,DUMMY);
		return false;
	}
}
bool setspPCADimension (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
		const char* filename)
{
	if ((atoi(variable_value) > SP_PCA_DIMENSION_CONSTRAINT_LOW )&&
					(atoi(variable_value) < SP_PCA_DIMENSION_CONSTRAINT_HIGH))
	{
		config->spPCADimension = atoi(variable_value);
		return true;
	}
	else
	{
		*msg = SP_CONFIG_INVALID_INTEGER;
		spRegularErrorPrinter(filename,line,ERROR_ARRAY_TYPE_CONSTEINT_NOT_MET,DUMMY);
		return false;
	}
}
bool setspPCAFilename (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg)
{
	free(config->spPCAFilename);
	config->spPCAFilename = (char*)malloc((strlen(variable_value) +1) * sizeof(char));
	if (!config->spPCAFilename)
	{
		*msg = SP_CONFIG_ALLOC_FAIL;
		return false;
	}
	strcpy(config->spPCAFilename,variable_value);
	return true;
}
bool setspNumOfFeatures (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
		const char* filename)
{
	if (atoi(variable_value) > 0)
	{
		config->spNumOfFeatures = atoi(variable_value);
		return true;
	}
	else
	{
		*msg = SP_CONFIG_INVALID_INTEGER;
		spRegularErrorPrinter(filename,line,ERROR_ARRAY_TYPE_CONSTEINT_NOT_MET,DUMMY);
		return false;
	}
}
bool setspExtractionMode (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
		const char* filename)
{
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
		spRegularErrorPrinter(filename,line,ERROR_ARRAY_TYPE_CONSTEINT_NOT_MET,DUMMY);
		return false;
	}
}
bool setspNumOfSimilarImages (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
			const char* filename)
{
	if(atoi(variable_value) > 0)
	{
		config->spNumOfSimilarImages  = atoi(variable_value);
		return true;
	}
	else
	{
		*msg = SP_CONFIG_INVALID_INTEGER;
		spRegularErrorPrinter(filename,line,ERROR_ARRAY_TYPE_CONSTEINT_NOT_MET,DUMMY);
		return false;
	}
}

bool setspKDTreeSplitMethod (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
			const char* filename)
{
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
		spRegularErrorPrinter(filename,line,ERROR_ARRAY_TYPE_CONSTEINT_NOT_MET,DUMMY);
		return false;
	}
	return true;
}

bool setspKNN (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
			const char* filename)
{
	if(atoi(variable_value) > 0)
	{
		config->spKNN = atoi(variable_value);
		return true;
	}
	else
	{
		*msg = SP_CONFIG_INVALID_INTEGER;
		spRegularErrorPrinter(filename,line,ERROR_ARRAY_TYPE_CONSTEINT_NOT_MET,DUMMY);
		return false;
	}
}
bool setspMinimalGUI (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
			const char* filename)
{
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
		spRegularErrorPrinter(filename,line,ERROR_ARRAY_TYPE_CONSTEINT_NOT_MET,DUMMY);
		return false;
	}
	return true;
}
bool setspLoggerLevel (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg, int line,
			const char* filename)
{
	if ((atoi(variable_value) > LOGGER_LEVEL_MIN) && (atoi(variable_value) < LOGGER_LEVEL_MAX))
	{
		config->spLoggerLevel = atoi(variable_value);
		return true;
	}
	else
	{
		*msg = SP_CONFIG_INVALID_INTEGER;
		spRegularErrorPrinter(filename,line,ERROR_ARRAY_TYPE_CONSTEINT_NOT_MET,DUMMY);
		return false;
	}
}
bool setspLoggerFilename (SPConfig config, char* variable_value,SP_CONFIG_MSG* msg)
{
	free(config->spLoggerFilename);
	config->spLoggerFilename = (char*)malloc((strlen(variable_value) +1) * sizeof(char));
	if (!config->spLoggerFilename)
	{
		*msg = SP_CONFIG_ALLOC_FAIL;
		return false;
	}
	strcpy(config->spLoggerFilename,variable_value);
	return true;
}


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
	else if (! strcmp(variable_name,SP_LOGGER_LEVEL))
	{
		return setspLoggerLevel(config, variable_value, msg, line, filename);
	}
	else if (! strcmp(variable_name,SP_LOGGER_FILE_NAME))
	{
		return setspLoggerFilename(config, variable_value, msg);
	}
	else
	{
		*msg = SP_CONFIG_INVALID_STRING;
		spRegularErrorPrinter(filename,line,ERROR_ARRAY_TYPE_INVALID_CONFIGURATION_LINE,DUMMY);
		return false;
	}
}

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

char* cleanAssignmentOperand(char* str, SP_CONFIG_MSG* msg, const char* filename, int line)
{
	char* result = NULL;
	int i = 0;
	char* trimmedStr = NULL;
	int length = 0;

	trimmedStr = trimWhitespace(str);
	length = strlen(trimmedStr);

	for (i = 0; i < length; i++)
	{
		//	checking if the letter is # and in the middle
		if( (trimmedStr[i] == COMMENT_MARK) || isspace(trimmedStr[i]))
		{
			*msg = SP_CONFIG_INVALID_STRING;
			spRegularErrorPrinter(filename, line, 0, DUMMY);
			return NULL;
		}
	}

	result = (char*)malloc((strlen(trimmedStr) + 1) * sizeof(char));
	if (!result)
	{
		// TODO: handle
	}

	strcpy(result, trimmedStr);

	return result;
}

bool proccesAssignmentLine(SPConfig config, const char* line, const char* filename, int lineNumber, SP_CONFIG_MSG* configMsg)
{
	char* equalMarkIndex = NULL;
	char* tmpVariableName = NULL;
	char* tmpVariableValue = NULL;
	int variableLen = 0;
	int valueLen = 0;
	char* variableName = NULL;
	char* variableValue = NULL;

	equalMarkIndex = strchr(line, EQUAL_MARK);
	if (!equalMarkIndex)
	{
		// TODO: print error in line
	}

	variableLen = (int)(equalMarkIndex - line);
	valueLen = (int)(line + strlen(line) - equalMarkIndex);

	tmpVariableName = (char*)malloc((variableLen + 1) * sizeof(char));
	tmpVariableValue = (char*)malloc((valueLen + 1) * sizeof(char));

	strncpy(tmpVariableName, line, variableLen);
	tmpVariableName[variableLen - 1] = NULL_TERMINATE;
	strncpy(tmpVariableValue, line + variableLen + 1, valueLen);
	tmpVariableValue[valueLen - 1] = NULL_TERMINATE;

	// if '=' not exist we need to check if its comment line,empty line or an error
	variableName = cleanAssignmentOperand(tmpVariableName, configMsg, filename, lineNumber);
	if (!variableName)
	{
		// TODO: handle
		return false;
	}

	variableValue = cleanAssignmentOperand(tmpVariableValue, configMsg, filename, lineNumber);
	if (!variableValue)
	{
		// TODO: handle
		return false;
	}

	free(tmpVariableName);
	free(tmpVariableValue);

	if (!spAssignArgument(config, variableName, variableValue,
			configMsg, lineNumber, filename))
	{
		return false;
	}

	return true;
}

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg)
{
	int line_counter = 0;
	bool isDefuatInit = true;
	char* input = NULL;
	char* tmpInput = NULL;
	FILE *file;
	char* newLinePointer = NULL;

	if (!filename)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		printf("%s %s %s/n", ERROR_THE_CONFIGURATION_FILE, filename, ERROR_COULD_NOT_OPEN);
		fflush(NULL);
		return NULL;
	}

	file = fopen(filename, "r");
	//deal with error 3 file couldnt open
	if (!file)
	{
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		printf("%s,%s,%s/n",ERROR_THE_CONFIGURATION_FILE,filename,ERROR_COULD_NOT_OPEN);
		fflush(NULL);
		return NULL;
	}

	SPConfig config = spConfigInit();
	if (!config)
	{
		*msg = SP_CONFIG_ALLOC_FAIL;
		fclose(file);
		return NULL;
	}

	input = (char*)malloc((MAX_LINE_LENGTH + 1)* sizeof(char));
	if (!input)
	{
		// TODO: handle
	}

	while (fgets(input, MAX_LINE_LENGTH + 1, file) != NULL)
	{
		tmpInput = input;

		if ((newLinePointer = strchr(tmpInput, '\n'/*NEW_LINE*/)) != NULL)
		{
		    *newLinePointer = NULL_TERMINATE;
		}

		tmpInput = trimWhitespace(tmpInput);

		if ((*tmpInput != COMMENT_MARK) && (*tmpInput != NULL_TERMINATE))
		{
			if (!proccesAssignmentLine(config, tmpInput, filename, line_counter, msg))
			{
				spConfigDestroy(config);
				free(input);
				fclose(file);
				return false;
			}
		}

		line_counter++;
	}

	free(input);
	fclose(file);
	// check all must initiate variables were initiate
	isDefuatInit = spIsDefaultInitiate(filename, msg, config, line_counter);
	if (!isDefuatInit)
	{
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
SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index)
{
	return spConfigGetImageCombPath(imagePath, config, index, config->spImagesSuffix);
}
SP_CONFIG_MSG spConfigGetImageFeatsPath(char* imageFeatsPath, const SPConfig config,
		int index)
{
	return spConfigGetImageCombPath(imageFeatsPath, config, index, FEATS_SUFFIX);
}

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

	// check numbers set up
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
		// TODO: CHECK IF ITS THE RIGHT HANDLE
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
		}
		if (config->spImagesPrefix)
		{
			free(config->spImagesPrefix);
		}
		if (config->spImagesSuffix)
		{
			free(config->spImagesSuffix);
		}
		if (config->spPCAFilename)
		{
			free(config->spPCAFilename);
		}
		if (config->spLoggerFilename)
		{
			free(config->spLoggerFilename);
		}
		free (config);
	}
}
bool spIsDefaultInitiate(const char* filename, SP_CONFIG_MSG* msg,SPConfig config, int line)
{
	if (!config->spImagesDirectory)
	{
		*msg = SP_CONFIG_MISSING_DIR;
		spRegularErrorPrinter(filename,line,2,SP_IMAGES_DIRECTORY);
		spConfigDestroy(config);
		return false;
	}
	if (!config->spImagesPrefix)
	{
		*msg = SP_CONFIG_MISSING_PREFIX;
		spRegularErrorPrinter(filename,line,2,SP_IMAGES_PREFIX);
		spConfigDestroy(config);
		return false;
	}
	if (!config->spImagesSuffix)
	{
		*msg = SP_CONFIG_MISSING_SUFFIX;
		spRegularErrorPrinter(filename,line,2,SP_IMAGES_SUFFIX);
		spConfigDestroy(config);
		return false;
	}
	if (config->spNumOfImages < 1)
	{
		*msg = SP_CONFIG_MISSING_NUM_IMAGES;
		spRegularErrorPrinter(filename,line,2,SP_NUM_OF_IMAGES);
		spConfigDestroy(config);
		return false;
	}
	return true;
}

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


SP_CONFIG_MSG spConfigGetLoggerFilename(char* loggerFileName, const SPConfig config)
{
	SP_CONFIG_MSG msg;
		if ((!config)||(!loggerFileName))
		{
			msg = SP_CONFIG_INVALID_ARGUMENT;
			return msg;
		}

		if (!sprintf(loggerFileName, "%s", config->spLoggerFilename))
			// TODO: CHECK IF ITS THE RIGHT HANDLE
			msg = SP_CONFIG_ALLOC_FAIL;
			return msg;

		msg = SP_CONFIG_SUCCESS;
		return msg;
}


