#pragma once
#ifndef __UTILS_H
#define __UTILS_H

#include "scene.h"

extern Camera camera;

inline Ray calculatePixelCoords(int pixelPositionAlongWidth, 
	int pixelPositonAlongHeight, Point2 positionInsidePixel = Point2(0.0f,0.0f))
{
	Point3 cameraRight = (camera.dir ^ camera.up).GetNormalized();
	double aspectRatio = static_cast<double>(camera.imgWidth) / 
									static_cast<double>(camera.imgHeight);
	
	float camera_l = camera.focaldist / (tan((camera.fov / 2) * (M_PI / 180)));

	Point3 Sx = cameraRight;
	Point3 Sy = (-1.0f) * camera.up;
	Point3 pixel;
	Point3 k = camera.pos + camera_l* camera.dir;// -cameraRight + camera.up;
	float flipped_i = camera.imgHeight - pixelPositonAlongHeight - 1;
	pixel = k + (((2.0f*aspectRatio * (pixelPositionAlongWidth /*+ 0.5f*/ + positionInsidePixel.x)) / camera.imgWidth) - aspectRatio)*Sx + ((((flipped_i /*+ 0.5*/ + positionInsidePixel.y) * 2) / camera.imgHeight) - 1)* Sy;
	Ray sampleRay;
	sampleRay.p = camera.pos;
	sampleRay.dir = (pixel - camera.pos).GetNormalized();
	return sampleRay;
}


inline Point3 getSphericalCoordinates(float i_radius, float i_theta, float i_phi)
{
	i_theta = i_theta * (M_PI / 180);
	i_phi = i_phi * (M_PI / 180);
	Point3 sphericalCoordinate;
	sphericalCoordinate.x = i_radius * sin(i_theta) * sin(i_phi);
	sphericalCoordinate.z = i_radius * cos(i_theta);
	sphericalCoordinate.y = i_radius * sin(i_theta) * cos(i_phi);
	return sphericalCoordinate;
}

inline Point3 getCoordinateOnCircle(float i_radius, float i_theta)
{
	i_theta = i_theta * (M_PI / 180);
	Point3 circularCoordinate;
	circularCoordinate.x = i_radius * cos(i_theta);
	circularCoordinate.y = 0.0f;
	circularCoordinate.z = i_radius * sin(i_theta);
	return circularCoordinate;
}

#endif