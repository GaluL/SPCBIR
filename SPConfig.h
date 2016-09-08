#ifndef SPCONFIG_H_
#define SPCONFIG_H_

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SPLogger.h"

#define MAX_LINE_LENGTH 1024
#define SIZE_OF_PCAYML 7
#define SIZE_OF_STDOUT 6
#define NUM_OF_CONFIG_VARIABLES 14
#define PCA_DIMENSION_DEFULT 20
#define NUM_OF_FEATURES_DEFULT 100
#define NUM_OF_SIMILAR_IMAGES_DEFULT 1
#define KNN_DEFULT 1
#define LOGGER_LEVEL_DEFULT 3
#define SP_PCA_DIMENSION_CONSTRAINT_LOW 9
#define SP_PCA_DIMENSION_CONSTRAINT_HIGH 29
#define NUM_OF_ERROR_IN_ARRAY 4
#define SIZE_OF_SKIP 4
#define SIZE_OF_SKIP_SKIP_EMPTY_LINE 15
#define ERROR_ARRAY_TYPE_INVALID_CONFIGURATION_LINE 0
#define ERROR_ARRAY_TYPE_CONSTEINT_NOT_MET 1
#define ERROR_ARRAY_TYPE_PARAMTER_NOT_SET 2

#define PCA_FILE_NAME_DEFULT "pca.yml"
#define LOGGER_FILE_NAME_DEFULT "stdout"
#define COMMENT_MARK '#'
#define EQUAL_MARK '='
#define SP_IMAGES_DIRECTORY "spImagesDirectory"
#define SP_IMAGES_PREFIX "spImagesPrefix"
#define SP_IMAGES_SUFFIX "spImagesSuffix"
#define SP_NUM_OF_IMAGES "spNumOfImages"
#define SP_PCA_DIMENSION "spPCADimension"
#define SP_PCA_FILE_NAME "spPCAFilename"
#define SP_NUM_OF_FEATURES "spNumOfFeatures"
#define SP_EXTRACTION_MODE "spExtractionMode"
#define SP_NUM_OF_SIMILAR_IMAGES "spNumOfSimilarImages"
#define SP_KD_TREE_SPLIT_METHOD "spKDTreeSplitMethod"
#define SP_KNN "spKNN"
#define SP_MINMIMAL_GUI "spMinimalGUI"
#define SP_LOGGER_LEVEL "spLoggerLevel"
#define SP_LOGGER_FILE_NAME "spLoggerFilename"
#define ERROR_INVALID_CONFIGURATION_LINE "Invalid configuration line"
#define ERROR_INVALID_VALUE "Invalid value - constraint not met"
#define ERROR_PARAMTER "Parameter"
#define ERROR_IS_NOT_SET "is not set"
#define ERROR_FILE "File:"
#define ERROR_LINE "Line:"
#define ERROR_MASSAGE "Massage:"
#define ERROR_THE_CONFIGURATION_FILE "The configuration file"
#define ERROR_COULD_NOT_OPEN "couldn�t be open"
#define DUMMY "temp"
#define SKIP "skip"
#define SKIP_EMPTY_LINE "skip empty line"
#define SUFFIX_JPEG ".jpeg"
#define SUFFIX_PNG ".png"
#define SUFFIX_BMP ".bmp"
#define SUFFIX_GIF ".gif"
#define filenameofek "est.txt"
#define FEATS_SUFFIX ".feats"
/**
 * A data-structure which is used for configuring the system.
 */


typedef enum sp_config_msg_t {
	SP_CONFIG_MISSING_DIR,
	SP_CONFIG_MISSING_PREFIX,
	SP_CONFIG_MISSING_SUFFIX,
	SP_CONFIG_MISSING_NUM_IMAGES,
	SP_CONFIG_CANNOT_OPEN_FILE,
	SP_CONFIG_ALLOC_FAIL,
	SP_CONFIG_INVALID_INTEGER,
	SP_CONFIG_INVALID_STRING,
	SP_CONFIG_INVALID_ARGUMENT,
	SP_CONFIG_INDEX_OUT_OF_RANGE,
	SP_CONFIG_SUCCESS
} SP_CONFIG_MSG;
typedef enum sp_KDTree_split_method_t
{
	RANDOM,
	MAX_SPREAD,
	INCREMENTAL
} SP_KDTREE_SPLIT_METHOD;

typedef struct sp_config_t* SPConfig;

