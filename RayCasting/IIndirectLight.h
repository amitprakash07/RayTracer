#ifndef __I_INDIRECT_LIGHT_H
#define __I_INDIRECT_LIGHT_H

#include "scene.h"

class I_IndirectIlluminate
{
public:
	virtual Color indirectIlluminate() = 0;
	I_IndirectIlluminate(){}
	virtual ~I_IndirectIlluminate(){}
};


#endif