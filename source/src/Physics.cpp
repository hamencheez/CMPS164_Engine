#include "Physics.h"

#define PI 3.141592653589793

const float FRICTION = 0.995f;
const float GRAVITY = 0.001f;

//We can construct a plane out of the normal in the equation
//P => ax + by + c = ax1 + by1 + cz1
//Where (a,b,c) is the normal, (x1,y1,z1) is a known point on the plane
//and (x,y,z) is the point we want. We know x and z, so we solve for y to find the height

float Physics::getHeightOfBall(Tile tile, Ball* ball){
	Vector3* norm = tile.tileNorm;
	float x, y, z, ballHeight;
	x = norm->getX() * (tile.xVertex.at(0) - ball->position->getX());
	y = norm->getY() * tile.yVertex.at(0);
	z = norm->getZ() * (tile.zVertex.at(0) - ball->position->getZ());
	ballHeight = norm->multiply(0.07)->getY();
	return ((x + y + z) / norm->getY()) + ballHeight;
}

Vector3* Physics::applyForces(Vector3 *velocity, float angle, Vector3 *down) {
	//First we want to apply friction
	velocity = velocity->multiply(FRICTION);

	//Then we want to calculate gravity

	float gravity = abs(GRAVITY * sin(angle));
	//if (gravity != 0)
	//	std::cout <<"Gravity is " << gravity << std::endl;
	Vector3 *gForce =  down->multiply(gravity);
	velocity = velocity->add(gForce);
	velocity->setY(0);

	return velocity;
}

//Used the algorithm for point on plane from
//http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
bool Physics::pointOnPlane(std::vector<float> xVertex, std::vector<float> zVertex, float x, float z) {
	unsigned int curr = 0;
	unsigned int prev = xVertex.size() - 1;

	bool pointOnPlane = false;

	for (; curr < xVertex.size(); curr++) {
		if (((zVertex.at(curr) > z) != (zVertex.at(prev) > z)) && 
			(x < (xVertex.at(prev) - xVertex.at(curr)) * (z - zVertex.at(curr)) / (zVertex.at(prev) - zVertex.at(curr)) + xVertex.at(curr))) {
				pointOnPlane = !pointOnPlane;
		}
		prev = curr;
	}
	return pointOnPlane;
}

bool Physics::pointOnLine(float x1, float y1, float x2, float y2, float x, float y) {
	//Given two points, form a line between them
	//then check if (x, y) is a valid point
	float slope = (y2 - y1) / (x2 - x1);
	float tempY = (slope * (x - x1)) + y1;
	if (tempY == y) return true;
	if (abs(tempY - y) < 0.2) return true;

	//Test: See if we need to check the other way too
	slope = 1 / slope;
	float tempX = (slope * (y - y1)) + x1;
	if (tempX == x) return true;
	if (abs(tempX - x) < 0.2) return true;
	return false;
}

Vector3* Physics::calculateReflection(float x1, float z1, float x2, float z2, Vector3* ball) {
	//First, calculate the normal based on the passed in points
	Vector3 *v1 = new Vector3(x1 - x2, 0, z1 - z2);
	Vector3 *v2 = new Vector3(0, 1, 0);
	Vector3 *normal = v1->cross(v2);
	normal = normal->normalize();
	std::cout <<"Normal is (" << normal->getX() << ", " << normal->getY() << ", " << normal->getZ() << ")" <<std::endl;

	float scalar = 2 * ball->dot(normal);
	normal = normal->multiply(scalar);
	Vector3 *result = ball->subtract(normal);

	delete v1;
	delete v2;
	delete normal;
	return result;
}

Vector3* Physics::calculateReflection(Vector3 *normal, Vector3 *ball) {
	std::cout <<"Normal is (" << normal->getX() << ", " << normal->getY() << ", " << normal->getZ() << ")" <<std::endl;
	float scalar = 2 * ball->dot(normal);
	normal = normal->multiply(scalar);
	Vector3 *result = ball->subtract(normal);

	return result;
}

float Physics::minNumber(std::vector<float> data) {
	float min = 999999;
	for (int itor = 0; itor < data.size(); ++itor) {
		if (data.at(itor) < min) min = data.at(itor);
	}
	return min;
}

float Physics::maxNumber(std::vector<float> data) {
	float max = -999999;
	for (int itor = 0; itor < data.size(); ++itor) {
		if (data.at(itor) > max) max = data.at(itor);
	}
	return max;
}
