/*
 * SPKDArray.c
 *
 *  Created on: Aug 23, 2016
 *      Author: galkl
 */
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include "SPKDArray.h"
#include "SPHelperMethods.h"

#define MAP_NOT_EXIST -1

struct sp_kdarray_t {
	int size;
	SPPoint* points;
	int** dimsSortedIndexesMat;
};

struct sp_kdsplittedarray_t {
	SPKDArray kdLeft;
	SPKDArray kdRight;
};

typedef struct sp_indexandcoord_t {
	int index;
	double coord;
} SPIndexAndCoord;

int cmpCoords(const void* p1, const void* p2)
{
	const SPIndexAndCoord* point1 = (SPIndexAndCoord*)p1;
	const SPIndexAndCoord* point2 = (SPIndexAndCoord*)p2;

	return point1->coord - point2->coord;
}

void sortPointIndexesByDim(SPPoint* points, int pointsCount,int** indexesMat , int axis)
{
	int i = 0;
	SPIndexAndCoord* dimCoords = (SPIndexAndCoord*)malloc(pointsCount * sizeof(SPIndexAndCoord));

	for (i = 0; i < pointsCount; ++i)
	{
		dimCoords[i].index = i;
		dimCoords[i].coord = spPointGetAxisCoor(points[i], axis);
	}

	qsort(dimCoords, pointsCount, sizeof(SPIndexAndCoord), cmpCoords);

	for (i = 0; i < pointsCount; ++i)
	{
		indexesMat[axis][i] = dimCoords[i].index;
	}

	free(dimCoords);
}

SPKDArray spKDArrayInit(SPPoint* arr, int size)
{
	SPKDArray kdArray = NULL;
	int i = 0;
	int pointsDim = 0;

	if (!arr)
	{
		return NULL;
	}

	kdArray = (SPKDArray)malloc(sizeof(*kdArray));
	if (!kdArray)
	{
		return NULL;
	}

	kdArray->size = size;
	kdArray->points = (SPPoint*)malloc(size * sizeof(SPPoint));
	if (!kdArray->points)
	{
		return NULL;
	}

	if (size > 0)
	{
		pointsDim = spPointGetDimension(arr[0]);
	}

	kdArray->dimsSortedIndexesMat = (int**)malloc(size * sizeof(int*));
	if (!kdArray->dimsSortedIndexesMat)
	{
		return NULL;
	}

	for(i = 0; i < size; ++i)
	{
		kdArray->dimsSortedIndexesMat[i] = (int*)malloc(pointsDim * sizeof(int));
		if (!kdArray->dimsSortedIndexesMat[i])
		{
			// TODO: handle
		}
	}

	for (i = 0; i < size; ++i)
	{
		SPPoint point = spPointCopy(arr[i]);
		if (!point)
		{
			//TODO: Handle failure and destruction of previous allocations
			return NULL;
		}

		kdArray->points[i] = point;
	}

	for (i = 0; i < pointsDim; ++i)
	{
		sortPointIndexesByDim(kdArray->points, kdArray->size, kdArray->dimsSortedIndexesMat, i);
	}

	return kdArray;
}

void fillMatFromParent(SPKDArray kdLeft, SPKDArray kdRight, SPKDArray kdParent,
		int* leftIndexMap, int* rightIndexMap)
{
	int i = 0;
	int j = 0;
	int pointsDim = spPointGetDimension(kdParent->points[0]);
	int leftCurrRowPos = 0;
	int rightCurrRowPos = 0;
	int currIndex = MAP_NOT_EXIST;

	kdLeft->dimsSortedIndexesMat = (int**)malloc(kdLeft->size * sizeof(int*));
	if (!kdLeft->dimsSortedIndexesMat)
	{
		// TODO: handle
	}

	for(i = 0; i < kdLeft->size; ++i)
	{
		kdLeft->dimsSortedIndexesMat[i] = (int*)malloc(pointsDim * sizeof(int));
		if (!kdLeft->dimsSortedIndexesMat[i])
		{
			// TODO: handle
		}
	}

	kdRight->dimsSortedIndexesMat = (int**)malloc(kdRight->size * sizeof(int*));
	if (!kdRight->dimsSortedIndexesMat)
	{
		// TODO: handle
	}

	for(i = 0; i < kdRight->size; ++i)
	{
		kdRight->dimsSortedIndexesMat[i] = (int*)malloc(pointsDim * sizeof(int));
		if (!kdRight->dimsSortedIndexesMat[i])
		{
			// TODO: handle
		}
	}

	for (i = 0; i < pointsDim; ++i)
	{
		for (j = 0; j < kdParent->size; ++j)
		{
			currIndex = kdParent->dimsSortedIndexesMat[i][j];

			if (leftIndexMap[currIndex] != MAP_NOT_EXIST)
			{
				kdLeft->dimsSortedIndexesMat[i][leftCurrRowPos] = leftIndexMap[currIndex];
				leftCurrRowPos++;
			}
			else
			{
				kdRight->dimsSortedIndexesMat[i][rightCurrRowPos] = rightIndexMap[currIndex];
				rightCurrRowPos++;
			}
		}

		leftCurrRowPos = 0;
		rightCurrRowPos = 0;
	}
}

