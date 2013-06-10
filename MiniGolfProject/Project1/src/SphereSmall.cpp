#include "SphereSmall.h"


SphereSmall::SphereSmall(void)
{
}


SphereSmall::~SphereSmall(void)
{
}

SphereSmall::SphereSmall(float x, float y, float z, int ID) {
	position = new Vector3(x, y, z);
	id = ID;
	value = 10;
}

bool SphereSmall::collide(float x, float z){
	return false;
}

bool SphereSmall::collide(Vector3* otherPos){
	return (position->subtract(otherPos)->getLength() < 0.07);
}