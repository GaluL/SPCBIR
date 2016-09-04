/*
 * SPImage.h
 *
 *  Created on: Aug 28, 2016
 *      Author: galkl
 */
#include <stdbool.h>
#include "SPPoint.h"


#ifndef SPIMAGE_H_
#define SPIMAGE_H_

#define FEATS_FILE_WRITE_MODE "wb"
#define FEATS_FILE_READ_MODE "rb"
#define NEW_LINE "\n"
#define INVALID_VALUE "\n"

typedef struct sp_image_t *SPImage;

SPImage spImageCreateFromImg(SPPoint* feats, int featsCount);
SPImage spImageCreateFromFeats(const char* featsFileName);
int spImageGetNumOfFeature(SPImage image);
SPPoint spImageGetFeature(SPImage image, int index);
bool spImageSaveToFeats(SPImage image, const char* featsFileName);
void spImageDestroy(SPImage);

#endif /* SPIMAGE_H_ */
