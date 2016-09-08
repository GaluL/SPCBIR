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
}

SPImage extractImageFeatures(const SPConfig config, sp::ImageProc* imgProc, const char* imagePath, int index)
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
		return NULL;
	}

	for (i = 0; i < numOfImages; ++i)
	{
		spConfigGetImagePath(imagePath, config, i);
		imagesFeatures[i] = extractImageFeatures(config, imgProc, imagePath, i);
		if (!imagesFeatures)
		{
			// TODO: handle
			return NULL;
		}
	}

	return imagesFeatures;
}

void UnitTest()
{
	srand(0);
	SPPoint* pointsArr = (SPPoint*)malloc(5 * sizeof(SPPoint));
	SPImage image;

	double median = -1;

	double coords0[2] = {1, 2};
	pointsArr[0] = spPointCreate(coords0, 2, 0);

	double coords1[2] = {123, 70};
	pointsArr[1] = spPointCreate(coords1, 2, 1);

	double coords2[2] = {2, 7};
	pointsArr[2] = spPointCreate(coords2, 2, 2);

	double coords3[2] = {9, 11};
	pointsArr[3] = spPointCreate(coords3, 2, 3);

	double coords4[2] = {3, 4};
	pointsArr[4] = spPointCreate(coords4, 2, 4);

	image = spImageCreateFromImg(pointsArr, 5);
	spImageSaveToFeats(image, "testImage.feat");
	//image = spImageCreateFromFeats("testImage.feat");

	SPKDArray kdArr = spKDArrayInit(pointsArr, 5);
	spKDArraySplit(kdArr, 0, &median);

	if (image)
	{
		return;
	}
}

int main(int argc, char** argv)
{
	bool terminate = false;
	SPConfig config = NULL;
	SP_CONFIG_MSG configMsg;
	char* configFileName = spGetConfigFileName(argc, argv);
	sp::ImageProc* imgProc = NULL;
	int numOfImages = 0;
	SPImage* imagesFeatures = NULL;
	SPLogger logger = NULL;
	bool extractMode = false;
	char* queryPath = NULL;
	SPKDTreeNode tree = NULL;
	char** SimilarImagesPathes = NULL;
	int i = 0;

	//UnitTest();


	if (!configFileName)
	{
		if (strcmp(configFileName,DEFAULT_CONFIG_FILE) == 0)
		{
			flushed_printf(ERROR_SPCBIR_NOT_OPEN);
		}
		else
		{
			flushed_printf(ERROR_THE_CONFIGURATION);
			flushed_printf(configFileName);
			flushed_printf(ERROR_COULD_NOT_OPEN);
		}
		return 0;
	}

	config = spConfigCreate(configFileName, &configMsg);
	if (config)
	{
		//TODO: wait for ofek's implementation
		//logger = spLoggerCreate(, )
		imgProc = new sp::ImageProc(config);
		// TODO: handle failures
		numOfImages = spConfigGetNumOfImages(config, &configMsg);
		extractMode = spConfigIsExtractionMode(config, &configMsg);

		if (extractMode)
		{
			imagesFeatures = extractImagesFeatures(config, imgProc, numOfImages);
			if (!imagesFeatures)
			{
				//TODO: handle
			}
			// TO DO : WHY THIS WITHOUT & AND NEXT CALL IS WITH &
			if (!spSerializeImagesFeatures(imagesFeatures, config))
			{
				return -1;
			}
		}
		else
		{
			if (!spDeserializeImagesFeatures(&imagesFeatures, config))
			{
				return -1;
			}
		}

		SPKDTreeNode tree = spCreateKDTreeFromImages(imagesFeatures, config);
	}

	while (!terminate)
	{
		flushed_printf(QUERY_IMAGE_PROMPT);
		queryPath = flushed_gets();
		if (strcmp(TERMINATION_SIGN, queryPath) != 0)
		{
		//	if (tree)

		//	{
				// WHAY IS 666???
				SPImage query = extractImageFeatures(config, imgProc, queryPath, 666);
				SimilarImagesPathes = spGetSimilarImagesPathes(config, query, tree);
				if (spConfigMinimalGui(config,&configMsg))
				{
					for (i = 0; i < spConfigGetNumOfImages(config, &configMsg) ; i++)
					{
						imgProc->showImage(SimilarImagesPathes[i]);
					}
				}
				else
					flushed_printf(BEST_CANDIDATES);
					flushed_printf(queryPath);
					flushed_printf(ARE);
				for (i = 0; i < spConfigGetNumOfImages(config, &configMsg) ; i++)
				{
					flushed_printf_newline(SimilarImagesPathes[i]);
				}

		//	}
		}

		else
		{
			//TODO: DESTROY ALL MEMORY;
			flushed_printf(MSG_EXIT);
			terminate = true;
		}
	}
	return 0;
}
