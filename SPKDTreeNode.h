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

SPKDTreeNode spKDTreeNodeCreate(SPPoint* features, int size, SPConfig config);
void spKDTreeNodeDestroy(SPKDTreeNode kdTreeNode);
SPKDTreeNode spCreateKDTreeFromImages(SPImage* imagesFeatures, SPConfig config);
bool spKDTreeNodeKNNSearch(SPKDTreeNode curr , SPBPQueue bpq, SPPoint testPoint);

#endif /* SPKDTREENODE_H_ */
