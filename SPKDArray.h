/*
 * SPKDArray.h
 *
 *  Created on: Aug 23, 2016
 *      Author: galkl
 */

#ifndef SPKDARRAY_H_
#define SPKDARRAY_H_

#include "SPPoint.h"

/** Type for defining the kdarray */
typedef struct sp_kdarray_t *SPKDArray;

typedef struct sp_kdsplittedarray_t *SPKDSplittedArray;

SPKDArray spKDArrayInit(SPPoint* arr, int size);

SPKDSplittedArray spKDArraySplit(SPKDArray kdArr, int axis);

int spKDArrayGetSize(SPKDArray kdArr);

SPPoint spKDArrayGetPoint(SPKDArray kdArr, int index);

SPKDArray spKDSplittedArrayGetRight(SPKDSplittedArray kdSplittedArray);

SPKDArray spKDSplittedArrayGetLeft(SPKDSplittedArray kdSplittedArray);

double spKDArrayGetSplitMedian(SPKDArray kdArray, int dimension);

#endif /* SPKDARRAY_H_ */
