#pragma once

#include "TileObject.h"

class SphereLarge: public TileObject
{
public:
	SphereLarge(void);
	~SphereLarge(void);

	SphereLarge(float x, float y, float z);

	bool collide(float x, float z);
};

