//Ball.h

#include "Ball.h"

const float MAX_SPEED = 0.04f;

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
	
//untested
void Ball::update(float y, Vector3 *down) {
	//If the ball is moving, simply update it
	if (isMoving) {
		position = position->add(velocity);
		position->setY(y);
		if ((velocity->getLength() < 0.01) && (down->getLength() == 0)) { 
			isMoving = false;
			velocity = new Vector3(0.0, 0.0, 0.0);
		}
	}
}

//untested
void Ball::launchBall() {
	isMoving = true;
	aim = (aim * 3.1415926) / 180;

	velocity->setPos(cos(aim), 0, sin(aim));
	velocity = velocity->normalize();
	velocity = velocity->multiply(MAX_SPEED);
}

//untested
void Ball::resetBall() {
	isMoving = false;
	position = new Vector3(*initialPosition);
	velocity = new Vector3(0.0, 0.0, 0.0);
	aim = 0.0;
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

