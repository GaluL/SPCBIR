/*
 * SPKDArray.c
 *
 *  Created on: Aug 23, 2016
 *      Author: galkl
 */
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include "SPKDArray.h"
#include "SPLogger.h"
#include "SPCommonDefs.h"

#define MAP_NOT_EXIST -1

struct sp_kdarray_t {
	// The number of features in the array
	int size;

	// Features array
	SPPoint* points;

	// A matrix of size "size" x features dimension, storing the indexes of features in "point"
	// Sorted in each row by the coord of the points in that axis
	int** dimsSortedIndexesMat;
};

struct sp_kdsplittedarray_t {
	SPKDArray kdLeft;
	SPKDArray kdRight;
};

// Holder for specific axis coord and the point index in point array - used for sorting
// matrix rows and keeping the orignial indexes
typedef struct sp_indexandcoord_t {
	int index;
	double coord;
} SPIndexAndCoord;

/**
 * SPIndexAndCoord compare function - Ascending order (according to coord field)
 */
int cmpCoords(const void* p1, const void* p2)
{
	const SPIndexAndCoord* point1 = (SPIndexAndCoord*)p1;
	const SPIndexAndCoord* point2 = (SPIndexAndCoord*)p2;

	return point1->coord - point2->coord;
}

bool sortPointIndexesByDim(SPKDArray kdArr , int axis)
{
	int i = 0;
	SPIndexAndCoord* dimCoords = (SPIndexAndCoord*)malloc(kdArr->size * sizeof(SPIndexAndCoord));

	// Check if alloaction succeeded - if not print log and return false to indicate failure
	if (!dimCoords)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		return false;
	}

	// Iterate over the point of the KDArray and copy each array index and the coord of the
	// requested axis to the IndexAndCoords array
	for (i = 0; i < kdArr->size; ++i)
	{
		dimCoords[i].index = i;
		dimCoords[i].coord = spPointGetAxisCoor(kdArr->points[i], axis);
	}

	// Sorting the indexAndCoords array by the coord field in ascending order
	qsort(dimCoords, kdArr->size, sizeof(SPIndexAndCoord), cmpCoords);

	// Copy the indexes of the sorted array to the matching row in dimSortedIndexesMat
	// to represent the points of the array indexes sorted by the requested axis
	for (i = 0; i < kdArr->size; ++i)
	{
		kdArr->dimsSortedIndexesMat[i][axis] = dimCoords[i].index;
	}

	free(dimCoords);

	// If nothing went wrong - return true
	return true;
}

SPKDArray spKDArrayInit(SPPoint* arr, int size)
{
	SPKDArray kdArray = NULL;
	int i = 0;
	int pointsDim = 0;

	// Validity checking for array argument
	if (!arr || size < 0)
	{
		spLoggerPrintError(SP_BAD_ARGUMENT, __FILE__, __func__, __LINE__);
		return NULL;
	}

	kdArray = (SPKDArray)malloc(sizeof(*kdArray));
	if (!kdArray)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		return NULL;
	}

	kdArray->size = size;

	// Allocating KDArray points array
	kdArray->points = (SPPoint*)malloc(size * sizeof(SPPoint));
	if (!kdArray->points)
	{
		spLoggerPrintError(SP_BAD_ARGUMENT, __FILE__, __func__, __LINE__);

		spKDArrayDestroy(kdArray);
		return NULL;
	}

	// TODO: check if change to spPCADim is needed
	if (size > 0)
	{
		pointsDim = spPointGetDimension(arr[0]);
	}

	// Allocating indexes matrix
	kdArray->dimsSortedIndexesMat = (int**)malloc(size * sizeof(int*));
	if (!kdArray->dimsSortedIndexesMat)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);

		spKDArrayDestroy(kdArray);
		return NULL;
	}

	for(i = 0; i < size; ++i)
	{
		kdArray->dimsSortedIndexesMat[i] = (int*)malloc(pointsDim * sizeof(int));
		if (!kdArray->dimsSortedIndexesMat[i])
		{
			spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);

			spKDArrayDestroy(kdArray);
			return NULL;
		}
	}

	// Copying the points passed as array argument to the KDArray points array field
	for (i = 0; i < size; ++i)
	{
		SPPoint point = spPointCopy(arr[i]);
		if (!point)
		{
			spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);

			spKDArrayDestroy(kdArray);
			return NULL;
		}

		kdArray->points[i] = point;
	}

	// Sorting each dimension indexes
	for (i = 0; i < pointsDim; ++i)
	{
		 if (!sortPointIndexesByDim(kdArray, i))
		 {
			 // If failed sorting and placing indexes in the matrix, destroy the object
			 // Error print is handled by sortPointIndexesByDim function
			 spKDArrayDestroy(kdArray);
			 return NULL;
		 }
	}

	return kdArray;
}

