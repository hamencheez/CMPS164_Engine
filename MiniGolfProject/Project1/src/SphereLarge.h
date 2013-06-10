#pragma once

#include "TileObject.h"

class SphereLarge: public TileObject
{
public:
	int id;
	SphereLarge(void);
	~SphereLarge(void);

	SphereLarge(float x, float y, float z, int ID);

	bool collide(float x, float z);
	bool collide(Vector3* otherPos);
};

