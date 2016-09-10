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

#define MAX_FILE_PATH_LEN 1024
#define ERROR_INVALID_COMAND_LINE "Invalid command line : use -c <config_filename>\n"
#define ERROR_THE_CONFIGURATION "The configuration file "
//#define ERROR_COULD_NOT_OPEN "couldn’t be open\n"
#define ERROR_SPCBIR_NOT_OPEN "The default configuration file spcbir.config couldn’t be open\n"
#define MSG_EXIT "Exiting...\n"
#define CONFIG_ARGUMENT_FLAG "-c"
#define DEFAULT_CONFIG_FILE "spcbir.config"
#define QUERY_IMAGE_PROMPT "Please enter query image path\n"
#define TERMINATION_SIGN "<>"
#define BEST_CANDIDATES "Best candidates for - "
#define ARE " are:\n"

#define MAX_INPUT_LENGTH 1024

char* spGetConfigFileName(int argc, char** argv);
bool spSerializeImagesFeatures(SPImage* imagesFeatures, SPConfig config);
bool spDeserializeImagesFeatures(SPImage** imagesFeatures, SPConfig config);
char** spGetSimilarImagesPathes(SPConfig config, SPImage queryImage, SPKDTreeNode imagesDB);
void flushed_printf(const char* str);
void flushed_printf_newline(const char* str);
char* flushed_gets();
void freeSimilarImagesPathes(char** SimilarImagesPathes,SPConfig config );

#endif /* SPMAINAUX_H_ */
