/*
 * main.cpp
 *
 *  Created on: Aug 13, 2016
 *      Author: galkl
 */
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
extern "C"
{
#include "SPPoint.h"
#include "SPKDArray.h"
}

int main(int argc, char** argv)
{
	srand(0);
	SPPoint* pointsArr = (SPPoint*)malloc(5 * sizeof(SPPoint));

	double coords0[2] = {1, 2};
	pointsArr[0] = spPointCreate(coords0, 2, 0);

	double coords1[2] = {123, 70};
	pointsArr[1] = spPointCreate(coords1, 2, 1);

	double coords2[2] = {2, 7};
	pointsArr[2] = spPointCreate(coords2, 2, 2);

	double coords3[2] = {9, 11};
	pointsArr[3] = spPointCreate(coords3, 2, 3);

	double coords4[2] = {3, 4};
	pointsArr[4] = spPointCreate(coords4, 2, 4);

	fflush(NULL);
	SPKDArray test =  spKDArrayInit(pointsArr, 5);
	SPKDSplittedArray test1 = spKDArraySplit(test, 0);
}
