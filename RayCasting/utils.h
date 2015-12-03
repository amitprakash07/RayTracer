#pragma once
#ifndef __UTILS_H
#define __UTILS_H

#include "scene.h"
#include "CircleSampler.h"
#include <time.h>

extern Camera camera;

inline Ray calculatePixelCoords(int pixelPositionAlongWidth, 
	int pixelPositonAlongHeight, Point2 positionInsidePixel = Point2(0.5f,0.5f))
{
	Point3 cameraRight = (camera.dir ^ camera.up).GetNormalized();
	double aspectRatio = static_cast<double>(camera.imgWidth) / 
									static_cast<double>(camera.imgHeight);

	float camera_l;
	float imagePlaneHeight = 1.0f;
	
	if(camera.focaldist > 1.0f)
	{
		camera_l = camera.focaldist;
		imagePlaneHeight = camera_l * tan((camera.fov / 2) * (M_PI / 180));
	}
	else
	{
		camera_l = 1 / (tan((camera.fov / 2) * (M_PI / 180)));
	}

	Point3 Sx = cameraRight;
	Point3 Sy = (-1.0f) * camera.up;
	Point3 pixel;
	Point3 k = camera.pos + camera_l* camera.dir;// -cameraRight + camera.up;
	float flipped_i = camera.imgHeight - pixelPositonAlongHeight - 1;
	pixel = k + 
		(imagePlaneHeight * aspectRatio * ((2 * ((pixelPositionAlongWidth + positionInsidePixel.x) / camera.imgWidth)) - 1))*Sx +
		(imagePlaneHeight * ((2*((flipped_i + positionInsidePixel.y) / camera.imgHeight)) - 1))*Sy;
		
	Ray sampleRay;
	Sample cameraSample;
	if(camera.dof > 0.0f)
	{
		Sampler *circleRandomSampler = new CircleSampler(10, 10, camera.dof, camera.pos, camera.pos);
		circleRandomSampler->generateSamples();
		int sampleCount = circleRandomSampler->getCurrentSampleCount();
		cameraSample = circleRandomSampler->getSample(static_cast<int>(static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * sampleCount));
		Point3 offset = cameraSample.getOffset();
		sampleRay.p = camera.pos + offset;
		sampleRay.dir = (pixel - sampleRay.p).GetNormalized();
		delete circleRandomSampler;
	}
	else
	{
		sampleRay.p = camera.pos;
		sampleRay.dir = (pixel - camera.pos).GetNormalized();
	}
	return sampleRay;
}


inline Point3 getSphericalCoordinates(float i_radius, float i_theta, float i_phi)
{
	i_theta = i_theta * (M_PI / 180);
	i_phi = i_phi * (M_PI / 180);
	/*Point3 cameraRight = (camera.dir ^ camera.up).GetNormalized();*/
	Point3 sphericalCoordinate;
	//sphericalCoordinate = (i_radius * sin(i_theta) * sin(i_phi))*cameraRight + (i_radius * sin(i_theta) * cos(i_phi))*camera.dir + (i_radius * cos(i_theta)) * camera.up;
	sphericalCoordinate.x = i_radius * sin(i_theta) * sin(i_phi);
	sphericalCoordinate.y = i_radius * cos(i_theta);
	sphericalCoordinate.z = i_radius * sin(i_theta) * cos(i_phi);
	return sphericalCoordinate;
}

inline Point3 getCoordinateOnCircle(float i_radius, float i_theta)
{
	i_theta = i_theta * (M_PI / 180);
	//Point3 cameraRight = (camera.dir ^ camera.up).GetNormalized();
	Point3 circularCoordinate;
	//circularCoordinate = (i_radius * cos(i_theta))*cameraRight + (i_radius * sin(i_theta))*camera.up;
	circularCoordinate.x = i_radius * cos(i_theta);
	circularCoordinate.y = i_radius * sin(i_theta);
	circularCoordinate.z = 0.0f;
	return circularCoordinate;
}


inline Point3 getRandomVector()
{
	Point3 randomVector;
	randomVector.x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	randomVector.y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	randomVector.z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	return randomVector;
}

inline int getRandomNumber(int max)
{
	return static_cast<int>((static_cast<float>(rand()) / static_cast<float>(RAND_MAX))*max);
}

#define RANDOMCOSINEANGLE 0.75

inline void getOrthoNormalBasisVector(Point3 i_up, Point3 &o_out_vector /*U*/, Point3& o_vector_right /*v*/)
{
	Point3 randomVectorW;
	bool foundRandomVector = false;
	while (!foundRandomVector)
	{
		randomVectorW = getRandomVector();
		if (i_up.Dot(randomVectorW) < RANDOMCOSINEANGLE)
		{
			foundRandomVector = true;
			o_out_vector = i_up.Cross(randomVectorW);
			o_vector_right = i_up.Cross(o_out_vector).GetNormalized();
			o_out_vector.Normalize();
		}
	}
}

#endif