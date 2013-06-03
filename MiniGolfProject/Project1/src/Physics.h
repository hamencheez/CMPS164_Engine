#pragma once

#include <vector>
#include <iostream>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "Ball.h"

#define PI 3.141592653589793

/* NOTE: This class is very much untested at the moment
 * none of what's in here is guaranteed to work, don't be afraid
 * to completely change structure, etc.
*/

class Physics {
private:
	int currentTime;

public:

	Physics() {currentTime = clock();}

	//We can construct a plane out of the normal in the equation
	//P => ax + by + c = ax1 + by1 + cz1
	//Where (a,b,c) is the normal, (x1,y1,z1) is a known point on the plane
	//and (x,y,z) is the point we want. We know x and z, so we solve for y to find the height

	float Physics::getHeightOfBall(Tile tile, Ball* ball);

	Vector3* Physics::applyForces(Vector3 *velocity, float angle, Vector3 *down);

	bool Physics::pointOnPlane(std::vector<float> xVertex, std::vector<float> zVertex, float x, float z);

	bool Physics::pointOnLine(float x1, float y1, float x2, float y2, float x, float y) ;

	Vector3* Physics::calculateReflection(float x1, float z1, float x2, float z2, Vector3* ball);

	Vector3* Physics::calculateReflection(Vector3 *normal, Vector3 *ball);

	float Physics::minNumber(std::vector<float> data);

	float Physics::maxNumber(std::vector<float> data);
};