#pragma once

#include "Vector3.h"

class TileObject
{	
public:
	float height;
	float radius;
	float value;

	Vector3 *position; 
	Vector3 *velocity;
	Vector3 *initialPosition;
	bool isMoving;

	TileObject(void);
	~TileObject(void);

	TileObject(float x, float y, float z);

	virtual bool collide(float x, float z) = 0;

	void Initialize(float x, float y, float z);

	void setPos(float x, float y, float z);

	void setVelocity(float x, float y, float z);
	
	void Update(float y, Vector3 *down);	
};

