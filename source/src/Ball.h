//Ball.h
#pragma once

#include "Vector3.h"
#include "Tile.h"
#include "../GL/freeglut.h"

class Ball {
private:
	
public:
	float groundOffset;
	float aim;

	Vector3 *position; 
	Vector3 *velocity;
	Vector3 *initialPosition;
	bool isMoving;

	Ball(void){};
	~Ball(void){};

	Ball(float x, float y, float z);

	void initialize(float x, float y, float z);

	void setPos(float x, float y, float z);

	void setVelocity(float x, float y, float z);
	
	//untested
	void update(float y, Vector3 *down);

	//untested
	void launchBall();

	//untested
	void resetBall();

};