//Ball.h

#include "Ball.h"

void Ball::launchBall(float speed, float angle) {
	isMoving = true;
	aim = (aim * 3.1415926) / 180;

	velocity->setPos(cos(aim), 0, sin(aim));
	velocity = velocity->normalize();
	velocity = velocity->multiply(speed);
}

void Ball::addVelocity(float speed, float angle) {
	isMoving = true;
	aim = (aim * 3.1415926) / 180;
	Vector3* velToAdd = new Vector3(cos(aim), 0, sin(aim));

	velocity = velocity->add(velToAdd);
	//velocity = velocity->normalize();
	//velocity = velocity->multiply(speed);
}

bool Ball::collide(float x, float z){
	return false;
}