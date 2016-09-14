/*
 * SPKDArray.h
 *
 *  Created on: Aug 23, 2016
 *      Author: galkl
 */

#ifndef SPKDARRAY_H_
#define SPKDARRAY_H_

#include "SPPoint.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>


#define INVALID_VALUE -1

/** Type for defining the kdarray **/
typedef struct sp_kdarray_t *SPKDArray;

/** Type for defining the the split of kdarray to two arrays, left and right **/
typedef struct sp_kdsplittedarray_t *SPKDSplittedArray;

/**
 * Init a new instance of KDArray
 *
 * @param imagePath - the target imagePath
 * @param index - the index  of the image in the database
 * @param numOfFeats - a pointer in which the actual number of feats extracted
 * 					   will be stored
 * @return
 * An array of the actual features extracted. NULL is returned in case of
 * an error.
 */
SPKDArray spKDArrayInit(SPPoint* arr, int size);

/**
 * Returns an array of features for the image imagePath. All SPPoint elements
 * will have the index given by index. The actual number of features extracted
 * for this image will be stored in the pointer given by numOfFeats.
 *
 * @param imagePath - the target imagePath
 * @param index - the index  of the image in the database
 * @param numOfFeats - a pointer in which the actual number of feats extracted
 * 					   will be stored
 * @return
 * An array of the actual features extracted. NULL is returned in case of
 * an error.
 */
bool spKDArraySplit(SPKDArray kdArr, int axis, SPKDArray* kdLeft, SPKDArray* kdRight, double* splitMedian);
/**
 * Frees all memory resources associate with SPKDArray.
 * If SPKDArray == NULL nothig is done.
 */
void spKDArrayDestroy(SPKDArray kdArr);

/**
 * Returns an array of features for the image imagePath. All SPPoint elements
 * will have the index given by index. The actual number of features extracted
 * for this image will be stored in the pointer given by numOfFeats.
 *
 * @param imagePath - the target imagePath
 * @param index - the index  of the image in the database
 * @param numOfFeats - a pointer in which the actual number of feats extracted
 * 					   will be stored
 * @return
 * An array of the actual features extracted. NULL is returned in case of
 * an error.
 */
int spKDArrayGetSize(SPKDArray kdArr);

/**
 * Returns an array of features for the image imagePath. All SPPoint elements
 * will have the index given by index. The actual number of features extracted
 * for this image will be stored in the pointer given by numOfFeats.
 *
 * @param imagePath - the target imagePath
 * @param index - the index  of the image in the database
 * @param numOfFeats - a pointer in which the actual number of feats extracted
 * 					   will be stored
 * @return
 * An array of the actual features extracted. NULL is returned in case of
 * an error.
 */
SPPoint spKDArrayGetPoint(SPKDArray kdArr, int index);

/**
 * Returns an array of features for the image imagePath. All SPPoint elements
 * will have the index given by index. The actual number of features extracted
 * for this image will be stored in the pointer given by numOfFeats.
 *
 * @param imagePath - the target imagePath
 * @param index - the index  of the image in the database
 * @param numOfFeats - a pointer in which the actual number of feats extracted
 * 					   will be stored
 * @return
 * An array of the actual features extracted. NULL is returned in case of
 * an error.
 */
int spKDSArrayGetMaxSpreadDimension(SPKDArray kdArray);

/**
 * Returns an array of features for the image imagePath. All SPPoint elements
 * will have the index given by index. The actual number of features extracted
 * for this image will be stored in the pointer given by numOfFeats.
 *
 * @param imagePath - the target imagePath
 * @param index - the index  of the image in the database
 * @param numOfFeats - a pointer in which the actual number of feats extracted
 * 					   will be stored
 * @return
 * An array of the actual features extracted. NULL is returned in case of
 * an error.
 */
SPKDArray spKDSplittedArrayGetRight(SPKDSplittedArray kdSplittedArray);

/**
 * Returns an array of features for the image imagePath. All SPPoint elements
 * will have the index given by index. The actual number of features extracted
 * for this image will be stored in the pointer given by numOfFeats.
 *
 * @param imagePath - the target imagePath
 * @param index - the index  of the image in the database
 * @param numOfFeats - a pointer in which the actual number of feats extracted
 * 					   will be stored
 * @return
 * An array of the actual features extracted. NULL is returned in case of
 * an error.
 */
SPKDArray spKDSplittedArrayGetLeft(SPKDSplittedArray kdSplittedArray);

#endif /* SPKDARRAY_H_ */
