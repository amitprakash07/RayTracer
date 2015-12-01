#include "lights.h"
#include "RayIntersection.h"
#include "CircleSampler.h"
#include "utils.h"

extern Node rootNode;

#define SHADOW_SAMPLE_COUNT 8


float GenLight::Shadow(Ray ray, float t_max)
{
	HitInfo hInfo;
	hInfo.Init();
	hInfo.z = t_max;
	if (TraceRay(&rootNode, ray, hInfo, HIT_FRONT))
		return 0.0f;
	return 1.0f;
}


Color PointLight::Illuminate(const Point3& p, const Point3& N) const
{
	Sampler * randomCircleSampler = new CircleSampler(SHADOW_SAMPLE_COUNT, SHADOW_SAMPLE_COUNT, size, p, position);
	randomCircleSampler->generateSamples();
	Point3 randomVectorW;
	Point3 vectorU;
	Point3 vectorV;
	Point3 lightDir = Direction(p);
	srand(time(nullptr));
	for (int i = 0; i < randomCircleSampler->getCurrentSampleCount(); ++i)
	{
		getOrthoNormalBasisVector(lightDir, vectorU, vectorV);
		Point3 offset = randomCircleSampler->getSample(i).getOffset();
		Point3 samplePosition = position + offset.x*vectorU + offset*vectorV;
		Ray sampleRay;
		sampleRay.dir = samplePosition - p;
		sampleRay.p = p;
		randomCircleSampler->setSampleColor(i, Shadow(sampleRay)* intensity);
		randomCircleSampler->setIsSampleHit(i, true);
	}

	Color returnColor = randomCircleSampler->getAveragedSampleListColor();
	delete randomCircleSampler;
	return returnColor;
}


