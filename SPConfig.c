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
	char * errorArray[] = {ERROR_INVALID_CONFIGURATION_LINE,ERROR_INVALID_VALUE,
			 ERROR_PARAMTER,ERROR_IS_NOT_SET};

	if (ErrorTypeNum != 2)
	{
		printf("%s%s\n%s%d\n%s%s\n",ERROR_FILE, filename, ERROR_LINE, line, ERROR_MASSAGE,
				 errorArray[ErrorTypeNum]);
		fflush(NULL);
	}
	else
	{
		printf("%s%s\n%s%d\n%s%s%s%s\n", ERROR_FILE, filename, ERROR_LINE, line,
				 ERROR_MASSAGE,errorArray[ErrorTypeNum],paramterName,errorArray[+1]);
		fflush(NULL);
	}

}
char* spCleaningString(char* str,SP_CONFIG_MSG* msg,const char* filename,int line)
{
	char* result;
	int length = strlen(str);
	char* temp = (char*)malloc((length + 1) * sizeof(char));
	char letter;
	int i = 0;
	int j = 0;
	int isBlankFound = 0;
	int isCharFound = 0;
// blank
	for (i = 0; i < length; i++)
	{
		letter = str[i];
		//	checking if the letter is # and in the middle
		if( (letter == COMMENT_MARK) && (j != 0) )
		{
			*msg = SP_CONFIG_INVALID_STRING;
			spRegularErrorPrinter(filename,line,0,DUMMY);
			return NULL;
		}
		// if the previous condition didn't meet and the the letter is # this is a comment line and should be skipped
		else if(letter == COMMENT_MARK)
		{
			result = (char*)malloc((SIZE_OF_SKIP + 1) * sizeof(char));
			result = SKIP;
			return result;
		}
		else
			// if space is found we need to check that it is not in the middle of the variable
		{
			if (isspace(letter))
			{
				isBlankFound = 1;
			}
			else
			{
				// if both are on means there is a problem and we find a space in the middle
				if ((isCharFound)&&(isBlankFound))
				{
					*msg = SP_CONFIG_INVALID_STRING;
					spRegularErrorPrinter(filename,line,0,DUMMY);
					return NULL;
				}
				else
				{
					temp[j] = letter;
					j++;
					isBlankFound = 0;
					isCharFound = 1;
				}
			}
		 }
	}
	if (j == 0)
	{
		result = (char*)malloc((SIZE_OF_SKIP_SKIP_EMPTY_LINE + 1) * sizeof(char));
		result = SKIP_EMPTY_LINE;
		return result;
	}
	// reduce j by 1 because the last letter incremented it
	temp[j] = '\0';
	length = j-1;
	result = (char*)malloc((length + 1) * sizeof(char));
	if (!result)
	{
		printf("memory allocation problem");
		return NULL;
	}

	strcpy(result, temp);
	free (temp);
	return result;
	}

