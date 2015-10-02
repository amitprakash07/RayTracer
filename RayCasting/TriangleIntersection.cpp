#include  "objects.h"
#include <iostream>

#define NORMALINTERPOLATE 1
#define TRIANGLEBIAS (2000* FLT_EPSILON)

typedef cyTriMesh::cyTriFace MeshIndices;

bool TriObj::IntersectRay(const Ray& ray, HitInfo& hInfo, int hitSide) const
{
	//std::cout << NF()<<std::endl;
	
	bool isHit = false;
	for (size_t i = 0; i < NF(); i++)
	{
		//cyTriFace f = F(i);
		//std::cout << f.v[0]<<"/"<<f.v[1]<<"/"<<f.v[2]<<std::endl;
		isHit = IntersectTriangle(ray, hInfo, hitSide, i);
		if (isHit)
			break;
	}
	
	return isHit;
}

bool TriObj::IntersectTriangle(const Ray& ray, HitInfo& hInfo, int hitSide, unsigned faceID) const
{
	MeshIndices vertex = this->F(faceID);
	Point3 vertexA = this->V(vertex.v[0]);
	Point3 vertexB = this->V(vertex.v[1]);
	Point3 vertexC = this->V(vertex.v[2]);
	Point3 faceNormalNormalized = Point3(0, 0, 0);
	bool ishit = false;
	Point3 normal;
	float t;
	
	normal = (vertexB - vertexA).Cross(vertexC - vertexA);
	faceNormalNormalized = normal.GetNormalized();
	//float D = vertexA.Dot(faceNormal);
	
	t = -(ray.p - vertexA).Dot(faceNormalNormalized) / ray.dir.Dot(faceNormalNormalized);

	if (t > TRIANGLEBIAS && t < hInfo.z && hitSide == HIT_FRONT)
	{
		Point3 P = ray.p + t * ray.dir;
		
		float totalArea = normal.Length() / 2;
		
		float areaA = (vertexB - P).Cross(vertexC - P).Dot(faceNormalNormalized) / 2;

		if (areaA > 0)
		{
			float areaB = (vertexC - P).Cross(vertexA - P).Dot(faceNormalNormalized) / 2;
			if (areaB > 0)
			{
				float areaC = (vertexA - P).Cross(vertexB - P).Dot(faceNormalNormalized) / 2;
				if (areaC > 0)
				{
					float alpha = areaA / totalArea;
					float beta = areaB / totalArea;
					float gamma = areaC / totalArea;
					if (alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1)
					{
						if (NORMALINTERPOLATE)
						{
							P = alpha*vertexA + beta*vertexB + gamma*vertexC;
							faceNormalNormalized = GetNormal(faceID, P).GetNormalized();
						}
						
						hInfo.p = P;
						hInfo.N = faceNormalNormalized;
						hInfo.z = t;
						if (ray.dir.Dot(hInfo.N) > 0)
							hInfo.front = true;
						else  hInfo.front = false;
						ishit = true;
					}
				}
			}
		}

	}

	
	if (t >= TRIANGLEBIAS && t > hInfo.z && hitSide == HIT_BACK)
	{
		Point3 P = ray.p + t * ray.dir;
		float totalArea = normal.Length() / 2;
		float areaA = (vertexB - P).Cross(vertexC - P).Dot(faceNormalNormalized) / 2;

		if (areaA > 0)
		{
			float areaB = (vertexC - P).Cross(vertexA - P).Dot(faceNormalNormalized) / 2;
			if (areaB > 0)
			{
				float areaC = (vertexA - P).Cross(vertexB - P).Dot(faceNormalNormalized) / 2;
				if (areaC > 0)
				{
					float alpha = areaA / totalArea;
					float beta = areaB / totalArea;
					float gamma = areaC / totalArea;
					if (alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1)
					{
						if (NORMALINTERPOLATE)
						{
							P = alpha*vertexA + beta*vertexB + gamma*vertexB;
							faceNormalNormalized = GetNormal(faceID, P);
						}
						hInfo.p = P;
						hInfo.N = GetNormal(faceID, P);;
						hInfo.z = t;
						if (ray.dir.Dot(hInfo.N) > 0)
							hInfo.front = true;
						else  hInfo.front = false;
						ishit = true;
					}
				}
			}
		}
	}

	if (t < TRIANGLEBIAS)
		ishit = false;

	return ishit;
}

