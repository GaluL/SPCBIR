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
#include "SPCommonDefs.h"

struct sp_kdtreenode_t {
	// Stores the split dimension of this tree junction
	int Dim;
	// Stores the split median of the split dimension
	double Val;
	// Stores the left subtree
	SPKDTreeNode Left;
	// Stores the right subtree
	SPKDTreeNode Right;
	// Stores the feature (for a leaf and null otherwise)
	SPPoint Data;
};

int getSplitDimension(SPConfig config, SPKDArray kdArr, int upperLevelSplitDim)
{
	SP_CONFIG_MSG configMsg;
	SP_KDTREE_SPLIT_METHOD splitMethod;
	int pcaDim = 0;

	splitMethod = spConfigGetspKDTreeSplitMethod(config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS)
	{
		spConfigPrintConfigMsgToLogger(configMsg, __FILE__, __func__, __LINE__);
		return SP_INVALID_NEG_VALUE;
	}

	pcaDim = spConfigGetPCADim(config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS)
	{
		spConfigPrintConfigMsgToLogger(configMsg, __FILE__, __func__, __LINE__);
		return SP_INVALID_NEG_VALUE;
	}

	// Returns split dimension according to split method
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
			// Returns dimension which is greater by 1 (mod PCADim) from it's parent split dimension
			upperLevelSplitDim++;

			return (upperLevelSplitDim % pcaDim);
		}
		default:
		{
			return SP_INVALID_NEG_VALUE;
		}
	}
}

void freeChildArrays(SPKDArray kdLeft, SPKDArray kdRight)
{
	spKDArrayDestroy(kdLeft);
	spKDArrayDestroy(kdRight);
}

SPKDTreeNode createKDTreeFromKDArray(SPKDArray kdArray, int upperLevelSplitDim, SPConfig config)
{
	SPKDTreeNode kdTreeNode = NULL;
	SPKDArray kdArrayLeft = NULL;
	SPKDArray kdArrayRight = NULL;

	// Memory allocation for the node to be built
	kdTreeNode = (SPKDTreeNode)malloc(sizeof(*kdTreeNode));
	if (!kdTreeNode)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		return NULL;
	}

	// If base kdArray has one element then tree node should be a leaf
	if (spKDArrayGetSize(kdArray) == 1)
	{
		kdTreeNode->Dim = SP_INVALID_NEG_VALUE;
		kdTreeNode->Val = SP_INVALID_NEG_VALUE;
		kdTreeNode->Left = NULL;
		kdTreeNode->Right = NULL;
		kdTreeNode->Data = spPointCopy(spKDArrayGetPoint(kdArray, 0));
	}
	// If not a leaf
	else
	{
		// Getting the dimension to split according, by config manipulation
		kdTreeNode->Dim = getSplitDimension(config, kdArray, upperLevelSplitDim);
		if (kdTreeNode->Dim == SP_INVALID_NEG_VALUE)
		{
			freeChildArrays(kdArrayLeft, kdArrayRight);
			spKDTreeNodeDestroy(kdTreeNode);
			return NULL;
		}

		// Splitting the kdArray according to split dimension in-order to get left kdArr, right
		// one, and the split median
		if (!spKDArraySplit(kdArray, kdTreeNode->Dim, &kdArrayLeft, &kdArrayRight, &kdTreeNode->Val))
		{
			freeChildArrays(kdArrayLeft, kdArrayRight);
			spKDTreeNodeDestroy(kdTreeNode);
			return NULL;
		}

		// Creating left subtree from left kdArray
		kdTreeNode->Left = createKDTreeFromKDArray(kdArrayLeft, kdTreeNode->Dim, config);
		if (!kdTreeNode->Left)
		{
			freeChildArrays(kdArrayLeft, kdArrayRight);
			spKDTreeNodeDestroy(kdTreeNode);
			return NULL;
		}

		// Creating left subtree from right kdArray
		kdTreeNode->Right = createKDTreeFromKDArray(kdArrayRight, kdTreeNode->Dim, config);
		if (!kdTreeNode->Right)
		{
			freeChildArrays(kdArrayLeft, kdArrayRight);
			spKDTreeNodeDestroy(kdTreeNode);
			return NULL;
		}

		kdTreeNode->Data = NULL;

		freeChildArrays(kdArrayLeft, kdArrayRight);
	}

	return kdTreeNode;
}

SPKDTreeNode spKDTreeNodeCreate(SPPoint* features, int size, SPConfig config)
{
	SPKDTreeNode resultTree = NULL;
	// Initialize a new kdArray based on the features array received
	SPKDArray kdArray = spKDArrayInit(features, size);
	if (!kdArray)
	{
		return NULL;
	}

	// Calling the recursive method to build the tree from the recently created kdArray - the Invlid vlaue passed represting that it's the root of the tree
	// means no previous splitDimension used
	resultTree = createKDTreeFromKDArray(kdArray, SP_INVALID_NEG_VALUE, config);

	spKDArrayDestroy(kdArray);

	return resultTree;
}

