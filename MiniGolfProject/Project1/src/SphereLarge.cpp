#include "SphereLarge.h"


SphereLarge::SphereLarge(void)
{
}


SphereLarge::~SphereLarge(void)
{
}

SphereLarge::SphereLarge(float x, float y, float z, int ID) {
	position = new Vector3(x, y, z);
	id = ID;
	value = 100;
}

bool SphereLarge::collide(float x, float z){
	return false;
}

bool SphereLarge::collide(Vector3* otherPos){
	return (position->subtract(otherPos)->getLength() < 0.07);
}