bool fillMatFromParent(SPKDArray kdLeft, SPKDArray kdRight, SPKDArray kdParent,
		int* leftIndexMap, int* rightIndexMap)
{
	int i = 0;
	int j = 0;
	int leftCurrRowPos = 0;
	int rightCurrRowPos = 0;
	int currIndex = MAP_NOT_EXIST;
	//TODO: check if has to be spPCADim
	int pointsDim = spPointGetDimension(kdParent->points[0]);

	// Memory allocation of left kdArray index matrix
	kdLeft->dimsSortedIndexesMat = (int**)malloc(kdLeft->size * sizeof(int*));
	if (!kdLeft->dimsSortedIndexesMat)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);

		return false;
	}

	for(i = 0; i < kdLeft->size; ++i)
	{
		kdLeft->dimsSortedIndexesMat[i] = (int*)malloc(pointsDim * sizeof(int));
		if (!kdLeft->dimsSortedIndexesMat[i])
		{
			spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);

			return false;
		}
	}

	// Memory allocation of right kdArray index matrix
	kdRight->dimsSortedIndexesMat = (int**)malloc(kdRight->size * sizeof(int*));
	if (!kdRight->dimsSortedIndexesMat)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);

		return false;
	}

	for(i = 0; i < kdRight->size; ++i)
	{
		kdRight->dimsSortedIndexesMat[i] = (int*)malloc(pointsDim * sizeof(int));
		if (!kdRight->dimsSortedIndexesMat[i])
		{
			spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);

			return false;
		}
	}

	// Iterating over each of the dimensions
	for (i = 0; i < pointsDim; ++i)
	{
		// Foreach dimension iterating over the parent array sorted indexes for the dim
		for (j = 0; j < kdParent->size; ++j)
		{
			currIndex = kdParent->dimsSortedIndexesMat[j][i];

			// If current index is mapped to left child array
			if (leftIndexMap[currIndex] != MAP_NOT_EXIST)
			{
				// Placing the "translated" index in the left matrix
				kdLeft->dimsSortedIndexesMat[leftCurrRowPos][i] = leftIndexMap[currIndex];
				leftCurrRowPos++;
			}
			// Mapped to right child array
			else
			{
				// Placing the "translated" index in the right matrix
				kdRight->dimsSortedIndexesMat[rightCurrRowPos][i] = rightIndexMap[currIndex];
				rightCurrRowPos++;
			}
		}

		leftCurrRowPos = 0;
		rightCurrRowPos = 0;
	}

	return true;
}

/**
 * Frees all passed indexes map arrays if allocated
 */
void freeIndexMaps(int* indexMapToSide, int* indexMapLeft, int* indexMapRight)
{
	if (indexMapRight)
	{
		free(indexMapRight);
	}

	if (indexMapLeft)
	{
		free(indexMapLeft);
	}

	if (indexMapToSide)
	{
		free(indexMapToSide);
	}
}

bool spKDArraySplit(SPKDArray kdArr, int axis, SPKDArray* kdLeft, SPKDArray* kdRight, double* splitMedian)
{
	SPPoint currPoint = NULL;
	int leftIndex = 0;
	int rightIndex = 0;
	int* indexMapToSide = NULL;
	int* indexMapLeft = NULL;
	int* indexMapRight = NULL;
	int i = 0;
	int pointIndex = MAP_NOT_EXIST;
	bool matrixCopySuccess = true;

	// Checking for argument validity
	if (!kdArr || !kdLeft || !kdRight)
	{
		spLoggerPrintError(SP_BAD_ARGUMENT, __FILE__, __func__, __LINE__);
		return false;
	}

	// Allocating memory for left kdArray
	*kdLeft = (SPKDArray)malloc(sizeof(**kdLeft));
	if (!kdLeft)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		return false;
	}

	// Allocating memory for right kdArray
	*kdRight = (SPKDArray)malloc(sizeof(**kdRight));
	if (!kdRight)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		return false;
	}

	// Splitting the kdArray by two equal size arrays if size is even, if odd - left side will
	// be greater by one element
	(*kdLeft)->size = (int)ceil(kdArr->size / 2.0);
	(*kdLeft)->points = (SPPoint*)malloc((*kdLeft)->size * sizeof(SPPoint));
	if (!(*kdLeft)->points)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		return false;
	}

	// Right size will be the size of the parent array minus the size of the left (equal to left
	// or smaller by one element)
	(*kdRight)->size = kdArr->size - (*kdLeft)->size;
	(*kdRight)->points = ((SPPoint*)malloc((*kdRight)->size * sizeof(SPPoint)));
	if (!(*kdRight)->points)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		return false;
	}

	// Allocating memory for indexMapToSide which maps feature index to side - 0 for left,
	// 1 for right
	indexMapToSide = (int*)malloc(kdArr->size * sizeof(int));
	if (!indexMapToSide)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		return false;
	}

	// Allocating memory for indexMapLeft which maps index in the parent array to index in left array
	indexMapLeft = (int*)malloc(kdArr->size * sizeof(int));
	if (!indexMapLeft)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);

		freeIndexMaps(indexMapToSide, indexMapLeft, indexMapRight);
		return false;
	}

	// Allocating memory for indexMapLeft which maps index in the parent array to index in left array
	indexMapRight = (int*)malloc(kdArr->size * sizeof(int));
	if (!indexMapRight)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);

		freeIndexMaps(indexMapToSide, indexMapLeft, indexMapRight);
		return false;
	}

	// Init maps to non-exsiting values aka -1
	for (i = 0; i < kdArr->size; ++i)
	{
		indexMapLeft[i] = MAP_NOT_EXIST;
		indexMapRight[i] = MAP_NOT_EXIST;
	}

	// Iterating over the row of the split dimension in the matrix
	for (i = 0; i < kdArr->size; ++i)
	{
		// Getting current point index
		pointIndex = kdArr->dimsSortedIndexesMat[i][axis];

		// Till reaching the median - mapping indexes to left array, after that to the right
		indexMapToSide[pointIndex] = (i < (*kdLeft)->size) ? 0 : 1;

		// If current index is the split median (last element of left array) storing the coord
		// of the point which owns this index of the split dimension in the passed argument
		if (i == (*kdLeft)->size - 1 && splitMedian)
		{
			*splitMedian = spPointGetAxisCoor(kdArr->points[pointIndex], axis);
		}
	}

	// Creating the left and right KDArrays
	for (i = 0; i < kdArr->size; ++i)
	{
		// Copying each point of the parent array
		currPoint = spPointCopy(kdArr->points[i]);
		if (!currPoint)
		{
			spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);

			freeIndexMaps(indexMapToSide, indexMapLeft, indexMapRight);
			return false;
		}

		// Selecting in which "child" array to place the copied point (left/right)
		if (indexMapToSide[i] == 0)
		{
			(*kdLeft)->points[leftIndex] = currPoint;
			// Mapping the parent index to the left index the point received
			indexMapLeft[i] = leftIndex;
			leftIndex++;
		}
		else
		{
			(*kdRight)->points[rightIndex] = currPoint;
			// Mapping the parent index to the right index the point received
			indexMapRight[i] = rightIndex;
			rightIndex++;
		}
	}

	// Filling the left and right index matrixes according to parent matrix (in O(n * d))
	matrixCopySuccess = fillMatFromParent(*kdLeft, *kdRight, kdArr, indexMapLeft, indexMapRight);

	// Free all index maps arrays
	freeIndexMaps(indexMapToSide, indexMapLeft, indexMapRight);

	return matrixCopySuccess;
}

