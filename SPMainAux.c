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



char* spGetConfigFileName(int argc, char** argv)
{
	char* defaultFile = NULL;

	int i = 0;

	for (i = 0; i < argc; ++i)
	{
		if ((strcmp(argv[i], CONFIG_ARGUMENT_FLAG) == 0) && (i + 1 < argc))
		{
			return argv[i + 1];
		}

	}

	defaultFile = (char*)malloc((strlen(DEFAULT_CONFIG_FILE) + 1) * sizeof(char));
	if (!sprintf("%s", DEFAULT_CONFIG_FILE))
	{
		return NULL;
	}

	return defaultFile;
}

bool spSerializeImagesFeatures(SPImage* imagesFeatures, SPConfig config)
{
	int i = 0;
	int numOfImages = 0;
	SP_CONFIG_MSG configMsg;
	char* imageFeatsPath = (char*)malloc((MAX_FILE_PATH_LEN + 1) * sizeof(char));
	if (!imageFeatsPath)
	{
		//TODO: handle
		return false;
	}

	//TODO: check config msg
	numOfImages = spConfigGetNumOfImages(config, &configMsg);

	for (i = 0; i < numOfImages; ++i)
	{
		//TODO: check config msg
		configMsg = spConfigGetImageFeatsPath(imageFeatsPath, config, i);
		if (!spImageSaveToFeats(imagesFeatures[i], imageFeatsPath))
		{
			return false;
		}
	}

	free(imageFeatsPath);

	return true;
}

bool spDeserializeImagesFeatures(SPImage** imagesFeatures, SPConfig config)
{
	int i = 0;
	int numOfImages = 0;
	SP_CONFIG_MSG configMsg;
	char* imageFeatsPath = (char*)malloc((MAX_FILE_PATH_LEN + 1) * sizeof(char));
	if (!imageFeatsPath)
	{
		//TODO: handle
		return false;
	}

	//TODO: check config msg
	numOfImages = spConfigGetNumOfImages(config, &configMsg);
	*imagesFeatures = (SPImage*)malloc(numOfImages * sizeof(SPImage));
	if (!(*imagesFeatures))
	{
		//TODO: handle
		return false;
	}

	for (i = 0; i < numOfImages; ++i)
	{
		// TODO: handle failure
		 configMsg = spConfigGetImageFeatsPath(imageFeatsPath, config, i);

		 (*imagesFeatures)[i] = spImageCreateFromFeats(imageFeatsPath, i);
		 if (!(*imagesFeatures)[i])
		 {
			 // TODO: handle
			 return false;
		 }
	}

	free(imageFeatsPath);

	return true;
}

char** spGetSimilarImagesPathes(SPConfig config, SPImage queryImage, SPKDTreeNode imagesDB)
{
	int i = 0;
	SPBPQueue topMatchQueue = NULL;
	int* occurencesArr = NULL;
	int numOfImages = 0;
	int spKNN = 0;
	int numOfSimilarImages = 0;
	SP_CONFIG_MSG configMsg;
	SPListElement indexAndDistance = NULL;


	// TODO: handle config msgs
	numOfImages = spConfigGetNumOfImages(config, &configMsg);
	numOfSimilarImages = spConfigGetNumOfSimilarImage(config, &configMsg);
	spKNN = spConfigGetKNN(config, &configMsg);

	topMatchQueue = spBPQueueCreate(spKNN);
	if (!topMatchQueue)
	{
		return NULL;
	}

	occurencesArr = (int*)malloc(sizeof(int) * numOfImages);
	if (!occurencesArr)
	{
		spBPQueueDestroy(topMatchQueue);
		return NULL;
	}

	for (i = 0; i < numOfImages; ++i)
	{
		occurencesArr[i] = 0;
	}

	int queryNumOfFeatures = spImageGetNumOfFeature(queryImage);
	for (i = 0; i < queryNumOfFeatures; ++i)
	{
		spKDTreeNodeKNNSearch(imagesDB, topMatchQueue, spImageGetFeature(queryImage, i));

		while(!spBPQueueIsEmpty(topMatchQueue))
		{
			indexAndDistance = spBPQueuePeek(topMatchQueue);
			spBPQueueDequeue(topMatchQueue);

			occurencesArr[spListElementGetIndex(indexAndDistance)]++;
		}
	}

	for (i =0; i < numOfImages; ++i)
	{
		printf("%d, ", occurencesArr[i]);
		fflush(NULL);
	}

	return NULL;
}
void flushed_printf(const char* str)
{
	printf("%s", str);
	fflush(NULL);
}

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
