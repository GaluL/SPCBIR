/*
 * SPKDTreeNode.h
 *
 *  Created on: Aug 27, 2016
 *      Author: galkl
 */
#include "SPPoint.h"
#include "SPImage.h"
#include "SPConfig.h"
#include "SPBPriorityQueue.h"


#ifndef SPKDTREENODE_H_
#define SPKDTREENODE_H_

typedef struct sp_kdtreenode_t *SPKDTreeNode;

/*
 * Initialize a new kdTree based on the features array received as an argument
 *
 * @param features - the features data base to create the tree from.
 * @param int - the size of the  features array
 * @param config - the config.
 *
 * @return
* - SPKDTreeNode represent who is the root of the KDtree
 *
 * -  NULL if something went wrong
 *
 */
SPKDTreeNode spKDTreeNodeCreate(SPPoint* features, int size, SPConfig config);

/**
 * Frees all memory resources associate with SPKDTreeNode.
 * If SPKDTreeNode == NULL nothig is done.
 */
void spKDTreeNodeDestroy(SPKDTreeNode kdTreeNode);

/*
 * create kdTree based on array of images  received as an argument
 *
 * @param imagesFeatures - the images data base to create the tree from.
 * @param config - the config.
 *
 * @return
 * - SPKDTreeNode represent who is the root of the KDtree
 *
 * - NULL - if some error occurred
 * - print to logger file if some error occurred
 *
 *
 */
SPKDTreeNode spCreateKDTreeFromImages(SPImage* imagesFeatures, SPConfig config);

/*
 * Assign to the SPBPQueue received as argument the k nearest neighbors to the SPPoint received
 *
 * @param curr - A SPKDTreeNode represent a root of a kdtree.
 * @param bpq - a priority queue to hold the k nearest neighbors
 * @param testPoint - the point to find her k nearest neighbors
 *
 * @return
 * - true if the process finished with no errors
 *
 * - false - if some error occurred
 *  		 print to logger file if some error occurred
 *
 *
 */
bool spKDTreeNodeKNNSearch(SPKDTreeNode curr , SPBPQueue bpq, SPPoint testPoint);

#endif /* SPKDTREENODE_H_ */
