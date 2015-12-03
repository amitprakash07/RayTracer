#include "RandomSampler.h"
#include <time.h>
#include  "utils.h"
#include "Sampler.h"

RandomSampler::RandomSampler(int i_minSampleCount, int i_maxSampleCount, double i_minVarianceThreshold, double i_maxVarianceThreshold)
{
	setMinThreshold(i_minVarianceThreshold);
	setMaxThreshold(i_maxVarianceThreshold);
	initSampler(i_minSampleCount, i_maxSampleCount);
	setCurrentSampleCount(0);
}

void RandomSampler::generateSamples(float pixelIndexAlongWidth, float pixelIndexAlongHeight)
{
	clearSamples();
	increaseSampleCount();
	Point2 tempOffset;
	Sample tempSample;
	for (int i = 0; i < getCurrentSampleCount(); ++i)
	{
		tempOffset.x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		tempOffset.y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		tempSample.setRay(calculatePixelCoords(pixelIndexAlongWidth, pixelIndexAlongHeight, tempOffset));
		tempSample.setColor(Color(0.0f));
		addSampleToList(tempSample);
	}	
}






















