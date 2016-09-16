/*
 * SPMainAux.c
 *
 *  Created on: Aug 29, 2016
 *      Author: galkl
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "SPMainAux.h"
#include "SPKDTreeNode.h"

// free allocated memory for the of imagesFeatures
void freeImagesFeatures(SPImage* imagesFeatures, int numOfImages)
{
	int i = 0;

	if (!imagesFeatures)
	{
		return;
	}

	for (i = 0; i < numOfImages; ++i)
	{
		if (imagesFeatures[i])
		{
			spImageDestroy(imagesFeatures[i]);
		}
	}

	free(imagesFeatures);
	imagesFeatures = NULL;
}
// retrieve the file name  argument fro, the argv.
char* spGetConfigFileName(int argc, char** argv)
{
	char* configFilePath = NULL;

	configFilePath = (char*)malloc((MAX_FILE_PATH_LEN + 1) * sizeof(char));
	if (!configFilePath)
	{
		flushed_printf(SP_ALLOCATION_FAILURE);
		return NULL;
	}

	// No config file entered, using the default one
	if (argc == 1)
	{
		strcpy(configFilePath, DEFAULT_CONFIG_FILE);
	}
	else if (argc == 3 && strcmp(argv[1], CONFIG_ARGUMENT_FLAG) == 0)
	{
		strcpy(configFilePath, argv[2]);
	}
	else
	{
		printf("%s%s\n", ERROR_INVALID_COMAND_LINE, DEFAULT_CONFIG_FILE);
		fflush(NULL);

		free(configFilePath);
		configFilePath = NULL;
	}

	return configFilePath;
}
// Write the image features to  a file received from the config file.
bool spSerializeImagesFeatures(SPImage* imagesFeatures, SPConfig config)
{
	int i = 0;
	int numOfImages = 0;
	SP_CONFIG_MSG configMsg;
	char* imageFeatsPath = (char*)malloc((MAX_FILE_PATH_LEN + 1) * sizeof(char));
	if (!imageFeatsPath)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		return false;
	}

	// getting the the num of images -. check config msg if not success end the program
	numOfImages = spConfigGetNumOfImages(config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS)
	{
		spConfigPrintConfigMsgToLogger(configMsg,__FILE__,__func__, __LINE__);
		free(imageFeatsPath);
		imageFeatsPath = NULL;
		return false;
	}
	for (i = 0; i < numOfImages; ++i)
	{
		// getting the path to the place to write the images features
		configMsg = spConfigGetImageFeatsPath(imageFeatsPath, config, i);
		if (configMsg != SP_CONFIG_SUCCESS)
		{
			spConfigPrintConfigMsgToLogger(configMsg,__FILE__,__func__, __LINE__);
			free(imageFeatsPath);
			imageFeatsPath = NULL;
			return false;
		}
		// saving the image[i] features the the image features path received from config
		if (!spImageSaveToFeats(imagesFeatures[i], imageFeatsPath))
		{
			free(imageFeatsPath);
			imageFeatsPath = NULL;
			return false;
		}
	}

	free(imageFeatsPath);
	imageFeatsPath = NULL;

	return true;
}
// reading features from file and assigning them to imagesFeature structure
bool spDeserializeImagesFeatures(SPImage** imagesFeatures, SPConfig config)
{
	int i = 0;
	int numOfImages = 0;
	SP_CONFIG_MSG configMsg;
	// allocating memory for image path
	char* imageFeatsPath = (char*)malloc((MAX_FILE_PATH_LEN + 1) * sizeof(char));
	if (!imageFeatsPath)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		return false;
	}
	// getting the number of images as delivered in config file
	numOfImages = spConfigGetNumOfImages(config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS)
	{
		spConfigPrintConfigMsgToLogger(configMsg,__FILE__,__func__, __LINE__);
		free(imageFeatsPath);
		imageFeatsPath = NULL;
		return false;
	}
	// allocating memory for the images features
	*imagesFeatures = (SPImage*)malloc(numOfImages * sizeof(SPImage));
	if (!(*imagesFeatures))
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		free(imageFeatsPath);
		imageFeatsPath = NULL;
		return false;
	}
	// for each image assign to image structure from the features in the file
	for (i = 0; i < numOfImages; ++i)
	{
		 configMsg = spConfigGetImageFeatsPath(imageFeatsPath, config, i);
		if (configMsg != SP_CONFIG_SUCCESS)
		{
			spConfigPrintConfigMsgToLogger(configMsg,__FILE__,__func__, __LINE__);
			free(imageFeatsPath);
			imageFeatsPath = NULL;
			return false;
		}
		 // for each image assign to image structure from the features in the file
		 (*imagesFeatures)[i] = spImageCreateFromFeats(imageFeatsPath, i);
		 if (!(*imagesFeatures)[i])
		 {
			// if a problem occurred freeing all allocated memory
			freeImagesFeatures((*imagesFeatures), i);
			*imagesFeatures = NULL;
			free(imageFeatsPath);
			imageFeatsPath = NULL;

			return false;
		 }
	}

	free(imageFeatsPath);
	imageFeatsPath = NULL;

	return true;
}

typedef struct sp_indexkeeper_t {
	int index;
	int value;
} SPIndexKeeper;


// compare function used to compare to points by value while keeping there index
int cmpIndexKeepersDesceding(const void* p1, const void* p2)
{
	int valuesDiff = 0;
	const SPIndexKeeper* obj1 = (SPIndexKeeper*)p1;
	const SPIndexKeeper* obj2 = (SPIndexKeeper*)p2;

	valuesDiff = obj2->value - obj1->value;

	return valuesDiff ? valuesDiff : obj1->index - obj2->index;
}
// get the path of the most similar images the query image from the images data base
char** spGetSimilarImagesPathes(SPConfig config, SPImage queryImage, SPKDTreeNode imagesDB)
{
	int i = 0;
	SPBPQueue topMatchQueue = NULL;
	SPIndexKeeper* occurencesArr = NULL;
	int numOfImages = 0;
	int spKNN = 0;
	int numOfSimilarImages = 0;
	SP_CONFIG_MSG configMsg;
	SPListElement indexAndDistance = NULL;
	char** result = NULL;


	numOfImages = spConfigGetNumOfImages(config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS)
	{
		spConfigPrintConfigMsgToLogger(configMsg,__FILE__,__func__, __LINE__);
		return false;
	}

	numOfSimilarImages = spConfigGetNumOfSimilarImage(config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS)
	{
		spConfigPrintConfigMsgToLogger(configMsg,__FILE__,__func__, __LINE__);
		return false;
	}

	spKNN = spConfigGetKNN(config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS)
	{
		spConfigPrintConfigMsgToLogger(configMsg,__FILE__,__func__, __LINE__);
		return false;
	}

	topMatchQueue = spBPQueueCreate(spKNN);
	if (!topMatchQueue)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		return NULL;
	}

	occurencesArr = (SPIndexKeeper*)malloc(sizeof(SPIndexKeeper) * numOfImages);
	if (!occurencesArr)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		spBPQueueDestroy(topMatchQueue);
		return NULL;
	}
	// filling the index keeper structure;
	for (i = 0; i < numOfImages; ++i)
	{
		occurencesArr[i].index = i;
		occurencesArr[i].value = 0;
	}
	// getting the num of features for image - if happened, error printed to logger from the function
	int queryNumOfFeatures = spImageGetNumOfFeature(queryImage);
	if (queryNumOfFeatures < 0)
	{

		free(occurencesArr);
		occurencesArr = NULL;
		spBPQueueDestroy(topMatchQueue);
		return NULL;
	}

	for (i = 0; i < queryNumOfFeatures; ++i)
	{
		if (!spKDTreeNodeKNNSearch(imagesDB, topMatchQueue, spImageGetFeature(queryImage, i)))
		{
			free(occurencesArr);
			occurencesArr = NULL;
			spBPQueueDestroy(topMatchQueue);
			return NULL;
		}

		while(!spBPQueueIsEmpty(topMatchQueue))
		{
			indexAndDistance = spBPQueuePeek(topMatchQueue);
			spBPQueueDequeue(topMatchQueue);
			occurencesArr[spListElementGetIndex(indexAndDistance)].value++;
			spListElementDestroy(indexAndDistance);
		}
	}
	spBPQueueDestroy(topMatchQueue);
	// the occurrence array by value while keeping the index
	qsort(occurencesArr, numOfImages, sizeof(SPIndexKeeper), cmpIndexKeepersDesceding);

	result = (char**)malloc(numOfSimilarImages * sizeof(char*));
	if (!result)
	{
		free(occurencesArr);
		occurencesArr = NULL;
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
	}

	for (i = 0; i < numOfSimilarImages; ++i)
	{
		result[i] = (char*)malloc(MAX_FILE_PATH_LEN * sizeof(char));
		if (!result[i])
		{
			spDestroyResult(result, numOfSimilarImages);
			free(occurencesArr);
			occurencesArr = NULL;
			spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		}
	}
	// getting the path of the most similar images.
	for (i = 0; i < numOfSimilarImages; ++i)
	{
		if (spConfigGetImagePath(result[i], config, occurencesArr[i].index) != SP_CONFIG_SUCCESS )
		{
			spDestroyResult(result, numOfSimilarImages);
			free(occurencesArr);
			occurencesArr = NULL;
			spConfigPrintConfigMsgToLogger(configMsg,__FILE__,__func__, __LINE__);
			return NULL;
		}
	}
	free(occurencesArr);
	occurencesArr = NULL;
	return result;
}
// free allocated array of strings
void spDestroyResult(char** result,int numOfSimilarImages)
{
	int i = 0;
	if (result)
	{
		for (i = 0; i < numOfSimilarImages; i++)
		{
			if (result[i])
			{
				free(result[i]);
				result[i] = NULL;
			}
		}
	free (result);
	result = NULL;
	}
}
// print a string
void flushed_printf(const char* str)
{
	printf("%s", str);
	fflush(NULL);
}
//print a string with a newline
void flushed_printf_newline(const char* str)
{
	printf("%s\n", str);
	fflush(NULL);
}
// get a string from stdin
char* flushed_gets()
{
	char input[MAX_INPUT_LENGTH];
	char* result = NULL;

	fgets(input, sizeof(input), stdin);
	fflush(NULL);

	strtok(input, "\n");

	result = (char*)malloc((strlen(input) + 1) * sizeof(char));
	if (result)
	{
		strcpy(result, input);
	}

	return result;
}

void freeSimilarImagesPathes(char** SimilarImagesPathes,SPConfig config )
{
	int i = 0;
	int numOfSimilarImages = 0;
	SP_CONFIG_MSG configMsg;

	if (!SimilarImagesPathes)
	{
		return;
	}

	numOfSimilarImages = spConfigGetNumOfSimilarImage(config, &configMsg);
	for (i = 0; i < numOfSimilarImages; ++i)
	{
		if (SimilarImagesPathes[i])
		{
			free(SimilarImagesPathes[i]);
			SimilarImagesPathes[i] = NULL;
		}
	}

	if (SimilarImagesPathes)
	{
		free(SimilarImagesPathes);
		SimilarImagesPathes = NULL;
	}
}

// Initializing the logger according to level and filename setting stored in config file
bool initLoggerFromConfig(SPConfig config)
{
	SP_CONFIG_MSG configMsg;
	char* loggerFileName = NULL;
	int loggerLevel = -1;
	bool isStdout = false;

	// Allocating buffer for logger file name
	loggerFileName = (char*)malloc((MAX_FILE_PATH_LEN + 1) * sizeof(char));
	if (!loggerFileName)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		return false;
	}

	// Getting logger file name to write to
	configMsg = spConfigGetLoggerFilename(loggerFileName, config);
	if (configMsg != SP_CONFIG_SUCCESS)
	{
		spConfigPrintConfigMsgToLogger(configMsg, __FILE__, __func__, __LINE__);
		free(loggerFileName);
		loggerFileName = NULL;
		return false;
	}

	// Getting logger reporting level
	loggerLevel = spConfigLoggerLevel(config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS)
	{
		spConfigPrintConfigMsgToLogger(configMsg, __FILE__, __func__, __LINE__);
		free(loggerFileName);
		loggerFileName = NULL;
		return false;
	}

	// Stores true if logger filename is "stdout" and else otherwise
	isStdout = (strcmp(loggerFileName, LOGGER_FILE_NAME_DEFULT) == 0);

	// Passing null if logger output is stdout (to make logger write to stdout)
	// and logger filename otherwise
	if (spLoggerCreate((isStdout ? NULL : loggerFileName), loggerLevel) != SP_LOGGER_SUCCESS)
	{
		free(loggerFileName);
		loggerFileName = NULL;
		return false;
	}

	free(loggerFileName);
	loggerFileName = NULL;

	return true;
}

void destroyVariables(char* configFileName, SPConfig config, SPImage* imagesFeatures,
		char* queryPath, SPKDTreeNode featuresKDTree, SPImage query, char** SimilarImagesPathes)
{
	SP_CONFIG_MSG configMsg;

	spLoggerDestroy();

	if (configFileName)
	{
		free(configFileName);
		configFileName = NULL;
	}

	if (imagesFeatures)
	{
		freeImagesFeatures(imagesFeatures, spConfigGetNumOfImages(config, &configMsg));
	}

	if (queryPath)
	{
		free(queryPath);
		queryPath = NULL;
	}

	if (featuresKDTree)
	{
		spKDTreeNodeDestroy(featuresKDTree);
	}

	if (query)
	{
		spImageDestroy(query);
	}

	if (SimilarImagesPathes)
	{
		freeSimilarImagesPathes(SimilarImagesPathes, config);
	}

	if (config)
	{
		spConfigDestroy(config);
	}
}