/**
 * Creates a new system configuration struct. The configuration struct
 * is initialized based on the configuration file given by 'filename'.
 * 
 * @param filename - the name of the configuration file
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return NULL in case an error occurs. Otherwise, a pointer to a struct which
 * 		   contains all system configuration.
 * 
 * The resulting value stored in msg is as follow:
 * - SP_CONFIG_INVALID_ARGUMENT - if filename == NULL
 * - SP_CONFIG_CANNOT_OPEN_FILE - if the configuration file given by filename cannot be open
 * - SP_CONFIG_ALLOC_FAIL - if an allocation failure occurred
 * - SP_CONFIG_INVALID_INTEGER - if a line in the config file contains invalid integer
 * - SP_CONFIG_INVALID_STRING - if a line in the config file contains invalid string
 * - SP_CONFIG_MISSING_DIR - if spImagesDirectory is missing
 * - SP_CONFIG_MISSING_PREFIX - if spImagesPrefix is missing
 * - SP_CONFIG_MISSING_SUFFIX - if spImagesSuffix is missing 
 * - SP_CONFIG_MISSING_NUM_IMAGES - if spNumOfImages is missing
 * - SP_CONFIG_SUCCESS - in case of success
 *
 *
 */

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg);

/*
 * Returns true if spExtractionMode = true, false otherwise.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return true if spExtractionMode = true, false otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns true if spMinimalGUI = true, false otherwise.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return true if spExtractionMode = true, false otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns the number of images set in the configuration file, i.e the value
 * of spNumOfImages.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns the number of features to be extracted. i.e the value
 * of spNumOfFeatures.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg);

/**
 * Returns the dimension of the PCA. i.e the value of spPCADimension.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg);

/**
 * Given an index 'index' the function stores in imagePath the full path of the
 * ith image.
 *
 * For example:
 * Given that the value of:
 *  spImagesDirectory = "./images/"
 *  spImagesPrefix = "img"
 *  spImagesSuffix = ".png"
 *  spNumOfImages = 17
 *  index = 10
 *
 * The functions stores "./images/img10.png" to the address given by imagePath.
 * Thus the address given by imagePath must contain enough space to
 * store the resulting string.
 *
 * @param imagePath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @param index - the index of the image.
 *
 * @return
 * - SP_CONFIG_INVALID_ARGUMENT - if imagePath == NULL or config == NULL
 * - SP_CONFIG_INDEX_OUT_OF_RANGE - if index >= spNumOfImages
 * - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index);

SP_CONFIG_MSG spConfigGetImageFeatsPath(char* imageFeatsPath, const SPConfig config,
		int index);

SP_CONFIG_MSG spConfigGetImageCombPath(char* imagePath, const SPConfig config,
		int index, char* suffix);

/**
 * The function stores in pcaPath the full path of the pca file.
 * For example given the values of:
 *  spImagesDirectory = "./images/"
 *  spPcaFilename = "pca.yml"
 *
 * The functions stores "./images/pca.yml" to the address given by pcaPath.
 * Thus the address given by pcaPath must contain enough space to
 * store the resulting string.
 *
 * @param imagePath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @return
 *  - SP_CONFIG_INVALID_ARGUMENT - if imagePath == NULL or config == NULL
 *  - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config);

/**
 * Frees all memory resources associate with config. 
 * If config == NULL nothig is done.
 */
void spConfigDestroy(SPConfig config);

int spConfigGetNumOfSimilarImage (const SPConfig config,SP_CONFIG_MSG* msg);

SP_KDTREE_SPLIT_METHOD spConfigGetspKDTreeSplitMethod (const SPConfig config, SP_CONFIG_MSG* msg);

int spConfigGetKNN (const SPConfig config, SP_CONFIG_MSG* msg);

int spConfigLoggerLevel (const SPConfig config, SP_CONFIG_MSG* msg);

SP_CONFIG_MSG spConfigGetLoggerFilename(char* loggerFileName, const SPConfig config);


bool spIsDefaultInitiate(const char* filename, SP_CONFIG_MSG* msg,SPConfig config, int line);

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg);
void spRegularErrorPrinter(const char* filename, int line,int ErrorTypeNum, char* paramterName);
int spAssignArgument(SPConfig config, char* variable_name, char* variable_value,SP_CONFIG_MSG* msg, int line,
		const char* filename);


#endif /* SPCONFIG_H_ */
