/*
 * main.cpp
 *
 *  Created on: Aug 13, 2016
 *      Author: galkl
 */
#include <cstdlib>
#include <cstdio>
#include <cstdbool>
#include "SPImageProc.h"
extern "C"
{
#include "SPPoint.h"
#include "SPKDArray.h"
#include "SPImage.h"
#include "SPMainAux.h"
#include "SPConfig.h"
#include "SPKDTreeNode.h"
#include "SPLogger.h"
}

SPImage extractImageFeatures(sp::ImageProc* imgProc, const char* imagePath, int index)
{
	SPPoint* features = NULL;
	int featuresExtracted = 0;

	features = imgProc->getImageFeatures(imagePath, index, &featuresExtracted);
	if (!features)
	{
		return NULL;
	}

	return spImageCreateFromImg(features, featuresExtracted);
}

SPImage* extractImagesFeatures(const SPConfig config, sp::ImageProc* imgProc, int numOfImages)
{
	SPImage* imagesFeatures = NULL;
	int i = 0;
	char* imagePath = (char*)malloc((MAX_FILE_PATH_LEN + 1) * sizeof(char));
	if (!imagePath)
	{
		return NULL;
	}

	imagesFeatures = (SPImage*)malloc(numOfImages * sizeof(SPImage));
	if (!imagesFeatures)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		return NULL;
	}

	for (i = 0; i < numOfImages; ++i)
	{
		spConfigGetImagePath(imagePath, config, i);
		imagesFeatures[i] = extractImageFeatures(imgProc, imagePath, i);
		if (!imagesFeatures)
		{
			spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
			freeImagesFeatures(imagesFeatures,numOfImages);
			return NULL;
		}
	}

	return imagesFeatures;
}

void destroyMainVariables(char* configFileName, SPConfig config, sp::ImageProc* imgProc,
		SPImage* imagesFeatures, char* queryPath, SPKDTreeNode featuresKDTree, SPImage query,
		char** SimilarImagesPathes)
{
	if (imgProc)
	{
		delete imgProc;
	}

	destroyVariables(configFileName, config, imagesFeatures, queryPath, featuresKDTree, query,
		SimilarImagesPathes);
}

int main(int argc, char** argv)
{
	SPConfig config = NULL;
	SP_CONFIG_MSG configMsg;
	sp::ImageProc* imgProc = NULL;
	int numOfImages = 0;
	SPImage* imagesFeatures = NULL;
	bool extractMode = false;
	char* queryPath = NULL;
	SPKDTreeNode featuresKDTree = NULL;
	char** SimilarImagesPathes = NULL;
	int i = 0;
	SPImage query = NULL;
	bool minimalGUI = false;
	char* configFileName = spGetConfigFileName(argc, argv);

	if (!configFileName)
	{
		return 0;
	}

	config = spConfigCreate(configFileName, &configMsg);
	if (config)
	{
		if (!initLoggerFromConfig(config))
		{
			// TODO: handle
		}

		imgProc = new sp::ImageProc(config);

		numOfImages = spConfigGetNumOfImages(config, &configMsg);
		if (configMsg != SP_CONFIG_SUCCESS)
		{
			spConfigPrintConfigMsgToLogger(configMsg);
			spLoggerDestroy();
			spConfigDestroy(config);

			return 0;
		}

		extractMode = spConfigIsExtractionMode(config, &configMsg);
		if (configMsg != SP_CONFIG_SUCCESS)
		{
			spConfigPrintConfigMsgToLogger(configMsg);
			destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
					featuresKDTree, query, SimilarImagesPathes);

			return 0;
		}

		if (extractMode)
		{
			imagesFeatures = extractImagesFeatures(config, imgProc, numOfImages);
			if (!imagesFeatures)
			{
				destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
						featuresKDTree, query, SimilarImagesPathes);

				return 0;
			}

			if (!spSerializeImagesFeatures(imagesFeatures, config))
			{
				destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
						featuresKDTree, query, SimilarImagesPathes);

				return 0;
			}
		}
		else
		{
			if (!spDeserializeImagesFeatures(&imagesFeatures, config))
			{
				destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
						featuresKDTree, query, SimilarImagesPathes);

				return 0;
			}
		}

		featuresKDTree = spCreateKDTreeFromImages(imagesFeatures, config);
		if (!featuresKDTree)
		{
			destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
					featuresKDTree, query, SimilarImagesPathes);

			return 0;
		}
	}
	else
	{
		spConfigPrintConfigMsgToStdout(configMsg);
		free(configFileName);
		return 0;
	}

	flushed_printf(QUERY_IMAGE_PROMPT);
	queryPath = flushed_gets();
	if (!queryPath)
	{
		destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
				featuresKDTree, query, SimilarImagesPathes);

		return 0;
	}

	while (strcmp(TERMINATION_SIGN, queryPath) != 0)
	{
		query = extractImageFeatures(imgProc, queryPath, 666);
		if (!query)
		{
			destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
					featuresKDTree, query, SimilarImagesPathes);

			return 0;
		}

		SimilarImagesPathes = spGetSimilarImagesPathes(config, query, featuresKDTree);
		if (!SimilarImagesPathes)
		{
			destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
					featuresKDTree, query, SimilarImagesPathes);

			return 0;
		}

		minimalGUI = spConfigMinimalGui(config,&configMsg);
		if (configMsg != SP_CONFIG_SUCCESS)
		{
			destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
					featuresKDTree, query, SimilarImagesPathes);

			return 0;
		}

		if (minimalGUI)
		{
			for (i = 0; i < spConfigGetNumOfSimilarImage(config, &configMsg) ; i++)
			{
				imgProc->showImage(SimilarImagesPathes[i]);
			}
		}
		else
		{
			printf("%s %s %s", BEST_CANDIDATES, queryPath, ARE);
			fflush(NULL);

			for (i = 0; i < spConfigGetNumOfSimilarImage(config, &configMsg) ; i++)
			{
				flushed_printf_newline(SimilarImagesPathes[i]);
			}
		}

		free(queryPath);
		spImageDestroy(query);
		freeSimilarImagesPathes(SimilarImagesPathes, config);

		flushed_printf(QUERY_IMAGE_PROMPT);
		queryPath = flushed_gets();
		if (!queryPath)
		{
			destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
					featuresKDTree, query, SimilarImagesPathes);

			return 0;
		}
	}

	flushed_printf(MSG_EXIT);

	destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
			featuresKDTree, query, SimilarImagesPathes);

	return 0;
}
