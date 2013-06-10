#pragma once

#include "TileObject.h"

class SphereSmall: public TileObject
{
public:
	int id;
	SphereSmall(void);
	~SphereSmall(void);
	SphereSmall(float x, float y, float z, int ID);

	bool collide(float x, float z);
	bool collide(Vector3* otherPos);
};