SPKDSplittedArray spKDArraySplit(SPKDArray kdArr, int axis)
{
	SPKDSplittedArray result = NULL;
	SPKDArray kdLeft = NULL;
	SPKDArray kdRight = NULL;
	SPPoint currPoint = NULL;
	int leftIndex = 0;
	int rightIndex = 0;
	int* indexMapToSide = NULL;
	int* indexMapLeft = NULL;
	int* indexMapRight = NULL;
	int i = 0;
	int pointIndex = MAP_NOT_EXIST;

	assert(kdArr);

	kdLeft = (SPKDArray)malloc(sizeof(*kdLeft));
	if (!kdLeft)
	{
		//TODO: handle
	}

	kdRight = (SPKDArray)malloc(sizeof(*kdLeft));
	if (!kdRight)
	{
		//TODO: handle
	}

	kdLeft->size = (int)ceil(kdArr->size / 2.0);
	kdLeft->points = (SPPoint*)malloc(kdLeft->size * sizeof(SPPoint));
	if (!kdLeft->points)
	{
		//TODO: handle
	}

	kdRight->size = kdArr->size - kdLeft->size;
	kdRight->points = ((SPPoint*)malloc(kdRight->size * sizeof(SPPoint)));
	if (!kdRight->points)
	{
		//TODO: handle
	}

	indexMapToSide = (int*)malloc(kdArr->size * sizeof(int));
	if (!indexMapToSide)
	{
		//TODO: handle
	}

	indexMapLeft = (int*)malloc(kdArr->size * sizeof(int));
	if (!indexMapLeft)
	{
		//TODO: handle
	}

	indexMapRight = (int*)malloc(kdArr->size * sizeof(int));
	if (!indexMapRight)
	{
		//TODO: handle
	}

	for (i = 0; i < kdArr->size; ++i)
	{
		indexMapLeft[i] = MAP_NOT_EXIST;
		indexMapRight[i] = MAP_NOT_EXIST;
	}

	for (i = 0; i < kdArr->size; ++i)
	{
		pointIndex = kdArr->dimsSortedIndexesMat[axis][i];

		indexMapToSide[pointIndex] = (i < kdLeft->size) ? 0 : 1;
	}

	for (i = 0; i < kdArr->size; ++i)
	{
		currPoint = spPointCopy(kdArr->points[i]);
		if (!currPoint)
		{
			//TODO: handle
		}

		if (indexMapToSide[i] == 0)
		{
			kdLeft->points[leftIndex] = currPoint;
			indexMapLeft[i] = leftIndex;
			leftIndex++;
		}
		else
		{
			kdRight->points[rightIndex] = currPoint;
			indexMapRight[i] = rightIndex;
			rightIndex++;
		}
	}

	free(indexMapToSide);

	fillMatFromParent(kdLeft, kdRight, kdArr, indexMapLeft, indexMapRight);

	free(indexMapLeft);
	free(indexMapRight);

	result = (SPKDSplittedArray)malloc(sizeof(*result));
	if (!result)
	{
		//TODO: handle
	}

	result->kdLeft = kdLeft;
	result->kdRight = kdRight;

	return result;
}
