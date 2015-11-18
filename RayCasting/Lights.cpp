#include "lights.h"
#include "RayIntersection.h"
#include "SphereSampler.h"

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
	Sampler * randomSphereSampler = new SphereSampler(SHADOW_SAMPLE_COUNT, SHADOW_SAMPLE_COUNT, size, p, position);
	randomSphereSampler->generateSamples();
	for (int i = 0; i < randomSphereSampler->getCurrentSampleCount(); ++i)
	{
		Ray sampleRay = randomSphereSampler->getSampleRay(i);
		randomSphereSampler->setSampleColor(i, Shadow(sampleRay)* intensity);
		randomSphereSampler->setIsSampleHit(i, true);
	}
	Color returnColor = randomSphereSampler->getAveragedSampleListColor();
	delete randomSphereSampler;
	return returnColor;
}


