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
#include "SPConfig.h"
#include "SPPoint.h"
#include "SPKDArray.h"
#include "SPImage.h"
#include "SPMainAux.h"
#include "SPKDTreeNode.h"
#include "SPLogger.h"
}

// extracting the features from image path
SPImage extractImageFeatures(sp::ImageProc* imgProc, const char* imagePath, int index)
{
	SPPoint* features = NULL;
	int featuresExtracted = 0;

	features = imgProc->getImageFeatures(imagePath, index, &featuresExtracted);
	if (!features)
	{
		spLoggerPrintError(ERROR_FEATURES_EXTRACT_FAILURE, __FILE__, __func__, __LINE__);
		return NULL;
	}

	return spImageCreateFromImg(features, featuresExtracted);
}
// creating the images feature Data base.
SPImage* extractImagesFeatures(const SPConfig config, sp::ImageProc* imgProc, int numOfImages)
{
	SPImage* imagesFeatures = NULL;
	int i = 0;
	char* imagePath = (char*)malloc((MAX_FILE_PATH_LEN + 1) * sizeof(char));
	if (!imagePath)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		return NULL;
	}
	// allocate memory for the images features data base
	imagesFeatures = (SPImage*)malloc(numOfImages * sizeof(SPImage));
	if (!imagesFeatures)
	{
		free(imagePath);
		imagePath = NULL;
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		return NULL;
	}
	// Iterating over the images extracting the features of each one of them
	for (i = 0; i < numOfImages; ++i)
	{
		spConfigGetImagePath(imagePath, config, i);
		imagesFeatures[i] = extractImageFeatures(imgProc, imagePath, i);
		if (!imagesFeatures[i])
		{
			free(imagePath);
			freeImagesFeatures(imagesFeatures, i);
			return NULL;
		}
	}

	return imagesFeatures;
}
// free all allocated memory in the received arguments
void destroyMainVariables(char* configFileName, SPConfig config, sp::ImageProc* imgProc,
		SPImage* imagesFeatures, char* queryPath, SPKDTreeNode featuresKDTree, SPImage query,
		char** SimilarImagesPathes)
{
	if (imgProc)
	{
		delete imgProc;
	}
	// call the function to free allocated memory.
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
	// getting from user the config file name
	// if nothing was entered default will be chosen.
	char* configFileName = spGetConfigFileName(argc, argv);

	// Checking if config file name "digging" succeeded
	if (!configFileName)
	{
		return 0;
	}

	// creating the config
	config = spConfigCreate(configFileName, &configMsg);
	if (config)
	{
		if (!initLoggerFromConfig(config))
		{
			flushed_printf_newline(SP_ERROR_OPEN_LOGGER);
			spConfigDestroy(config);
			free(configFileName);
			configFileName = NULL;
			return 0;
		}

		// Since c'tor of ImageProc throwing exceptions for failures like missing PCA file (etc.)
		// We will deal with those failures is handling by c++ try & catch
		try
		{
			imgProc = new sp::ImageProc(config);
		}
		catch(const std::exception& e)
		{
			spConfigDestroy(config);
			free(configFileName);
			configFileName = NULL;

			return 0;
		}

		// get the number of images
		numOfImages = spConfigGetNumOfImages(config, &configMsg);
		if (configMsg != SP_CONFIG_SUCCESS)
		{
			spConfigPrintConfigMsgToLogger(configMsg,__FILE__,__func__, __LINE__);
			spConfigDestroy(config);
			free(configFileName);
			configFileName = NULL;
			return 0;
		}
		// get the extract mode
		extractMode = spConfigIsExtractionMode(config, &configMsg);
		if (configMsg != SP_CONFIG_SUCCESS)
		{
			spConfigPrintConfigMsgToLogger(configMsg,__FILE__,__func__, __LINE__);
			destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
					featuresKDTree, query, SimilarImagesPathes);

			return 0;
		}
		// if extraction needed:
		if (extractMode)
		{
			// creating the images features data base
			imagesFeatures = extractImagesFeatures(config, imgProc, numOfImages);
			if (!imagesFeatures)
			{
				destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
						featuresKDTree, query, SimilarImagesPathes);

				return 0;
			}
			// Serialize the ImagesFeatures to the file given in the config
			if (!spSerializeImagesFeatures(imagesFeatures, config))
			{
				destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
						featuresKDTree, query, SimilarImagesPathes);

				return 0;
			}
		}
		else
		{
			//deSerialize the ImagesFeatures - assigning them to images feature database
			if (!spDeserializeImagesFeatures(&imagesFeatures, config))
			{
				destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
						featuresKDTree, query, SimilarImagesPathes);

				return 0;
			}
		}
		//create kdTree based on array of imagesFeatures
		featuresKDTree = spCreateKDTreeFromImages(imagesFeatures, config);
		if (!featuresKDTree)
		{
			destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
					featuresKDTree, query, SimilarImagesPathes);

			return 0;
		}

		freeImagesFeatures(imagesFeatures, numOfImages);
		imagesFeatures = NULL;
	}
	else
	{
		free(configFileName);
		configFileName = NULL;
		return 0;
	}

	// asking the user for an image
	flushed_printf(QUERY_IMAGE_PROMPT);
	queryPath = flushed_gets();
	// if something went wrong print the error to terminate the program
	if (!queryPath)
	{
		flushed_printf_newline(SP_ALLOCATION_FAILURE);
		destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
				featuresKDTree, query, SimilarImagesPathes);

		return 0;
	}
	// while the termination sign(<>) was nor given :
	while (strcmp(TERMINATION_SIGN, queryPath) != 0)
	{
		// getting the query features and assign them to image structure
		// Index 0 is given since it doesn't matter that it's not really image 0 because
		// image features do not have to be distinguished from database (tree) other's images features
		// since it's the query image (and by assignment 3 input conditions,
		// negative index argument for points is forbidden.
		query = extractImageFeatures(imgProc, queryPath, 0);
		if (query)
		{
			// find the most similar images paths from the database received before
			SimilarImagesPathes = spGetSimilarImagesPathes(config, query, featuresKDTree);
			if (!SimilarImagesPathes)
			{
				destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
						featuresKDTree, query, SimilarImagesPathes);

				return 0;
			}
			// check minimal GUI mode for presenting the results
			minimalGUI = spConfigMinimalGui(config,&configMsg);
			if (configMsg != SP_CONFIG_SUCCESS)
			{
				destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
						featuresKDTree, query, SimilarImagesPathes);

				return 0;
			}
			// if true present the images
			if (minimalGUI)
			{
				for (i = 0; i < spConfigGetNumOfSimilarImage(config, &configMsg) ; i++)
				{
					imgProc->showImage(SimilarImagesPathes[i]);
				}
			}
			// if false writing the most similar images paths
			else
			{
				printf("%s %s %s", BEST_CANDIDATES, queryPath, ARE);
				fflush(NULL);

				for (i = 0; i < spConfigGetNumOfSimilarImage(config, &configMsg) ; i++)
				{
					flushed_printf_newline(SimilarImagesPathes[i]);
				}
			}
			// clean allocated memory and getting ready for the next image from user
			spImageDestroy(query);
			query = NULL;
			freeSimilarImagesPathes(SimilarImagesPathes, config);
			SimilarImagesPathes = NULL;
		}
		else
		{
			spLoggerPrintWarning(ERROR_QUERY_LOAD_FAILURE, __FILE__, __func__, __LINE__);
			flushed_printf(ERROR_QUERY_LOAD_FAILURE);
		}

		free(queryPath);
		queryPath = NULL;

		// ask for the next image from user
		flushed_printf(QUERY_IMAGE_PROMPT);
		queryPath = flushed_gets();
		if (!queryPath)
		{
			flushed_printf_newline(SP_ALLOCATION_FAILURE);
			destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
					featuresKDTree, query, SimilarImagesPathes);

			return 0;
		}
	}

	flushed_printf(MSG_EXIT);
	// free all allocated memory before terminating the program
	destroyMainVariables(configFileName, config, imgProc, imagesFeatures, queryPath,
			featuresKDTree, query, SimilarImagesPathes);

	return 0;
}