void spKDTreeNodeDestroy(SPKDTreeNode kdTreeNode)
{
	if (!kdTreeNode)
	{
		return;
	}

	if (kdTreeNode->Left)
	{
		spKDTreeNodeDestroy(kdTreeNode->Left);
	}

	if (kdTreeNode->Right)
	{
		spKDTreeNodeDestroy(kdTreeNode->Right);
	}

	if (kdTreeNode->Data)
	{
		spPointDestroy(kdTreeNode->Data);
	}

	free(kdTreeNode);
	kdTreeNode = NULL;
}

bool SPKDTreeNodeIsLeaf(SPKDTreeNode node)
{
	// Return true if both right subtree and left subtree are NULL (means node has no subtrees so it's a leaf)
	// and false otherwise
	return (node->Left == NULL && node->Right == NULL) ? true : false;
}

bool spKDTreeNodeKNNSearch(SPKDTreeNode curr , SPBPQueue bpq, SPPoint testPoint)
{
	SPListElement elementToAdd = NULL;
	int lastStep = 0;
	if (curr == NULL)
	{
		return false;
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
			spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
			return false;
		}

		// Adding the feature index and distance to priority queue
		if (spBPQueueEnqueue(bpq, elementToAdd) != SP_BPQUEUE_SUCCESS)
		{
			spListElementDestroy(elementToAdd);
			spLoggerPrintError(SP_QUEUE_ERROR, __FILE__, __func__, __LINE__);
			return false;
		}

		spListElementDestroy(elementToAdd);

		return true;
	}

	/* Recursively search the half of the tree that contains the test point. */
	if(spPointGetAxisCoor(testPoint, curr->Dim) <= curr->Val)
	{
		if (!spKDTreeNodeKNNSearch(curr->Left, bpq, testPoint))
		{
			return false;
		}

		// Remember that we've searched the left subtree
		lastStep = 0;
	}
	else
	{
		if (!spKDTreeNodeKNNSearch(curr->Right, bpq, testPoint))
		{
			return false;
		}

		// Remember that we've searched the right subtree
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
			 if (!spKDTreeNodeKNNSearch(curr->Left, bpq, testPoint))
			 {
				 return false;
			 }
		}
		else
		{
			// The opposite
			if(!spKDTreeNodeKNNSearch(curr->Right, bpq, testPoint))
			{
				return false;
			}
		}
	}

	// If nothing went wrong - return true
	return true;
}

void freeFeaturesArray(SPPoint* featuresArr, int size)
{
	int i = 0;

	if (featuresArr)
	{
		for(i = 0; i < size; ++i)
		{
			if (featuresArr[i])
			{
				spPointDestroy(featuresArr[i]);
			}
		}

		free(featuresArr);
		featuresArr = NULL;
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
	int copiedFeats = 0;
	int numOfImages = 0;

	numOfImages = spConfigGetNumOfImages(config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS)
	{
		spLoggerPrintError(SP_ERROR_READING_CONFIG, __FILE__, __func__, __LINE__);
		return NULL;
	}

	// Calculating the sum of all images features amount
	for (i = 0; i < numOfImages; ++i)
	{
		numOfAllFeatures += spImageGetNumOfFeature(imagesFeatures[i]);
	}

	// Allocating memory for all features array
	allFeatures = (SPPoint*)malloc(numOfAllFeatures * sizeof(SPPoint));
	if (!allFeatures)
	{
		spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);
		return NULL;
	}

	// Iterating over each image
	for (i = 0; i < numOfImages; ++i)
	{
		// Getting current images num of features
		currImageNumOfFeatures = spImageGetNumOfFeature(imagesFeatures[i]);

		// Iterating over each feature
		for (j = 0; j < currImageNumOfFeatures; ++j)
		{
			// Copying the feature to all features array
			allFeatures[copiedFeats] = spPointCopy(spImageGetFeature(imagesFeatures[i], j));
			if (!allFeatures[copiedFeats])
			{
				spLoggerPrintError(SP_ALLOCATION_FAILURE, __FILE__, __func__, __LINE__);

				freeFeaturesArray(allFeatures, numOfAllFeatures);
				return NULL;
			}

			copiedFeats++;
		}
	}

	// Create kdTree based on the features array we've just build
	kdTree = spKDTreeNodeCreate(allFeatures, numOfAllFeatures, config);

	freeFeaturesArray(allFeatures, numOfAllFeatures);

	return kdTree;
}
