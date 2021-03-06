#include "objects.h"

#define BIAS ( 2000* FLT_EPSILON)

bool Sphere::IntersectRay(const Ray& ray, HitInfo& hInfo, int hitSide) const
{
	bool isHit = false;
	float a = ray.dir.Dot(ray.dir);
	float b = 2 * ray.p.Dot(ray.dir);
	float c = ray.p.Dot(ray.p) - 1;

	float d = (b*b) - (4.0f * a * c);

	if (d >= 0.0f)
	{
		float t1 = ((-b) + sqrt(d)) / (2.0f * a);
		float t2 = ((-b) - sqrt(d)) / (2.0f * a);

		if (t2 < BIAS && t1 < BIAS)
			isHit = false;

		if (t2 < BIAS && t1 > BIAS)
			hInfo.front = false;

		if (t2 < BIAS && t1 > BIAS)
			hInfo.front = false;


		if ((t2 > BIAS) && (t2 < hInfo.z))
		{
			isHit = true;
			hInfo.z = t2;
			hInfo.p = ray.p + hInfo.z* ray.dir;
			hInfo.N = (hInfo.p - Point3(0.0f, 0.0f, 0.0f)).GetNormalized();
			hInfo.front = true;
		}

		if ((t1 > BIAS) && (t1 < hInfo.z))
		{
			isHit = true;
			hInfo.z = t1;
			hInfo.p = ray.p + hInfo.z* ray.dir;
			hInfo.N = (hInfo.p - Point3(0.0f, 0.0f, 0.0f)).GetNormalized();
			hInfo.front = false;
		}

	}
	else
	{
		isHit = false;
	}

	return isHit;
}