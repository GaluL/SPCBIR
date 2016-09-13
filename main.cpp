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

//void UnitTest()
//{
//	srand(0);
//	SPPoint* pointsArr = (SPPoint*)malloc(5 * sizeof(SPPoint));
//	SPImage image;
//
//	double median = -1;
//
//	double coords0[2] = {1, 2};
//	pointsArr[0] = spPointCreate(coords0, 2, 0);
//
//	double coords1[2] = {123, 70};
//	pointsArr[1] = spPointCreate(coords1, 2, 1);
//
//	double coords2[2] = {2, 7};
//	pointsArr[2] = spPointCreate(coords2, 2, 2);
//
//	double coords3[2] = {9, 11};
//	pointsArr[3] = spPointCreate(coords3, 2, 3);
//
//	double coords4[2] = {3, 4};
//	pointsArr[4] = spPointCreate(coords4, 2, 4);
//
//	image = spImageCreateFromImg(pointsArr, 5);
//	spImageSaveToFeats(image, "testImage.feat");
//	//image = spImageCreateFromFeats("testImage.feat");
//
//	//SPKDArray kdArr = spKDArrayInit(pointsArr, 5);
//	//spKDArraySplit(kdArr, 0, &median);
//
//	if (image)
//	{
//		return;
//	}
//}

int main(int argc, char** argv)
{
	bool terminate = false;
	SPConfig config = NULL;
	SP_CONFIG_MSG configMsg;
	SP_LOGGER_LEVEL loggerLevel;
	sp::ImageProc* imgProc = NULL;
	int numOfImages = 0;
	SPImage* imagesFeatures = NULL;
	//SPLogger logger = NULL;
	bool extractMode = false;
	char* queryPath = NULL;
	SPKDTreeNode tree = NULL;
	char** SimilarImagesPathes = NULL;
	int i = 0;
	char* configFileName = spGetConfigFileName(argc, argv);
	char* loggerFilename = NULL;
	int intLoggerLevel = 0;

	if (!configFileName)
	{
		return 0;
	}

	config = spConfigCreate(configFileName, &configMsg);
	if (config)
	{

		loggerFilename = (char*)malloc((MAX_LINE_LENGTH + 1)* sizeof(char));
		if (!loggerFilename)
		{
			flushed_printf_newline(SP_ALLOCATION_FAILURE);
			destroyMain(config, loggerFilename, configFileName, imagesFeatures,
					tree, SP_DUMMY_NUM_OF_IMAGES);
			return 0;
		}
		if (spConfigGetLoggerFilename(loggerFilename,config) != SP_CONFIG_SUCCESS)
		{
			flushed_printf_newline(SP_ERROR_READING_CONFIG);
			spConfigDestroy(config);
			destroyMain(config, loggerFilename, configFileName, imagesFeatures,
					tree, SP_DUMMY_NUM_OF_IMAGES);
			return 0;
		}
		intLoggerLevel = spConfigLoggerLevel(config, &configMsg);
		setMyLoggerLevel(intLoggerLevel,&loggerLevel);
		if (spLoggerCreate(loggerFilename,loggerLevel)!= SP_LOGGER_SUCCESS)
		{
			destroyMain(config, loggerFilename, configFileName, imagesFeatures,
					tree, SP_DUMMY_NUM_OF_IMAGES);
			flushed_printf_newline(SP_ERROR_OPEN_LOGGER);
			return 0;
		}

		imgProc = new sp::ImageProc(config);

		numOfImages = spConfigGetNumOfImages(config, &configMsg);
		if (configMsg != SP_CONFIG_SUCCESS)
		{
			spConfigPrintConfigMsgToLogger(&configMsg);
			spLoggerDestroy();
			destroyMain(config, loggerFilename, configFileName, imagesFeatures,
					tree, SP_DUMMY_NUM_OF_IMAGES);
			return 0;
		}
		extractMode = spConfigIsExtractionMode(config, &configMsg);
		if (configMsg != SP_CONFIG_SUCCESS)
		{
			spConfigPrintConfigMsgToLogger(&configMsg);
			spLoggerDestroy();
			destroyMain(config, loggerFilename, configFileName, imagesFeatures, tree, numOfImages);
			return 0;
		}
		if (extractMode)
		{
			imagesFeatures = extractImagesFeatures(config, imgProc, numOfImages);
			if (!imagesFeatures)
			{
				spLoggerDestroy();
				destroyMain(config, loggerFilename, configFileName, imagesFeatures, tree, numOfImages);
			}
			if (!spSerializeImagesFeatures(imagesFeatures, config))
			{
				spLoggerDestroy();
				destroyMain(config, loggerFilename, configFileName, imagesFeatures, tree, numOfImages);
				return 0;
			}
		}
		else
		{
			if (!spDeserializeImagesFeatures(&imagesFeatures, config))
			{
				spLoggerDestroy();
				destroyMain(config, loggerFilename, configFileName, imagesFeatures, tree, numOfImages);
				return 0;
			}
		}

		tree = spCreateKDTreeFromImages(imagesFeatures, config);
	}
	else
	{
		free(configFileName);
		return 0;
	}
	while (!terminate)
	{
		flushed_printf(QUERY_IMAGE_PROMPT);
		queryPath = flushed_gets();
		if (strcmp(TERMINATION_SIGN, queryPath) != 0)
		{
			if (tree)

			{
				// WHY 666???
				SPImage query = extractImageFeatures(imgProc, queryPath, 666);
				SimilarImagesPathes = spGetSimilarImagesPathes(config, query, tree);
				if (spConfigMinimalGui(config,&configMsg))
				{
					for (i = 0; i < spConfigGetNumOfSimilarImage(config, &configMsg) ; i++)
					{
						imgProc->showImage(SimilarImagesPathes[i]);
					}
				}
				else
				{
					flushed_printf(BEST_CANDIDATES);
					flushed_printf(queryPath);
					flushed_printf(ARE);
					for (i = 0; i < spConfigGetNumOfSimilarImage(config, &configMsg) ; i++)
					{
						flushed_printf_newline(SimilarImagesPathes[i]);
					}
				}
				free(queryPath);
				spImageDestroy(query);
				freeSimilarImagesPathes(SimilarImagesPathes, config);

			}
		}
		else
		{
			spLoggerDestroy();
			destroyMain(config, loggerFilename, configFileName, imagesFeatures, tree, numOfImages);
			flushed_printf(MSG_EXIT);
			terminate = true;
		}
	}
	return 0;
}
