#include "scene.h"

#define BOX_BIAS (1200* FLT_EPSILON)
#include <iostream>

bool Box::IntersectRay(const Ray& r, float t_max) const
{
	bool isHit = false;

	if (r.dir.x != 0.0f && r.dir.y != 0.0f && r.dir.z != 0.0f)
	{
		//Check X-Normal
		float tx1 = (-r.p.x + pmin.x) / r.dir.x;
		float tx2 = (-r.p.x + pmax.x) / r.dir.x;

		float txEntry = min(tx1, tx2);
		float txExit = max(tx1, tx2);


		//Check y-Plane
		float ty1 = (-r.p.y + pmin.y) / r.dir.y;
		float ty2 = (-r.p.y + pmax.y) / r.dir.y;
		float tyEntry = min(ty1, ty2);
		float tyExit = max(ty1, ty2);
		

		//Check z-Plane
		float tz1 = (-r.p.z + pmin.z) / r.dir.z;
		float tz2 = (-r.p.z + pmax.z) / r.dir.z;
		float tzEntry = min(tz1, tz2);
		float tzExit = max(tz1, tz2);
		

		float tEntry = max(txEntry, max(tyEntry, tzEntry));
		float tExit = min(txExit, min(tyExit, tzExit));


		if (tEntry < tExit )
		{
			/*t_max = tEntry;*/
			isHit = true;
			//std::cout << "\nBox InterSection Successful";
		}
	}
	
	//std::cout << "\nBox InterSection UnSuccessful";
	return isHit;
}


