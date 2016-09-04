/*
 * SPKDTreeNode.h
 *
 *  Created on: Aug 27, 2016
 *      Author: galkl
 */
#include "SPPoint.h"


#ifndef SPKDTREENODE_H_
#define SPKDTREENODE_H_

typedef struct sp_kdtreenode_t *SPKDTreeNode;

SPKDTreeNode spKDTreeNodeCreate(SPPoint* features, int size, int splitDimension);

#endif /* SPKDTREENODE_H_ */
