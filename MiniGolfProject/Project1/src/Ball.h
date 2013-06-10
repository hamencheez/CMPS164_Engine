//Ball.h
#pragma once

#include "TileObject.h"

class Ball: public TileObject {
private:
	
public:
	//float height;
	float aim;

	//Vector3 *position; 
	//Vector3 *velocity;
	//Vector3 *initialPosition;
	//bool isMoving;

	Ball(void){};
	~Ball(void){};

	//void Initialize(float x, float y, float z);
	
	void setPos(float x, float y, float z);

	void setVelocity(float x, float y, float z);

	void launchBall(float speed, float angle);	
};