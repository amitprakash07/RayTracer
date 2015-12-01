#ifndef __MONTE_CARLO_H
#define __MONTE_CARLO_H

#include "IIndirectLight.h"

class MonteCarloGI:public I_IndirectIlluminate
{
public:
	Color indirectIlluminate() override;
	MonteCarloGI();
	~MonteCarloGI();
private:
	
};

#endif