#ifndef __ADDITIONAL_IMAGE
#define __ADDITIONAL_IMAGE

#include "cyColor.h"
typedef cyColor24 Color24;

struct __ADDITIONAL_IMAGES
{
	Color24* normalImage;
	Color24* perturbedNormal;
	Color24 *operationCountImage;
	__ADDITIONAL_IMAGES()
	{
		normalImage = new Color24[800 * 600];
		perturbedNormal = new Color24[800 * 600];
		operationCountImage = new Color24[800 * 600];
	}

};
typedef __ADDITIONAL_IMAGES AdditionalImages;

#endif