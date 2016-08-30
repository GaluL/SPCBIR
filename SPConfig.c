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
	config->spPCAFilename = PCA_FILE_NAME_DEFULT;
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
	config->spLoggerFilename = LOGGER_FILE_NAME_DEFULT;
	return config;
}
void spRegularErrorPrinter(const char* filename, int line,int ErrorTypeNum, char* paramterName)
{
	char * errorArray[] = {ERROR_INVALID_CONFIGURATION_LINE,ERROR_INVALID_VALUE,
			 ERROR_PARAMTER,ERROR_IS_NOT_SET};

	if (ErrorTypeNum != 2)
	{
		printf("%s,%s\n,%s,%d\n,%s,%s\n",ERROR_FILE, filename, ERROR_LINE, line, ERROR_MASSAGE,
				 errorArray[ErrorTypeNum]);
		fflush(NULL);
	}
	else
	{
		printf("%s,%s\n,%s,%d\n,%s,%s,%s,%s\n", ERROR_FILE, filename, ERROR_LINE, line,
				 ERROR_MASSAGE,errorArray[ErrorTypeNum],paramterName,errorArray[+1]);
		fflush(NULL);
	}

}
char* spCleaningString(char* str,SP_CONFIG_MSG* msg,const char* filename,int line)
{
	char* result;
	int length = strlen(str);
	char temp[length];
	char letter;
	int i = 0;
	int j = 0;
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
		// check if the char is not a space assign
		else if (!isspace(letter))
		{
			temp [j] = letter;
			j++;
		}
		else if (j != 0)
		{
			*msg = SP_CONFIG_INVALID_STRING;
			spRegularErrorPrinter(filename,line,0,DUMMY);
			return NULL;
		}
	}
	if (j == 0)
	{
		result = (char*)malloc((SIZE_OF_SKIP_SKIP_EMPTY_LINE + 1) * sizeof(char));
		result = SKIP_EMPTY_LINE;
		return result;
	}
	length = strlen(temp);
	result = (char*)malloc((length + 1) * sizeof(char));
	if (!result)
	{
		printf("memory allocation problem");
		return NULL;
	}
	strcpy(result, temp);
	return result;
	}
void spConfigDestroy(SPConfig config)
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
int spAssignArgument(SPConfig config, char* variable_name, char* variable_value,SP_CONFIG_MSG* msg)
{
	int i = 2;
	if (! strcmp(variable_name,SP_IMAGES_DIRECTORY))
	{
		config->spImagesDirectory = variable_value;
		i = 1;
	}
	else if (! strcmp(variable_name,SP_IMAGES_PREFIX))
	{
		config->spImagesPrefix = variable_value;
		i = 1;
	}
	else if (! strcmp(variable_name,SP_IMAGES_SUFFIX))
	{
		if ((strcmp(variable_value,SUFFIX_JPEG)== 0) || (strcmp(variable_value,SUFFIX_PNG)== 0)|| \
				(strcmp(variable_value,SUFFIX_BMP)== 0) || (strcmp(variable_value,SUFFIX_GIF)== 0))
		{
			config->spImagesSuffix = variable_value;
			i = 1;
		}
		else
		{
			printf("Message: Invalid value - constraint not met");
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
		}
		else
		{
			printf("Message: Invalid value - constraint not met");
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
		}
		else
		{
			printf("Message: Invalid value - constraint not met");
			spConfigDestroy(config);
			return 0;
		}
	}
	else if (! strcmp(variable_name,SP_PCA_FILE_NAME))
	{
		config->spPCAFilename = variable_value;
		i = 1;
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
			printf("Message: Invalid value - constraint not met");
			spConfigDestroy(config);
			return 0;
		}
	}
	else if (! strcmp(variable_name,SP_EXTRACTION_MODE))
	{
		if ((strcmp(variable_value,"true")== 0) || (strcmp(variable_value,"false")== 0))
		{
			config->spExtractionMode = variable_value;
			i = 1;
		}
		else
		{
			printf("Message: Invalid value - constraint not met");
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
			printf("Message: Invalid value - constraint not met");
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
			printf("error constraint not met");
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
			printf("Message: Invalid value - constraint not met");
			spConfigDestroy(config);
			return 0;
		}
	}
	else if (! strcmp(variable_name,SP_MINMIMAL_GUI))
	{
		if ((strcmp(variable_value,"true")== 0) || (strcmp(variable_value,"false")== 0))
		{
			config->spMinimalGUI = variable_value;
			i = 1;
		}
		else
		{
			printf("Message: Invalid value - constraint not met");
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
			printf("Message: Invalid value - constraint not met");
			spConfigDestroy(config);
			return 0;
		}
	}
	else if (! strcmp(variable_name,SP_LOGGER_FILE_NAME))
	{
		config->spLoggerFilename = variable_value;
		i = 1;
	}
	if (i == 2)
	{
		printf("error Message: Invalid configuration line");
		return i;
	}

	return i;
}

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg)
{
	int succeeded;
	int line_counter = 0;
	char* index_of_equal;
	char input[MAX_LINE_LENGTH];
	char* variable_name = NULL;
	char* variable_value = NULL;
	FILE *file = fopen (filename, "r");
	//deal with error 3 file couldnt open
	if (!file)
	{
		printf("%s,%s,%s/n",ERROR_THE_CONFIGURATION_FILE,filename,ERROR_COULD_NOT_OPEN);
		fflush(NULL);
		return NULL;
	}
	SPConfig config = spConfigInit();
	if (!config)
	{
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
		// if '=' not exist we need to check if its comment line,empty line or an error
		variable_name = spCleaningString(input,msg,filename,line_counter);
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
			variable_value = spCleaningString(index_of_equal,msg,filename,line_counter);
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
		succeeded = spAssignArgument(config,variable_name,variable_value,msg);
		if (succeeded == 1)
		{
		line_counter++;
		}
		else
		{
			return NULL;
		}
	}


	return NULL;
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
	return false;
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
	return false;
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
	return 0;
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
	return 0;
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
	return 0;
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
SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index)
{
	return SP_CONFIG_SUCCESS;
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
	return SP_CONFIG_SUCCESS;
}

/**
 * Frees all memory resources associate with config.
 * If config == NULL nothig is done.
 */
/*void spConfigDestroy(SPConfig config)
{
	return;
}
*/


