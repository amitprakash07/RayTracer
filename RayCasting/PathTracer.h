#ifndef __MONTE_CARLO_H
#define __MONTE_CARLO_H

#include "IIndirectLight.h"

class PathTracer :public I_IndirectIlluminate
{
public:
	Color indirectIlluminate(HitInfo) override;
	PathTracer();
	~PathTracer();
private:

};

#endif