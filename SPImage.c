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

struct sp_image_t {
	SPPoint* feats;
	int featsCount;
};

SPImage spImageCreateFromImg(SPPoint* feats, int featsCount)
{
	SPImage result = (SPImage)malloc(sizeof(*result));
	if (!result)
	{
		// TODO: handle logging
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
	SPPoint currPoint = NULL;
	int i = 0;
	int j = 0;
	int currPointDim = 0;
	double* currPointCoords = NULL;

	fp = fopen(featsFileName, FEATS_FILE_READ_MODE);
	if (!fp)
	{
		return NULL;
	}

	res = (SPImage)malloc(sizeof(*res));
	if (!res)
	{
		return NULL;
	}

	if (!fscanf(fp, "%d", &(res->featsCount)))
	{
		return NULL;
	}

	res->feats = (SPPoint*)malloc(res->featsCount * sizeof(SPPoint));
	if (!res->feats)
	{
		return NULL;
	}

	for (i = 0; i < res->featsCount; ++i)
	{
		if (!fscanf(fp, "%d", &currPointDim))
		{
			// Handle: memory free
			return NULL;
		}

		currPointCoords = (double*)malloc(currPointDim * sizeof(double));
		if (!currPointCoords)
		{
			// Handle: memory free
			return NULL;
		}

		for (j = 0; j < currPointDim; ++j)
		{
			if (!fscanf(fp, "%lf", &(currPointCoords[j])))
			{
				// Handle: memory free
				return NULL;
			}
		}

		res->feats[i] = spPointCreate(currPointCoords, currPointDim, imgIndex);

		if (currPointCoords)
		{
			free(currPointCoords);
		}
	}

	fclose(fp);

	return res;
}

bool spImageSaveToFeats(SPImage image, const char* featsFileName)
{
	FILE *fp;
	int i = 0;
	int j = 0;
	int currPointDim = 0;

	assert(image);

	fp = fopen(featsFileName, FEATS_FILE_WRITE_MODE);
	if (!fp)
	{
		return false;
	}

	fprintf(fp, "%d\n", image->featsCount);

	for (i = 0; i < image->featsCount; ++i)
	{
		currPointDim = spPointGetDimension(image->feats[i]);

		fprintf(fp, "%d ", currPointDim);

		for (j = 0; j < currPointDim; ++j)
		{
			fprintf(fp, "%f ", spPointGetAxisCoor(image->feats[i], j));
		}

		fprintf(fp, NEW_LINE);
	}

	fclose(fp);

	return true;
}

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
				}
			}

			free(image->feats);
		}

		free(image);
	}
}

int spImageGetNumOfFeature(SPImage image)
{
	if (!image)
	{
		return INVALID_VALUE;
	}

	return image->featsCount;
}

SPPoint spImageGetFeature(SPImage image, int index)
{
	if (index >= image->featsCount)
	{
		return NULL;
	}

	return image->feats[index];
}
