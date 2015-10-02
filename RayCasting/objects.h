//-------------------------------------------------------------------------------
///
/// \file       objects.h 
/// \author     Cem Yuksel (www.cemyuksel.com)
/// \version    5.0
/// \date       September 24, 2015
///
/// \brief Example source for CS 6620 - University of Utah.
///
//-------------------------------------------------------------------------------

#ifndef _OBJECTS_H_INCLUDED_
#define _OBJECTS_H_INCLUDED_

#include "scene.h"
#include "cyTriMesh.h"

//-------------------------------------------------------------------------------

class Sphere : public Object
{
public:
	virtual bool IntersectRay( const Ray &ray, HitInfo &hInfo, int hitSide=HIT_FRONT ) const override;
	virtual Box GetBoundBox() const override { return Box(-1,-1,-1,1,1,1); }
	virtual void ViewportDisplay (const Material *mtl) const override;
};

extern Sphere theSphere;

//-------------------------------------------------------------------------------

class Plane : public Object
{
public:
	virtual bool IntersectRay( const Ray &ray, HitInfo &hInfo, int hitSide=HIT_FRONT ) const override;
	virtual Box GetBoundBox() const override { return Box(-1,-1,0,1,1,0); }
	virtual void ViewportDisplay (const Material *mtl) const override;
};

extern Plane thePlane;

//-------------------------------------------------------------------------------

class TriObj : public Object, public cyTriMesh
{
public:
	virtual bool IntersectRay( const Ray &ray, HitInfo &hInfo, int hitSide=HIT_FRONT ) const override;
	virtual Box GetBoundBox() const override { return Box(GetBoundMin(),GetBoundMax()); }
	virtual void ViewportDisplay(const Material *mtl) const override;

	bool Load(const char *filename)
	{
		if ( ! LoadFromFileObj( filename ) ) return false;
		if ( ! HasNormals() ) ComputeNormals();
		ComputeBoundingBox();
		return true;
	}

private:
	bool IntersectTriangle( const Ray &ray, HitInfo &hInfo, int hitSide, unsigned int faceID ) const;
};

//-------------------------------------------------------------------------------

#endif
