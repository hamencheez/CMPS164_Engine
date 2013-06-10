#pragma once

#include "TileObject.h"

class SphereSmall: public TileObject
{
public:
	SphereSmall(void);
	~SphereSmall(void);
	SphereSmall(float x, float y, float z);

	bool collide(float x, float z);
};