void spKDArrayDestroy(SPKDArray kdArr)
{
	int i = 0;
	if (!kdArr)
	{
		return;
	}
	// Frees all the points of the array if allocated
	if (kdArr->points)
	{
		for (i = 0; i < kdArr->size; ++i)
		{
			if (kdArr->points[i])
			{
				spPointDestroy(kdArr->points[i]);
			}
		}

		// Frees the array point itself
		free(kdArr->points);
	}

	// Frees the index matrix if exists
	if (kdArr->dimsSortedIndexesMat)
	{
		for (i = 0; i < kdArr->size; ++i)
		{
			if (kdArr->dimsSortedIndexesMat[i])
			{
				free(kdArr->dimsSortedIndexesMat[i]);
			}
		}

		free(kdArr->dimsSortedIndexesMat);
	}

	// Frees the array object
	free(kdArr);
}

int spKDArrayGetSize(SPKDArray kdArr)
{
	if (kdArr == NULL)
	{
		return INVALID_VALUE;
	}

	return kdArr->size;
}

SPPoint spKDArrayGetPoint(SPKDArray kdArr, int index)
{
	if (kdArr == NULL || index >= kdArr->size)
	{
		return NULL;
	}

	return kdArr->points[index];
}

SPKDArray spKDSplittedArrayGetRight(SPKDSplittedArray kdSplittedArray)
{
	if (!kdSplittedArray)
	{
		return NULL;
	}

	return kdSplittedArray->kdRight;
}

SPKDArray spKDSplittedArrayGetLeft(SPKDSplittedArray kdSplittedArray)
{
	if (!kdSplittedArray)
	{
		return NULL;
	}

	return kdSplittedArray->kdLeft;
}

int spKDSArrayGetMaxSpreadDimension(SPKDArray kdArray)
{
	int i = 0;
	int maxSpreadDim = 0;
	double maxSpread = 0;
	double currDimSpread = 0;

	// Checking arguments validity
	if (!kdArray || !kdArray->points || !kdArray->dimsSortedIndexesMat || kdArray->size == 0)
	{
		return INVALID_VALUE;
	}

	// Iterating over each of the dimensions of the kdArray
	// TODO: check if should be spPCADim
	for (i = 0; i < spPointGetDimension(kdArray->points[0]); ++i)
	{
		// Getting the max coord value of the i'th axis and the min one, calculating the spread
		// which is the distance between them
		currDimSpread = spPointGetAxisCoor(kdArray->points[kdArray->dimsSortedIndexesMat[kdArray->size - 1][i]], i) -
				spPointGetAxisCoor(kdArray->points[kdArray->dimsSortedIndexesMat[0][i]], i);

		// if the current dimension spread is bigger than the one already found store this
		// Dim as the max spread dim and it's spread as the max spread
		if (currDimSpread > maxSpread)
		{
			maxSpread = currDimSpread;
			maxSpreadDim = i;
		}
	}

	return maxSpreadDim;
}