int spAssignArgument(SPConfig config, char* variable_name, char* variable_value,SP_CONFIG_MSG* msg, int line,
		const char* filename)
{
	int i = 2;
	if (! strcmp(variable_name,SP_IMAGES_DIRECTORY))
	{
		config->spImagesDirectory = (char*)malloc((strlen(variable_value) +1) * sizeof(char));
		if (!config->spImagesDirectory)
		{
			*msg = SP_CONFIG_ALLOC_FAIL;
			spConfigDestroy(config);
			return 3;
		}
		strcpy(config->spImagesDirectory,variable_value);
		i = 1;
		return i;
	}
	else if (! strcmp(variable_name,SP_IMAGES_PREFIX))
	{
		config->spImagesPrefix = (char*)malloc((strlen(variable_value) +1) * sizeof(char));
		if (!config->spImagesPrefix)
		{
			*msg = SP_CONFIG_ALLOC_FAIL;
			spConfigDestroy(config);
			return 3;
		}
		strcpy(config->spImagesPrefix,variable_value);
		i = 1;
		return i;
	}
	else if (! strcmp(variable_name,SP_IMAGES_SUFFIX))
	{
		if ((strcmp(variable_value,SUFFIX_JPEG)== 0) || (strcmp(variable_value,SUFFIX_PNG)== 0)|| \
				(strcmp(variable_value,SUFFIX_BMP)== 0) || (strcmp(variable_value,SUFFIX_GIF)== 0))
		{
			config->spImagesSuffix = (char*)malloc((strlen(variable_value) +1) * sizeof(char));
			if (!config->spImagesSuffix)
			{
				*msg = SP_CONFIG_ALLOC_FAIL;
				spConfigDestroy(config);
				return 3;
			}
			strcpy(config->spImagesSuffix,variable_value);
			i = 1;
			return i;
		}
		else
		{
			*msg = SP_CONFIG_INVALID_STRING;
			spRegularErrorPrinter(filename,line,1,DUMMY);
			spConfigDestroy(config);
			return 0;
		}
	}
	else if (! strcmp(variable_name,SP_NUM_OF_IMAGES))
	{
		if (atoi(variable_value) > 0)
		{
			config->spNumOfImages = atoi(variable_value);
			i = 1;
			return i;
		}
		else
		{
			*msg = SP_CONFIG_INVALID_INTEGER;
			spRegularErrorPrinter(filename,line,1,DUMMY);
			spConfigDestroy(config);
			return 0;
		}
	}
	else if (! strcmp(variable_name,SP_PCA_DIMENSION))
	{
		if ((atoi(variable_value) > SP_PCA_DIMENSION_CONSTRAINT_LOW )&& \
				(atoi(variable_value) < SP_PCA_DIMENSION_CONSTRAINT_HIGH))
		{
			config->spPCADimension = atoi(variable_value);
			i = 1;
			return i;
		}
		else
		{
			*msg = SP_CONFIG_INVALID_INTEGER;
			spRegularErrorPrinter(filename,line,1,DUMMY);
			spConfigDestroy(config);
			return 0;
		}
	}
	else if (! strcmp(variable_name,SP_PCA_FILE_NAME))
	{
		free(config->spPCAFilename);
		config->spPCAFilename = (char*)malloc((strlen(variable_value) +1) * sizeof(char));
		if (!config->spPCAFilename)
		{
		*msg = SP_CONFIG_ALLOC_FAIL;
		spConfigDestroy(config);
		return 3;
		}
		strcpy(config->spPCAFilename,variable_value);
		i = 1;
		return i;
	}
	else if (! strcmp(variable_name,SP_NUM_OF_FEATURES))
	{
		if (atoi(variable_value) > 0)
		{
			config->spNumOfFeatures = atoi(variable_value);
			i = 1;
		}
		else
		{
			*msg = SP_CONFIG_INVALID_INTEGER;
			spRegularErrorPrinter(filename,line,1,DUMMY);
			spConfigDestroy(config);
			return 0;
		}
	}
	else if (! strcmp(variable_name,SP_EXTRACTION_MODE))
	{
		if (strcmp(variable_value,"true")== 0)
		{

			config->spExtractionMode = true;
			i = 1;
			return i;
		}
		else if (strcmp(variable_value,"false")== 0)
		{
			config->spExtractionMode = false;
			i = 1;
			return i;
		}
		else
		{
			*msg = SP_CONFIG_INVALID_STRING;
			spRegularErrorPrinter(filename,line,1,DUMMY);
			spConfigDestroy(config);
			return 0;
		}

	}
	else if (! strcmp(variable_name,SP_NUM_OF_SIMILAR_IMAGES))
	{
		if(atoi(variable_value) > 0)
		{
			config->spNumOfSimilarImages  = atoi(variable_value);
			i = 1;
		}
		else
		{
			*msg = SP_CONFIG_INVALID_INTEGER;
			spRegularErrorPrinter(filename,line,1,DUMMY);
			spConfigDestroy(config);
			return 0;
		}

	}
	else if (! strcmp(variable_name,SP_KD_TREE_SPLIT_METHOD))
	{
		if (strcmp(variable_value,"RANDOM"))
		{
		config->spKDTreeSplitMethod  = RANDOM;
		i = 1;
		}
		else if (strcmp(variable_value,"MAX_SPREAD"))
		{
			config->spKDTreeSplitMethod  = MAX_SPREAD;
			i = 1;
		}
		else if (strcmp(variable_value,"INCREMENTAL"))
		{
		config->spKDTreeSplitMethod  = INCREMENTAL;
		i = 1;
		}
		else
		{
			*msg = SP_CONFIG_INVALID_STRING;
			spRegularErrorPrinter(filename,line,1,DUMMY);
			spConfigDestroy(config);
			return 0;
		}

	}
	else if (! strcmp(variable_name,SP_KNN))
	{
		if(atoi(variable_value) > 0)
		{
			config->spKNN = atoi(variable_value);
			i = 1;
		}
		else
		{
			*msg = SP_CONFIG_INVALID_INTEGER;
			spRegularErrorPrinter(filename,line,1,DUMMY);
			spConfigDestroy(config);
			return 0;
		}
	}
	else if (! strcmp(variable_name,SP_MINMIMAL_GUI))
	{
		if (strcmp(variable_value,"true")== 0)
		{
			config->spMinimalGUI = true;
			i = 1;
			return i;
		}
		else if (strcmp(variable_value,"false")== 0)
		{
			config->spMinimalGUI = false;
			i = 1;
			return i;
		}
		else
		{
			*msg = SP_CONFIG_INVALID_STRING;
			spRegularErrorPrinter(filename,line,1,DUMMY);
			spConfigDestroy(config);
			return 0;
		}

	}
	else if (! strcmp(variable_name,SP_LOGGER_LEVEL))
	{
		if ((atoi(variable_value) > 0) && (atoi(variable_value) < 5))
		{
			config->spLoggerLevel = atoi(variable_value);
			i = 1;
		}
		else
		{
			*msg = SP_CONFIG_INVALID_INTEGER;
			spRegularErrorPrinter(filename,line,1,DUMMY);
			spConfigDestroy(config);
			return 0;
		}
	}
	else if (! strcmp(variable_name,SP_LOGGER_FILE_NAME))
	{
		free(config->spLoggerFilename);
		config->spLoggerFilename = (char*)malloc((strlen(variable_value) +1) * sizeof(char));
		if (!config->spLoggerFilename)
		{
		*msg = SP_CONFIG_ALLOC_FAIL;
		spConfigDestroy(config);
		return 3;
		}
		strcpy(config->spLoggerFilename,variable_value);
		i = 1;
		return i;
	}
	if (i == 2)
	{
		*msg = SP_CONFIG_INVALID_STRING;
		spRegularErrorPrinter(filename,line,1,DUMMY);
		spConfigDestroy(config);
		return 0;
	}

	return i;
}

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg)
{
	int succeeded;
	int line_counter = 0;
	char* index_of_equal;
	char input[MAX_LINE_LENGTH];
	char* temp_variable;
	char* variable_name = NULL;
	char* variable_value = NULL;
	bool isDefuatInit = true;
	if (!filename)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		printf("%s%s%s/n",ERROR_THE_CONFIGURATION_FILE,filename,ERROR_COULD_NOT_OPEN);
		fflush(NULL);
		return NULL;
	}
	FILE *file = fopen(filename, "r");
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
	while ( fgets ( input, sizeof(input), file ) != NULL )
	{
		strtok(input, "\n");
		index_of_equal = strchr(input,EQUAL_MARK);
		// if '=' exist
		if (index_of_equal)
		{
			*index_of_equal= '\0';
			index_of_equal++;
		}
		temp_variable = (char*)malloc((strlen(input) + 1) * sizeof(char));
		strcpy(temp_variable,input);
		// if '=' not exist we need to check if its comment line,empty line or an error
		variable_name = spCleaningString(temp_variable,msg,filename,line_counter);
		free(temp_variable);
		if(variable_name == NULL)
		{

			spConfigDestroy(config);
			fclose(file);
			return NULL;
		}
		// if SKIP return meens this is a comment line need to be skipped
		else if (strcmp(variable_name,SKIP) == 0)
		{
			free(variable_name);
			line_counter++;
			continue;
		}
		// check if this is an empty line if yes need to skip
		else if ((strcmp(variable_name,SKIP_EMPTY_LINE) == 0) && (!index_of_equal))
		{
			free(variable_name);
			line_counter++;
			continue;
		}
		// if we got here means we have an error no '=' in the line
		else if (!index_of_equal)
		{
			*msg = SP_CONFIG_INVALID_STRING;
			spRegularErrorPrinter(filename,line_counter,0,DUMMY);
			free(variable_name);
			spConfigDestroy(config);
			fclose(file);
			return NULL;
		}
		else
		{
			temp_variable = (char*)malloc((strlen(index_of_equal) + 1) * sizeof(char));
			strcpy(temp_variable,index_of_equal);
			variable_value = spCleaningString(index_of_equal,msg,filename,line_counter);
			free(temp_variable);
		}

		if ((variable_value == NULL)||(strcmp(variable_value,SKIP) == 0))
		{
			*msg = SP_CONFIG_INVALID_STRING;
			spRegularErrorPrinter(filename,line_counter,0,DUMMY);
			free(variable_name);
			free(variable_value);
			spConfigDestroy(config);
			fclose(file);
			return NULL;
		}
		else if (strcmp(variable_value,SKIP_EMPTY_LINE) == 0)
		{
			free(variable_name);
			free(variable_value);
			line_counter++;
			continue;
		}
		succeeded = spAssignArgument(config, variable_name, variable_value, msg, line_counter, filename);
		if (succeeded == 1)
		{
		free(variable_name);
		free(variable_value);
		line_counter++;
		}
		else if (succeeded == 0)
		{
			free(variable_name);
			free(variable_value);
			fclose(file);
			return NULL;
		}
		else if (succeeded == 3)
		{
		free(variable_name);
		free(variable_value);
		fclose(file);
		return NULL;
		}
	}
	// check all must initiate variables were initiate
	isDefuatInit = spIsDefaultInitiate(filename, msg, config, line_counter);
	if (!isDefuatInit)
	{
		fclose(file);
		return NULL;
	}
	*msg = SP_CONFIG_SUCCESS;
	fclose(file);
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
		// TODO: handle
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
		// TODO: handle
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
		return config->spNumOfImages;
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
		{
			// TODO: handle
		}

		msg = SP_CONFIG_SUCCESS;
		return msg;
}


