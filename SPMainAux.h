/*
 * SPMainAux.h
 *
 *  Created on: Aug 29, 2016
 *      Author: galkl
 */
#ifndef SPMAINAUX_H_
#define SPMAINAUX_H_

#include "SPImage.h"
#include "SPKDTreeNode.h"
#include "SPCommonDefs.h"
#include "SPLogger.h"
#include "SPConfig.h"

#define MAX_FILE_PATH_LEN 1024
#define ERROR_INVALID_COMAND_LINE "Invalid command line : use -c "
#define ERROR_SPCBIR_NOT_OPEN "The default configuration file spcbir.config couldn't be open\n"
#define ERROR_FEATURES_EXTRACT_FAILURE "Could not extract features for one or more images"
#define ERROR_QUERY_LOAD_FAILURE "Could not load query image, try again...\n"
#define DEBUG_CONFIG_LOAD "Config file loaded successfully"
#define DEBUG_IMGPROC_LOAD "Image Processing library initialized successfully"
#define DEBUG_EXTRACT_MODE "Extracting images features from image files"
#define DEBUG_EXTRACT_RESULT "Images features extracted successfully"
#define DEBUG_FEATS_FILES ".feats files successfully stored"
#define DEBUG_NON_EXTRACTION_MODE "Extracting features from .feats files previously stored"
#define DEBUG_EXTRACT_FROM_FEATS_RES "Images features extracted successfully from previously stored .feats files"
#define DEBUG_TREE_LOAD "KDTree features database successfully created"
#define DEBUG_QUERY_IMAGE_LOAD "Query image features successfully extracted"
#define DEBUG_SIMILIAR_LOADED "Similar images successfully retrieved"
#define MSG_EXIT "Exiting...\n"
#define CONFIG_ARGUMENT_FLAG "-c"
#define DEFAULT_CONFIG_FILE "spcbir.config"
#define QUERY_IMAGE_PROMPT "Please enter query image path\n"
#define TERMINATION_SIGN "<>"
#define BEST_CANDIDATES "Best candidates for -"
#define ARE "- are:\n"
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
 *  - string represent the config file name
 *  - if nothing was entered default will be chosen
 *  -
 *  - error will be printed to stdout if user entered invalid command line
 */
char* spGetConfigFileName(int argc, char** argv);


/*
 * Write the image features to  a file received from the config file.
 *
 * @param imagesFeatures - the images data base to create the tree from.
 * @param config - the config.
 *
 * @return
 * - true if all features were saved correctly
 *
 * - false - if some error occurred
 * - print to logger file if some error occurred
 *
 *
 */
bool spSerializeImagesFeatures(SPImage* imagesFeatures, SPConfig config);

/*
 * reading features from file and assigning them to imagesFeature structure
 *
 * @param imagesFeatures - the images  data base that each the feature read from file will assign to
 * @param config - the config.
 *
 * @return
 * - true if all pictures features were saved correctly
 *
 * - false - if some error occurred
 * - print to logger file if some error occurred
 *
 *
 */
bool spDeserializeImagesFeatures(SPImage** imagesFeatures, SPConfig config);

/*
 * get the path of the most similar images the query image from the images data base
 *
 * @param config - the config.
 * @param queryImage - the query image for it we want to find the similar pictures
 * @param config - imagesDB the kd tree data base were we look for the  similar images
 *
 * @return
 * - A sorted array of string representing the path of the most similar images to the query
 * - in the first place the most similar image path
 *
 * - NULL - if some error occurred
 * - print to logger file if some error occurred
 *
 *
 */
char** spGetSimilarImagesPathes(SPConfig config, SPImage queryImage, SPKDTreeNode imagesDB);
/*
 * print to stdout the received string argument
 */
void flushed_printf(const char* str);
/*
 * print to stdout the received string argument with a new line
 */
void flushed_printf_newline(const char* str);
/*
 * return the received value from stdin
 */
char* flushed_gets();
/*
 * free allocated memory in SimilarImagesPathes
 */
void freeSimilarImagesPathes(char** SimilarImagesPathes,SPConfig config );
/*
 * free allocated memory in imagesFeatures
 */
void freeImagesFeatures(SPImage* imagesFeatures, int numOfImages);
/*
 * free allocated memory in result
 */
void spDestroyResult(char** result,int numOfSimilarImages);

/*
 * free allocated memory in  the received variable.
 *
 */
void destroyVariables(char* configFileName, SPConfig config, SPImage* imagesFeatures,
		char* queryPath, SPKDTreeNode featuresKDTree, SPImage query, char** SimilarImagesPathes);

/*
 * initiate the logger base on the logger file name received in the config
 *
 * @param config - the config.
 * @return
 * - true if the logger was initiated properly
 *
 * - false - if some error occurred
 * - print to stdout  if some error occurred
 *
 *
 */
bool initLoggerFromConfig(SPConfig config);

#endif /* SPMAINAUX_H_ */
