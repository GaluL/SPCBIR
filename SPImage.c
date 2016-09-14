/*
 * SPImage.c
 *
 *  Created on: Aug 28, 2016
 *      Author: galkl
 */
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "SPImage.h"
#include "SPPoint.h"
#include "SPCommonDefs.h"
#include "SPLogger.h"


struct sp_image_t {
	SPPoint* feats;
	int featsCount;
};
/**
 * Assigning the feats into the image struct
 */
SPImage spImageCreateFromImg(SPPoint* feats, int featsCount)
{
	// Allocate the memory for the struct
	SPImage result = (SPImage)malloc(sizeof(*result));
	if (!result)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		return NULL;
	}

	result->feats = feats;
	result->featsCount = featsCount;

	return result;
}

SPImage spImageCreateFromFeats(const char* featsFileName, int imgIndex)
{
	FILE *fp;
	SPImage res = NULL;
	int i = 0;
	int j = 0;
	int currPointDim = 0;
	double* currPointCoords = NULL;
	// open the file to read from
	fp = fopen(featsFileName, FEATS_FILE_READ_MODE);
	if (!fp)
	{
		spLoggerPrintError(SP_ERROR_FILE_NOT_OPEN, __FILE__, __func__, __LINE__);
		return NULL;
	}
	// allocate the result structure
	res = (SPImage)malloc(sizeof(*res));
	if (!res)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		fclose(fp);
		return NULL;
	}
	// Assign to res->featsCount the number of features
	if (!fscanf(fp, "%d", &(res->featsCount)))
	{
		spLoggerPrintError(SP_FAILED_READ_FROM_FILE, __FILE__, __func__, __LINE__);
		spImageDestroy(res);
		fclose(fp);
		return NULL;
	}
	// Allocate the the features array
	res->feats = (SPPoint*)malloc(res->featsCount * sizeof(SPPoint));
	if (!res->feats)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		spImageDestroy(res);
		fclose(fp);
		return NULL;
	}
	// for each feature read the value of the dimension
	for (i = 0; i < res->featsCount; ++i)
	{
		if (!fscanf(fp, "%d", &currPointDim))
		{
			spLoggerPrintError(SP_FAILED_READ_FROM_FILE, __FILE__, __func__, __LINE__);
			spImageDestroy(res);
			fclose(fp);
			return NULL;
		}

		currPointCoords = (double*)malloc(currPointDim * sizeof(double));
		if (!currPointCoords)
		{
			spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
			spImageDestroy(res);
			fclose(fp);
			return NULL;
		}
		// save the coords value in to an array
		for (j = 0; j < currPointDim; ++j)
		{
			if (!fscanf(fp, "%lf", &(currPointCoords[j])))
			{
				spLoggerPrintError(SP_FAILED_READ_FROM_FILE, __FILE__, __func__, __LINE__);
				free(currPointCoords);
				currPointCoords = NULL;
				spImageDestroy(res);
				fclose(fp);
				return NULL;
			}
		}
		// create the point from the array read earlier
		res->feats[i] = spPointCreate(currPointCoords, currPointDim, imgIndex);

		if (currPointCoords)
		{
			free(currPointCoords);
			currPointCoords = NULL;
		}
	}

	fclose(fp);

	return res;
}
/**
 * Write the image features to file featsFileName received as argument
 */
bool spImageSaveToFeats(SPImage image, const char* featsFileName)
{
	FILE *fp;
	int i = 0;
	int j = 0;
	int currPointDim = 0;

	if (!image)
	{
		spLoggerPrintError(SP_BAD_ARGUMENT, __FILE__, __func__, __LINE__);
		return false;
	}
	// open the featsFileName file in write mode
	fp = fopen(featsFileName, FEATS_FILE_WRITE_MODE);
	if (!fp)
	{
		spLoggerPrintError(SP_ERROR_FILE_NOT_OPEN, __FILE__, __func__, __LINE__);
		return false;
	}
	// write the number of the features in the head of the file
	if (fprintf(fp, "%d\n", image->featsCount) < 0)
	{
		spLoggerPrintError(SP_FAILED_WRITE_TO_FILE, __FILE__, __func__, __LINE__);
		fclose(fp);
		return false;

	}
	// iterating over the features in order to write to file
	for (i = 0; i < image->featsCount; ++i)
	{
		currPointDim = spPointGetDimension(image->feats[i]);
		// write the first column represent how many dimensions the feature has
		if (fprintf(fp, "%d ", currPointDim) < 0 )
		{
			spLoggerPrintError(SP_FAILED_WRITE_TO_FILE, __FILE__, __func__, __LINE__);
			fclose(fp);
			return false;
		}

		for (j = 0; j < currPointDim; ++j)
		{
			// write the  features values by axis
			if (fprintf(fp, "%f ", spPointGetAxisCoor(image->feats[i], j)) < 0)
			{
				spLoggerPrintError(SP_FAILED_WRITE_TO_FILE, __FILE__, __func__, __LINE__);
				fclose(fp);
				return false;
			}
		}

		if (fprintf(fp, NEW_LINE) < 0)
		{
			spLoggerPrintError(SP_FAILED_WRITE_TO_FILE, __FILE__, __func__, __LINE__);
			fclose(fp);
			return false;
		}
	}

	fclose(fp);

	return true;
}
// Release allocated memory of the struct SPimage
void spImageDestroy(SPImage image)
{
	int i = 0;

	if (image)
	{
		if (image->feats)
		{
			for (i = 0; i < image->featsCount; ++i)
			{
				if (image->feats[i])
				{
					spPointDestroy(image->feats[i]);
					image->feats[i] = NULL;
				}
			}

			free(image->feats);
			image->feats = NULL;
		}

		free(image);
		image = NULL;
	}
}

int spImageGetNumOfFeature(SPImage image)
{
	if (!image)
	{
		spLoggerPrintError(SP_BAD_ARGUMENT, __FILE__, __func__, __LINE__);
		return SP_INVALID_NEG_VALUE;
	}

	return image->featsCount;
}

SPPoint spImageGetFeature(SPImage image, int index)
{
	if (index >= image->featsCount)
	{
		spLoggerPrintError(SP_BAD_ARGUMENT, __FILE__, __func__, __LINE__);
		return NULL;
	}

	return image->feats[index];
}
