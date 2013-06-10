#include "Physics.h"

#define PI 3.141592653589793

const float FRICTION = 0.995f;
const float GRAVITY = 0.0006f;

//We can construct a plane out of the normal in the equation
//P => ax + by + c = ax1 + by1 + cz1
//Where (a,b,c) is the normal, (x1,y1,z1) is a known point on the plane
//and (x,y,z) is the point we want. We know x and z, so we solve for y to find the height

float Physics::getHeightOfBall(Tile* tile, Ball* ball){
	Vector3* norm = tile->tileNorm;
	float x, y, z, ballHeight;
	x = norm->getX() * (tile->xVertex.at(0) - ball->position->getX());
	y = norm->getY() * tile->yVertex.at(0);
	z = norm->getZ() * (tile->zVertex.at(0) - ball->position->getZ());
	ballHeight = norm->multiply(0.04)->getY();
	return ((x + y + z) / norm->getY()) + abs(ballHeight);
}

float Physics::getHeightOfPoint(Tile* tile, int x1, int z1){
	Vector3* norm = tile->tileNorm;
	float x, y, z, height;
	x = norm->getX() * (tile->xVertex.at(0) - x1);
	y = norm->getY() * tile->yVertex.at(0);
	z = norm->getZ() * (tile->zVertex.at(0) - z1);
	height = norm->multiply(0.04)->getY();
	return ((x + y + z) / norm->getY()) + abs(height);
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

void Physics::checkForCollision(Tile* tile, Ball* ball) {
	//3D plane
	//If we dot the normal of the plane with the ray direction, we know if we're approaching or not
	//1 means they're heading straight at each other
	//0 means parallel
	//-1 means moving away
	//So if the dot product is less than or equal to 0, we can ignore it

	//If: Position vector to plane distance > 0 and Position plus velocity to plane distance <= 0
	//    Or opposite
	//Then: We have a collision

	for (unsigned int itor = 0; itor < tile->neighbors.size(); ++itor) {
		//if (tile->neighbors.at(itor) != 0) continue;

		//First check the dot product between the wall's normal and the
		//direction of the ball, aka the velocity vector
		
		//To do this, calculate the wall's normal
		Vector3 *vec1 = new Vector3(tile->xVertex.at(itor) - tile->xVertex.at((itor + 1) % 
			tile->neighbors.size()), 0, tile->zVertex.at(itor) - tile->zVertex.at((itor + 1) % 
			tile->neighbors.size()));
		Vector3 *vec2 = new Vector3(0, 1, 0);
		Vector3 *wallNormal = vec1->cross(vec2);
		wallNormal = wallNormal->normalize();
		
		//std::cout <<"Normal is (" << wallNormal->getX() << ", " << wallNormal->getY() << ", " << wallNormal->getZ() << ")" <<std::endl;

		double dotProduct = wallNormal->dot(ball->velocity);

		//std::cout <<"Dot product is " << dotProduct <<std::endl;
		if (dotProduct >= 0) continue;

		//cout << "expected collision" << endl;

		//If greater than one, we are moving towards the wall
		//and need to check for a collision

		//We can determine the distance from the plane by the equation
		//D = N dot (x0 - x1), where x1 is any point we know is on the plane
		Vector3* v1 = vec1;
		v1 = new Vector3(tile->xVertex.at(itor), tile->yVertex.at(itor), tile->zVertex.at(itor));

		double distanceBallToWall = wallNormal->dot(ball->position->subtract(v1));
		
		Vector3* v2 = vec2;
		//Now add the velocity to the position of the ball, then see if we've passed the wall
		v2 = (ball->position->add(ball->velocity));
		double distanceNextStep = wallNormal->dot(v2->subtract(v1));
		v2 = (ball->position->add(ball->velocity->multiply(2)));
		double distanceNextStep2 = wallNormal->dot(v2->subtract(v1));

		//std::cout <<"Distance " << distanceBallToWall << " " <<distanceNextStep <<std::endl;

		int neigh = tile->neighbors.at(itor);

		if (distanceNextStep <= 0 || distanceNextStep2 <= 0 ||
			(distanceBallToWall > 0.04 && distanceNextStep < 0.04) ||  
			(distanceBallToWall > 0.04 && distanceNextStep < 0.04))
		{
			if (tile->neighbors.at(itor) == 0) {
				ball->velocity = calculateReflection(wallNormal, ball->velocity);
			}
		}

		delete v1;
		delete v2;
		delete wallNormal;
	}
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

bool Physics::pointContained(Tile* tile, Vector3* pos) {
	int maxX, maxZ, maxY;
	int minX, minZ, minY;
	maxX = maxZ = maxY = -9999;
	minX = minZ = minY = 9999;

	bool pointOnPlane = false;

	for (unsigned int curr = 0; curr < tile->xVertex.size() - 1; curr++) {
		if (tile->xVertex[curr] < minX) minX = tile->xVertex[curr];
		if (tile->xVertex[curr] > maxX) maxX = tile->xVertex[curr];
		if (tile->zVertex[curr] < minX) minZ = tile->zVertex[curr];
		if (tile->zVertex[curr] > maxX) maxZ = tile->zVertex[curr];
		if (tile->zVertex[curr] < minX) minZ = tile->zVertex[curr];
		if (tile->zVertex[curr] > maxX) maxZ = tile->zVertex[curr];
	}

	return (pos->getX() >= (minX - 0.1) && pos->getX() <= (maxX + 0.1)
		 && pos->getZ() >= (minZ - 0.1) && pos->getZ() <= (maxZ + 0.1)
		 && pos->getY() >= (minY - 0.1) && pos->getY() <= (maxY + 0.1));
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
