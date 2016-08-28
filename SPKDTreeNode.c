/*
 * SPKDTreeNode.c
 *
 *  Created on: Aug 27, 2016
 *      Author: galkl
 */
#include "SPKDTreeNode.h"
#include "SPPoint.h"

struct sp_kdtreenode_t {
	int Dim;
	double Val;
	SPKDTreeNode Left;
	SPKDTreeNode Right;
	SPPoint Data;
};
