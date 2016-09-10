/*
 * SPKDTreeNode.c
 *
 *  Created on: Aug 27, 2016
 *      Author: galkl
 */
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "SPKDTreeNode.h"
#include "SPKDArray.h"
#include "SPPoint.h"
#include "SPBPriorityQueue.h"
#include "SPConfig.h"
#include "SPImage.h"

struct sp_kdtreenode_t {
	int Dim;
	double Val;
	SPKDTreeNode Left;
	SPKDTreeNode Right;
	SPPoint Data;
};

int getSplitDimension(SPConfig config, SPKDArray kdArr, int upperLevelSplitDim)
{
	SP_CONFIG_MSG configMsg;
	SP_KDTREE_SPLIT_METHOD splitMethod;
	int pcaDim = 0;

	//TODO: handle config msg
	splitMethod = spConfigGetspKDTreeSplitMethod(config, &configMsg);
	pcaDim = spConfigGetPCADim(config, &configMsg);

	switch(splitMethod)
	{
		case RANDOM:
		{
			srand(time(NULL));
			return (rand() % pcaDim);
		}
		case MAX_SPREAD:
		{
			return spKDSArrayGetMaxSpreadDimension(kdArr);
		}
		case INCREMENTAL:
		{
			upperLevelSplitDim++;

			return (upperLevelSplitDim % pcaDim);
		}
		default:
		{
			return INVALID_VALUE;
		}
	}
}

SPKDTreeNode createKDTreeFromKDArray(SPKDArray kdArray, int upperLevelSplitDim, SPConfig config)
{
	SPKDTreeNode kdTreeNode = NULL;
	SPKDArray kdArrayLeft = NULL;
	SPKDArray kdArrayRight = NULL;

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
		kdTreeNode->Dim = getSplitDimension(config, kdArray, upperLevelSplitDim);

		if (!spKDArraySplit(kdArray, kdTreeNode->Dim, &kdArrayLeft, &kdArrayRight, &kdTreeNode->Val))
		{
			// TODO: handle
		}

		kdTreeNode->Left = createKDTreeFromKDArray(kdArrayLeft, kdTreeNode->Dim, config);
		if (!kdTreeNode->Left)
		{
			//TODO: handle
		}

		kdTreeNode->Right = createKDTreeFromKDArray(kdArrayRight, kdTreeNode->Dim, config);
		if (!kdTreeNode->Right)
		{
			//TODO: handle
		}

		kdTreeNode->Data = NULL;
	}

	return kdTreeNode;
}

SPKDTreeNode spKDTreeNodeCreate(SPPoint* features, int size, SPConfig config)
{
	SPKDArray kdArray = spKDArrayInit(features, size);
	if (!kdArray)
	{
		// TODO: handle
	}

	return createKDTreeFromKDArray(kdArray, INVALID_VALUE, config);
}

bool SPKDTreeNodeIsLeaf(SPKDTreeNode node)
{
	return (node->Left == NULL && node->Right == NULL) ? true : false;
}

void spKDTreeNodeKNNSearch(SPKDTreeNode curr , SPBPQueue bpq, SPPoint testPoint)
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
		int index = spPointGetIndex(curr->Data);
		double distance = spPointL2SquaredDistance(curr->Data, testPoint);
		elementToAdd = spListElementCreate(index, distance);

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
		spKDTreeNodeKNNSearch(curr->Left, bpq, testPoint);

		lastStep = 0;
	}
	else
	{
		spKDTreeNodeKNNSearch(curr->Right, bpq, testPoint);

		lastStep = 1;
	}

	/* If the candidate hypersphere crosses this splitting plane, look on the
	* other side of the plane by examining the other subtree*/
	if (!spBPQueueIsFull(bpq) ||
			(pow(curr->Val - spPointGetAxisCoor(testPoint, curr->Dim), (double)2) < spBPQueueMaxValue(bpq)))
	{
		//recursively search the other subtree on the next axis
		if (lastStep)
		{
			// If we searched the right subtree - go to the left to fill the missing value
			spKDTreeNodeKNNSearch(curr->Left, bpq, testPoint);
		}
		else
		{
			// The opposite
			spKDTreeNodeKNNSearch(curr->Right, bpq, testPoint);
		}
	}
}

SPKDTreeNode spCreateKDTreeFromImages(SPImage* imagesFeatures, SPConfig config)
{
	SP_CONFIG_MSG configMsg;
	SPKDTreeNode kdTree = NULL;
	int i = 0;
	int j = 0;
	int numOfAllFeatures = 0;
	SPPoint* allFeatures = NULL;
	int currImageNumOfFeatures = 0;
	int numOfImages = spConfigGetNumOfImages(config, &configMsg);
	int copiedFeats = 0;

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
			allFeatures[copiedFeats] = spPointCopy(spImageGetFeature(imagesFeatures[i], j));
			if (!allFeatures[copiedFeats])
			{
				// TODO: handle
			}

			copiedFeats++;
		}
	}

	kdTree = spKDTreeNodeCreate(allFeatures, numOfAllFeatures, config);

	// TODO: free all SPImage shit

	return kdTree;
}
