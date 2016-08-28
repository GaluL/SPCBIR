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


typedef struct sp_config_t
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

} CONFIG_STRUCT;

// function that sets the defult values of the struct.

SPConfig spConfigInit()
{
	SPConfig config = NULL;
	config = (SPConfig)malloc(sizeof(*config));
	if (!config)
	{
		// add error memory allocation failure
		return NULL;
	}
	config->spPCADimension = 20;
	config->spPCAFilename = (char*)malloc((SIZE_OF_PCAYML +1) * sizeof(char));
	if (!config->spPCAFilename)
	{
		// add error memory allocation failure
		free(config);
		return NULL;
	}
	config->spPCAFilename = "pca.yml";
	config->spNumOfFeatures = 100;
	config->spExtractionMode = true;
	config->spNumOfSimilarImages = 1;
	config->spKDTreeSplitMethod = MAX_SPREAD;
	config->spKNN = 1;
	config->spMinimalGUI = false;
	config->spLoggerLevel = 3;
	config->spLoggerFilename  = (char*)malloc((SIZE_OF_STDOUT +1) * sizeof(char));
		if (!config->spLoggerFilename)
		{
			// add error memory allocation failure
			free(config->spPCAFilename);
			free(config);
			return NULL;
		}
	config->spLoggerFilename = "stdout";
	return config;
}
char* spCleaningString(char* str)
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
		if( (letter == '#') && (j != 0) )
		{
			return "error # in the middle";
		}
		// if the previous condition didn't meet and the the letter is # this is a comment line and should be skipped
		else if(letter =='#')
		{
			return NULL;
		}
		// check if the char is not a space assign
		else if (!isspace(letter))
		{
			temp [j] = letter;
			j++;
		}
		else if (j != 0)
		{
			return "error space in the middle";
		}
	length = strlen(temp);
	result = (char*)malloc((length + 1) * sizeof(char));
	if (result)
	{
		strcpy(result, temp);}
	}
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

}
SPConfig spAssignArgument(SPConfig config, char* variable_name, char* variable_value)
{

	int i;
		if (! strcmp(variable_name,"spImagesDirectory"))
		{
			config->spImagesDirectory = variable_value;
			i = 1;
		}
		else if (! strcmp(variable_name,"spImagesPrefix"))
		{
			config->spImagesPrefix = variable_value;
			i = 1;
		}
		else if (! strcmp(variable_name,"spImagesSuffix"))
		{
			if ((strcmp(variable_value,".jpeg")== 0) || (strcmp(variable_value,".png")== 0)|| \
					(strcmp(variable_value,".bmp")== 0) || (strcmp(variable_value,".gif")== 0))
			{
				config->spImagesSuffix = variable_value;
				i = 1;
			}
			else
			{
				printf("Message: Invalid value - constraint not met");
				spConfigDestroy(config);
				return NULL;
			}
		}
		else if (! strcmp(variable_name,"spNumOfImages"))
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
				return NULL;
			}
		}
		else if (! strcmp(variable_name,"spPCADimension"))
		{
			if ((atoi(variable_value) > 9 )&& (atoi(variable_value) < 29))
			{
				config->spPCADimension = atoi(variable_value);
				i = 1;
			}
			else
			{
				printf("Message: Invalid value - constraint not met");
				spConfigDestroy(config);
				return NULL;
			}
		}
		else if (! strcmp(variable_name,"spPCAFilename"))
		{
			config->spPCAFilename = variable_value;
			i = 1;
		}
		else if (! strcmp(variable_name,"spNumOfFeatures"))
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
				return NULL;
			}
		}
		else if (! strcmp(variable_name,"spExtractionMode"))
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
				return NULL;
			}

		}
		else if (! strcmp(variable_name,"spNumOfSimilarImages"))
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
				return NULL;
			}

		}
		else if (! strcmp(variable_name,"spKDTreeSplitMethod"))
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
				return NULL;
			}

		}
		else if (! strcmp(variable_name,"spKNN"))
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
				return NULL;
			}
		}
		else if (! strcmp(variable_name,"spMinimalGUI"))
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
				return NULL;
			}

		}
		else if (! strcmp(variable_name,"spLoggerLevel"))
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
				return NULL;
			}
		}
		else if (! strcmp(variable_name,"spLoggerFilename"))
		{
			config->spLoggerFilename = variable_value;
			i = 1;
		}

	return config;
}

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg)
{
	char* index_of_equl;
	char input[MAX_LINE_LENGTH];
	char* variable_name = NULL;
	char* variable_value = NULL;
	FILE *file = fopen (filename, "r");
	if (!file)
	{
		//ERROR CANT OPEN FILE
		return NULL;
	}
	SPConfig config = spConfigInit();
	if (!config)
	{
		return NULL;
	}
	while ( fgets ( input, sizeof(input), file ) != NULL )
	{
		strtok(input, "\n");
		index_of_equl = strchr(input,'=');
		*index_of_equl= '\0';
		index_of_equl++;
		variable_name = spCleaningString(input);
		if (variable_name == NULL)
		{
			continue;
		}
		else
		{
			variable_value = spCleaningString(index_of_equl);
		}
		if (variable_name == NULL)
		{
			free (variable_name);
			continue;
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


