/*
 * SPImage.h
 *
 *  Created on: Aug 28, 2016
 *      Author: galkl
 */
#include <stdbool.h>
#include "SPPoint.h"


#ifndef SPIMAGE_H_
#define SPIMAGE_H_

#define FEATS_FILE_WRITE_MODE "wb"
#define FEATS_FILE_READ_MODE "rb"
#define NEW_LINE "\n"

typedef struct sp_image_t *SPImage;


/**
 * Assigning the feats into the image struct
 *
 * @param feats - a data base of points represent the features
 * @param featsCount - int represent the number of features needed to be assigned
 *
 * @return
 * SPImage structure with his fields assigned
 *
 * - NULL - if allocation failure occurred
 * - print to logger file if some error occurred
 *
 */
SPImage spImageCreateFromImg(SPPoint* feats, int featsCount);

/**
 * create SPImage structure from the features in featsFileName
 *
 * @param featsFileName - the file holding the features to read from
 * @param imgIndex - int represent the image index
 *
 * @return
 * SPImage structure with his fields assigned
 *
 * - NULL - if allocation failure occurred
 * - print to logger file if some error occurred
 *
 */
SPImage spImageCreateFromFeats(const char* featsFileName, int imgIndex);

/**
 * return the num of features in an image
 *
 * @param image - the image
 *
 * @return
 * - int represent the num of features in an image
 *
 * - -1  if a bad argument was given
 * - print to logger file if some error occurred
 *
 */
int spImageGetNumOfFeature(SPImage image);

/**
 * return the feature in the index that was given
 *
 * @param image - the image
 * @param index - the index of the feature needed
 * @return
 * - SPPoint represent the features in that index
 *
 * - NULL if a bad argument was given
 * - print to logger file if some error occurred
 *
 */
SPPoint spImageGetFeature(SPImage image, int index);

/**
 * Write the image features to file featsFileName received as argument
 *
 * @param image - the image
 * @param featsFileName - the file holding the features to read from
 *
 * @return
 * - true if the features were written successfully
 *
 * - false - if some error occurred
 * - print to logger file if some error occurred
 *
 */
bool spImageSaveToFeats(SPImage image, const char* featsFileName);

/**
 * Frees all memory resources associate with SPimage.
 * If SPImage == NULL nothig is done.
 */
void spImageDestroy(SPImage image);



#endif /* SPIMAGE_H_ */
