#include "SphereSmall.h"


SphereSmall::SphereSmall(void)
{
}


SphereSmall::~SphereSmall(void)
{
}

SphereSmall::SphereSmall(float x, float y, float z) {
	position = new Vector3(x, y, z);
	value = 10;
}

bool SphereSmall::collide(float x, float z){
	return false;
}