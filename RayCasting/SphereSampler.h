#ifndef __SPHERE_SAMPLER_H
#define __SPHERE_SAMPLER_H

#include "Sampler.h"

class SphereSampler:public Sampler
{
public :
	void generateSamples(float i = 0.0f, float j = 0.0f) override;
	void setRadius(float);
	float getRadius();
	Point3 getSampleOrigin();
	void setSampleOrigin(Point3);
	Point3 getTargetPosition();
	void setTargetPosition(Point3);
	SphereSampler(int, int, float, Point3, Point3);
private :
	float radius;
	Point3 sampleOrigin;
	Point3 targetPosition;
	
};

#endif