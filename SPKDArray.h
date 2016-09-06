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

/** Type for defining the kdarray */
typedef struct sp_kdarray_t *SPKDArray;

typedef struct sp_kdsplittedarray_t *SPKDSplittedArray;

SPKDArray spKDArrayInit(SPPoint* arr, int size);

SPKDSplittedArray spKDArraySplit(SPKDArray kdArr, int axis);

int spKDArrayGetSize(SPKDArray kdArr);

SPPoint spKDArrayGetPoint(SPKDArray kdArr, int index);

double spKDArrayGetSplitMedian(SPKDArray kdArray, int dimension);

int spKDSArrayGetMaxSpreadDimension(SPKDArray kdArray);

SPKDArray spKDSplittedArrayGetRight(SPKDSplittedArray kdSplittedArray);

SPKDArray spKDSplittedArrayGetLeft(SPKDSplittedArray kdSplittedArray);

#endif /* SPKDARRAY_H_ */
