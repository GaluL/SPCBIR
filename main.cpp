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
}
/*
SPImage* extractImagesFeatures(const SPConfig config, sp::ImageProc imgProc, int numOfImages)
{
	SPImage* imagesFeatures = NULL;
	SPPoint* features = NULL;
	SP_CONFIG_MSG configMsg;
	int i = 0;
	int featuresExtracted = 0;

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
		features = imgProc.getImageFeatures(imagePath, i, &featuresExtracted);
		imagesFeatures[i] = spImageCreateFromImg(features, featuresExtracted);
		if (!imagesFeatures)
		{
			// TODO: handle
			return NULL;
		}
	}

	return imagesFeatures;
}
*/
int main(int argc, char** argv)
{
	SPConfig config = NULL;
	SP_CONFIG_MSG configMsg;
	char* defaultFile;
/*	char* configFileName = spGetConfigFileName(argc, argv);
	sp::ImageProc imgProc = NULL;
	int numOfImages = 0;
	SPImage* imagesFeatures = NULL;
	SPLogger logger = NULL;

//	if (!configFileName)
//	{
		// TODO: handle
//		return 0;
//	}
 *
 */
	defaultFile = (char*)malloc((strlen(filenameofek) + 1) * sizeof(char));
	strcpy(defaultFile,filenameofek);
	config = spConfigCreate(defaultFile, &configMsg);
	if (config)
	{
		printf("success");
	}
	/*
	char* input;
	char* temp_variable;
	char* variable_name = NULL;
	char* variable_value = NULL;
	temp_variable = (char*)malloc((strlen(input) + 1) * sizeof(char));
	strcpy(temp_variable,input);
	variable_name = spCleaningString(temp_variable,msg,filename,line_counter);
	*/
	/*
	 *
		//TODO: wait for ofek's implementation
		//logger = spLoggerCreate(, )
		imgProc = sp::ImageProc(config);
		// TODO: handle failures
		numOfImages = spConfigGetNumOfImages(config, &configMsg);

		imagesFeatures = extractImagesFeatures(config, imgProc, numOfImages);
		if (!imagesFeatures)
		{
			//TODO: handle
		}

		if (!spSerializeImagesFeatures(imagesFeatures, config))
		{

		}
	}
*/
	return 0;
}
