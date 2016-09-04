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
#include "SPBPriorityQueue.h"
#include "SPConfig.h"
#include "SPImage.h"

#define INVALID_VALUE -1

struct sp_kdtreenode_t {
	int Dim;
	double Val;
	SPKDTreeNode Left;
	SPKDTreeNode Right;
	SPPoint Data;
};

int getSplitDimension(SPConfig config, int upperLevelSplitDim)
{
	SP_CONFIG_MSG configMsg;
	SP_KDTREE_SPLIT_METHOD splitMethod;

	//TODO: handle config msg
	splitMethod = spConfigGetspKDTreeSplitMethod(config, &configMsg);

	switch(splitMethod)
	{
		case RANDOM:
		{
			break;
		}
		case MAX_SPREAD:
		{
			break;
		}
		case INCREMENTAL:
		{
			return upperLevelSplitDim++;
		}
	}
}

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
	else
	{
		kdSplittedArray = spKDArraySplit(kdArray, splitDimension);
		if (!kdSplittedArray)
		{
			// TODO: handle
		}

		kdTreeNode->Dim = splitDimension;
		kdTreeNode->Val = spKDArrayGetSplitMedian(kdArray, splitDimension);
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

	return kdTreeNode;
}

SPKDTreeNode spKDTreeNodeCreate(SPPoint* features, int size, int splitDimension)
{
	SPKDArray kdArray = spKDArrayInit(features, size);
	if (!kdArray)
	{
		// TODO: handle
	}

	return createKDTreeFromKDArray(kdArray, splitDimension);
}

bool SPKDTreeNodeIsLeaf(SPKDTreeNode node)
{
	return (node->Left == NULL && node->Right == NULL) ? true : false;
}

void KNNSearch(SPKDTreeNode curr , SPBPQueue bpq, SPPoint testPoint)
{
	SPListElement elementToAdd = NULL;
	int lastStep = 0;
	if (curr == NULL)
	{
		return;
	}

	/* Add the current point to the BPQ. Note that this is a no-op if the
	* point is not as good as the points we've seen so far.*/
	if (SPKDTreeNodeIsLeaf(curr))
	{
		elementToAdd = spListElementCreate(
				spPointGetIndex(curr->Data), spPointL2SquaredDistance(curr->Data, testPoint));

		if (!elementToAdd)
		{
			// TODO: handle
		}

		spBPQueueEnqueue(bpq, elementToAdd);

		return;
	}

	/* Recursively search the half of the tree that contains the test point. */
	if(spPointGetAxisCoor(testPoint, curr->Dim) <= curr->Val)
	{
		KNNSearch(curr->Left, bpq, testPoint);

		lastStep = 0;
	}
	else
	{
		KNNSearch(curr->Right, bpq, testPoint);

		lastStep = 1;
	}

	/* If the candidate hypersphere crosses this splitting plane, look on the
	* other side of the plane by examining the other subtree*/
	if (!spBPQueueIsFull(bpq) ||
			(pow(curr->Val - spPointGetAxisCoor(testPoint, curr->Dim), 2) < spBPQueueMaxValue(bpq)))
	{
		//recursively search the other subtree on the next axis
		if (lastStep)
		{
			// If we searched the left subtree - go to the right to fill the missing value
			KNNSearch(curr->Right, bpq, testPoint);
		}
		else
		{
			// The opposite
			KNNSearch(curr->Left, bpq, testPoint);
		}
	}
}

SPKDTreeNode spCreateKDTreeFromImages(SPImage* imagesFeatures, SPConfig config, int splitDimension)
{
	SP_CONFIG_MSG configMsg;
	SPKDTreeNode kdTree = NULL;
	int i = 0;
	int j = 0;
	int numOfAllFeatures = 0;
	SPPoint* allFeatures = NULL;
	int currImageNumOfFeatures = 0;
	int numOfImages = spConfigGetNumOfImages(config, &configMsg);

	for (i = 0; i < numOfImages; ++i)
	{
		numOfAllFeatures += spImageGetNumOfFeature(imagesFeatures[i]);
	}

	allFeatures = (SPPoint*)malloc(numOfAllFeatures * sizeof(SPPoint));
	if (!allFeatures)
	{
		return NULL;
	}

	for (i = 0; i < numOfImages; ++i)
	{
		currImageNumOfFeatures = spImageGetNumOfFeature(imagesFeatures[i]);

		for (j = 0; j < currImageNumOfFeatures; ++j)
		{
			allFeatures[i + j] = spPointCopy(spImageGetFeature(imagesFeatures[i], j));
			if (!allFeatures[i + j])
			{
				// TODO: handle
			}
		}
	}

	kdTree = spKDTreeNodeCreate(allFeatures, numOfAllFeatures, splitDimension);

	// TODO: free all SPImage shit

	return kdTree;
}
