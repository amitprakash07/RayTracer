#include "MonteCarloGI.h"
#include "HemiSphereSampler.h"

#define GI_SAMPLE 10
#define BOUNCE_COUNT 1

Color MonteCarloGI::indirectIlluminate(HitInfo i_hitInfo)
{
	Color indirectIlluminateColor = Color(0.0f);
	
	return indirectIlluminateColor;
}

MonteCarloGI::MonteCarloGI()
{
	
}

MonteCarloGI::~MonteCarloGI()
{
	
}


