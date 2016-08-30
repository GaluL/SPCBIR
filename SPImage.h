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

typedef struct sp_image_t *SPImage;

SPImage spImageCreateFromImg(SPPoint* feats, int featsCount);
//SPImage spImageCreateFromFeats(const char* featsFileName);
bool spImageSaveToFeats(SPImage image, const char* featsFileName);
void spImageDestroy(SPImage);

#endif /* SPIMAGE_H_ */
