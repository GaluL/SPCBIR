/*
 * SPMainAux.c
 *
 *  Created on: Aug 29, 2016
 *      Author: galkl
 */
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "SPMainAux.h"

#define CONFIG_ARGUMENT_FLAG "-c"
#define DEFAULT_CONFIG_FILE "spcbir.config"

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
	char* imgPath = NULL;
	char* imgPrefix = NULL;
	char* imagePath = (char*)malloc((MAX_FILE_PATH_LEN + 1) * sizeof(char));

	/*numOfImages = spConfigGetImagePath(c);

	for (i = 0; i < numOfImages; ++i)
	{
		spImageSaveToFeats(imagesFeatures[i], );
	}*/
}
