//Ball.h

#include "Ball.h"


Ball::Ball(float x, float y, float z) {
	groundOffset = 0.1;
	position = new Vector3(x, y, z);
	velocity = new Vector3(0.05, 0.0, -0.04);
	initialPosition = new Vector3(*position);
	isMoving = false;
}

void Ball::initialize(float x, float y, float z){
	groundOffset = 0.1;
	position = new Vector3(x, y, z);
	velocity = new Vector3(0.0, 0.0, 0.0);
	initialPosition = new Vector3(*position);
	isMoving = false;
	
}

void Ball::launchBall(float speed) {
	isMoving = true;
	aim = (aim * 3.1415926) / 180;

	velocity->setPos(cos(aim), 0, sin(aim));
	velocity = velocity->normalize();
	velocity = velocity->multiply(speed);
}

void Ball::setPos(float x, float y, float z){
	position->setX(x);
	position->setY(y);
	position->setZ(z);
}

void Ball::setVelocity(float x, float y, float z){
	velocity->setX(x);
	velocity->setY(y);
	velocity->setZ(z);
}

