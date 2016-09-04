/*
 * SPMainAux.h
 *
 *  Created on: Aug 29, 2016
 *      Author: galkl
 */
#ifndef SPMAINAUX_H_
#define SPMAINAUX_H_

#include "SPImage.h"
#include "SPConfig.h"

#define MAX_FILE_PATH_LEN 1024

char* spGetConfigFileName(int argc, char** argv);
bool spSerializeImagesFeatures(SPImage* imagesFeatures, SPConfig config);
bool spDeserializeImagesFeatures(SPImage** imagesFeatures, SPConfig config);

#endif /* SPMAINAUX_H_ */
