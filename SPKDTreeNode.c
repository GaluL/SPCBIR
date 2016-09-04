/*
 * SPKDTreeNode.c
 *
 *  Created on: Aug 27, 2016
 *      Author: galkl
 */
#include <stdlib.h>
#include "SPKDTreeNode.h"
#include "SPKDArray.h"
#include "SPPoint.h"

#define INVALID_VALUE -1

struct sp_kdtreenode_t {
	int Dim;
	double Val;
	SPKDTreeNode Left;
	SPKDTreeNode Right;
	SPPoint Data;
};

SPKDTreeNode createKDTreeFromKDArray(SPKDArray kdArray, int splitDimension)
{
	SPKDTreeNode kdTreeNode = NULL;
	SPKDSplittedArray kdSplittedArray = NULL;

	kdTreeNode = (SPKDTreeNode)malloc(sizeof(*kdTreeNode));
	if (!kdTreeNode)
	{
		// TODO: handle
	}

	if (spKDArrayGetSize(kdArray) == 1)
	{
		kdTreeNode->Dim = INVALID_VALUE;
		kdTreeNode->Val = INVALID_VALUE;
		kdTreeNode->Left = NULL;
		kdTreeNode->Right = NULL;
		kdTreeNode->Data = spPointCopy(spKDArrayGetPoint(kdArray, 0));
	}

	kdSplittedArray = spKDArraySplit(kdArray, splitDimension);
	if (!kdSplittedArray)
	{
		// TODO: handle
	}

	kdTreeNode->Dim = splitDimension;
	kdTreeNode->Val = 0; //TODO: should be split median - check what's the meaning
	kdTreeNode->Left = createKDTreeFromKDArray(spKDSplittedArrayGetLeft(kdSplittedArray), splitDimension);
	if (!kdTreeNode->Left)
	{
		//TODO: handle
	}

	kdTreeNode->Right = createKDTreeFromKDArray(spKDSplittedArrayGetRight(kdSplittedArray), splitDimension);
	if (!kdTreeNode->Right)
	{
		//TODO: handle
	}

	kdTreeNode->Data = NULL;

	free(kdSplittedArray);
}

SPKDTreeNode spKDTreeNodeCreate(SPPoint* features, int size, int splitDimension)
{
	SPKDArray kdArray = spKDArrayInit(features, size);
	if (!kdArray)
	{
		// TODO: handle
	}

	createKDTreeFromKDArray(kdArray, splitDimension);
}
