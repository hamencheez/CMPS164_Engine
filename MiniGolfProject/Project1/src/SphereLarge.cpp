#include "SphereLarge.h"


SphereLarge::SphereLarge(void)
{
}


SphereLarge::~SphereLarge(void)
{
}

SphereLarge::SphereLarge(float x, float y, float z) {
	position = new Vector3(x, y, z);
	value = 100;
}

bool SphereLarge::collide(float x, float z){
	return false;
}