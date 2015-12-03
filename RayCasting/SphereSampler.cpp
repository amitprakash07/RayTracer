#include "SphereSampler.h"
#include <time.h>
#include "utils.h"

SphereSampler::SphereSampler(int i_minSampleCount, int i_maxSampleCount, float i_radius, Point3 i_origin, Point3 i_target)
{
	sampleOrigin = i_origin;
	initSampler(i_minSampleCount, i_maxSampleCount);
	radius = i_radius;
	targetPosition = i_target;
}

float SphereSampler::getRadius()
{
	return radius;
}

Point3 SphereSampler::getTargetPosition()
{
	return targetPosition;}

void SphereSampler::setTargetPosition(Point3 i_target)
{
	targetPosition = i_target;
}

void SphereSampler::setRadius(float i_radius)
{
	radius = i_radius;
}

Point3 SphereSampler::getSampleOrigin()
{
	return sampleOrigin;
}

void SphereSampler::setSampleOrigin(Point3 i_sampleOrigin)
{
	sampleOrigin = i_sampleOrigin;
}


void SphereSampler::generateSamples(float, float)
{
	clearSamples();
	increaseSampleCount();
	Sample tempSample;
	bool rejectionSampling = true;
	bool sampleFound = false;
	float x, y, z;
	for (int i = 0; i < getCurrentSampleCount(); i++)
	{
		if (rejectionSampling)
		{
			while (!sampleFound)
			{
				x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				if ((x*x + y*y + z*z) <= (radius *radius))
					sampleFound = true;;
			}
			sampleFound = false;
			tempSample.setOffset(Point3(x, y, z));
		}
		else
		{
			float sampleRadius = sqrt(static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * radius;
			float theta = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 180.0f;
			float phi = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 360.0f;
			Point3 offset = getSphericalCoordinates(sampleRadius, theta, phi);
			tempSample.setOffset(offset);
		}
		addSampleToList(tempSample);
	}
}




