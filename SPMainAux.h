/*
 * SPMainAux.h
 *
 *  Created on: Aug 29, 2016
 *      Author: galkl
 */
#ifndef SPMAINAUX_H_
#define SPMAINAUX_H_

#include "SPImage.h"
#include "SPConfig.h"
#include "SPKDTreeNode.h"
#include "SPCommonDefs.h"
#include "SPLogger.h"

#define MAX_FILE_PATH_LEN 1024
#define ERROR_INVALID_COMAND_LINE "Invalid command line : use -c "
#define ERROR_THE_CONFIGURATION "The configuration file"
//#define ERROR_COULD_NOT_OPEN "couldn�t be open\n"
#define ERROR_SPCBIR_NOT_OPEN "The default configuration file spcbir.config couldn�t be open\n"
#define MSG_EXIT "Exiting...\n"
#define CONFIG_ARGUMENT_FLAG "-c"
#define DEFAULT_CONFIG_FILE "spcbir.config"
#define QUERY_IMAGE_PROMPT "Please enter query image path\n"
#define TERMINATION_SIGN "<>"
#define BEST_CANDIDATES "Best candidates for -"
#define ARE " are:\n"
#define INVALID_COMAND_LINE "Invalid command line"

#define MAX_INPUT_LENGTH 1024


/**
 * The function receive the config file name from the user, or  assign default nothing received
 *
 *
 *
 * @param argv
 * @param argc
 * @return
 *  - string represent the configfile name
 *  -
 *  - SP_CONFIG_ALLOC_FAIL - if allocation failure occurred
 *  - SP_CONFIG_SUCCESS - in case of success
 */
char* spGetConfigFileName(int argc, char** argv);



bool spSerializeImagesFeatures(SPImage* imagesFeatures, SPConfig config);
bool spDeserializeImagesFeatures(SPImage** imagesFeatures, SPConfig config);
char** spGetSimilarImagesPathes(SPConfig config, SPImage queryImage, SPKDTreeNode imagesDB);
void flushed_printf(const char* str);
void flushed_printf_newline(const char* str);
char* flushed_gets();
void freeSimilarImagesPathes(char** SimilarImagesPathes,SPConfig config );
void freeImagesFeatures(SPImage* imagesFeatures, int numOfImages);
void spDestroyResult(char** result,int numOfSimilarImages);
void setMyLoggerLevel(int level, SP_LOGGER_LEVEL* loggerLevel);
void destroyVariables(char* configFileName, SPConfig config, SPImage* imagesFeatures,
		char* queryPath, SPKDTreeNode featuresKDTree, SPImage query, char** SimilarImagesPathes);
bool initLoggerFromConfig(SPConfig config);

#endif /* SPMAINAUX_H_ */
