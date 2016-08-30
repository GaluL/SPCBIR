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

#define FEATS_FILE_WRITE_MODE "wb"
#define FEATS_FILE_READ_MODE "rb"
#define NEW_LINE "\n"

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

/*SPImage spImageCreateFromFeats(const char* featsFileName)
{
	FILE *fp;

	fp = fopen(featsFileName, FEATS_FILE_READ_MODE);
	if (!fp)
	{
		return false;
	}


}*/

/*bool spImageSaveToFeats(SPImage image, const char* featsFileName)
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

	if (!fwrite(fp,"%d\n", image->featsCount))
	{
		return false;
	}

	for(i = 0; i < image->featsCount; ++i)
	{
		currPointDim = spPointGetDimension(image->feats[0]);
		if (!fwrite(fp, "%d ", currPointDim))
		{
			return false;
		}

		for(j = 0; j < currPointDim; ++j)
		{
			if(!fwrite(fp, "%f ", spPointGetAxisCoor(image->feats[i], j)))
			{
				return false;
			}
		}

		if (!fwrite(fp, NEW_LINE,))
		{
			return false;
		}
	}

	fclose(fp);

	return true;
}*/

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
