#include "TileObject.h"


TileObject::TileObject(void)
{
}


TileObject::~TileObject(void)
{
}

TileObject::TileObject(float x, float y, float z) {
	height = 0;
	position = new Vector3(x, y, z);
	velocity = new Vector3(0.05, 0.0, -0.04);
	initialPosition = new Vector3(*position);
	isMoving = false;
}

void TileObject::Initialize(float x, float y, float z){
	height = 0;
	position = new Vector3(x, y, z);
	velocity = new Vector3(0.0, 0.0, 0.0);
	initialPosition = new Vector3(*position);
	isMoving = false;
	
}

void TileObject::setPos(float x, float y, float z){
	position->setX(x);
	position->setY(y);
	position->setZ(z);
}

void TileObject::setVelocity(float x, float y, float z){
	velocity->setX(x);
	velocity->setY(y);
	velocity->setZ(z);
}

