#include "lights.h"
#include "RayIntersection.h"

extern Node rootNode;

float GenLight::Shadow(Ray ray, float t_max)
{
	HitInfo hInfo;
	hInfo.Init();
	hInfo.z = t_max;
	if (TraceRay(&rootNode, ray, hInfo, HIT_FRONT))
		return 0.0f;
	return 1.0f;
}