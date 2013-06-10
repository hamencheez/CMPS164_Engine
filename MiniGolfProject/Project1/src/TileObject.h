#pragma once

#include "Vector3.h"

class TileObject
{	
public:
	float height;

	Vector3 *position; 
	Vector3 *velocity;
	Vector3 *initialPosition;
	bool isMoving;

	TileObject(void);
	~TileObject(void);

	TileObject(float x, float y, float z);

	void Initialize(float x, float y, float z);

	void setPos(float x, float y, float z);

	void setVelocity(float x, float y, float z);
	
	void Update(float y, Vector3 *down);	
};

