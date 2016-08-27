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

SPKDArray Init(SPPoint* arr, int size);

SPKDSplittedArray Split(SPKDArray kdArr, int axis);

#endif /* SPKDARRAY_H_ */
